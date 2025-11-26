///
/// @file      AstTestMacro.h
/// @brief     
/// @details   ~
/// @author    jinke18
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
#include <math.h>	// for fabs
#include <stdio.h>	// for printf
#include <iostream> // for std::cout 
#include <gtest/gtest.h>


#define GTEST_MAIN() \
int main(int argc, char **argv) { \
  printf("Running main() from %s\n", __FILE__); \
  testing::InitGoogleTest(&argc, argv); \
  return RUN_ALL_TESTS(); \
}


#if !defined GTEST_API_ && 0


#define ASSERT_EQ(val1, val2)\
if((val1) != (val2))\
{\
	printf("\nat file %s(%d)\n", __FILE__, __LINE__);\
	std::cout<< "val1" << #val1 << " = " << val1 << std::endl;\
	std::cout<< "val2" << #val2 << " = " << val2 << std::endl;\
	return -1;\
}


#define ASSERT_NEAR(val1, val2, eps)\
if( fabs((val1) - (val2)) > fabs(eps))\
{\
	printf("\nat file %s(%d)\n", __FILE__, __LINE__);\
	std::cout<< "val1 " << #val1 << " = " << val1 << std::endl;\
	std::cout<< "val2 " << #val2 << " = " << val2 << std::endl;\
	std::cout<< "eps" << #eps << " = " << eps << std::endl;\
	std::cout<< "delta" << " = " << fabs(val1 - val2) << std::endl;\
	return -1;\
}

#define ASSERT_FALSE(val)\
if((val))\
{\
	printf("\nat file %s(%d)\n", __FILE__, __LINE__);\
	std::cout<< "val " << #val << " = " << val << std::endl;\
	return -1;\
}


#define ASSERT_TRUE(val)\
if(!(val))\
{\
	printf("\nat file %s(%d)\n", __FILE__, __LINE__);\
	std::cout<< "val " << #val << " = " << (val) << std::endl;\
	return -1;\
}

#endif

