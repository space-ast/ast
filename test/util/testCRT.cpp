///
/// @file      testCRT.cpp
/// @brief     CRT模块单元测试
/// @details   测试C运行时封装功能
/// @author    axel
/// @date      2026-04-15
/// @copyright 版权所有 (C) 2026-present, ast项目.

#include "ast/Test.h"

AST_USING_NAMESPACE

// CRT模块当前为空实现，跳过测试
TEST(CRT, Skipped)
{
    GTEST_SKIP() << "CRT module not implemented yet";
}

GTEST_MAIN()
