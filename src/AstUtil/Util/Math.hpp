///
/// @file      Math.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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
#include "AstUtil/MathDegree.hpp"
#include "AstUtil/Constants.h"
#include <cmath>
#include <algorithm>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Util
	@{
*/


#ifdef A_GCC    //  GCC 内建 sincos 函数
// #pragma message("Using GCC built-in sincos function")
using ::sincos;

#else

/// @brief 计算 sin(x) 和 cos(x) 值
/// @param x 输入角度（弧度）
/// @param psin 指向 sin(x) 值的指针
/// @param pcos 指向 cos(x) 值的指针
A_ALWAYS_INLINE void sincos (double x, double *psin, double *pcos)
{
    *psin = sin(x);
    *pcos = cos(x);
}

#endif


/// @brief 包含 sin(x) 和 cos(x) 值的结构体
class SinCos {
public:
    A_ALWAYS_INLINE
    double sin() const { return sin_; }
    A_ALWAYS_INLINE
    double& sin() { return sin_; }
    A_ALWAYS_INLINE
    double cos() const { return cos_; }
    A_ALWAYS_INLINE
    double& cos() { return cos_; }
public:
    double sin_;
    double cos_;
};


/// @brief 计算 sin 和 cos 值
/// @param x 输入角度（弧度）
/// @return SinCos 对象，包含 sin(x) 和 cos(x) 的值
A_ALWAYS_INLINE SinCos sincos(double x)
{
    SinCos sc;
    sincos(x, &sc.sin_, &sc.cos_);
    return sc;
}



/// @brief 计算两个数中的较大值
template<typename Scalar>
Scalar aMax(Scalar a, Scalar b)
{
    return a > b ? a : b;
}

/// @brief 计算两个数中的较小值
template<typename Scalar>
Scalar aMin(Scalar a, Scalar b)
{
    return a < b ? a : b;
}

using std::max;
using std::min;


/// @brief 将角度量规范化到指定起始范围
/// @param angle 输入角度（弧度）
/// @param start 起始角度（弧度）
/// @return 规范化后的角度 angle - k*2π, 其中k是整数，使得 angle - k*2π >= start
A_ALWAYS_INLINE double aNormalizeAngleStart(double angle, double start)
{
    return angle - floor((angle + start) / kTwoPI) * kTwoPI;
}


/// @brief 将角度量规范化到指定中心范围
/// @param angle 输入角度（弧度）
/// @param center 中心角度（弧度）
/// @return 规范化后的角度 angle - k*2π, 其中k是整数，使得 angle - k*2π >= center - kPI
A_ALWAYS_INLINE double aNormalizeAngle(double angle, double center)
{
    return aNormalizeAngleStart(angle, center - kPI);
}


/// @brief 将角度量规范化到 0 到 2π 范围
/// @param angle 输入角度（弧度）
/// @return 规范化后的角度（0 到 2π）
A_ALWAYS_INLINE double aNormalizeAngle0To2Pi(double angle)
{
    return aNormalizeAngleStart(angle, 0);
}

/// @brief 将角度量规范化到 -π 到 π 范围
/// @param angle 输入角度（弧度）
/// @return 规范化后的角度（-π 到 π）
A_ALWAYS_INLINE double aNormalizeAngleNegPiToPi(double angle)
{
    return aNormalizeAngleStart(angle, -kPI);
}

/// @brief 将角度量规范化到 -2π 到 0 范围
/// @param angle 输入角度（弧度）
/// @return 规范化后的角度（-2π 到 0）
A_ALWAYS_INLINE double aNormalizeAngleNeg2PiTo0(double angle)
{
    return aNormalizeAngleStart(angle, -kTwoPI);
}

/*! @} */

AST_NAMESPACE_END
