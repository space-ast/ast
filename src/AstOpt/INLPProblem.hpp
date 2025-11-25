///
/// @file      INLPProblem.hpp
/// @brief     非线性规划问题描述接口
/// @details   ~
/// @author    jinke18
/// @date      16.11.2025
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
#include <vector>


AST_NAMESPACE_BEGIN



/// @brief 非线性规划问题的维度信息
struct NLPInfo
{
    int  numVariable{ 0 };           // 优化变量的维度，默认为0
    int  numConstraintEq{ 0 };       // 等式约束的维度，默认为0
    int  numConstraintIneq{ 0 };     // 不等式约束维度，默认为0
    int  numObjective{ 0 };          // 优化目标的维度，默认为0


    int getNumConstraint() const
    {
        return getNumConstraintEq() + getNumConstraintIneq();
    }
    int getNumObjConstr() const
    {
        return getNumObjective() + getNumConstraint();
    }
    int getNumVariable() const
    {
        return numVariable;
    }
    int getNumObjective() const
    {
        return numObjective;
    }
    int getNumConstraintEq() const
    {
        return numConstraintEq;
    }
    int getNumConstraintIneq() const
    {
        return numConstraintIneq;
    }
};


// @brief 非线性规划问题的雅可比矩阵信息 
struct NLPJacInfo
{
    std::vector<int> idxVariable;       // 变量索引，从0开始
    std::vector<int> idxConstraint;     // 约束索引，从0开始
};

/// @brief 非线性规划问题的向量上下界
struct NLPVectorBounds
{
    int     size{ 0 };              // 维度
    double* lower{ nullptr };       // 下界
    double* upper{ nullptr };       // 上界
};

/// @brief 非线性规划问题的向量值
struct NLPVector
{
    double* value{ nullptr };         // 数值
    int     size{ 0 };                // 维度
};

/// @brief 非线性规划问题的上下界值
struct NLPBounds
{
    NLPVectorBounds variable;        // 优化变量的上下界，不设置则默认是  -∞ <= variable <= +∞
    NLPVectorBounds constraintIneq;  // 不等式约束上下界，不设置则默认是  -∞ <= constrIneq <= 0
    NLPVector       constraintEq;    // 等式约束的期望值，不设置则默认是 0
};

/// @brief 非线性规划问题输入
struct NLPInput
{
    NLPVector variable;
};

/// @brief 非线性规划问题输出
struct NLPOutput
{
    NLPVector objective;
    NLPVector constraintEq;
    NLPVector constraintIneq;
};


class INLPProblem
{
public:
    /// @brief 虚析构函数
    virtual ~INLPProblem(){}

    /// @brief 获取问题的维度信息等
    virtual err_t getInfo(NLPInfo& info) const = 0;

    /// @brief 获取问题的雅可比矩阵信息
    virtual err_t getJacInfo(NLPJacInfo& info) const = 0;

    /// @brief 获取问题的设计变量与约束的上下界
    virtual err_t getBounds(NLPBounds& bounds) const = 0;


    /// @biref 计算目标函数和约束
    virtual err_t evalFitness(const NLPInput& input, NLPOutput& output) const = 0;


    /// @brief 计算非线性方程的解析(数值)Jacobi矩阵
    /// @param ndim          维度   
    /// @param x_input       变量
    /// @param nzElemjacobi  jacobi矩阵非零元素
    virtual err_t evalJacobi(int numVariable, const double* variable, int numConstraint, int nnzJacobi, double* nzElemjacobi) const = 0;


    /// @brief 计算目标函数梯度
    /// @param numVariable 
    /// @param variable 
    /// @return 
    virtual err_t evalGradient(int numVariable, const double* variable, double* grad) const = 0;


    /// @brief 获取初始猜测值(迭代初值/迭代起始点)
    virtual err_t getInitialGuess(int numVariable, double* variable) const = 0;


};

 
AST_NAMESPACE_END
 
