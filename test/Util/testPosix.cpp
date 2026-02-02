///
/// @file      testPosix.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-02-02
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstUtil/Posix.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE 



TEST(PosixTest, isdir)
{
    struct posix::stat st;
    ASSERT_EQ(posix::fstat(0, &st), 0);
    ASSERT_FALSE(posix::isdir(st));
}

TEST(PosixTest, unicode)
{
    int rc;
    fs::path unicode_dir = u8"./测试目录_for_posix_Unicode";
    EXPECT_TRUE(fs::create_directory(unicode_dir));
    EXPECT_TRUE(fs::exists(unicode_dir));
    EXPECT_TRUE(fs::is_directory(unicode_dir));
    rc = posix::chdir(unicode_dir.string().c_str());
    EXPECT_EQ(rc, 0);

    char buf[256];
    char* cwd = posix::getcwd(buf, 256);
    posix::printf("cwd: %s\n", cwd);
    rc = posix::chdir(cwd);
    EXPECT_EQ(rc, 0);

    rc = posix::chdir("..");
    EXPECT_EQ(rc, 0);

    rc = posix::rmdir(unicode_dir.string().c_str());
    EXPECT_EQ(rc, 0);
    EXPECT_FALSE(fs::exists(unicode_dir));
    EXPECT_FALSE(fs::is_directory(unicode_dir));
}


GTEST_MAIN()

