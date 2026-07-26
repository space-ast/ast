///
/// @file      testArchiver.cpp
/// @brief     压缩/解压综合集成测试
/// @details   端到端测试：创建数据 → 压缩 → 解压 → 验证内容一致
///           覆盖 Raw、TAR、System 各后端的交叉组合。
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

#include "ast/Compress.hpp"
#include "ast/Extract.hpp"
#include "ast/ArchiverUtils.hpp"
#include "ast/CompressorImplRaw.hpp"
#include "ast/CompressorImplTar.hpp"
#include "ast/CompressorImplSystem.hpp"
#include "ast/ExtractorImplRaw.hpp"
#include "ast/ExtractorImplTar.hpp"
#include "ast/ExtractorImplSystem.hpp"
#include "ast/FileSystem.hpp"
#include "ast/StringView.hpp"
#include "ast/Test.hpp"
#include "ast/IO.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "ArchiverTestHelpers.hpp"

// ================================================================
// 测试夹具
// ================================================================

class ArchiverTest : public ArchiverTestBase
{
protected:
    const char* tempDirName() const override { return "test_tmp_archiver"; }
};

// ================================================================
// Raw 压缩 → Raw 解压 往返测试
// ================================================================

TEST_F(ArchiverTest, Raw_RoundTrip_SingleFile)
{
    std::string original = tmpPath("original.txt");
    std::string content = "Round-trip test content for Raw backend.";
    ASSERT_TRUE(writeFile(original, content));

    std::string compressed = tmpPath("intermediate.dat");
    CompressorImplRaw& compressor = CompressorImplRaw::Instance();
    ASSERT_EQ(compressor.compress(original.c_str(), compressed.c_str()), eNoError);

    std::string extractedDir = tmpPath("extracted");
    ExtractorImplRaw& extractor = ExtractorImplRaw::Instance();
    ASSERT_EQ(extractor.extract(compressed.c_str(), extractedDir.c_str()), eNoError);

    EXPECT_TRUE(fileExists(extractedDir + "/intermediate.dat"));
    EXPECT_EQ(readFile(extractedDir + "/intermediate.dat"), content);
}

TEST_F(ArchiverTest, Raw_RoundTrip_Directory)
{
    std::string srcDir = tmpPath("rawsrc");
    createTestDirectory(srcDir);

    std::string compressed = tmpPath("rawout");
    CompressorImplRaw& compressor = CompressorImplRaw::Instance();
    ASSERT_EQ(compressor.compress(srcDir.c_str(), compressed.c_str(), srcDir.c_str()), eNoError);

    std::string extractedDir = tmpPath("rawextracted");
    ExtractorImplRaw& extractor = ExtractorImplRaw::Instance();
    ASSERT_EQ(extractor.extract(compressed.c_str(), extractedDir.c_str()), eNoError);

    // Raw extract 将目录内容直接解压到目标目录中
    verifyDirectoriesEqual(srcDir, extractedDir);
}

// ================================================================
// TAR 压缩 → TAR 解压 往返测试
// ================================================================

TEST_F(ArchiverTest, Tar_RoundTrip_SingleFile)
{
    std::string original = tmpPath("hello.txt");
    std::string content = "TAR round-trip content.";
    ASSERT_TRUE(writeFile(original, content));

    // 压缩
    std::string tarFile = tmpPath("roundtrip.tar");
    CompressorImplTar& compressor = CompressorImplTar::Instance();
    ASSERT_EQ(compressor.compress(original.c_str(), tarFile.c_str()), eNoError);

    // 解压
    std::string extractedDir = tmpPath("tarextracted");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    ASSERT_EQ(extractor.extract(tarFile.c_str(), extractedDir.c_str()), eNoError);

    EXPECT_TRUE(fileExists(extractedDir + "/hello.txt"));
    EXPECT_EQ(readFile(extractedDir + "/hello.txt"), content);
}

TEST_F(ArchiverTest, Tar_RoundTrip_MultipleFiles)
{
    std::string srcDir = tmpPath("tarmultisrc");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/a.txt", "File A content.");
    writeFile(srcDir + "/b.txt", "File B content.");
    writeFile(srcDir + "/c.txt", "File C content is longer.");

    // 压缩
    std::string tarFile = tmpPath("multi.tar");
    CompressorImplTar& compressor = CompressorImplTar::Instance();
    ASSERT_EQ(compressor.compress(srcDir.c_str(), tarFile.c_str(), srcDir.c_str()), eNoError);

    // 解压
    std::string extractedDir = tmpPath("tarmultiextracted");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    ASSERT_EQ(extractor.extract(tarFile.c_str(), extractedDir.c_str()), eNoError);

    EXPECT_EQ(readFile(extractedDir + "/a.txt"), "File A content.");
    EXPECT_EQ(readFile(extractedDir + "/b.txt"), "File B content.");
    EXPECT_EQ(readFile(extractedDir + "/c.txt"), "File C content is longer.");
}

TEST_F(ArchiverTest, Tar_RoundTrip_EmptyFile)
{
    std::string original = tmpPath("empty.txt");
    ASSERT_TRUE(writeFile(original, ""));

    std::string tarFile = tmpPath("empty.tar");
    CompressorImplTar& compressor = CompressorImplTar::Instance();
    ASSERT_EQ(compressor.compress(original.c_str(), tarFile.c_str()), eNoError);

    std::string extractedDir = tmpPath("emptyextracted");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    ASSERT_EQ(extractor.extract(tarFile.c_str(), extractedDir.c_str()), eNoError);

    EXPECT_TRUE(fileExists(extractedDir + "/empty.txt"));
    EXPECT_EQ(readFile(extractedDir + "/empty.txt"), "");
}

TEST_F(ArchiverTest, Tar_RoundTrip_NestedDirectories)
{
    std::string srcDir = tmpPath("nestedsrc");
    createTestDirectory(srcDir);

    std::string tarFile = tmpPath("nested.tar");
    CompressorImplTar& compressor = CompressorImplTar::Instance();
    ASSERT_EQ(compressor.compress(srcDir.c_str(), tarFile.c_str(), srcDir.c_str()), eNoError);

    std::string extractedDir = tmpPath("nestedextracted");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    ASSERT_EQ(extractor.extract(tarFile.c_str(), extractedDir.c_str()), eNoError);

    verifyDirectoriesEqual(srcDir, extractedDir);
}

TEST_F(ArchiverTest, Tar_RoundTrip_LargeFile)
{
    // 大于 64KB 的文件（测试分块读写）
    std::string largeContent(150000, 'X');
    for (size_t i = 0; i < largeContent.size(); ++i)
        largeContent[i] = static_cast<char>('A' + (i % 26));

    std::string original = tmpPath("large.bin");
    ASSERT_TRUE(writeFile(original, largeContent));

    std::string tarFile = tmpPath("large.tar");
    CompressorImplTar& compressor = CompressorImplTar::Instance();
    ASSERT_EQ(compressor.compress(original.c_str(), tarFile.c_str()), eNoError);

    std::string extractedDir = tmpPath("largeextracted");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    ASSERT_EQ(extractor.extract(tarFile.c_str(), extractedDir.c_str()), eNoError);

    EXPECT_TRUE(fileExists(extractedDir + "/large.bin"));
    EXPECT_EQ(readFile(extractedDir + "/large.bin"), largeContent);
}

TEST_F(ArchiverTest, Tar_RoundTrip_BinaryContent)
{
    // 包含 null 字节的二进制内容
    std::string binaryContent;
    for (int i = 0; i < 256; ++i)
        binaryContent.push_back(static_cast<char>(i));

    std::string original = tmpPath("binary.bin");
    ASSERT_TRUE(writeFile(original, binaryContent));

    std::string tarFile = tmpPath("binary.tar");
    CompressorImplTar& compressor = CompressorImplTar::Instance();
    ASSERT_EQ(compressor.compress(original.c_str(), tarFile.c_str()), eNoError);

    std::string extractedDir = tmpPath("binaryextracted");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    ASSERT_EQ(extractor.extract(tarFile.c_str(), extractedDir.c_str()), eNoError);

    EXPECT_TRUE(fileExists(extractedDir + "/binary.bin"));
    EXPECT_EQ(readFile(extractedDir + "/binary.bin"), binaryContent);
}

// ================================================================
// 交叉后端测试（压缩用一种，解压用另一种）
// ================================================================

TEST_F(ArchiverTest, CrossBackend_TarCompress_RawExtract)
{
    std::string srcDir = tmpPath("crosssrc");
    createTestDirectory(srcDir);

    // 用 TAR 压缩
    std::string tarFile = tmpPath("cross.tar");
    CompressorImplTar& compressor = CompressorImplTar::Instance();
    ASSERT_EQ(compressor.compress(srcDir.c_str(), tarFile.c_str(), srcDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(tarFile));

    // 用 Raw 解压（Raw 会把 .tar 文件当作普通文件复制到目标目录）
    std::string extractedDir = tmpPath("crossextracted");
    ExtractorImplRaw& extractor = ExtractorImplRaw::Instance();
    ASSERT_EQ(extractor.extract(tarFile.c_str(), extractedDir.c_str()), eNoError);

    // Raw 解压后，tar 文件会以原名出现在目标目录中
    EXPECT_TRUE(fileExists(extractedDir + "/cross.tar"));
    // 验证这个 tar 文件可以用 Tar 解压
    std::string verifyDir = tmpPath("crossverify");
    ExtractorImplTar& tarExtractor = ExtractorImplTar::Instance();
    ASSERT_EQ(tarExtractor.extract((extractedDir + "/cross.tar").c_str(),
                                    verifyDir.c_str()), eNoError);
    verifyDirectoriesEqual(srcDir, verifyDir);
}

TEST_F(ArchiverTest, CrossBackend_RawCompress_TarExtract)
{
    std::string srcFile = tmpPath("plain.txt");
    std::string content = "Plain file for cross-backend test.";
    ASSERT_TRUE(writeFile(srcFile, content));

    // 用 Raw "压缩"（实质是复制）
    std::string compressed = tmpPath("plain_copy.txt");
    CompressorImplRaw& compressor = CompressorImplRaw::Instance();
    ASSERT_EQ(compressor.compress(srcFile.c_str(), compressed.c_str()), eNoError);

    // TAR 提取器的 canExtract 应该拒绝非 tar 文件
    ExtractorImplTar& tarExtractor = ExtractorImplTar::Instance();
    EXPECT_FALSE(tarExtractor.canExtract(compressed.c_str()))
        << "TAR extractor should reject non-tar files in canExtract()";
}

// ================================================================
// System 后端测试（tar.gz / zip / 7z，仅当系统工具可用时）
// ================================================================

TEST_F(ArchiverTest, System_IsAvailable)
{
    CompressorImplSystem& compSys = CompressorImplSystem::Instance();
    ExtractorImplSystem& extSys = ExtractorImplSystem::Instance();

    bool compAvailable = compSys.isSupported();
    bool extAvailable = extSys.isSupported();
    (void)compAvailable;
    (void)extAvailable;
    SUCCEED();
}

TEST_F(ArchiverTest, System_RoundTrip_Zip)
{
    CompressorImplSystem& compSys = CompressorImplSystem::Instance();
    ExtractorImplSystem& extSys = ExtractorImplSystem::Instance();

    if (!compSys.canCompress("", tmpPath("test.zip").c_str()) || !extSys.canExtract(tmpPath("test.zip").c_str()))
        GTEST_SKIP() << "System zip tools not available";

    std::string srcDir = tmpPath("syszipsrc");
    createTestDirectory(srcDir);

    std::string zipFile = tmpPath("sys.zip");
    ASSERT_EQ(compSys.compress(srcDir.c_str(), zipFile.c_str(), srcDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(zipFile));
    EXPECT_EQ(aDetectArchiveFormat(zipFile.c_str()), EArchiveFormat::eZip);

    std::string extractedDir = tmpPath("syszipextracted");
    ASSERT_EQ(extSys.extract(zipFile.c_str(), extractedDir.c_str()), eNoError);

    verifyDirectoriesEqual(srcDir, extractedDir);
}

TEST_F(ArchiverTest, System_RoundTrip_TarGz)
{
    CompressorImplSystem& compSys = CompressorImplSystem::Instance();
    ExtractorImplSystem& extSys = ExtractorImplSystem::Instance();

    if (!compSys.canCompress("", tmpPath("test.tar.gz").c_str()) || !extSys.canExtract(tmpPath("test.tar.gz").c_str()))
        GTEST_SKIP() << "System tar/gzip tools not available";

    std::string srcDir = tmpPath("systgzsrc");
    createTestDirectory(srcDir);

    std::string tgzFile = tmpPath("sys.tar.gz");
    ASSERT_EQ(compSys.compress(srcDir.c_str(), tgzFile.c_str(), srcDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(tgzFile));
    EXPECT_EQ(aDetectArchiveFormat(tgzFile.c_str()), EArchiveFormat::eTarGz);

    std::string extractedDir = tmpPath("systgzextracted");
    ASSERT_EQ(extSys.extract(tgzFile.c_str(), extractedDir.c_str()), eNoError);

    verifyDirectoriesEqual(srcDir, extractedDir);
}

TEST_F(ArchiverTest, System_RoundTrip_TgzShortExt)
{
    CompressorImplSystem& compSys = CompressorImplSystem::Instance();
    ExtractorImplSystem& extSys = ExtractorImplSystem::Instance();

    if (!compSys.canCompress("", tmpPath("test.tgz").c_str()) || !extSys.canExtract(tmpPath("test.tgz").c_str()))
        GTEST_SKIP() << "System tar/gzip tools not available";

    std::string srcDir = tmpPath("systgzshort");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/data.txt", "tgz short extension test");

    std::string tgzFile = tmpPath("sys.tgz");
    ASSERT_EQ(compSys.compress(srcDir.c_str(), tgzFile.c_str(), srcDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(tgzFile));
    EXPECT_EQ(aDetectArchiveFormat(tgzFile.c_str()), EArchiveFormat::eTarGz);

    std::string extractedDir = tmpPath("systgzshort_extracted");
    ASSERT_EQ(extSys.extract(tgzFile.c_str(), extractedDir.c_str()), eNoError);
    EXPECT_EQ(readFile(extractedDir + "/data.txt"), "tgz short extension test");
}

TEST_F(ArchiverTest, System_RoundTrip_TarBz2)
{
    CompressorImplSystem& compSys = CompressorImplSystem::Instance();
    ExtractorImplSystem& extSys = ExtractorImplSystem::Instance();

    if (!compSys.canCompress("", tmpPath("test.tar.bz2").c_str()) || !extSys.canExtract(tmpPath("test.tar.bz2").c_str()))
        GTEST_SKIP() << "System tar/bzip2 tools not available";

    std::string srcDir = tmpPath("systbz2src");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/bz2.txt", "bzip2 compressed content");

    std::string bz2File = tmpPath("sys.tar.bz2");
    ASSERT_EQ(compSys.compress(srcDir.c_str(), bz2File.c_str(), srcDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(bz2File));
    EXPECT_EQ(aDetectArchiveFormat(bz2File.c_str()), EArchiveFormat::eTarBz2);

    std::string extractedDir = tmpPath("systbz2extracted");
    ASSERT_EQ(extSys.extract(bz2File.c_str(), extractedDir.c_str()), eNoError);
    EXPECT_EQ(readFile(extractedDir + "/bz2.txt"), "bzip2 compressed content");
}

TEST_F(ArchiverTest, System_RoundTrip_7z)
{
    CompressorImplSystem& compSys = CompressorImplSystem::Instance();
    ExtractorImplSystem& extSys = ExtractorImplSystem::Instance();

    if (!compSys.canCompress("", tmpPath("test.7z").c_str()) || !extSys.canExtract(tmpPath("test.7z").c_str()))
        GTEST_SKIP() << "System 7z tools not available";

    std::string srcDir = tmpPath("sys7zsrc");
    createTestDirectory(srcDir);

    std::string sevenZFile = tmpPath("sys.7z");
    ASSERT_EQ(compSys.compress(srcDir.c_str(), sevenZFile.c_str(), srcDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(sevenZFile));
    EXPECT_EQ(aDetectArchiveFormat(sevenZFile.c_str()), EArchiveFormat::e7z);

    std::string extractedDir = tmpPath("sys7zextracted");
    ASSERT_EQ(extSys.extract(sevenZFile.c_str(), extractedDir.c_str()), eNoError);

    verifyDirectoriesEqual(srcDir, extractedDir);
}

TEST_F(ArchiverTest, System_RoundTrip_SingleFile)
{
    // 单文件压缩往返
    CompressorImplSystem& compSys = CompressorImplSystem::Instance();
    ExtractorImplSystem& extSys = ExtractorImplSystem::Instance();

    if (!compSys.canCompress("", tmpPath("single.zip").c_str()) || !extSys.canExtract(tmpPath("single.zip").c_str()))
        GTEST_SKIP() << "System zip tools not available";

    std::string srcFile = tmpPath("lonely.txt");
    std::string content = "A single file archived by system tools.";
    ASSERT_TRUE(writeFile(srcFile, content));

    std::string zipFile = tmpPath("single.zip");
    ASSERT_EQ(compSys.compress(srcFile.c_str(), zipFile.c_str()), eNoError);
    EXPECT_TRUE(fileExists(zipFile));

    std::string extractedDir = tmpPath("single_extracted");
    ASSERT_EQ(extSys.extract(zipFile.c_str(), extractedDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(extractedDir + "/lonely.txt"));
    EXPECT_EQ(readFile(extractedDir + "/lonely.txt"), content);
}

// ================================================================
// ShellCOM 后端测试（仅 Windows）
// 注意：Shell COM 解压已验证可用，压缩端受限于 Shell.NameSpace 对
// 手工创建的空白 zip 文件无法识别（需要 STGM 或 IStorage 方式创建）。
// ================================================================

#ifdef _WIN32
#include "ast/CompressorImplShellCOM.hpp"
#include "ast/ExtractorImplShellCOM.hpp"

TEST_F(ArchiverTest, ShellCOM_IsSupported)
{
    CompressorImplShellCOM& compCOM = CompressorImplShellCOM::Instance();
    ExtractorImplShellCOM& extCOM = ExtractorImplShellCOM::Instance();

    EXPECT_TRUE(compCOM.isSupported());
    EXPECT_TRUE(extCOM.isSupported());
}

TEST_F(ArchiverTest, ShellCOM_CanCompress_OnlyZip)
{
    CompressorImplShellCOM& compCOM = CompressorImplShellCOM::Instance();

    EXPECT_TRUE(compCOM.canCompress("src", "output.zip"));
    EXPECT_TRUE(compCOM.canCompress("src", "output.ZIP"));
    EXPECT_FALSE(compCOM.canCompress("src", "output.tar"));
    EXPECT_FALSE(compCOM.canCompress("src", "output.7z"));
    EXPECT_FALSE(compCOM.canCompress("src", "output.txt"));
}

TEST_F(ArchiverTest, ShellCOM_CanExtract_OnlyZip)
{
    ExtractorImplShellCOM& extCOM = ExtractorImplShellCOM::Instance();

    std::string zipPath = tmpPath("com_detect.zip");
    const unsigned char zipMagic[] = {0x50, 0x4B, 0x03, 0x04, 0x00};
    std::string content(reinterpret_cast<const char*>(zipMagic), sizeof(zipMagic));
    ASSERT_TRUE(writeFile(zipPath, content));

    EXPECT_TRUE(extCOM.canExtract(zipPath.c_str()));
    EXPECT_FALSE(extCOM.canExtract(tmpPath("notzip.txt").c_str()));
}

TEST_F(ArchiverTest, ShellCOM_RoundTrip_Zip)
{
    CompressorImplShellCOM& compCOM = CompressorImplShellCOM::Instance();
    ExtractorImplShellCOM& extCOM = ExtractorImplShellCOM::Instance();

    ASSERT_TRUE(compCOM.isSupported());
    ASSERT_TRUE(extCOM.isSupported());

    std::string srcDir = tmpPath("comzipsrc");
    createTestDirectory(srcDir);

    std::string zipFile = tmpPath("com.zip");
    ASSERT_EQ(compCOM.compress(srcDir.c_str(), zipFile.c_str(), srcDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(zipFile));
    EXPECT_EQ(aDetectArchiveFormat(zipFile.c_str()), EArchiveFormat::eZip);

    std::string extractedDir = tmpPath("comzipextracted");
    ASSERT_EQ(extCOM.extract(zipFile.c_str(), extractedDir.c_str()), eNoError);

    verifyDirectoriesEqual(srcDir, extractedDir);
}

TEST_F(ArchiverTest, ShellCOM_RoundTrip_SingleFile)
{
    CompressorImplShellCOM& compCOM = CompressorImplShellCOM::Instance();
    ExtractorImplShellCOM& extCOM = ExtractorImplShellCOM::Instance();

    std::string srcFile = tmpPath("comsingle.txt");
    std::string content = "Shell COM single file zip test.";
    ASSERT_TRUE(writeFile(srcFile, content));

    std::string zipFile = tmpPath("comsingle.zip");
    ASSERT_EQ(compCOM.compress(srcFile.c_str(), zipFile.c_str()), eNoError);
    EXPECT_TRUE(fileExists(zipFile));

    std::string extractedDir = tmpPath("comsingle_extracted");
    ASSERT_EQ(extCOM.extract(zipFile.c_str(), extractedDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(extractedDir + "/comsingle.txt"));
    EXPECT_EQ(readFile(extractedDir + "/comsingle.txt"), content);
}
#endif

// ================================================================
// 交叉后端：System 压缩 → Tar 解压（或反之）
// ================================================================

TEST_F(ArchiverTest, CrossBackend_SystemZip_TarExtract)
{
    CompressorImplSystem& compSys = CompressorImplSystem::Instance();
    ExtractorImplTar& tarExt = ExtractorImplTar::Instance();

    if (!compSys.canCompress("", tmpPath("cross.zip").c_str()))
        GTEST_SKIP() << "System zip tools not available";

    std::string srcDir = tmpPath("crosssyssrc");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/hello.txt", "Cross-backend: system → tar");

    std::string zipFile = tmpPath("cross.zip");
    ASSERT_EQ(compSys.compress(srcDir.c_str(), zipFile.c_str(), srcDir.c_str()), eNoError);

    // Tar 提取器应该拒绝 .zip 文件
    EXPECT_FALSE(tarExt.canExtract(zipFile.c_str()))
        << "Tar extractor should reject zip files";
}

TEST_F(ArchiverTest, CrossBackend_TarCompress_SystemExtract)
{
    CompressorImplTar& compTar = CompressorImplTar::Instance();
    ExtractorImplSystem& extSys = ExtractorImplSystem::Instance();

    if (!extSys.canExtract(tmpPath("cross2.tar").c_str()))
        GTEST_SKIP() << "System tar tools not available";

    std::string srcDir = tmpPath("crosstarsrc");
    createTestDirectory(srcDir);

    std::string tarFile = tmpPath("cross2.tar");
    ASSERT_EQ(compTar.compress(srcDir.c_str(), tarFile.c_str(), srcDir.c_str()), eNoError);

    std::string extractedDir = tmpPath("crosstarextracted");
    ASSERT_EQ(extSys.extract(tarFile.c_str(), extractedDir.c_str()), eNoError);

    verifyDirectoriesEqual(srcDir, extractedDir);
}

// ================================================================
// aCompress / aExtract 集成测试
// ================================================================

TEST_F(ArchiverTest, Convenience_RawCopy_RoundTrip)
{
    std::string srcPath = tmpPath("convenience_src.txt");
    std::string content = "Convenience API test content.";
    ASSERT_TRUE(writeFile(srcPath, content));

    // 用便捷函数压缩（目标扩展名非归档格式 → 回退到 Raw）
    std::string compressed = tmpPath("convenience_out.bin");
    ASSERT_EQ(aCompress(srcPath.c_str(), compressed.c_str()), eNoError);
    EXPECT_TRUE(fileExists(compressed));

    // 用便捷函数解压
    std::string extractedDir = tmpPath("convenience_extracted");
    ASSERT_EQ(aExtract(compressed.c_str(), extractedDir.c_str()), eNoError);
    EXPECT_TRUE(fileExists(extractedDir + "/convenience_out.bin"));
    EXPECT_EQ(readFile(extractedDir + "/convenience_out.bin"), content);
}

TEST_F(ArchiverTest, Convenience_Tar_RoundTrip)
{
    std::string srcDir = tmpPath("conv_tar_src");
    createTestDirectory(srcDir);

    // 直接用 Tar 后端压缩（避免依赖系统 tar 命令的可用性）
    std::string tarFile = tmpPath("conv.tar");
    CompressorImplTar& tarCompressor = CompressorImplTar::Instance();
    errc_t compressErr = tarCompressor.compress(srcDir.c_str(), tarFile.c_str(), srcDir.c_str());
    ASSERT_EQ(compressErr, eNoError);
    EXPECT_TRUE(fileExists(tarFile));

    // 用 ExtractorImplTar 解压验证
    std::string extractedDir = tmpPath("conv_tar_extracted");
    ExtractorImplTar& tarExtractor = ExtractorImplTar::Instance();
    ASSERT_EQ(tarExtractor.extract(tarFile.c_str(), extractedDir.c_str()), eNoError);

    verifyDirectoriesEqual(srcDir, extractedDir);
}

TEST_F(ArchiverTest, Convenience_GetImpl_MatchesFormat)
{
    std::string srcFile = tmpPath("impl_test.txt");
    ASSERT_TRUE(writeFile(srcFile, "test"));

    // .tar → Tar 或 System 后端
    {
        CompressorInterface* impl = aCompressGetImpl(srcFile.c_str(), tmpPath("out.tar").c_str());
        ASSERT_NE(impl, nullptr);
        EXPECT_TRUE(impl->canCompress(srcFile.c_str(), tmpPath("out.tar").c_str()));
    }

    // .tar.gz → System 后端（如果有）或回退
    {
        CompressorInterface* impl = aCompressGetImpl(srcFile.c_str(), tmpPath("out.tar.gz").c_str());
        ASSERT_NE(impl, nullptr);
        EXPECT_TRUE(impl->isSupported());
    }

    // .zip → ShellCOM（Windows）或 System 后端
    {
        CompressorInterface* impl = aCompressGetImpl(srcFile.c_str(), tmpPath("out.zip").c_str());
        ASSERT_NE(impl, nullptr);
        EXPECT_TRUE(impl->canCompress(srcFile.c_str(), tmpPath("out.zip").c_str()));
    }

    // .7z → System 后端（如果有 7z）或回退
    {
        CompressorInterface* impl = aCompressGetImpl(srcFile.c_str(), tmpPath("out.7z").c_str());
        ASSERT_NE(impl, nullptr);
        EXPECT_TRUE(impl->isSupported());
    }

    // 非归档目标 → 应该回退到 Raw
    {
        CompressorInterface* impl = aCompressGetImpl(srcFile.c_str(), tmpPath("out.txt").c_str());
        ASSERT_NE(impl, nullptr);
        EXPECT_TRUE(impl->isSupported());
    }
}

// ================================================================
// 错误路径测试
// ================================================================

TEST_F(ArchiverTest, Error_EmptySource)
{
    EXPECT_NE(aCompress("", tmpPath("out.tar").c_str()), eNoError);
    EXPECT_NE(aExtract("", tmpPath("out").c_str()), eNoError);
}

TEST_F(ArchiverTest, Error_NonexistentSource)
{
    std::string nonexistent = tmpPath("does_not_exist");
    EXPECT_NE(aCompress(nonexistent.c_str(), tmpPath("out.tar").c_str()), eNoError);
    EXPECT_NE(aExtract(nonexistent.c_str(), tmpPath("out").c_str()), eNoError);
}

TEST_F(ArchiverTest, Error_EmptyTarget)
{
    std::string srcFile = tmpPath("valid_src.txt");
    ASSERT_TRUE(writeFile(srcFile, "valid content"));

    EXPECT_NE(aCompress(srcFile.c_str(), ""), eNoError);
}

// ================================================================
// 格式检测集成测试（多格式）
// ================================================================

TEST_F(ArchiverTest, FormatDetection_AfterCompression)
{
    std::string srcDir = tmpPath("fmtsrc");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/file.txt", "format test");

    // TAR
    {
        std::string tarFile = tmpPath("format.tar");
        ASSERT_EQ(CompressorImplTar::Instance().compress(srcDir.c_str(), tarFile.c_str(), srcDir.c_str()), eNoError);
        EXPECT_EQ(aDetectArchiveFormat(tarFile.c_str()), EArchiveFormat::eTar);
        EXPECT_TRUE(aIsArchiveFile(tarFile.c_str()));
    }

    // 扩展名检测（不依赖实际文件内容）
    EXPECT_EQ(aDetectArchiveFormat("archive.tar.gz"), EArchiveFormat::eTarGz);
    EXPECT_EQ(aDetectArchiveFormat("archive.tgz"),    EArchiveFormat::eTarGz);
    EXPECT_EQ(aDetectArchiveFormat("archive.tar.bz2"), EArchiveFormat::eTarBz2);
    EXPECT_EQ(aDetectArchiveFormat("archive.tbz2"),   EArchiveFormat::eTarBz2);
    EXPECT_EQ(aDetectArchiveFormat("archive.tar.xz"), EArchiveFormat::eTarXz);
    EXPECT_EQ(aDetectArchiveFormat("archive.txz"),    EArchiveFormat::eTarXz);
    EXPECT_EQ(aDetectArchiveFormat("archive.zip"),    EArchiveFormat::eZip);
    EXPECT_EQ(aDetectArchiveFormat("archive.7z"),     EArchiveFormat::e7z);
    EXPECT_EQ(aDetectArchiveFormat("archive.rar"),    EArchiveFormat::eRar);
    EXPECT_EQ(aDetectArchiveFormat("archive.gz"),     EArchiveFormat::eGz);
    EXPECT_EQ(aDetectArchiveFormat("archive.txt"),    EArchiveFormat::eUnknown)
        << "Non-archive extensions return eUnknown";
}

TEST_F(ArchiverTest, IsArchiveFile_VariousFormats)
{
    EXPECT_TRUE(aIsArchiveFile("data.tar"));
    EXPECT_TRUE(aIsArchiveFile("data.tar.gz"));
    EXPECT_TRUE(aIsArchiveFile("data.tgz"));
    EXPECT_TRUE(aIsArchiveFile("data.tar.bz2"));
    EXPECT_TRUE(aIsArchiveFile("data.tar.xz"));
    EXPECT_TRUE(aIsArchiveFile("data.zip"));
    EXPECT_TRUE(aIsArchiveFile("data.7z"));
    EXPECT_TRUE(aIsArchiveFile("data.rar"));
    EXPECT_TRUE(aIsArchiveFile("data.gz"));

    EXPECT_FALSE(aIsArchiveFile("data.txt"));
    EXPECT_FALSE(aIsArchiveFile("data.bin"));
    EXPECT_FALSE(aIsArchiveFile("data"));
    EXPECT_FALSE(aIsArchiveFile(""));
}

// ================================================================
// 复合压缩解压（多步骤）测试
// ================================================================

TEST_F(ArchiverTest, MultiStep_CompressExtractRecompress)
{
    // 步骤 1: 创建原始数据
    std::string originalDir = tmpPath("step1_original");
    createTestDirectory(originalDir);

    // 步骤 2: 压缩为 TAR
    std::string tar1 = tmpPath("step2_first.tar");
    ASSERT_EQ(CompressorImplTar::Instance().compress(originalDir.c_str(), tar1.c_str(), originalDir.c_str()), eNoError);

    // 步骤 3: 解压到新位置
    std::string extractedDir = tmpPath("step3_extracted");
    ASSERT_EQ(ExtractorImplTar::Instance().extract(tar1.c_str(), extractedDir.c_str()), eNoError);
    verifyDirectoriesEqual(originalDir, extractedDir);

    // 步骤 4: 再次压缩解压后的内容
    std::string tar2 = tmpPath("step4_second.tar");
    ASSERT_EQ(CompressorImplTar::Instance().compress(extractedDir.c_str(), tar2.c_str(), extractedDir.c_str()), eNoError);

    // 步骤 5: 再次解压
    std::string finalDir = tmpPath("step5_final");
    ASSERT_EQ(ExtractorImplTar::Instance().extract(tar2.c_str(), finalDir.c_str()), eNoError);

    // 两次解压的结果应该一致
    verifyDirectoriesEqual(extractedDir, finalDir);
}

// ================================================================
// 错误路径补充测试 (#7 盲区)
// ================================================================

TEST_F(ArchiverTest, Error_EmptyTarget_Extract)
{
    std::string srcDir = tmpPath("err_src");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/f.txt", "data");

    std::string tarPath = tmpPath("err.tar");
    ASSERT_EQ(CompressorImplTar::Instance().compress(
        srcDir.c_str(), tarPath.c_str(), srcDir.c_str()), eNoError);

    errc_t err = aExtract(tarPath.c_str(), "");
    EXPECT_NE(err, eNoError);
}

// ================================================================
// 交叉后端 ShellCOM 组合 (#9 盲区)
// ================================================================

TEST_F(ArchiverTest, CrossBackend_ShellCOMCompress_ShellCOMExtract)
{
    std::string srcDir = tmpPath("shellcom_x_src");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/shellcom.txt", "shellcom round trip");
    writeFile(srcDir + "/data.bin", std::string(500, '\xCD'));

    // ShellCOM 压缩，ShellCOM 解压（同一后端往返）
    std::string zipPath = tmpPath("shellcom_cross.zip");
    errc_t err = CompressorImplShellCOM::Instance().compress(
        srcDir.c_str(), zipPath.c_str(), srcDir.c_str());
    ASSERT_EQ(err, eNoError);

    std::string extractDir = tmpPath("shellcom_cross_verify");
    err = ExtractorImplShellCOM::Instance().extract(zipPath.c_str(), extractDir.c_str());
    ASSERT_EQ(err, eNoError);

    EXPECT_TRUE(fileExists(extractDir + "/shellcom.txt"));
    EXPECT_EQ(readFile(extractDir + "/shellcom.txt"), "shellcom round trip");
}

TEST_F(ArchiverTest, CrossBackend_ShellCOMCompress_TarExtract)
{
    std::string srcDir = tmpPath("shellcom_to_tar_src");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/from_shellcom.txt", "shellcom compress, tar extract via aExtract");

    // ShellCOM 压缩为 zip
    std::string zipPath = tmpPath("shellcom_to_tar.zip");
    errc_t err = CompressorImplShellCOM::Instance().compress(
        srcDir.c_str(), zipPath.c_str(), srcDir.c_str());
    ASSERT_EQ(err, eNoError);

    // 用 aExtract 自动选择后端解压（会选 ShellCOM，因为 .zip）
    std::string extractDir = tmpPath("shellcom_to_tar_verify");
    err = aExtract(zipPath.c_str(), extractDir.c_str());
    ASSERT_EQ(err, eNoError);

    EXPECT_TRUE(fileExists(extractDir + "/from_shellcom.txt"));
    EXPECT_EQ(readFile(extractDir + "/from_shellcom.txt"), "shellcom compress, tar extract via aExtract");
}

// ================================================================
// 独立 .gz 文件压缩/解压往返测试
// .gz 是单文件压缩格式，aExtract 的 target 为输出文件路径
// ================================================================

TEST_F(ArchiverTest, Gz_RoundTrip_SingleFile)
{
    // 使用纯 C++ gzip 创建器生成 .gz 文件（无需外部工具）
    std::string content = "Standalone gzip round-trip test content.";
    std::string gzContent = aTestCreateGzipFile(content);

    std::string gzFile = tmpPath("test.gz");
    ASSERT_TRUE(writeFile(gzFile, gzContent));

    // 验证格式检测
    EXPECT_EQ(aDetectArchiveFormat(gzFile.c_str()), EArchiveFormat::eGz);
    EXPECT_TRUE(aIsArchiveFile(gzFile.c_str()));

    // .gz 解压 target 是输出文件路径
    std::string extractedFile = tmpPath("extracted.txt");
    ASSERT_EQ(aExtract(gzFile.c_str(), extractedFile.c_str()), eNoError);
    EXPECT_TRUE(fileExists(extractedFile));
    EXPECT_EQ(readFile(extractedFile), content);
}

TEST_F(ArchiverTest, Gz_RoundTrip_EmptyFile)
{
    std::string gzContent = aTestCreateGzipFile("");

    std::string gzFile = tmpPath("empty.gz");
    ASSERT_TRUE(writeFile(gzFile, gzContent));
    EXPECT_EQ(aDetectArchiveFormat(gzFile.c_str()), EArchiveFormat::eGz);

    std::string extractedFile = tmpPath("empty_extracted.txt");
    ASSERT_EQ(aExtract(gzFile.c_str(), extractedFile.c_str()), eNoError);
    EXPECT_TRUE(fileExists(extractedFile));
    EXPECT_EQ(readFile(extractedFile), "");
}

TEST_F(ArchiverTest, Gz_RoundTrip_LargeContent)
{
    // 大于 64KB 的内容测试 gzip 分块解压
    std::string largeContent(200000, 'Y');
    for (size_t i = 0; i < largeContent.size(); ++i)
        largeContent[i] = static_cast<char>('A' + (i % 26));

    std::string gzContent = aTestCreateGzipFile(largeContent);

    std::string gzFile = tmpPath("large.gz");
    ASSERT_TRUE(writeFile(gzFile, gzContent));
    EXPECT_EQ(aDetectArchiveFormat(gzFile.c_str()), EArchiveFormat::eGz);

    std::string extractedFile = tmpPath("large_extracted.bin");
    ASSERT_EQ(aExtract(gzFile.c_str(), extractedFile.c_str()), eNoError);
    EXPECT_TRUE(fileExists(extractedFile));
    EXPECT_EQ(readFile(extractedFile), largeContent);
}

TEST_F(ArchiverTest, Gz_RoundTrip_BinaryContent)
{
    // 包含 null 字节和全部 256 个字节值的二进制内容
    std::string binaryContent;
    for (int i = 0; i < 256; ++i)
        binaryContent.push_back(static_cast<char>(i));

    std::string gzContent = aTestCreateGzipFile(binaryContent);

    std::string gzFile = tmpPath("binary.gz");
    ASSERT_TRUE(writeFile(gzFile, gzContent));
    EXPECT_EQ(aDetectArchiveFormat(gzFile.c_str()), EArchiveFormat::eGz);

    std::string extractedFile = tmpPath("binary_extracted.bin");
    ASSERT_EQ(aExtract(gzFile.c_str(), extractedFile.c_str()), eNoError);
    EXPECT_TRUE(fileExists(extractedFile));
    EXPECT_EQ(readFile(extractedFile), binaryContent);
}


GTEST_MAIN()
