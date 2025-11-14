///
/// @file      AstTestMacro.h
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 根据 Apache 许可证 2.0 版本（"许可证"）授权；
/// 除非符合许可证，否则不得使用此文件。
/// 您可以在以下网址获取许可证副本：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 除非适用法律要求或书面同意，按"原样"分发的软件
/// 没有任何明示或暗示的担保或条件。
/// 请参阅许可证了解特定语言的权限和限制。
 
#pragma once
 
#include "AstGlobal.hpp"
#include <math.h>
 


#define ASSERT_EQ(val1, val2)   \
if((val1) != (val2))            \
return -1;                      \


#define ASSERT_NEAR(val1, val2, eps)    \
if( fabs((val1) - (val2)) > fabs(eps))  \
return -1;                              \


