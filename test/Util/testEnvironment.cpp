///
/// @file      testEnvironment.cpp
/// @brief     Environment模块单元测试
/// @details   测试环境检测、终端检测等功能
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

#include "ast/Environment.hpp"
#include "ast/Posix.hpp"
#include "ast/Test.h"
#include <cstdio>

AST_USING_NAMESPACE

// 测试CI环境检测
TEST(Environment, CIEnvironment)
{
    // 测试 aIsGithubCI - 检查是否在GitHub Actions中运行
    bool is_github_ci = aIsGithubCI();
    printf("Is GitHub CI: %s\n", is_github_ci ? "true" : "false");
    
    // 测试 aIsGitlabCI - 检查是否在GitLab CI中运行
    bool is_gitlab_ci = aIsGitlabCI();
    printf("Is GitLab CI: %s\n", is_gitlab_ci ? "true" : "false");
    
    // 测试 aIsCI - 综合CI检测
    bool is_ci = aIsCI();
    printf("Is CI: %s\n", is_ci ? "true" : "false");
}

// 测试项目名称获取
TEST(Environment, ProjectName)
{
    StringView project_name = aProjectName();
    printf("Project name: %.*s\n", (int)project_name.size(), project_name.data());
    EXPECT_FALSE(project_name.empty());
}

// 测试标准输入终端检测
TEST(Environment, StdInTerminal)
{
    bool is_terminal = aStdInIsTerminal();
    bool is_file = aStdInIsFile();
    printf("stdin is terminal: %s\n", is_terminal ? "true" : "false");
    printf("stdin is file: %s\n", is_file ? "true" : "false");
    // stdin通常是终端或管道，不是普通文件
}

// 测试标准输出终端检测
TEST(Environment, StdOutTerminal)
{
    bool is_terminal = aStdOutIsTerminal();
    bool is_file = aStdOutIsFile();
    printf("stdout is terminal: %s\n", is_terminal ? "true" : "false");
    printf("stdout is file: %s\n", is_file ? "true" : "false");
    // stdout通常是终端或管道
}

// 测试标准错误终端检测
TEST(Environment, StdErrTerminal)
{
    bool is_terminal = aStdErrIsTerminal();
    bool is_file = aStdErrIsFile();
    printf("stderr is terminal: %s\n", is_terminal ? "true" : "false");
    printf("stderr is file: %s\n", is_file ? "true" : "false");
    // stderr通常是终端
}

// 测试FILE指针终端检测
TEST(Environment, FILEIsTerminal)
{
    // 测试标准输出
    bool stdout_terminal = aIsTerminal(stdout);
    printf("stdout is terminal: %s\n", stdout_terminal ? "true" : "false");
    
    // 测试标准错误
    bool stderr_terminal = aIsTerminal(stderr);
    printf("stderr is terminal: %s\n", stderr_terminal ? "true" : "false");
    
    // 测试空指针
    bool null_terminal = aIsTerminal(nullptr);
    EXPECT_FALSE(null_terminal);
}

// 测试FILE指针普通文件检测
TEST(Environment, FILEIsFile)
{
    // 测试标准输入
    bool stdin_file = aIsFile(stdin);
    printf("stdin is file: %s\n", stdin_file ? "true" : "false");
    
    // 测试空指针
    bool null_file = aIsFile(nullptr);
    EXPECT_FALSE(null_file);
}


GTEST_MAIN()
