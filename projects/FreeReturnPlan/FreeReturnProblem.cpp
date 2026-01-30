///
/// @file      FreeReturnProblem.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-30
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

#include "FreeReturnProblem.hpp"
#include "FreeReturnPlan.hpp"
#include <cstdio>


err_t FreeReturnProblem::getInfo(NLPInfo &info) const
{
    info.numConstraintEq() = 3;
    info.numConstraintIneq() = 0;
    info.numVariable() = 3;
    return err_t();
}

err_t FreeReturnProblem::getJacInfo(NLPJacInfo &info) const
{
    return err_t();
}

err_t FreeReturnProblem::getBounds(NLPBounds &bounds) const
{
    return err_t();
}

err_t FreeReturnProblem::evalFitness(const NLPInput &input, NLPOutput &output) const
{
    freeRreturnTargetFunction(input.variable().data(), output.constraintEq().data());
    printf("var: %lf, %lf, %lf\n", input.variable()[0], input.variable()[1], input.variable()[2]);
    printf("con: %lf, %lf, %lf\n", output.constraintEq()[0], output.constraintEq()[1], output.constraintEq()[2]);

    return err_t();
}

err_t FreeReturnProblem::evalJacobi(int numVariable, const double *variable, int numConstraint, int nnzJacobi, double *nzElemjacobi) const
{
    return err_t();
}

err_t FreeReturnProblem::evalGradient(int numVariable, const double *variable, double *grad) const
{
    return err_t();
}

err_t FreeReturnProblem::getInitialGuess(int numVariable, double *variable) const
{
    variable[0] = 0.0;
    variable[1] = 0.0;
    variable[2] = 0.0;
    return err_t();
}


