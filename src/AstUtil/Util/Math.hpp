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
#include <cassert>
#include <algorithm>
#include <type_traits>
#include <limits>       // for std::numeric_limits<double>::quiet_NaN()

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

/// @brief 对值进行范围限制
/// @param val 输入值
/// @param low 最小值
/// @param high 最大值
/// @return 限制后的值
template<typename T>
A_CONSTEXPR_CXX14 const T& clamp(const T& val, const T& low, const T& high)
{
    #ifdef A_CXX14
    assert(low <= high && "low must be less than or equal to high");
    if(val < low){
        return low;
    }
    if(val > high){
        return high;
    }
    return val;
    #else
    return (val < low) ? low : ((val > high) ? high : val);
    #endif
}


/// @brief 安全反正弦，参数自动箝位到 [-1, 1]
/// @param x 正弦值
/// @return 反正弦值（弧度）
A_ALWAYS_INLINE double asinSafe(double x)
{
    return std::asin(clamp(x, -1.0, 1.0));
}

/// @brief 安全反余弦，参数自动箝位到 [-1, 1]
/// @param x 余弦值
/// @return 反余弦值（弧度）
A_ALWAYS_INLINE double acosSafe(double x)
{
    return std::acos(clamp(x, -1.0, 1.0));
}

/// @brief 安全平方根，参数自动箝位到 [0, +∞]
/// @param x 输入值
/// @return 平方根值
A_ALWAYS_INLINE double sqrtSafe(double x)
{
    return std::sqrt((std::max)(x, 0.0));
}

/// @brief 计算平方
/// @param x 输入值
template<typename Scalar>
Scalar square(Scalar x)
{
    return x * x;
}


/// @brief 将角度量规范化到指定起始范围
/// @param angle 输入角度（弧度）
/// @param start 起始角度（弧度）
/// @return 规范化后的角度 angle - k*2π, 其中k是整数，使得 angle - k*2π >= start
A_ALWAYS_INLINE double aNormalizeAngleStart(double angle, double start)
{
    return angle - floor((angle - start) / kTwoPI) * kTwoPI;
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


/// @brief 传播 NaN 值的数学工具函数
namespace propagate_nan
{
    /// @brief 计算两个数中的较小值(传播 NaN)
    A_ALWAYS_INLINE double (min)(double a, double b)
    {
        if(a <= b)
            return a;
        if(b <= a)
            return b;
        return std::numeric_limits<double>::quiet_NaN();
    }

    /// @brief 计算两个数中的较大值(传播 NaN)
    A_ALWAYS_INLINE double (max)(double a, double b)
    {
        if(a >= b)
            return a;
        if(b >= a)
            return b;
        return std::numeric_limits<double>::quiet_NaN();
    }

    // -------------------- 类型标记：类型是否允许使用减法，需要用户通过类型特化 allow_efficient_minus 来明确允许 --------------------
    template <typename T>
    struct allow_efficient_minus : std::false_type {};

    // -------------------- 工具：检测是否存在 operator- --------------------
    template <typename T, typename = void>
    struct has_minus : std::false_type {};

    template <typename T>
    struct has_minus<T, std::void_t<decltype(std::declval<T>() - std::declval<T>())>> 
        : std::true_type {};

    // -------------------- 类型标记：是否使用减法 --------------------
    template <typename T>
    struct use_efficient_minus
    {
        static constexpr bool value = allow_efficient_minus<T>::value && has_minus<T>::value;
    };

    template<typename T>
    typename std::enable_if<!use_efficient_minus<T>::value,const T&>::type 
    (min)(const T& a, const T& b)
    {
        // 不支持 operator- 的类型，直接比较大小
        if(a <= b)
            return a;
        if(b <= a)
            return b;
        if(a != a)
            return a;
        return b;
    }
    template <typename T>
    typename std::enable_if<use_efficient_minus<T>::value, const T&>::type 
    (min)(const T& a, const T& b) {
        // 支持 operator- 的类型，使用减法比较大小
        auto diff = a - b;         // 注意：这里会构造临时对象
        if (diff <= 0) return a;   // a <= b
        if (diff >= 0) return b;   // a >= b
        if(a != a)
            return a;
        return b;
    }
    
    template<typename T>
    typename std::enable_if<!use_efficient_minus<T>::value,const T&>::type 
    (max)(const T& a, const T& b)
    {
        // 不支持 operator- 的类型，直接比较大小
        if(a >= b)
            return a;
        if(b >= a)
            return b;
        if(a != a)
            return a;
        return b;
    }
    template <typename T>
    typename std::enable_if<use_efficient_minus<T>::value, const T&>::type 
    (max)(const T& a, const T& b) 
    {
        // 支持 operator- 的类型，使用减法比较大小
        auto diff = a - b;        // 注意：这里会构造临时对象
        if (diff >= 0) return a;   // a >= b
        if (diff <= 0) return b;   // a <= b
        if(a != a)
            return a;
        return b;
    }
}

/*! @} */

AST_NAMESPACE_END
