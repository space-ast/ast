///
/// @file      FreeReturnProblem.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstOpt/INLPProblem.hpp"

AST_NAMESPACE_BEGIN

class FreeReturnProblem: public INLPProblem
{
public:
/// @brief 获取问题的维度信息等
    err_t getInfo(NLPInfo& info) const override;

    /// @brief 获取问题的雅可比矩阵信息
    err_t getJacInfo(NLPJacInfo& info) const override;

    /// @brief 获取问题的设计变量与约束的上下界
    err_t getBounds(NLPBounds& bounds) const override;


    /// @brief 计算目标函数和约束
    err_t evalFitness(const NLPInput& input, NLPOutput& output) const override;


    /// @brief 计算非线性方程的解析(数值)Jacobi矩阵
    /// @param ndim          维度   
    /// @param x_input       变量
    /// @param nzElemjacobi  jacobi矩阵非零元素
    err_t evalJacobi(int numVariable, const double* variable, int numConstraint, int nnzJacobi, double* nzElemjacobi) const override;


    /// @brief 计算目标函数梯度
    /// @param numVariable 
    /// @param variable 
    /// @return 
    err_t evalGradient(int numVariable, const double* variable, double* grad) const override;


    /// @brief 获取初始猜测值(迭代初值/迭代起始点)
    err_t getInitialGuess(int numVariable, double* variable) const override;
protected:

};


AST_NAMESPACE_END
