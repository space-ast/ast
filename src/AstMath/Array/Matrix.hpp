///
/// @file      Matrix.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      15.11.2025
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
#include "AstMath/MathOperator.hpp"
#include "AstMath/Vector.hpp"
 
AST_NAMESPACE_BEGIN
 

template<typename _Scalar>
class Matrix
{
public:
    Matrix()
        : m_row{0}
        , m_col{0}
        , m_data{nullptr}
    {}
    size_t size() const{return m_row * m_col;}
    size_t row() const{return m_row;}
    size_t col() const{return m_col;}
    _Scalar* data() const{return m_data;}
protected:
    size_t m_row;
    size_t m_col;
    _Scalar* m_data;
};

template<typename _Scalar, size_t Row, size_t Col>
class MatrixMN
{
public:
    typedef MatrixMN< _Scalar, Row, Col> Self;
    typedef _Scalar Scalar;

    constexpr size_t size() const{return Row*Col;}
    constexpr size_t row() const{return Row;}
    constexpr size_t col() const{return Col;}
    constexpr Scalar* data() const{return (Scalar*)this;}
    Scalar operator[](size_t idx) const{return data()[idx];}
    Scalar operator()(size_t idx) const {return data()[idx]; }
    Scalar& operator()(size_t idx)  {return data()[idx]; }
    Scalar operator()(size_t row, size_t col) const {return data_[row][col]; }
    Scalar& operator()(size_t row, size_t col) {return data_[row][col]; }

public:
    Scalar data_[Row][Col];
};


template<typename _Scalar>
class MatrixMN<_Scalar, 3, 3>
{
public:
    enum {Row = 3, Col = 3};
    typedef MatrixMN< _Scalar, 3, 3> Self;
    typedef _Scalar Scalar;
    static Self Identity(){return Self{1,0,0,0,1,0,0,0,1};}

    constexpr size_t size() const{return Row*Col;}
    constexpr size_t row() const{return Row;}
    constexpr size_t col() const{return Col;}
    constexpr Scalar* data() const{return (Scalar*)this;}
    Scalar operator[](size_t idx) const{return data()[idx];}
    Scalar operator()(size_t idx) const {return data()[idx]; }
    Scalar& operator()(size_t idx)  {return data()[idx]; }
    Scalar operator()(size_t row, size_t col) const {return data_[row][col]; }
    Scalar& operator()(size_t row, size_t col) {return data_[row][col]; }

    Self transpose() const;
    void transposeInPlace();
    Self operator* (const Self& other) const;
    Self& operator*=(const Self& other);

    Vector3d operator*(const Vector3d& other) const;

public:
    Scalar data_[Row][Col];
};


typedef MatrixMN<double, 3, 3> Matrix3d;

template <typename _Scalar>
A_ALWAYS_INLINE typename MatrixMN<_Scalar, 3, 3>::Self MatrixMN<_Scalar, 3, 3>::transpose() const
{
    return Self{
        data_[0][0], data_[1][0], data_[2][0],
        data_[0][1], data_[1][1], data_[2][1],
        data_[0][2], data_[1][2], data_[2][2],
    };
}

template <typename _Scalar>
A_ALWAYS_INLINE void MatrixMN<_Scalar, 3, 3>::transposeInPlace()
{
    std::swap(data_[0][1], data_[1][0]);
    std::swap(data_[0][2], data_[2][0]);
    std::swap(data_[1][2], data_[2][1]);
}

template <typename _Scalar>
A_ALWAYS_INLINE typename MatrixMN<_Scalar, 3, 3>::Self MatrixMN<_Scalar, 3, 3>::operator*(const Self &other) const
{
    using namespace _AST math;
    return math::operator*(*this, other);
}

template <typename _Scalar>
A_ALWAYS_INLINE typename MatrixMN<_Scalar, 3, 3>::Self &MatrixMN<_Scalar, 3, 3>::operator*=(const Self &other)
{
    using namespace _AST math;
    *this = math::operator*(*this, other);
    return *this;
}

template <typename _Scalar>
A_ALWAYS_INLINE Vector3d MatrixMN<_Scalar, 3, 3>::operator*(const Vector3d &other) const
{
    using namespace _AST math;
    return math::operator*(*this, other);
}

AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Matrix3d)


