///
/// @file      Function.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-25
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

#pragma once

#include "AstGlobal.h"
#include "UnaryScalarFunc.hpp"
#include "UnaryVectorFunc.hpp"
#include "ScalarFunc.hpp"
#include "VectorFunc.hpp"


AST_NAMESPACE_BEGIN

/*
 * 含义解释：
 * 1. 一元：指函数只有一个变量，如 f(x) = x^2
 * 2. 多元：指函数有多个变量，如 f(x, y) = x^2 + y^2
 * 3. 标量：指函数的返回值是一个标量，如 f(x) = x^2
 * 4. 向量：指函数的返回值是一个向量，如 f(x) = [x, x^2]
 * 
 * 单词解释：
 * unary/univariate 一元
 * multivariate  多元
 * scalar 标量
 * vector 向量
 * 
 * 类型命名规则：
 * 1. 一元标量值函数：UnaryScalarFunc  (一元函数/标量函数)
 * 2. 一元向量值函数：UnaryVectorFunc 
 * 3. 多元标量值函数：MultivariateScalarFunc 
 * 4. 多元向量值函数：MultivariateVectorFunc 
 * 
 * 注意，在**不严格的定义**下，多元可以涵盖一元，向量可以涵盖标量。
 * 
 */

AST_NAMESPACE_END
