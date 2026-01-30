///
/// @file      NLPProblem.hpp
/// @brief     非线性规划问题与相关实用函数
/// @details   ~
/// @author    axel
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
#include "INLPProblem.hpp"

AST_NAMESPACE_BEGIN
 

//ptr2fnz：pointer to first none-zero


AST_OPT_CAPI void aPrintSparityPatternCOO(int nnz, int* iFun, int* jVar, int idx_style);


AST_OPT_CAPI void aPrintSparityPatternCCS();


AST_OPT_CAPI void aParternCOOToCCS(
    const std::vector<int>& iRow, const std::vector<int>& jCol,
    std::vector<int>& ptr2fnz, std::vector<int>& indexRow
);


AST_OPT_CAPI void aParternCCSToCOO(
    const std::vector<int>& ptr2fnz, const std::vector<int>& indexRow,
    std::vector<int>& iFunRow, std::vector<int>& jVarCol
);





/// @brief 非线性规划问题与相关函数
class AST_OPT_API NLPProblem
{
public:
    explicit NLPProblem(INLPProblem* problem);

    ~NLPProblem();

    /// @brief 获取问题的维度信息等
    err_t getInfo(NLPInfo& info) const;

    /// @brief 获取问题的雅可比矩阵信息
    err_t getJacInfo(NLPJacInfo& info) const;
    
    /// @brief 获取问题的设计变量与约束的上下界
    err_t getBounds(NLPBounds& bounds) const;

    /// @biref 计算目标函数和约束
    err_t evalFitness(const NLPInput& input, NLPOutput& output) const;

    /// @brief 计算目标函数与约束
    /// @param x 设计变量
    /// @return 目标函数 + 等式约束 + 不等式约束
    std::vector<double> evalFitness(const std::vector<double>& x) const;


    /// @brief 计算目标函数与约束
    /// @param x 设计变量
    /// @param f 目标函数 + 等式约束 + 不等式约束
    /// @return 错误码
    err_t evalFitness(const std::vector<double>& x, std::vector<double>& f) const;
    

    /// @brief 计算目标函数与约束
    /// @param numVariable 
    /// @param variable 
    /// @param numObjective 
    /// @param objective 
    /// @param numConstrEq 
    /// @param constrEq 
    /// @param numConstrIneq 
    /// @param constrIneq 
    /// @return 
    err_t evalFitness(
        int numVariable, double* variable,
        int numObjective, double* objective,
        int numConstrEq, double* constrEq,
        int numConstrIneq, double* constrIneq
    ) const;

    /// @brief 计算非线性方程的解析(数值)Jacobi矩阵
    /// @param ndim          维度   
    /// @param x_input       变量
    /// @param nzElemjacobi  jacobi矩阵非零元素
    err_t evalJacobi(int numVariable, const double* variable, int numConstraint, int nnzJacobi, double* nzElemjacobi) const;


    /// @brief 计算目标函数的梯度向量
    /// @param numVariable 
    /// @param variable 
    /// @return 
    err_t evalGradient(int numVariable, const double* variable, double* grad) const;


    /// @brief 获取初始猜测值(初值/起始点)
    err_t getInitialGuess(int numVariable, double* variable) const;


public:
    /// @brief 计算约束
    /// @param variable 设计变量 
    /// @return 不等式约束 + 等式约束
    std::vector<double> evalConstraint(const std::vector<double>& variable) const;
    
    /// @brief 计算约束
    /// @param variable 设计变量
    /// @param constraint 不等式约束 + 等式约束
    /// @return 
    err_t evalConstraint(const std::vector<double>& variable, std::vector<double>& constraint) const;

    /// @brief 计算约束
    err_t evalConstraint(int numVariable, const double* variable, int numConstraint, double* contraint) const;
    
    /// @brief 计算目标函数
    err_t evalObjective(int numVariable, const double* variable, int numObjective, double* objective) const;

    /// @brief 计算目标函数
    err_t evalObjective(int numVariable, const double* variable, double& objective) const;



public:
    /// 一些实用函数


    /// @brief 中心差分求解梯度.
    err_t evalGradientCD(double ustep, int ndim, const double* x, double* grad) const;

    /// @brief 前向差分求解梯度.
    err_t evalGradientFD(double ustep, int ndim, const double* x, double* grad) const;

    /// @brief 后向差分求解梯度.
    err_t evalGradientBD(double ustep, int ndim, const double* x, double* grad) const;

    /// @brief 中心差分求解jacobi矩阵
    err_t evalNLEJacobiCD(double ustep, int ndim, const double* x_input, int nConstraint, double* colmaj_jacobi) const;
    
    /// @brief 前向差分求解jacobi矩阵
    err_t evalNLEJacobiFD(double ustep, int ndim, const double* x_input, int nConstraint, double* colmaj_jacobi) const;

    /// @brief 后向差分求解jacobi矩阵
    err_t evalNLEJacobiBD(double ustep, int ndim, const double* x_input, int nConstraint, double* colmaj_jacobi) const;

    /// @brief 根据输入的CCS稀疏模式计算Jacobi计算
    /// @param ndim 
    /// @param ustep 
    /// @param xinput 
    /// @param iFuncRow 
    /// @param idxNNZElem 
    /// @param jacobi_sparse_value 大小至少为nnz，即不小于idxNNZElem[ndim]
    /// @return 
    err_t evalNLEJacobiCCSFD(double ustep, int ndim, const double* xinput, int m, const int* iFuncRow, const int* idxNNZElem, double* jacobi_sparse_value) const;

    /// @brief 输出CCS格式描述的稀疏模式
    /// @param ndim 
    /// @param x_initguess 
    /// @param iFunRow 每个非零元素的行索引，元素按照列主排序
    /// @param idxNNZElem 每一列第一个元素的索引
    /// @return 
    err_t evalNLENNZJacCCSNan(int ndim, const double* x_initguess, int m, std::vector<int>& iFunRow, std::vector<int>& idxNNZElem) const;
    
    /// @brief 计算jacobi矩阵中的非零元素数量
    /// @param ndim 
    /// @param x_initguess 
    /// @param m 
    /// @param nnz_jac 
    /// @return 
    err_t evalNLENNZJacNan(int ndim, const double* x_initguess, int m, int& nnz_jac) const;
    
    /// @brief 利用IEEE-754标准的NaN传播规则计算jacobi矩阵的稀疏模式(编译时浮点模式不能为fast)
    ///        需要x的init guess值作为输入，已获得合法解/防止非法输入产生意外的NaN
    ///        输出COO格式描述的稀疏模式
    /// @param iFunRow 行索引
    /// @param jVarCol 列索引
    /// @return false: 用户要求停止
    err_t evalNLENNZJacCOONan(int ndim, const double* x_initguess, int m, std::vector<int>& iFunRow, std::vector<int>& jVarCol) const;


    /// @brief 使用snopt的snjac函数计算Jacobi矩阵的稀疏模式
    /// 输出COO格式描述的稀疏模式
    /// @param ndim 
    /// @param x_initguess 
    /// @param m 
    /// @param iFunRow 
    /// @param jVarCol 
    /// @return 
    err_t evalNLENNZJacCOOSnJac(int ndim, const double* x_initguess, int m, std::vector<int>& iFunRow, std::vector<int>& jVarCol);
protected:
    A_DISABLE_COPY(NLPProblem);
protected:
    INLPProblem* m_problem;     // 非线性规划问题描述接口
    NLPInfo      m_probInfo;    // 非线性规划问题维度信息缓存
};





AST_NAMESPACE_END
