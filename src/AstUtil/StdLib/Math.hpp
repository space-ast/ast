///
/// @file      Math.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-12
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
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include <math.h>

AST_NAMESPACE_BEGIN


#ifdef A_GCC    //  GCC 内建 sincos 函数
// #pragma message("Using GCC built-in sincos function")
using ::sincos;

#else

A_ALWAYS_INLINE void sincos (double x, double *psin, double *pcos)
{
    *psin = sin(x);
    *pcos = cos(x);
}

#endif

AST_NAMESPACE_END
