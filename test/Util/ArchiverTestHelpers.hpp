///
/// @file      ArchiverTestHelpers.hpp
/// @brief     Archiver 测试共享辅助函数和夹具
/// @details   供 testCompressor、testArchiver、testExtractor 共享使用
/// @author    axel
/// @date      2026-07-25
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "ast/FileSystem.hpp"
#include "ast/IO.hpp"
#include "ast/Logger.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <functional>

AST_USING_NAMESPACE

// ================================================================
// 共享辅助函数
// ================================================================

/// 读取文件全部内容
inline std::string aTestReadFile(const std::string& path)
{
    FILE* fp = posix::fopen(path.c_str(), "rb");
    if (!fp)
    {
        aError("aTestReadFile: failed to open file %s", path.c_str());
        return {};
    }
    std::string content;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0)
        content.append(buf, n);
    fclose(fp);
    return content;
}

/// 写入文件
inline bool aTestWriteFile(const std::string& path, const std::string& content)
{
    FILE* fp = posix::fopen(path.c_str(), "wb");
    if (!fp) return false;
    size_t written = fwrite(content.data(), 1, content.size(), fp);
    fclose(fp);
    return written == content.size();
}

/// 检查文件是否存在
inline bool aTestFileExists(const std::string& path)
{
    return fs::exists(fs::path(path));
}

/// 创建测试目录结构（含嵌套目录、多种文件大小）
inline void aTestCreateDirectory(const std::string& dirPath)
{
    fs::create_directories(dirPath);
    aTestWriteFile(dirPath + "/readme.txt",      "This is a test directory.");
    aTestWriteFile(dirPath + "/data.bin",         std::string(10000, '\xAB'));
    aTestWriteFile(dirPath + "/empty.dat",        "");
    fs::create_directories(dirPath + "/subdir");
    aTestWriteFile(dirPath + "/subdir/nested.txt", "Nested file content.");
    aTestWriteFile(dirPath + "/subdir/config.json", R"({"key": "value", "num": 42})");
    fs::create_directories(dirPath + "/subdir/deep");
    aTestWriteFile(dirPath + "/subdir/deep/deep.txt", "Deeply nested.");
}

/// 递归验证两个目录内容一致
inline void aTestVerifyDirectoriesEqual(const std::string& dirA, const std::string& dirB)
{
    std::function<void(const fs::path&, const fs::path&)> verifyDir =
        [&](const fs::path& a, const fs::path& b)
    {
        for (auto it = fs::directory_iterator(a); it != fs::directory_iterator(); ++it)
        {
            const auto& entry = *it;
            auto entryPath = entry.path();
            auto targetPath = b / entryPath.filename();

            if (fs::is_directory(entry.status()))
            {
                EXPECT_TRUE(fs::is_directory(targetPath))
                    << "Missing directory: " << entryPath.filename().string();
                if (fs::is_directory(targetPath))
                    verifyDir(entryPath, targetPath);
            }
            else
            {
                EXPECT_TRUE(fs::exists(targetPath))
                    << "Missing file: " << entryPath.filename().string();
                if (fs::exists(targetPath))
                {
                    std::string contentA = aTestReadFile(entryPath.string());
                    std::string contentB = aTestReadFile(targetPath.string());
                    EXPECT_EQ(contentA, contentB)
                        << "Content mismatch: " << entryPath.filename().string();
                }
            }
        }
    };
    verifyDir(fs::path(dirA), fs::path(dirB));
}

// ================================================================
// Gzip 文件构造（纯 C++，无需外部工具）
// ================================================================

/// @brief 创建有效的 gzip 文件内容（用于测试）
/// @details 使用 deflate stored block（无压缩）格式，包含 CRC32 校验。
///         兼容所有 gzip 解压工具（包括 PowerShell GZipStream）。
/// @param data 原始未压缩数据
/// @return gzip 格式的完整文件内容
inline std::string aTestCreateGzipFile(const std::string& data)
{
	// CRC32 查找表（多项式 0xEDB88320）
	static unsigned int crcTable[256] = {};
	static bool crcTableReady = false;
	if (!crcTableReady)
	{
		for (int i = 0; i < 256; ++i)
		{
			unsigned int crc = static_cast<unsigned int>(i);
			for (int j = 0; j < 8; ++j)
			{
				if (crc & 1)
					crc = 0xEDB88320U ^ (crc >> 1);
				else
					crc >>= 1;
			}
			crcTable[i] = crc;
		}
		crcTableReady = true;
	}

	// 计算 CRC32
	unsigned int crc = 0xFFFFFFFFU;
	for (unsigned char c : data)
		crc = crcTable[(crc ^ c) & 0xFF] ^ (crc >> 8);
	crc ^= 0xFFFFFFFFU;

	// 构建 gzip 文件
	std::string gz;

	// Gzip 头部: ID1=0x1F, ID2=0x8B, CM=0x08(deflate), FLG=0
	unsigned char header[] = {0x1F, 0x8B, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
	gz.append(reinterpret_cast<const char*>(header), sizeof(header));

	// Deflate 数据块：处理空数据和需要分块的大文件（每块最大 65535 字节）
	if (data.empty())
	{
		// 空文件：一个空的最终 stored block
		gz.push_back('\x01');            // BFINAL=1, BTYPE=00
		gz.push_back('\x00'); gz.push_back('\x00');  // LEN=0
		gz.push_back('\xFF'); gz.push_back('\xFF');  // NLEN=65535
	}
	else
	{
		size_t offset = 0;
		while (offset < data.size())
		{
			size_t chunkSize = data.size() - offset;
			if (chunkSize > 65535) chunkSize = 65535;

			bool isLast = (offset + chunkSize >= data.size());
			unsigned char bfinal = isLast ? 1 : 0;
			// BFINAL=bfinal, BTYPE=00 (stored/uncompressed)
			gz.push_back(static_cast<char>(bfinal));

			unsigned short len = static_cast<unsigned short>(chunkSize);
			unsigned short nlen = static_cast<unsigned short>(~len);
			gz.push_back(static_cast<char>(len & 0xFF));
			gz.push_back(static_cast<char>((len >> 8) & 0xFF));
			gz.push_back(static_cast<char>(nlen & 0xFF));
			gz.push_back(static_cast<char>((nlen >> 8) & 0xFF));

			if (chunkSize > 0)
				gz.append(data.data() + offset, chunkSize);

			offset += chunkSize;
	}
	}

	// CRC32 (4 字节，little-endian)
	gz.push_back(static_cast<char>(crc & 0xFF));
	gz.push_back(static_cast<char>((crc >> 8) & 0xFF));
	gz.push_back(static_cast<char>((crc >> 16) & 0xFF));
	gz.push_back(static_cast<char>((crc >> 24) & 0xFF));

	// ISIZE (原始大小 mod 2^32, 4 字节 little-endian)
	unsigned int isize = static_cast<unsigned int>(data.size());
	gz.push_back(static_cast<char>(isize & 0xFF));
	gz.push_back(static_cast<char>((isize >> 8) & 0xFF));
	gz.push_back(static_cast<char>((isize >> 16) & 0xFF));
	gz.push_back(static_cast<char>((isize >> 24) & 0xFF));

	return gz;
}

// ================================================================
// 共享测试夹具基类
// ================================================================

class ArchiverTestBase : public ::testing::Test
{
protected:
    /// 子类必须重写此方法以提供唯一的临时目录名
    virtual const char* tempDirName() const = 0;

    std::string getTempDir() const
    {
        fs::path tmp = fs::current_path() / tempDirName();
        return tmp.string();
    }

    void cleanupTempDir()
    {
        fs::path tmp(getTempDir());
        if (fs::exists(tmp))
            fs::remove_all(tmp);
    }

    void SetUp() override
    {
        cleanupTempDir();
        fs::create_directories(getTempDir());
    }

    void TearDown() override
    {
        cleanupTempDir();
    }

    std::string tmpPath(const std::string& name) const
    {
        return getTempDir() + "/" + name;
    }

    // 便捷包装
    std::string readFile(const std::string& path)    { return aTestReadFile(path); }
    bool        writeFile(const std::string& p, const std::string& c) { return aTestWriteFile(p, c); }
    bool        fileExists(const std::string& path)   { return aTestFileExists(path); }
    void        createTestDirectory(const std::string& p) { aTestCreateDirectory(p); }
    void        verifyDirectoriesEqual(const std::string& a, const std::string& b) { aTestVerifyDirectoriesEqual(a, b); }
};
