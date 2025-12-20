///
/// @file      MathOperator.hpp
/// @brief     定义向量运算实用函数
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
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
#include <math.h>       // for sqrt
#include <assert.h>     // for assert
#include <type_traits>  // for std::is_pointer
#include <array>        // for std::array
#include <iterator>     // for std::size in C++17
 
AST_NAMESPACE_BEGIN


typedef std::array<double, 3> array3d;
typedef std::array<double, 6> array6d;


/*
 * @note
 * 采用模板函数实现向量运算，只要类型支持标准化的接口即可使用这些函数
 * 
 * 一般来说，用于向量运算的类型都能支持随机访问，
 * 所以这里没有参考标准库的实现来处理不支持随机访问的数据容器
 * 直接使用使用for循环 + 下标访问实现向量运算
 * 
 * */

template<typename _Scalar, size_t Row, size_t Col>
class MatrixMN;

namespace math{


inline std::pair<double, double> twoSum(double a, double b) {
    double s = a + b;
    double t = s - a;
    double e = (a - (s - t)) + (b - t);
    return {s, e};
}

inline double eps(double t)
{
    if (t == 0)
        //return pow(2.0, -1074.0);
        return pow(2.0, -1075.0);
    else
        //return pow(2.0, -52.0 + floor(log(fabs(t)) / log(2.0)));
        return pow(2.0, -53.0 + floor(log(fabs(t)) / log(2.0)));
}

// msvc has std::size for vc2015+
#if defined _MSC_VER || __cplusplus >= 201703L

using std::size;  // use std::size in stdlib

#else

template <typename Container>
auto size(const Container& vec) noexcept
    -> decltype(vec.size())
{
    return vec.size();
}

template <class _Scalar, size_t N>
constexpr size_t size(const _Scalar (&)[N]) noexcept
{
    return N;
}
#endif


/// sign

template<typename _Scalar>
int sign(_Scalar val)
{
    if(val > 0)
        return 1;
    else if(val < 0)
        return -1;
    return 0;
}

template<typename T>
inline 
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
rem(T x, T y)
{
    return x - int(x / y) * y;
}


template<typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
mod(T x, T y)
{
    if (y == 0) return x;
    return x - floor(x / y) * y;
}


template<typename T>
inline
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
fix(T x)
{
    return (int)x;
}


/// cot acot

inline double cot(double a)
{
    return cos(a) / sin(a);
};

inline double acot(double a)
{
    return atan(1 / a);
}

/// dot

template<typename Container1, typename Container2>
inline double dot(const Container1& vec1, const Container2& vec2)
{
    assert (size(vec1) == size(vec2));
    double retval = 0;
    size_t s = size(vec2);
    for (size_t i = 0; i < s; i++)
    {
        retval += vec1[i] * vec2[i];
    }
    return retval;
}


template<size_t N1, size_t N2>
inline double dot(const double(&vec1)[N1], const double(&vec2)[N2])
{
    static_assert(N1 == N2, "dot product requires two vectors of the same size");
    double retval = 0;
    for (size_t i = 0; i < N1; i++)
    {
        retval += vec1[i] * vec2[i];
    }
    return retval;
}


inline double dot(const double* vec1, const double* vec2, size_t N)
{
    double retval = 0;
    for (size_t i = 0; i < N; i++)
    {
        retval += vec1[i] * vec2[i];
    }
    return retval;
}

inline double dot3(const double* vec1, const double* vec2)
{
    return dot(vec1, vec2, 3);
}


/// cross

template<typename Vector3D1, typename Vector3D2>
inline auto cross(const Vector3D1& vec1, const Vector3D2& vec2)
    -> typename std::enable_if<!std::is_pointer<Vector3D1>::value && !std::is_pointer<Vector3D2>::value, Vector3D1>::type
{
    assert (size(vec1) >= 3 && size(vec2) >= 3);

    return Vector3D1{
        vec1[1] * vec2[2] - vec1[2] * vec2[1],
        vec1[2] * vec2[0] - vec1[0] * vec2[2],
        vec1[0] * vec2[1] - vec1[1] * vec2[0]
    };
}

inline void cross(double* res, const double* vec1, const double* vec2)
{
    res[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    res[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    res[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}




template<size_t N1, size_t N2>
inline std::array<double, 3> cross(const double (&vec1)[N1], const double (&vec2)[N2])
{
    static_assert(N1 >= 3, "cross product requires vectors of at least size 3");
    static_assert(N2 >= 3, "cross product requires vectors of at least size 3");
    
    return std::array<double, 3>{
        vec1[1] * vec2[2] - vec1[2] * vec2[1],
        vec1[2] * vec2[0] - vec1[0] * vec2[2],
        vec1[0] * vec2[1] - vec1[1] * vec2[0]
    };
}


inline std::array<double, 3> cross3(const double* vec1, const double* vec2)
{
    return std::array<double, 3>{
        vec1[1] * vec2[2] - vec1[2] * vec2[1],
        vec1[2] * vec2[0] - vec1[0] * vec2[2],
        vec1[0] * vec2[1] - vec1[1] * vec2[0]
    };
}

/// squaredNorm

inline double squaredNorm(const double* vec, size_t N)
{
    double sumsq = 0;
    for (size_t i = 0; i < N; i++)
    {
        sumsq += vec[i] * vec[i];
    }
    return sumsq;
}


template<typename Vector>
inline auto squaredNorm(const Vector& vec)
    -> typename std::enable_if<!std::is_pointer<Vector>::value, double>::type
{
    double sumsq = 0.0;
    for (const auto& val : vec) {
        sumsq += val * val;
    }
    return sumsq;
}

template <size_t N>
inline double squaredNorm(const double (&vec)[N])
{
    double sumsq = 0;
    for (size_t i = 0; i < N; i++)
    {
        sumsq += vec[i] * vec[i];
    }
    return sumsq;
}


/// norm

inline double norm(const double* vec, size_t N)
{
    return sqrt(squaredNorm(vec, N));
}


template<typename Vector>
inline auto norm(const Vector& vec)
    -> typename std::enable_if<!std::is_pointer<Vector>::value, double>::type
{
    return sqrt(squaredNorm(vec));
}



template <size_t N>
inline double norm(const double (&vec)[N])
{
    return sqrt(squaredNorm(vec));
}


/// normalize


inline void normalize(double* vec, size_t N)
{
    double mag = norm(vec, N);
    if (mag == 0)
        return;
    for (size_t i = 0; i < N; i++)
    {
        vec[i] /= mag;
    }
}


template<typename Vector>
inline auto normalize(Vector& vec)
-> typename std::enable_if<!std::is_pointer<Vector>::value, void>::type
{
    double mag = norm(vec);
    if (mag == 0)
        return;
    for (auto& val : vec) {
        val /= mag;
    }
}



template <size_t N>
inline void normalize(double (&vec)[N])
{
    double mag = norm(vec);
    if (mag == 0)
        return;
    for (size_t i = 0; i < N; i++)
    {
        vec[i] /= mag;
    }
}


/// normalized


template<typename Vector>
inline auto normalized(const Vector& vec)
-> typename std::enable_if<!std::is_pointer<Vector>::value, Vector>::type
{
    double mag = norm(vec);
    if (mag == 0)
        return vec;
    Vector retval{ vec };
    normalize(retval);
    return retval;
}



template <size_t N>
inline std::array<double, N> normalized(double (&vec)[N])
{
    std::array<double, N> retval;
    double mag = norm(vec);
    if (mag == 0) {
        for (size_t i = 0; i < N; i++) {
            retval[i] = vec[i];
        }
    }
    else {
        for (size_t i = 0; i < N; i++)
        {
            retval[i] = vec[i] / mag;
        }
    }
    return retval;
}

template <size_t N>
inline std::array<double, N> normalized(const double* vec)
{
    std::array<double, N> retval;
    double mag = norm(vec, N);
    if (mag == 0) {
        for (size_t i = 0; i < N; i++) {
            retval[i] = vec[i];
        }
    }
    else {
        for (size_t i = 0; i < N; i++)
        {
            retval[i] = vec[i] / mag;
        }
    }
    return retval;
}

// 定义向量特征：必须要有 size() 和 operator[]
template<typename T>
struct is_vector_like {
private:
    template<typename U>
    static auto test(int) -> decltype(
        std::declval<U>()[0],       // 有 operator[]
        size(std::declval<U>()),    // 有 size() 函数
        std::true_type{}
    );
    
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};


/// + - * /

#define _AST_DEF_OP_SV(OP)                                                   \
template<typename Scalar, typename Vector>                                   \
inline auto operator OP(Scalar scalar, const Vector& vec)                    \
-> typename std::enable_if<std::is_arithmetic<Scalar>::value && is_vector_like<Vector>::value, Vector>::type  \
{                                                                            \
    Vector retval{ vec };                                                    \
    size_t s = size(vec);                                                    \
    for (size_t i = 0; i < s; i++)                                           \
    {                                                                        \
        retval[i] OP##= scalar;                                              \
    }                                                                        \
    return retval;                                                           \
}


#define _AST_DEF_OP_VS(OP)                                                   \
template<typename Vector, typename Scalar>                                   \
inline auto operator OP(const Vector& vec, Scalar scalar)                    \
-> typename std::enable_if<std::is_arithmetic<Scalar>::value && is_vector_like<Vector>::value, Vector>::type  \
{                                                                            \
    Vector retval{ vec };                                                    \
    size_t s = size(vec);                                                    \
    for (size_t i = 0; i < s; i++)                                           \
    {                                                                        \
        retval[i] OP##= scalar;                                              \
    }                                                                        \
    return retval;                                                           \
} 


#define _AST_DEF_OP_VV(OP)                                          \
template<typename Vector1, typename Vector2>                        \
inline auto operator OP(const Vector1& vec1, const Vector2& vec2)\
-> typename std::enable_if<is_vector_like<Vector1>::value && is_vector_like<Vector2>::value, Vector1>::type  \
{                                                                   \
    assert(size(vec1) == size(vec2));                               \
    Vector1 retval{ vec1 };                                         \
    size_t s = size(vec1);                                          \
    for (size_t i = 0; i < s; i++)                                  \
    {                                                               \
        retval[i] OP##= vec2[i];                                    \
    }                                                               \
    return retval;                                                  \
}


#define _AST_DEF_OP(OP) \
_AST_DEF_OP_SV(OP)      \
_AST_DEF_OP_VS(OP)      \
_AST_DEF_OP_VV(OP)      \



_AST_DEF_OP(+)
_AST_DEF_OP(-)


_AST_DEF_OP_SV(*)
_AST_DEF_OP_SV(/)

_AST_DEF_OP_VS(*)
_AST_DEF_OP_VS(/)

// 乘除运算对于向量不应该是按元素运算，不启用以避免歧义
// _AST_DEF_OP_VV(*) 
// _AST_DEF_OP_VV(/) 




#if 0
/// @fixme: 这个模板无法起作用，受限于c++语言机制，无法为原生数组类型直接定义运算符重载
template<size_t N1, size_t N2>
inline std::array<double, N1> operator+(const double (&vec1)[N1], const double (&vec2)[N2])
{
    static_assert(N1 == N2, "plus operator requires vectors of the same dimension");
    std::array<double, N1> retval{};
    for (size_t i = 0; i < N1; i++)
    {
        retval[i] = vec1[i] + vec2[i];
    }
    return retval;
}
#endif


/// 矩阵乘法



template<typename _Scalar, size_t I, size_t J, size_t K>
MatrixMN<_Scalar, I, K> operator* (
    const MatrixMN<_Scalar, I, J>& left,
    const MatrixMN<_Scalar, J, K>& right
)
{
    MatrixMN<_Scalar, I, K> retval;
    for (size_t i = 0; i < I; i++)
    {
        for (size_t k = 0; k < K; k++) {
            _Scalar value = 0;
            for (size_t j = 0; j < J; j++)
            {
                value += left(i, j) * right(j, k);
            }
            retval(i, k) = value;
        }
    }
    return retval;
}


template<typename _Scalar, size_t I, size_t J, size_t K>
std::array<std::array<_Scalar, K>, I> mtimes(
    const _Scalar(&left)[I][J],
    const _Scalar(&right)[J][K]
)
{
    std::array<std::array<_Scalar, K>, I> retval;
    for (size_t i = 0; i < I; i++)
    {
        for (size_t k = 0; k < K ; k++) {
            _Scalar value = 0;
            for (size_t j = 0; j < J; j++)
            {
                value += left[i][j] * right[j][k];
            }
            retval[i][k] = value;
        }
    }
    return retval;
}


}

#define _ASTMATH _AST math::

#ifdef AST_BUILD_LIB  // 防止与其他库使用时出现冲突，默认只在编译时使用，否则需要主动开启
using namespace math;
#endif

AST_NAMESPACE_END