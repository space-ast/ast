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

#include <gtest/gtest.h>
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
    if (!fp) return {};
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
