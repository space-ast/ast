///
/// @file      AstTestMacro.h
/// @brief     
/// @details   ~
/// @author    axel
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include <cmath>						// for fabs
#include <cstdio>						// for printf
#include <iostream> 					// for std::cout 
#include "AstTest/TestConfig.hpp"		// for test config
#include "AstUtil/Environment.hpp"		// for env check
#ifdef AST_WITH_GTEST
#include <gtest/gtest.h>
#else
#include "AstTest/TestFramework.hpp"		// 内置的 gtest 兼容测试框架
#endif

#define GTEST_MAIN() \
int main(int argc, char **argv) { \
  std::printf("Running main() from %s\n", __FILE__); \
  testing::InitGoogleTest(&argc, argv); \
  return RUN_ALL_TESTS(); \
}

#ifndef GTEST_SKIP
#	define GTEST_SKIP() return GTEST_SUCCESS_("Test skipped")  // 兼容老版本gtest
#endif


