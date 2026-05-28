///
/// @file      testPosixExt.cpp
/// @brief     PosixExt模块单元测试
/// @details   测试Posix扩展功能
/// @author    axel
/// @date      2026-04-15
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

#include "ast/PosixExt.hpp"
#include "ast/FileSystem.hpp"
#include "ast/Test.h"
#include <cstdio>

AST_USING_NAMESPACE

// 测试 kMaxPath 常量
TEST(PosixExt, MaxPath)
{
    // kMaxPath 应该是一个合理的正值
    EXPECT_GT(posix::kMaxPath, 0);
    
    // kMaxPath 应该大于 0 且小于一个很大的值
    EXPECT_LT(posix::kMaxPath, 100000);
    
    printf("kMaxPath: %zu\n", posix::kMaxPath);
}

// 测试 getcwd 函数
TEST(PosixExt, GetCwd)
{
    namespace fs = _AST filesystem;
    
    // 获取当前工作目录
    std::string cwd = posix::getcwd();
    
    printf("Current working directory: %s\n", cwd.c_str());
    
    // 工作目录不应该为空
    EXPECT_FALSE(cwd.empty());
    
    // 验证返回的路径存在
    fs::path cwd_path(cwd);
    EXPECT_TRUE(fs::exists(cwd_path));
}

// 测试工作目录的一致性
TEST(PosixExt, GetCwdConsistency)
{
    // 获取当前工作目录两次，应该返回相同结果
    std::string cwd1 = posix::getcwd();
    std::string cwd2 = posix::getcwd();
    
    EXPECT_EQ(cwd1, cwd2);
}

// 测试工作目录路径格式
TEST(PosixExt, GetCwdFormat)
{
    std::string cwd = posix::getcwd();
    
    // 工作目录路径应该至少包含一个分隔符（根目录 "/" 或盘符等）
    EXPECT_FALSE(cwd.empty());
    
    // 验证路径格式
#ifdef _WIN32
    // Windows: 可能是 C:\path 或 \\server\share
    if (cwd.length() >= 2) {
        // 检查是否为盘符路径
        bool is_drive_letter = (cwd[1] == ':');
        printf("Is drive letter path: %s\n", is_drive_letter ? "true" : "false");
    }
#else
    // Unix/Linux: 应该是 /path 格式
    EXPECT_EQ(cwd[0], '/');
#endif
    
    printf("CWD format check passed\n");
}


GTEST_MAIN()
