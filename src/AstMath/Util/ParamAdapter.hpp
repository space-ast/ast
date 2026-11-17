///
/// @file      ParamAdapter.hpp
/// @brief     参数适配器
/// @details   实现 C 风格 double 数组与 Vector3d/Quaternion/Matrix3d 等类型之间的轻量转换
/// @author    axel
/// @date      2026-10-24
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

#ifndef SWIG

/// @brief 输入适配器：把外部数据转换为函数的期望参数类型
template <typename T>
class In;

/// @brief 输出适配器：析构时把结果回写到外部缓冲区，或者直接将外部缓冲区绑定到函数的输出参数
template <typename T>
class Out;

template <typename T>
class InOut;

// -------------------------
// --- double* 参数适配器
// -------------------------

/// @brief 只读 `double*` 适配器，按 3/4 个连续 double 直接解释为 Vector3d/Quaternion
template <>
class In<const double*>
{
public:
    explicit In(const double* value) : value_(value) {}
    operator const Vector3d& () const { return *reinterpret_cast<const Vector3d*>(value_); }
    operator const Quaternion& () const { return *reinterpret_cast<const Quaternion*>(value_); }
private:
    const double* value_{};
};

/// @brief 可写 `double*` 适配器，直接绑定外部缓冲区
template <>
class Out<double*>
{
public:
    explicit Out(double* value) : value_(value) {}
    operator Vector3d& () { return *reinterpret_cast<Vector3d*>(value_); }
    operator Quaternion& () { return *reinterpret_cast<Quaternion*>(value_); }
private:
    double* value_{};
};



// -------------------------
// --- double[3][3] 参数适配器
// -------------------------

#if _AST_MATRIX_IS_ROW_MAJOR

/// @brief 只读 `double[3][3]` 适配器，内存布局与行优先存储的 Matrix3d 一致，直接绑定外部缓冲区
template <>
class In<const double (*)[3]>
{
public:
    explicit In(const double (*value)[3]) : value_(value) {}
    operator const Matrix3d& () const { return *reinterpret_cast<const Matrix3d*>(value_); }
private:
    const double (*value_)[3]{};
};



/// @brief 可写 `double[3][3]` 适配器，内存布局与行优先存储的 Matrix3d 一致，直接绑定外部缓冲区
template <>
class Out<double (*)[3]>
{
public:
    explicit Out(double (*value)[3]) : value_(value) {}
    operator Matrix3d& () { return *reinterpret_cast<Matrix3d*>(value_); }
private:
    double (*value_)[3]{};
};

#else

/// @brief 只读 `double[3][3]` 适配器，构造时逐元素复制到 Matrix3d
template <>
class In<const double (*)[3]>
{
public:
    explicit In(const double (*value)[3])
        : value_(value)
    {
        if (value_ == nullptr)
            return;
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                matrix_(i, j) = value_[i][j];
            }
        }
    }
    operator const Matrix3d& () const { return matrix_; }
private:
    Matrix3d matrix_{};
    const double (*value_)[3]{};
};



/// @brief 可写 `double[3][3]` 适配器，析构时逐元素回写
template <>
class Out<double (*)[3]>
{
public:
    explicit Out(double (*value)[3]) : matrix_(Matrix3d::Zero()), value_(value) {}
    /// 转移回写责任，源对象指针置空以免重复回写
    Out(Out&& other) : matrix_(other.matrix_), value_(other.value_) { other.value_ = nullptr; }
    operator Matrix3d& () { return matrix_; }
    ~Out() {
        if (value_ == nullptr)
            return;
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                value_[i][j] = matrix_(i, j);
            }
        }
    }
private:
    Matrix3d matrix_{};
    double (*value_)[3]{};
};

#endif

// -------------------------
// --- double[6][6] 参数适配器
// -------------------------


// Eigen 对定长矩阵可能带有对齐要求(例如大小为 16 字节整数倍的矩阵)
// 故不能把裸 double 缓冲区直接当作 Eigen::Matrix6d 对象使用，
#if _AST_MATRIX_IS_ROW_MAJOR && !defined(AST_USE_EIGEN_MATRIX)

/// @brief 只读 `double[6][6]` 适配器，内存布局与行优先存储的 Matrix6d 一致，直接绑定外部缓冲区
template <>
class In<const double (*)[6]>
{
public:
    explicit In(const double (*value)[6]) : value_(value) {}
    operator const Matrix6d& () const { return *reinterpret_cast<const Matrix6d*>(value_); }
private:
    const double (*value_)[6]{};
};

#else

template <>
class In<const double (*)[6]>
{
public:
    explicit In(const double (*value)[6])
        : value_(value)
    {
        if (value_ == nullptr)
            return;
        for(int i = 0; i < 6; i++)
        {
            for(int j = 0; j < 6; j++)
            {
                matrix_(i, j) = value_[i][j];
            }
        }
    }
    operator const Matrix6d& () const { return matrix_; }
private:
    Matrix6d matrix_{};
    const double (*value_)[6]{};
};
#endif

// Eigen 对定长矩阵可能带有对齐要求(例如大小为 16 字节整数倍的矩阵)
// 故不能把裸 double 缓冲区直接当作 Eigen::Matrix6d 对象使用，
#if _AST_MATRIX_IS_ROW_MAJOR && !defined(AST_USE_EIGEN_MATRIX)

/// @brief 只读 `double[6][6]` 适配器，内存布局与行优先存储的 Matrix6d 一致，直接绑定外部缓冲区
template <>
class Out<double (*)[6]>
{
public:
    explicit Out(double (*value)[6]) : value_(value) {}
    operator Matrix6d& () { return *reinterpret_cast<Matrix6d*>(value_); }
private:
    double (*value_)[6]{};
};

#else

/// @brief 可写 `double[6][6]` 适配器，析构时逐元素回写
template <>
class Out<double (*)[6]>
{
public:
    explicit Out(double (*value)[6]) : matrix_(Matrix6d::Zero()), value_(value) {}
    /// 转移回写责任，源对象指针置空以免重复回写
    Out(Out&& other) : matrix_(other.matrix_), value_(other.value_) { other.value_ = nullptr; }
    operator Matrix6d& () { return matrix_; }
    ~Out() {
        if (value_ == nullptr)
            return;
        for(int i = 0; i < 6; i++)
        {
            for(int j = 0; j < 6; j++)
            {
                value_[i][j] = matrix_(i, j);
            }
        }
    }
private:
    Matrix6d matrix_{};
    double (*value_)[6]{};
};

#endif


// -------------------------
// --- Matrix3d 参数适配器
// -------------------------

#if _AST_MATRIX_IS_ROW_MAJOR

/// @brief Matrix3d 只读适配器，行主存储时直接把内部缓冲区暴露为 `double[3][3]`
template <>
class In<Matrix3d>
{
public:
    explicit In(const Matrix3d& value) : value_(value) {}
    typedef const double(*CMatrixType)[3];
    operator CMatrixType () const { return reinterpret_cast<CMatrixType>(value_.data()); }
private:
    const Matrix3d& value_;
};

#else

/// @brief Matrix3d 只读适配器，转换为行主序 `double[3][3]`
template <>
class In<Matrix3d>
{
public:
    explicit In(const Matrix3d& value)
        : value_(value)
    {
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                matrix_[i][j] = value_(i, j);
            }
        }
    }
    typedef const double(*CMatrixType)[3];
    operator CMatrixType () const { return matrix_; }
private:
    double matrix_[3][3]{};
    const Matrix3d& value_{};
};

#endif



/// @brief 按实参的指针类型自动选择适配器，调用点不用写模板参数
/// @code
///     axisar(in(axis), angle, out(r));
/// @endcode


A_ALWAYS_INLINE In<const double*> in(const double* value)
{
    return In<const double*>(value);
}

A_ALWAYS_INLINE In<const double (*)[3]> in(const double (*value)[3])
{
    return In<const double (*)[3]>(value);
}

A_ALWAYS_INLINE In<const double (*)[6]> in(const double (*value)[6])
{
    return In<const double (*)[6]>(value);
}

A_ALWAYS_INLINE In<Matrix3d> in(const Matrix3d& value)
{
    return In<Matrix3d>(value);
}

A_ALWAYS_INLINE Out<double*> out(double* value)
{
    return Out<double*>(value);
}

A_ALWAYS_INLINE Out<double (*)[3]> out(double (*value)[3])
{
    return Out<double (*)[3]>(value);
}

A_ALWAYS_INLINE Out<double (*)[6]> out(double (*value)[6])
{
    return Out<double (*)[6]>(value);
}

#endif

/*! @} */

AST_NAMESPACE_END
