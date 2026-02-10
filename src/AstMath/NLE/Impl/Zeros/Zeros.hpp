///
/// @file      Zeros.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-29
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
#include "zeros.h"

AST_NAMESPACE_BEGIN

namespace details{
    template<typename Func>
    A_ALWAYS_INLINE double callback_func(double x, void* func)
    {
        return (*(Func*)func)(x);
    }
}

/*!
    @addtogroup NLE
    @{
*/


/// @brief 二分法求解器
/// @tparam Func 函数类型
/// @param f 函数对象
/// @param xa 区间下限
/// @param xb 区间上限
/// @param xtol 绝对误差 tolerance
/// @param rtol 相对误差 tolerance
/// @param iter 最大迭代次数
/// @param solver_stats 求解器统计信息
/// @return 根近似值
template<typename Func>
double bisect(Func f, double xa, double xb, double xtol, double rtol, int iter, scipy_zeros_info &solver_stats)
{
    return bisect(details::callback_func<Func>, xa, xb, xtol, rtol, iter, &f, &solver_stats);
}


/// @brief Ridder 方法求解器
/// @tparam Func 函数类型
/// @param f 函数对象
/// @param xa 区间下限
/// @param xb 区间上限
/// @param xtol 绝对误差 tolerance
/// @param rtol 相对误差 tolerance
/// @param iter 最大迭代次数
/// @param solver_stats 求解器统计信息
/// @return 根近似值
template<typename Func>
double ridder(Func f, double xa, double xb, double xtol, double rtol, int iter, scipy_zeros_info &solver_stats)
{
    return ridder(details::callback_func<Func>, xa, xb, xtol, rtol, iter, &f, &solver_stats);
}


/// @brief Brenth法求解器
/// @tparam Func 函数类型
/// @param f 函数对象
/// @param xa 区间下限
/// @param xb 区间上限
/// @param xtol 绝对误差 tolerance
/// @param rtol 相对误差 tolerance
/// @param iter 最大迭代次数
/// @param solver_stats 求解器统计信息
/// @return 根近似值
template<typename Func>
double brenth(Func f, double xa, double xb, double xtol, double rtol, int iter, scipy_zeros_info &solver_stats)
{
    return brenth(details::callback_func<Func>, xa, xb, xtol, rtol, iter, &f, &solver_stats);
}


/// @brief Brentq法求解器
/// @tparam Func 函数类型
/// @param f 函数对象
/// @param xa 区间下限
/// @param xb 区间上限
/// @param xtol 绝对误差 tolerance
/// @param rtol 相对误差 tolerance
/// @param iter 最大迭代次数
/// @param solver_stats 求解器统计信息
/// @return 根近似值
template<typename Func>
double brentq(Func f, double xa, double xb, double xtol, double rtol, int iter, scipy_zeros_info &solver_stats)
{
    return brentq(details::callback_func<Func>, xa, xb, xtol, rtol, iter, &f, &solver_stats);
}


/// @brief 割线法求解器
/// @tparam Func 函数类型
/// @param f 函数对象
/// @param xa 区间下限
/// @param xb 区间上限
/// @param xtol 绝对误差 tolerance
/// @param rtol 相对误差 tolerance
/// @param iter 最大迭代次数
/// @param solver_stats 求解器统计信息
/// @return 根近似值
template<typename Func>
double secant(Func f, double xa, double xb, double xtol, double rtol, int iter, scipy_zeros_info &solver_stats)
{
    return secant(details::callback_func<Func>, xa, xb, xtol, rtol, iter, &f, &solver_stats);
}


/*! @} */

AST_NAMESPACE_END
