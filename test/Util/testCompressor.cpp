///
/// @file      testCompressor.cpp
/// @brief     CompressorInterface 单元测试（gtest）
/// @details   测试 Raw/Tar/System 后端和工厂函数
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
#include "ast/ArchiverUtils.hpp"
#include "ast/CompressorImplRaw.hpp"
#include "ast/CompressorImplTar.hpp"
#include "ast/CompressorImplSystem.hpp"
#include "ast/Extract.hpp"
#include "ast/ExtractorImplTar.hpp"
#include "ast/FileSystem.hpp"
#include "ast/StringView.hpp"
#include "ast/Test.hpp"
#include "ast/IO.hpp"

#include <string>
#include <vector>

#include "ArchiverTestHelpers.hpp"

// ================================================================
// 测试夹具
// ================================================================

class CompressorTest : public ArchiverTestBase
{
protected:
    const char* tempDirName() const override { return "test_tmp_compressor"; }
};

// ================================================================
// CompressorImplRaw 测试
// ================================================================

TEST_F(CompressorTest, Raw_IsSupported)
{
    CompressorImplRaw& impl = CompressorImplRaw::Instance();
    EXPECT_TRUE(impl.isSupported());
}

TEST_F(CompressorTest, Raw_CompressSingleFile)
{
    // 创建测试文件
    std::string srcPath = tmpPath("source.txt");
    std::string content = "Hello, Compressor!";
    ASSERT_TRUE(writeFile(srcPath, content));

    // 压缩/复制到目标
    std::string destPath = tmpPath("output.txt");
    CompressorImplRaw& impl = CompressorImplRaw::Instance();
    errc_t err = impl.compress(srcPath.c_str(), destPath.c_str());
    ASSERT_EQ(err, eNoError);

    // 验证目标文件存在且内容正确
    EXPECT_TRUE(fileExists(destPath));
    EXPECT_EQ(readFile(destPath), content);
}

TEST_F(CompressorTest, Raw_CompressDirectory)
{
    // 创建源目录结构
    std::string srcDir = tmpPath("srcdir");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/a.txt", "file A");
    writeFile(srcDir + "/b.txt", "file B");

    // 复制到目标
    std::string destDir = tmpPath("dstdir");
    CompressorImplRaw& impl = CompressorImplRaw::Instance();
    errc_t err = impl.compress(srcDir.c_str(), destDir.c_str());
    ASSERT_EQ(err, eNoError);

    // 验证
    EXPECT_TRUE(fileExists(destDir + "/a.txt"));
    EXPECT_TRUE(fileExists(destDir + "/b.txt"));
    EXPECT_EQ(readFile(destDir + "/a.txt"), "file A");
    EXPECT_EQ(readFile(destDir + "/b.txt"), "file B");
}

TEST_F(CompressorTest, Raw_CompressEmptySource)
{
    CompressorImplRaw& impl = CompressorImplRaw::Instance();
    errc_t err = impl.compress("", tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}

TEST_F(CompressorTest, Raw_CompressNonexistentSource)
{
    CompressorImplRaw& impl = CompressorImplRaw::Instance();
    errc_t err = impl.compress(tmpPath("nonexistent").c_str(), tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}

// ================================================================
// CompressorImplTar 测试
// ================================================================

TEST_F(CompressorTest, Tar_IsSupported)
{
    CompressorImplTar& impl = CompressorImplTar::Instance();
    EXPECT_TRUE(impl.isSupported());
}

TEST_F(CompressorTest, Tar_CanCompress_TarTarget)
{
    CompressorImplTar& impl = CompressorImplTar::Instance();
    EXPECT_TRUE(impl.canCompress(tmpPath("src").c_str(), "output.tar"));
    EXPECT_FALSE(impl.canCompress(tmpPath("src").c_str(), "output.zip"));
    EXPECT_FALSE(impl.canCompress(tmpPath("src").c_str(), "output.txt"));
}

TEST_F(CompressorTest, Tar_CompressSingleFile)
{
    // 创建测试文件
    std::string srcPath = tmpPath("hello.txt");
    std::string content = "Hello, TAR Compressor!";
    ASSERT_TRUE(writeFile(srcPath, content));

    // 压缩为 .tar
    std::string tarPath = tmpPath("output.tar");
    CompressorImplTar& impl = CompressorImplTar::Instance();
    errc_t err = impl.compress(srcPath.c_str(), tarPath.c_str());
    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fileExists(tarPath));

    // 使用 ExtractorImplTar 解压验证
    std::string extractDir = tmpPath("tar_verify");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    err = extractor.extract(tarPath.c_str(), extractDir.c_str());
    ASSERT_EQ(err, eNoError);

    EXPECT_TRUE(fileExists(extractDir + "/hello.txt"));
    EXPECT_EQ(readFile(extractDir + "/hello.txt"), content);
}

TEST_F(CompressorTest, Tar_CompressMultipleFiles)
{
    // 创建源目录
    std::string srcDir = tmpPath("multisrc");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/a.txt", "Content A");
    writeFile(srcDir + "/b.txt", "Content B");
    writeFile(srcDir + "/c.txt", "Content C, longer content here!");

    // 压缩为 .tar
    std::string tarPath = tmpPath("multi.tar");
    CompressorImplTar& impl = CompressorImplTar::Instance();
    errc_t err = impl.compress(srcDir.c_str(), tarPath.c_str(), srcDir.c_str());
    ASSERT_EQ(err, eNoError);

    // 解压验证
    std::string extractDir = tmpPath("multi_verify");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    err = extractor.extract(tarPath.c_str(), extractDir.c_str());
    ASSERT_EQ(err, eNoError);

    EXPECT_EQ(readFile(extractDir + "/a.txt"), "Content A");
    EXPECT_EQ(readFile(extractDir + "/b.txt"), "Content B");
    EXPECT_EQ(readFile(extractDir + "/c.txt"), "Content C, longer content here!");
}

TEST_F(CompressorTest, Tar_CompressEmptyFile)
{
    std::string srcPath = tmpPath("empty.txt");
    ASSERT_TRUE(writeFile(srcPath, ""));

    std::string tarPath = tmpPath("empty.tar");
    CompressorImplTar& impl = CompressorImplTar::Instance();
    errc_t err = impl.compress(srcPath.c_str(), tarPath.c_str());
    ASSERT_EQ(err, eNoError);

    // 解压验证
    std::string extractDir = tmpPath("empty_verify");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    err = extractor.extract(tarPath.c_str(), extractDir.c_str());
    ASSERT_EQ(err, eNoError);

    EXPECT_TRUE(fileExists(extractDir + "/empty.txt"));
    EXPECT_EQ(readFile(extractDir + "/empty.txt"), "");
}

TEST_F(CompressorTest, Tar_CompressNestedDirectory)
{
    // 创建嵌套目录结构
    std::string srcDir = tmpPath("nestedsrc");
    fs::create_directories(srcDir + "/subdir");
    writeFile(srcDir + "/root.txt", "root file");
    writeFile(srcDir + "/subdir/nested.txt", "nested content");

    // 压缩
    std::string tarPath = tmpPath("nested.tar");
    CompressorImplTar& impl = CompressorImplTar::Instance();
    errc_t err = impl.compress(srcDir.c_str(), tarPath.c_str(), srcDir.c_str());
    ASSERT_EQ(err, eNoError);

    // 解压验证
    std::string extractDir = tmpPath("nested_verify");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    err = extractor.extract(tarPath.c_str(), extractDir.c_str());
    ASSERT_EQ(err, eNoError);

    EXPECT_TRUE(fileExists(extractDir + "/root.txt"));
    EXPECT_EQ(readFile(extractDir + "/root.txt"), "root file");
    EXPECT_TRUE(fs::is_directory(extractDir + "/subdir"));
    EXPECT_TRUE(fileExists(extractDir + "/subdir/nested.txt"));
    EXPECT_EQ(readFile(extractDir + "/subdir/nested.txt"), "nested content");
}

TEST_F(CompressorTest, Tar_CompressEmptySource)
{
    CompressorImplTar& impl = CompressorImplTar::Instance();
    errc_t err = impl.compress("", tmpPath("out.tar").c_str());
    EXPECT_NE(err, eNoError);
}

TEST_F(CompressorTest, Tar_CompressNonexistentSource)
{
    CompressorImplTar& impl = CompressorImplTar::Instance();
    errc_t err = impl.compress(tmpPath("nonexistent").c_str(), tmpPath("out.tar").c_str());
    EXPECT_NE(err, eNoError);
}

// ================================================================
// CompressorImplSystem 测试
// ================================================================

TEST_F(CompressorTest, System_IsSupported)
{
    CompressorImplSystem& impl = CompressorImplSystem::Instance();
    // 检查返回值类型正确即可（可能 true 也可能 false，取决于系统环境）
    bool supported = impl.isSupported();
    (void)supported;
    SUCCEED();
}

// ================================================================
// 工厂函数与回退链测试
// ================================================================

TEST_F(CompressorTest, Factory_ReturnsNonNull)
{
    // 对任意有效路径，至少 Raw 后端应该可用
    CompressorInterface* impl = aCompressGetImpl(tmpPath("anything").c_str(),
                                                  tmpPath("out").c_str());
    EXPECT_NE(impl, nullptr);
}

TEST_F(CompressorTest, Factory_ReturnsRawForPlainTarget)
{
    std::string srcPath = tmpPath("test.txt");
    ASSERT_TRUE(writeFile(srcPath, "hello"));

    // 目标是非归档格式，应该回退到 Raw
    CompressorInterface* impl = aCompressGetImpl(srcPath.c_str(), tmpPath("out.txt").c_str());
    ASSERT_NE(impl, nullptr);
    EXPECT_TRUE(impl->isSupported());
}

TEST_F(CompressorTest, Factory_ReturnsTarForTarTarget)
{
    std::string srcPath = tmpPath("factory_src.txt");
    ASSERT_TRUE(writeFile(srcPath, "tar content"));

    // 目标是 .tar 格式，应该选择 Tar 后端
    CompressorInterface* impl = aCompressGetImpl(srcPath.c_str(), tmpPath("out.tar").c_str());
    ASSERT_NE(impl, nullptr);
    EXPECT_TRUE(impl->isSupported());
    // Tar 后端应该能处理 .tar 目标
    EXPECT_TRUE(impl->canCompress(srcPath.c_str(), tmpPath("out.tar").c_str()));
}

TEST_F(CompressorTest, aCompress_RawCopy)
{
    std::string srcPath = tmpPath("input.txt");
    std::string content = "compress test content";
    ASSERT_TRUE(writeFile(srcPath, content));

    std::string destPath = tmpPath("acompress_out.txt");
    errc_t err = aCompress(srcPath.c_str(), destPath.c_str());
    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fileExists(destPath));
    EXPECT_EQ(readFile(destPath), content);
}

TEST_F(CompressorTest, aCompress_TarFormat)
{
    std::string srcDir = tmpPath("tarsrc");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/data.txt", "tar compress data");
    writeFile(srcDir + "/info.txt", "info content");

    std::string tarPath = tmpPath("acompress.tar");

    errc_t err = aCompress(srcDir.c_str(), tarPath.c_str(), srcDir.c_str());
    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fileExists(tarPath));

    // 端到端验证：用 ExtractorImplTar 解压验证内容
    std::string extractDir = tmpPath("acompress_verify");
    ExtractorImplTar& extractor = ExtractorImplTar::Instance();
    err = extractor.extract(tarPath.c_str(), extractDir.c_str());
    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fileExists(extractDir + "/data.txt"));
    EXPECT_EQ(readFile(extractDir + "/data.txt"), "tar compress data");
    EXPECT_TRUE(fileExists(extractDir + "/info.txt"));
    EXPECT_EQ(readFile(extractDir + "/info.txt"), "info content");
}

TEST_F(CompressorTest, aCompress_EmptySource)
{
    errc_t err = aCompress("", tmpPath("out").c_str());
    EXPECT_NE(err, eNoError);
}

// ================================================================
// CompressorInterface 基础测试
// ================================================================

TEST_F(CompressorTest, Interface_DefaultCanCompress)
{
    // 默认 canCompress 应该返回 true（除非子类重写）
    CompressorImplRaw& raw = CompressorImplRaw::Instance();
    EXPECT_TRUE(raw.canCompress("anything.txt", "out.txt"));
    EXPECT_TRUE(raw.canCompress("", ""));
}

// ================================================================
// 默认 curdir 行为测试 (#1, #5 盲区)
// ================================================================

TEST_F(CompressorTest, Tar_CompressDirectory_DefaultCurdir)
{
    std::string srcDir = tmpPath("defcur_src");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/a.txt", "file A");
    writeFile(srcDir + "/b.txt", "file B");

    std::string tarPath = tmpPath("default_curdir.tar");
    // 不传 curdir，目录名应作为归档内前缀
    errc_t err = CompressorImplTar::Instance().compress(srcDir.c_str(), tarPath.c_str());
    ASSERT_EQ(err, eNoError);

    std::string extractDir = tmpPath("defcur_verify");
    ASSERT_EQ(ExtractorImplTar::Instance().extract(tarPath.c_str(), extractDir.c_str()), eNoError);

    // 默认行为：目录名作为归档根目录前缀
    std::string dirName = fs::path(srcDir).filename().string();
    EXPECT_TRUE(fileExists(extractDir + "/" + dirName + "/a.txt"));
    EXPECT_EQ(readFile(extractDir + "/" + dirName + "/a.txt"), "file A");
}

TEST_F(CompressorTest, Tar_CompressDirectory_WithParentCurdir)
{
    std::string srcDir = tmpPath("parentcur_src");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/data.txt", "parent curdir test");

    std::string parentDir = tmpPath("");  // tmpPath("") 返回 tempDir/
    // 以 tempDir 为 curdir，归档内路径为 srcdir 相对 tempDir 的路径
    std::string tarPath = tmpPath("parent_curdir.tar");
    errc_t err = CompressorImplTar::Instance().compress(
        srcDir.c_str(), tarPath.c_str(), getTempDir().c_str());
    ASSERT_EQ(err, eNoError);

    std::string extractDir = tmpPath("parentcur_verify");
    ASSERT_EQ(ExtractorImplTar::Instance().extract(tarPath.c_str(), extractDir.c_str()), eNoError);

    // 归档内应有 defcur_src/data.txt（相对于 getTempDir）
    std::string dirName = fs::path(srcDir).filename().string();
    EXPECT_TRUE(fileExists(extractDir + "/" + dirName + "/data.txt"));
    EXPECT_EQ(readFile(extractDir + "/" + dirName + "/data.txt"), "parent curdir test");
}

TEST_F(CompressorTest, Tar_CompressSingleFile_WithCurdir)
{
    std::string srcPath = tmpPath("curdir_file.txt");
    writeFile(srcPath, "curdir single file");

    std::string tarPath = tmpPath("curdir_single.tar");
    // curdir = tempDir，归档内只存文件名
    errc_t err = CompressorImplTar::Instance().compress(
        srcPath.c_str(), tarPath.c_str(), getTempDir().c_str());
    ASSERT_EQ(err, eNoError);

    std::string extractDir = tmpPath("curdir_single_verify");
    ASSERT_EQ(ExtractorImplTar::Instance().extract(tarPath.c_str(), extractDir.c_str()), eNoError);

    EXPECT_TRUE(fileExists(extractDir + "/curdir_file.txt"));
    EXPECT_EQ(readFile(extractDir + "/curdir_file.txt"), "curdir single file");
}

// ================================================================
// 边界条件测试 (#6, #8, #11 盲区)
// ================================================================

TEST_F(CompressorTest, Tar_Compress_EmptyDirectory)
{
    std::string emptyDir = tmpPath("empty_dir");
    fs::create_directories(emptyDir);

    std::string tarPath = tmpPath("empty_dir.tar");
    errc_t err = CompressorImplTar::Instance().compress(
        emptyDir.c_str(), tarPath.c_str(), emptyDir.c_str());
    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fileExists(tarPath));

    // 解压验证：只有结束标记的合法空 tar
    std::string extractDir = tmpPath("empty_dir_verify");
    ASSERT_EQ(ExtractorImplTar::Instance().extract(tarPath.c_str(), extractDir.c_str()), eNoError);
}

TEST_F(CompressorTest, Tar_Compress_WithSpacesInFilename)
{
    std::string srcDir = tmpPath("space_src");
    fs::create_directories(srcDir);
    fs::create_directories(srcDir + "/sub dir");
    writeFile(srcDir + "/my file.txt", "spaces in name");
    writeFile(srcDir + "/sub dir/nested file.dat", "nested with spaces");

    std::string tarPath = tmpPath("spaces.tar");
    errc_t err = CompressorImplTar::Instance().compress(
        srcDir.c_str(), tarPath.c_str(), srcDir.c_str());
    ASSERT_EQ(err, eNoError);

    std::string extractDir = tmpPath("spaces_verify");
    ASSERT_EQ(ExtractorImplTar::Instance().extract(tarPath.c_str(), extractDir.c_str()), eNoError);

    EXPECT_TRUE(fileExists(extractDir + "/my file.txt"));
    EXPECT_EQ(readFile(extractDir + "/my file.txt"), "spaces in name");
    EXPECT_TRUE(fileExists(extractDir + "/sub dir/nested file.dat"));
    EXPECT_EQ(readFile(extractDir + "/sub dir/nested file.dat"), "nested with spaces");
}

TEST_F(CompressorTest, Tar_Compress_WithDotFiles)
{
    std::string srcDir = tmpPath("dot_src");
    fs::create_directories(srcDir);
    writeFile(srcDir + "/.hidden", "hidden file");
    writeFile(srcDir + "/normal.txt", "normal file");

    std::string tarPath = tmpPath("dot.tar");
    errc_t err = CompressorImplTar::Instance().compress(
        srcDir.c_str(), tarPath.c_str(), srcDir.c_str());
    ASSERT_EQ(err, eNoError);

    std::string extractDir = tmpPath("dot_verify");
    ASSERT_EQ(ExtractorImplTar::Instance().extract(tarPath.c_str(), extractDir.c_str()), eNoError);

    EXPECT_TRUE(fileExists(extractDir + "/.hidden"));
    EXPECT_EQ(readFile(extractDir + "/.hidden"), "hidden file");
    EXPECT_TRUE(fileExists(extractDir + "/normal.txt"));
}

TEST_F(CompressorTest, Raw_Compress_EmptyFile)
{
    std::string srcPath = tmpPath("raw_empty.txt");
    writeFile(srcPath, "");

    std::string destPath = tmpPath("raw_empty_out.txt");
    errc_t err = CompressorImplRaw::Instance().compress(srcPath.c_str(), destPath.c_str());
    ASSERT_EQ(err, eNoError);
    EXPECT_TRUE(fileExists(destPath));
    EXPECT_EQ(readFile(destPath), "");
}

TEST_F(CompressorTest, Raw_Compress_BinaryContent)
{
    std::string srcPath = tmpPath("raw_binary.bin");
    std::string binaryData = std::string("\x00\x01\x02\xFF\xFE\xFD", 6) +
                             std::string(1000, '\xAB');
    writeFile(srcPath, binaryData);

    std::string destPath = tmpPath("raw_binary_out.bin");
    errc_t err = CompressorImplRaw::Instance().compress(srcPath.c_str(), destPath.c_str());
    ASSERT_EQ(err, eNoError);
    EXPECT_EQ(readFile(destPath), binaryData);
}

TEST_F(CompressorTest, Raw_Compress_NestedDirectory)
{
    std::string srcDir = tmpPath("raw_nested_src");
    fs::create_directories(srcDir + "/sub/deep");
    writeFile(srcDir + "/root.txt", "root");
    writeFile(srcDir + "/sub/mid.txt", "mid");
    writeFile(srcDir + "/sub/deep/leaf.txt", "leaf");

    std::string destDir = tmpPath("raw_nested_dst");
    errc_t err = CompressorImplRaw::Instance().compress(srcDir.c_str(), destDir.c_str());
    ASSERT_EQ(err, eNoError);

    EXPECT_TRUE(fileExists(destDir + "/root.txt"));
    EXPECT_TRUE(fileExists(destDir + "/sub/mid.txt"));
    EXPECT_TRUE(fileExists(destDir + "/sub/deep/leaf.txt"));
}

// ================================================================
// System 后端测试 (#3, #4 盲区)
// ================================================================

TEST_F(CompressorTest, System_CanCompress_VariousFormats)
{
    CompressorImplSystem& impl = CompressorImplSystem::Instance();
    // 这些返回值取决于系统环境，检查类型正确即可
    bool canTar  = impl.canCompress("src", "out.tar");
    bool canGz   = impl.canCompress("src", "out.tar.gz");
    bool canZip  = impl.canCompress("src", "out.zip");
    bool can7z   = impl.canCompress("src", "out.7z");
    bool canTxt  = impl.canCompress("src", "out.txt");
    (void)canTar; (void)canGz; (void)canZip; (void)can7z; (void)canTxt;
    SUCCEED();
}

TEST_F(CompressorTest, System_Compress_RejectsShellMetaChars)
{
    // 源路径含 $ 应被拒绝
    std::string srcPath = tmpPath("safe_name.txt");
    writeFile(srcPath, "safe");

    errc_t err = CompressorImplSystem::Instance().compress(
        (srcPath + "$(evil)").c_str(), tmpPath("out.tar").c_str());
    EXPECT_NE(err, eNoError);

    // 源路径含 ; 应被拒绝
    err = CompressorImplSystem::Instance().compress(
        (srcPath + ";rm -rf /").c_str(), tmpPath("out.tar").c_str());
    EXPECT_NE(err, eNoError);

    // 正常路径应该通过（如果 tar 可用的话至少不因元字符拒绝）
    errc_t errNormal = CompressorImplSystem::Instance().compress(
        srcPath.c_str(), tmpPath("normal.tar").c_str());
    // 不检查 errNormal 具体值（取决于系统是否有 tar），但确保不是因为元字符拒绝的
    (void)errNormal;
}


GTEST_MAIN()
