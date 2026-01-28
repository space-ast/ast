///
/// @file      NLESolver.hpp
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
#include "AstMath/Function.hpp"
#include "AstMath/SolverStats.h"


AST_NAMESPACE_BEGIN


constexpr const double kDefaultRelTol = 1e-14;
constexpr const double kDefaultAbsTol = 1e-12;
constexpr const int kDefaultMaxIter = 100;

/// @brief 判断两个数是否接近
/// @param a 第一个数
/// @param b 第二个数
/// @param relTol 相对误差容限
/// @param absTol 绝对误差容限
/// @return 如果两个数接近，则返回true；否则返回false
bool aIsClose(double a, double b, double relTol = kDefaultRelTol, double absTol = kDefaultAbsTol);


/// @brief Lambda函数适配器（模板类）
template<typename Func>
class LambdaUnaryScalarFunc : public UnaryScalarFunc {
public:
    explicit LambdaUnaryScalarFunc(Func func) 
        : func_(std::move(func)) 
    {}
    
    void evaluate(double x, double* out) const override {
        *out = func_(x);
    }
    
private:
    Func func_;
};

/// @brief 一元方程求解器接口
class IUnarySolver
{
public:
    virtual ~IUnarySolver() = default;

    /// @brief 求解一元方程
    /// @param func 方程函数
    /// @param min 搜索下限
    /// @param max 搜索上限
    /// @param result 输出解，长度需大于等于方程维度
    virtual err_t solve(UnaryScalarFunc& func, double min, double max, double& result) = 0;
};


/// @brief 一元方程求解器
class UnarySolver: public IUnarySolver
{
public:
    UnarySolver();
    /// @brief 构造函数
    /// @param relTol 相对误差容限
    /// @param absTol 绝对误差容限
    /// @param maxIter 最大迭代次数
    UnarySolver(double relTol, double absTol, int maxIter);
    ~UnarySolver() override = default;

    /// @brief 获取相对误差容限
    double getRelTol() const{return relTol_;};
    /// @brief 获取绝对误差容限
    double getAbsTol() const{return absTol_;};
    /// @brief 获取最大迭代次数
    int getMaxIter() const{return maxIter_;};

    /// @brief 设置相对误差容限
    void setRelTol(double relTol){relTol_ = relTol;};
    /// @brief 设置绝对误差容限
    void setAbsTol(double absTol){absTol_ = absTol;};
    /// @brief 设置最大迭代次数
    void setMaxIter(int maxIter){maxIter_ = maxIter;};

    /// @brief 获取求解器统计信息
    const SolverStats& getStats() const{return stats_;};

    /// @brief 求解一元方程（支持lambda函数）
    /// @tparam Func lambda或可调用对象类型
    /// @param func lambda函数
    /// @param min 搜索下限
    /// @param max 搜索上限
    /// @param result 输出解
    template<typename Func>
    err_t solveFunc(Func func, double min, double max, double& result) {
        LambdaUnaryScalarFunc<Func> adapter(std::move(func));
        return solve(adapter, min, max, result);
    }
protected:
    static double unarycfunc(double x, void* params);
protected:
    double relTol_;         ///< 相对误差容限
    double absTol_;         ///< 绝对误差容限
    int maxIter_;           ///< 最大迭代次数
    SolverStats stats_;     ///< 求解器统计信息
};

using UnaryScalarSolver = UnarySolver;


AST_NAMESPACE_END
