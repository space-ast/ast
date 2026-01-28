///
/// @file      UnarySolver.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-27
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

#include "UnarySolver.hpp"
#include "AstMath/Function.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

bool aIsClose(double a, double b, double relTol, double absTol)
{
    double delta = fabs(a - b);
    return delta <= absTol || delta <= relTol*fabs(a) || delta <= relTol*fabs(b);
}


UnarySolver::UnarySolver()
    : relTol_(kDefaultRelTol), absTol_(kDefaultAbsTol), maxIter_(kDefaultMaxIter)
{
}

UnarySolver::UnarySolver(double relTol, double absTol, int maxIter)
    : relTol_(relTol), absTol_(absTol), maxIter_(maxIter)
{
}

UnarySolver::UnarySolver(double absTol)
    : relTol_(kDefaultRelTol), absTol_(absTol), maxIter_(kDefaultMaxIter)
{
}

double UnarySolver::unarycfunc(double x, void* params)
{
    UnaryScalarFunc* func = (UnaryScalarFunc*)params;
    double result;
    func->evaluate(x, &result);
    return result;
}

AST_NAMESPACE_END

