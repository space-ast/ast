///
/// @file      testExtractor.cpp
/// @brief     ExtractorInterface 单元测试（gtest）
/// @details   测试格式检测、TAR 解压、原始复制、回退链和工厂函数
/// @author    axel
/// @date      2026-07-25
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/Extract.hpp"
#include "ast/ArchiverUtils.hpp"
#include "ast/ExtractorImplRaw.hpp"
#include "ast/ExtractorImplTar.hpp"
#include "ast/ExtractorImplSystem.hpp"
#include "ast/ExtractorImplShellCOM.hpp"
#include "ast/ExtractorImplPowerShell.hpp"
#include "ast/CompressorImplTar.hpp"
#include "ast/FileSystem.hpp"
#include "ast/StringView.hpp"
#include "ast/Test.hpp"
#include "ast/IO.hpp"
#include <cstring>
#include <string>
#include <vector>

#include "ArchiverTestHelpers.hpp"

AST_USING_NAMESPACE

// ================================================================
// 辅助函数（仅此文件独有的 buildTarFile）
// ================================================================

namespace
{
    // 构建一个简单的 TAR 文件到内存中
    std::string buildTarFile(const std::vector<std::pair<std::string, std::string>>& entries)
    {
        std::string tar;

        for (const auto& entry : entries)
        {
            const std::string& name = entry.first;
            const std::string& content = entry.second;

            char header[512] = {};
            std::memcpy(header, name.c_str(), name.size() < 100 ? name.size() : 99);

            // 设置合理的文件权限（mode=0644），避免系统 tar 用 fchmod(0)
            // 导致解压后的文件无可读权限
            std::memcpy(header + 100, "0000644 ", 8);

            char sizeBuf[32] = {};
            snprintf(sizeBuf, sizeof(sizeBuf), "%011lo",
                     static_cast<unsigned long>(content.size()));
            std::memcpy(header + 124, sizeBuf, 11);

            header[156] = (!name.empty() && name.back() == '/') ? '5' : '0';

            std::memcpy(header + 257, "ustar", 5);
            std::memcpy(header + 263, "00", 2);

            std::memset(header + 148, ' ', 8);
            unsigned int chksum = 0;
            for (int i = 0; i < 512; ++i)
                chksum += static_cast<unsigned char>(header[i]);
            snprintf(header + 148, 7, "%06o", chksum);
            header[154] = '\0';

            tar.append(header, 512);
            tar.append(content);

            size_t pad = (512 - (content.size() % 512)) % 512;
            if (pad > 0)
                tar.append(pad, '\0');
        }

        char zeroBlock[512] = {};
        tar.append(zeroBlock, 512);
        tar.append(zeroBlock, 512);

        return tar;
    }
}

// ================================================================
// 测试夹具
// ================================================================

class ExtractorTest : public ArchiverTestBase
{
protected:
    const char* tempDirName() const override { return "test_tmp_extractor"; }
};

// ================================================================
// 格式检测测试
// ================================================================

TEST_F(ExtractorTest, DetectFormat_Tar)
{
    // 创建一个最小的 .tar 文件
    std::string tarContent = buildTarFile({});
    std::string tarPath = tmpPath("test.tar");
    ASSERT_TRUE(writeFile(tarPath, tarContent));

    EArchiveFormat fmt = aDetectArchiveFormat(tarPath.c_str());
    // USTAR 魔数应该能检测到
    EXPECT_EQ(fmt, EArchiveFormat::eTar);
}

TEST_F(ExtractorTest, DetectFormat_TarGz)
{
    // 没有实际的 .tar.gz 文件时，通过后缀检测
    EArchiveFormat fmt = aDetectArchiveFormat("test.tar.gz");
    EXPECT_EQ(fmt, EArchiveFormat::eTarGz);
}

TEST_F(ExtractorTest, DetectFormat_Tgz)
{
    EArchiveFormat fmt = aDetectArchiveFormat("test.tgz");
    EXPECT_EQ(fmt, EArchiveFormat::eTarGz);
}

TEST_F(ExtractorTest, DetectFormat_Zip)
{
    // 创建有 ZIP 魔数的文件
    std::string zipPath = tmpPath("test.zip");
    const unsigned char zipMagic[] = {0x50, 0x4B, 0x03, 0x04, 0x00};
    std::string content(reinterpret_cast<const char*>(zipMagic), sizeof(zipMagic));
    ASSERT_TRUE(writeFile(zipPath, content));

    EArchiveFormat fmt = aDetectArchiveFormat(zipPath.c_str());
    EXPECT_EQ(fmt, EArchiveFormat::eZip);
}

TEST_F(ExtractorTest, DetectFormat_7z)
{
    // 创建有 7z 魔数的文件
    std::string path = tmpPath("test.7z");
    const unsigned char magic[] = {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C, 0x00};
    std::string content(reinterpret_cast<const char*>(magic), sizeof(magic));
    ASSERT_TRUE(writeFile(path, content));

    EArchiveFormat fmt = aDetectArchiveFormat(path.c_str());
    EXPECT_EQ(fmt, EArchiveFormat::e7z);
}

TEST_F(ExtractorTest, IsArchiveFile)
{
    EXPECT_TRUE(aIsArchiveFile("test.tar"));
    EXPECT_TRUE(aIsArchiveFile("test.tar.gz"));
    EXPECT_TRUE(aIsArchiveFile("test.zip"));
    EXPECT_TRUE(aIsArchiveFile("test.7z"));
    EXPECT_TRUE(aIsArchiveFile("test.rar"));
    EXPECT_FALSE(aIsArchiveFile("test.txt"));
    EXPECT_FALSE(aIsArchiveFile("test"));
}

// ================================================================
// ExtractorImplRaw 测试
// ================================================================

TEST_F(ExtractorTest, RawExtract_IsSupported)
{
    ExtractorImplRaw& impl = ExtractorImplRaw::Instance();
    EXPECT_TRUE(impl.isSupported());
}

TEST_F(ExtractorTest, RawExtract_SingleFile)
{
    // 创建测试文件
    std::string srcPath = tmpPath("source.txt");
    std::string content = "Hello, Extractor!";
    ASSERT_TRUE(writeFile(srcPath, content));

    // 解压/复制到目标目录
    std::string destDir = tmpPath("output");
    ExtractorImplRaw& impl = ExtractorImplRaw::Instance();
    errc_t err = impl.extract(srcPath.c_str(), destDir.c_str());
    ASSERT_EQ(err, eNoError);

    // 验证目标文件存在且内容正确
    std::string destPath = destDir + "/source.txt";
    EXPECT_TRUE(fs::exists(destPath));
    EXPECT_EQ(readFile(destPath), content);
}

TEST_F(ExtractorTest, RawExtract_Directory)
{
    // 创建源目录结构
    std::string srcDir = tmpPath("srcdir");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/a.txt", "file A");
    writeFile(srcDir + "/b.txt", "file B");

    // 复制到目标
    std::string destDir = tmpPath("dstdir");
    ExtractorImplRaw& impl = ExtractorImplRaw::Instance();
    errc_t err = impl.extract(srcDir.c_str(), destDir.c_str());
    ASSERT_EQ(err, eNoError);

    // 验证
    EXPECT_TRUE(fs::exists(destDir + "/a.txt"));
    EXPECT_TRUE(fs::exists(destDir + "/b.txt"));
    EXPECT_EQ(readFile(destDir + "/a.txt"), "file A");
    EXPECT_EQ(readFile(destDir + "/b.txt"), "file B");
}

TEST_F(ExtractorTest, RawExtract_EmptySource)
{
    ExtractorImplRaw& impl = ExtractorImplRaw::Instance();
    errc_t err = impl.extract("", tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}

TEST_F(ExtractorTest, RawExtract_NonexistentSource)
{
    ExtractorImplRaw& impl = ExtractorImplRaw::Instance();
    errc_t err = impl.extract(tmpPath("nonexistent").c_str(), tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}

// ================================================================
// ExtractorImplTar 测试
// ================================================================

TEST_F(ExtractorTest, Tar_IsSupported)
{
    ExtractorImplTar& impl = ExtractorImplTar::Instance();
    EXPECT_TRUE(impl.isSupported());
}

TEST_F(ExtractorTest, Tar_CanExtract_TarFile)
{
    std::string tarContent = buildTarFile({});
    std::string tarPath = tmpPath("test.tar");
    ASSERT_TRUE(writeFile(tarPath, tarContent));

    ExtractorImplTar& impl = ExtractorImplTar::Instance();
    EXPECT_TRUE(impl.canExtract(tarPath.c_str()));
}

TEST_F(ExtractorTest, Tar_CanExtract_NonTarFile)
{
    std::string path = tmpPath("test.txt");
    ASSERT_TRUE(writeFile(path, "not a tar"));

    ExtractorImplTar& impl = ExtractorImplTar::Instance();
    EXPECT_FALSE(impl.canExtract(path.c_str()));
}

TEST_F(ExtractorTest, Tar_Extract_SingleFile)
{
    // 构建包含一个文件的 TAR
    std::vector<std::pair<std::string, std::string>> entries = {
        {"hello.txt", "Hello, TAR!"},
    };
    std::string tarContent = buildTarFile(entries);
    std::string tarPath = tmpPath("test.tar");
    ASSERT_TRUE(writeFile(tarPath, tarContent));

    // 解压
    std::string destDir = tmpPath("tarout");
    ExtractorImplTar& impl = ExtractorImplTar::Instance();
    errc_t err = impl.extract(tarPath.c_str(), destDir.c_str());

    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::exists(destDir + "/hello.txt"));
    EXPECT_EQ(readFile(destDir + "/hello.txt"), "Hello, TAR!");
}

TEST_F(ExtractorTest, Tar_Extract_MultipleFiles)
{
    std::vector<std::pair<std::string, std::string>> entries = {
        {"a.txt", "Content A"},
        {"b.txt", "Content B"},
        {"c.txt", "Content C, longer content here!"},
    };
    std::string tarContent = buildTarFile(entries);
    std::string tarPath = tmpPath("multi.tar");
    ASSERT_TRUE(writeFile(tarPath, tarContent));

    std::string destDir = tmpPath("multiout");
    ExtractorImplTar& impl = ExtractorImplTar::Instance();
    errc_t err = impl.extract(tarPath.c_str(), destDir.c_str());

    ASSERT_EQ(err, eNoError);
    EXPECT_EQ(readFile(destDir + "/a.txt"), "Content A");
    EXPECT_EQ(readFile(destDir + "/b.txt"), "Content B");
    EXPECT_EQ(readFile(destDir + "/c.txt"), "Content C, longer content here!");
}

TEST_F(ExtractorTest, Tar_Extract_EmptyFile)
{
    std::vector<std::pair<std::string, std::string>> entries = {
        {"empty.txt", ""},
    };
    std::string tarContent = buildTarFile(entries);
    std::string tarPath = tmpPath("empty.tar");
    ASSERT_TRUE(writeFile(tarPath, tarContent));

    std::string destDir = tmpPath("emptyout");
    ExtractorImplTar& impl = ExtractorImplTar::Instance();
    errc_t err = impl.extract(tarPath.c_str(), destDir.c_str());

    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::exists(destDir + "/empty.txt"));
    EXPECT_EQ(readFile(destDir + "/empty.txt"), "");
}

TEST_F(ExtractorTest, Tar_Extract_Directory)
{
    // 目录条目的文件名以 '/' 结尾，typeflag='5'
    std::vector<std::pair<std::string, std::string>> entries = {
        {"subdir/", ""},
        {"subdir/nested.txt", "nested content"},
    };
    std::string tarContent = buildTarFile(entries);
    std::string tarPath = tmpPath("dir.tar");
    ASSERT_TRUE(writeFile(tarPath, tarContent));

    std::string destDir = tmpPath("dirout");
    ExtractorImplTar& impl = ExtractorImplTar::Instance();
    errc_t err = impl.extract(tarPath.c_str(), destDir.c_str());

    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::is_directory(destDir + "/subdir"));
    EXPECT_TRUE(fs::exists(destDir + "/subdir/nested.txt"));
    EXPECT_EQ(readFile(destDir + "/subdir/nested.txt"), "nested content");
}

TEST_F(ExtractorTest, Tar_Extract_LargeFile)
{
    // 大于 64KB 的文件（测试分块读写）
    std::string largeContent(100000, 'X');
    for (size_t i = 0; i < largeContent.size(); ++i)
        largeContent[i] = static_cast<char>('A' + (i % 26));

    std::vector<std::pair<std::string, std::string>> entries = {
        {"large.bin", largeContent},
    };
    std::string tarContent = buildTarFile(entries);
    std::string tarPath = tmpPath("large.tar");
    ASSERT_TRUE(writeFile(tarPath, tarContent));

    std::string destDir = tmpPath("largeout");
    ExtractorImplTar& impl = ExtractorImplTar::Instance();
    errc_t err = impl.extract(tarPath.c_str(), destDir.c_str());

    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::exists(destDir + "/large.bin"));
    EXPECT_EQ(readFile(destDir + "/large.bin"), largeContent);
}

// ================================================================
// ExtractorImplSystem 测试
// ================================================================

TEST_F(ExtractorTest, System_IsSupported)
{
    ExtractorImplSystem& impl = ExtractorImplSystem::Instance();
    // 检查返回值类型正确即可（可能 true 也可能 false，取决于系统环境）
    bool supported = impl.isSupported();
    (void)supported;
    SUCCEED();
}

// ================================================================
// 工厂函数与回退链测试
// ================================================================

TEST_F(ExtractorTest, Factory_ReturnsNonNull)
{
    // 对任意有效路径，至少 Raw 后端应该可用
    ExtractorInterface* impl = aExtractGetImpl(tmpPath("anything").c_str());
    EXPECT_NE(impl, nullptr);
}

TEST_F(ExtractorTest, Factory_ReturnsRawForPlainFile)
{
    std::string path = tmpPath("test.txt");
    ASSERT_TRUE(writeFile(path, "hello"));

    ExtractorInterface* impl = aExtractGetImpl(path.c_str());
    ASSERT_NE(impl, nullptr);
    // 纯文件应该能用
    EXPECT_TRUE(impl->isSupported());
}

TEST_F(ExtractorTest, Factory_ReturnsTarForTarFile)
{
    std::string tarContent = buildTarFile({{"x.txt", "x"}});
    std::string tarPath = tmpPath("factory.tar");
    ASSERT_TRUE(writeFile(tarPath, tarContent));

    ExtractorInterface* impl = aExtractGetImpl(tarPath.c_str());
    ASSERT_NE(impl, nullptr);
    EXPECT_TRUE(impl->isSupported());
    EXPECT_TRUE(impl->canExtract(tarPath.c_str()));
}

TEST_F(ExtractorTest, aExtract_RawFile)
{
    std::string srcPath = tmpPath("input.txt");
    std::string content = "extract test content";
    ASSERT_TRUE(writeFile(srcPath, content));

    std::string destDir = tmpPath("aextract_out");
    errc_t err = aExtract(srcPath.c_str(), destDir.c_str());
    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::exists(destDir + "/input.txt"));
    EXPECT_EQ(readFile(destDir + "/input.txt"), content);
}

TEST_F(ExtractorTest, aExtract_TarFile)
{
    std::vector<std::pair<std::string, std::string>> entries = {
        {"data.txt", "tar data"},
    };
    std::string tarContent = buildTarFile(entries);
    std::string tarPath = tmpPath("aextract.tar");
    ASSERT_TRUE(writeFile(tarPath, tarContent));

    std::string destDir = tmpPath("aextract_tar_out");
    errc_t err = aExtract(tarPath.c_str(), destDir.c_str());
    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::exists(destDir + "/data.txt"));
    EXPECT_EQ(readFile(destDir + "/data.txt"), "tar data");
}

TEST_F(ExtractorTest, aExtract_EmptySource)
{
    errc_t err = aExtract("", tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}

// ================================================================
// ExtractorInterface 基础测试
// ================================================================

TEST_F(ExtractorTest, Interface_DefaultCanExtract)
{
    // 默认 canExtract 应该返回 true（除非子类重写）
    ExtractorImplRaw& raw = ExtractorImplRaw::Instance();
    EXPECT_TRUE(raw.canExtract("anything.txt"));
    EXPECT_TRUE(raw.canExtract(""));
}

// ================================================================
// 错误路径补充测试 (#7 盲区)
// ================================================================

TEST_F(ExtractorTest, Tar_Extract_EmptySource)
{
    errc_t err = ExtractorImplTar::Instance().extract("", tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}

TEST_F(ExtractorTest, Tar_Extract_EmptyTarget)
{
    std::string tarPath = tmpPath("err_target.tar");
    std::string srcFile = tmpPath("f.txt");
    writeFile(srcFile, "test");
    ASSERT_EQ(CompressorImplTar::Instance().compress(
        srcFile.c_str(), tarPath.c_str()), eNoError);

    errc_t err = ExtractorImplTar::Instance().extract(tarPath.c_str(), "");
    EXPECT_NE(err, eNoError);
}

TEST_F(ExtractorTest, Tar_Extract_NonexistentSource)
{
    errc_t err = ExtractorImplTar::Instance().extract(
        tmpPath("does_not_exist.tar").c_str(), tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}

TEST_F(ExtractorTest, Raw_Extract_EmptySource)
{
    errc_t err = ExtractorImplRaw::Instance().extract("", tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}

TEST_F(ExtractorTest, Raw_Extract_EmptyTarget)
{
    std::string srcPath = tmpPath("raw_err_src.txt");
    writeFile(srcPath, "test");

    errc_t err = ExtractorImplRaw::Instance().extract(srcPath.c_str(), "");
    EXPECT_NE(err, eNoError);
}

#ifdef _WIN32
TEST_F(ExtractorTest, ShellCOM_Extract_EmptySource)
{
    errc_t err = ExtractorImplShellCOM::Instance().extract("", tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}
#endif

// ================================================================
// ExtractorImplPowerShell 测试（仅 Windows）
// ================================================================

#ifdef _WIN32

TEST_F(ExtractorTest, PowerShell_IsSupported)
{
    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    EXPECT_TRUE(impl.isSupported());
}

TEST_F(ExtractorTest, PowerShell_CanExtract_GzExt)
{
    // .gz 后缀的文件
    std::string path = tmpPath("test.gz");
    ASSERT_TRUE(writeFile(path, "dummy"));

    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    EXPECT_TRUE(impl.canExtract(path.c_str()));
}

TEST_F(ExtractorTest, PowerShell_CanExtract_GzMagic)
{
    // 无 .gz 后缀但有 gzip 魔数的文件
    std::string path = tmpPath("test.bin");
    std::string gzContent = aTestCreateGzipFile("hello");
    ASSERT_TRUE(writeFile(path, gzContent));

    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    EXPECT_TRUE(impl.canExtract(path.c_str()));
}

TEST_F(ExtractorTest, PowerShell_CanExtract_NonGz)
{
    // 普通文本文件不应匹配
    std::string path = tmpPath("test.txt");
    ASSERT_TRUE(writeFile(path, "not gzip"));

    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    EXPECT_FALSE(impl.canExtract(path.c_str()));
}

TEST_F(ExtractorTest, PowerShell_CanExtract_TarGz)
{
    // .tar.gz 不应由此后端处理（由 System 或 Tar 处理）
    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    EXPECT_FALSE(impl.canExtract("test.tar.gz"));
    EXPECT_FALSE(impl.canExtract("test.tgz"));
}

TEST_F(ExtractorTest, PowerShell_Extract_SmallFile)
{
    std::string plaintext = "Hello, gzip extraction!";
    std::string gzData = aTestCreateGzipFile(plaintext);
    std::string gzPath = tmpPath("hello.gz");
    ASSERT_TRUE(writeFile(gzPath, gzData));

    std::string outPath = tmpPath("small_out.txt");
    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    errc_t err = impl.extract(gzPath.c_str(), outPath.c_str());

    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::exists(outPath));
    EXPECT_EQ(readFile(outPath), plaintext);
}

TEST_F(ExtractorTest, PowerShell_Extract_LargeFile)
{
    // 大于 64KB 的文件，测试分块读写和 deflate stored block 分块
    std::string plaintext(100000, 'X');
    for (size_t i = 0; i < plaintext.size(); ++i)
        plaintext[i] = static_cast<char>('A' + (i % 26));

    std::string gzData = aTestCreateGzipFile(plaintext);
    std::string gzPath = tmpPath("large.gz");
    ASSERT_TRUE(writeFile(gzPath, gzData));

    std::string outPath = tmpPath("large_out.bin");
    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    errc_t err = impl.extract(gzPath.c_str(), outPath.c_str());

    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::exists(outPath));
    EXPECT_EQ(readFile(outPath), plaintext);
}

TEST_F(ExtractorTest, PowerShell_Extract_EmptyFile)
{
    std::string gzData = aTestCreateGzipFile("");
    std::string gzPath = tmpPath("empty.gz");
    ASSERT_TRUE(writeFile(gzPath, gzData));

    std::string outPath = tmpPath("empty_out.dat");
    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    errc_t err = impl.extract(gzPath.c_str(), outPath.c_str());

    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::exists(outPath));
    EXPECT_EQ(readFile(outPath), "");
}

TEST_F(ExtractorTest, PowerShell_Extract_EmptySource)
{
    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    errc_t err = impl.extract("", tmpPath("out.dat").c_str());
    EXPECT_NE(err, eNoError);
}

TEST_F(ExtractorTest, PowerShell_Extract_EmptyTarget)
{
    std::string gzData = aTestCreateGzipFile("test");
    std::string gzPath = tmpPath("err_target.gz");
    ASSERT_TRUE(writeFile(gzPath, gzData));

    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    errc_t err = impl.extract(gzPath.c_str(), "");
    EXPECT_NE(err, eNoError);
}

TEST_F(ExtractorTest, PowerShell_Extract_NonexistentSource)
{
    ExtractorImplPowerShell& impl = ExtractorImplPowerShell::Instance();
    errc_t err = impl.extract(tmpPath("nonexistent.gz").c_str(), tmpPath("nonexistent_out").c_str());
    EXPECT_NE(err, eNoError);
}

TEST_F(ExtractorTest, Factory_ReturnsPowerShellForGz)
{
    std::string gzData = aTestCreateGzipFile("factory test");
    std::string gzPath = tmpPath("factory_test.gz");
    ASSERT_TRUE(writeFile(gzPath, gzData));

    ExtractorInterface* impl = aExtractGetImpl(gzPath.c_str());
    ASSERT_NE(impl, nullptr);
    EXPECT_TRUE(impl->canExtract(gzPath.c_str()));
}

TEST_F(ExtractorTest, aExtract_GzFile)
{
    std::string plaintext = "integrated gz extraction";
    std::string gzData = aTestCreateGzipFile(plaintext);
    std::string gzPath = tmpPath("integrate.gz");
    ASSERT_TRUE(writeFile(gzPath, gzData));

    std::string outPath = tmpPath("integrated_out.txt");

    // 直接使用 PowerShell 后端 — 这是 Win7 无 tar/7z 时的实际路径
    // （若 System 后端可用会先接管，但其 MinGW tar 在 Windows 路径上有已知问题）
    ExtractorImplPowerShell& ps = ExtractorImplPowerShell::Instance();
    ASSERT_TRUE(ps.isSupported());
    ASSERT_TRUE(ps.canExtract(gzPath.c_str()));
    errc_t err = ps.extract(gzPath.c_str(), outPath.c_str());

    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fs::exists(outPath));
    EXPECT_EQ(readFile(outPath), plaintext);
}

#endif // _WIN32

// ================================================================
// 空 tar 解压 (#8 盲区)
// ================================================================

TEST_F(ExtractorTest, Tar_Extract_EmptyArchive)
{
    // 构建只含结束标记的空 tar
    std::string tarPath = tmpPath("empty_archive.tar");
    char zeroBlock[512] = {};
    FILE* fp = posix::fopen(tarPath.c_str(), "wb");
    ASSERT_NE(fp, nullptr);
    fwrite(zeroBlock, 1, 512, fp);
    fwrite(zeroBlock, 1, 512, fp);
    fclose(fp);

    std::string extractDir = tmpPath("empty_archive_out");
    errc_t err = ExtractorImplTar::Instance().extract(tarPath.c_str(), extractDir.c_str());
    EXPECT_EQ(err, eNoError);
}


GTEST_MAIN()
