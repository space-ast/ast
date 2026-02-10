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

/*!
    @addtogroup Array
    @{
*/
 

template<typename _Scalar>
class Matrix
{
public:
    Matrix();
    Matrix(const Matrix& other);
    Matrix(Matrix&& other);
    ~Matrix();
    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other);
    void resize(size_t row, size_t col);
    void setZero();
    size_t size() const{return m_row * m_col;}
    size_t row() const{return m_row;}
    size_t col() const{return m_col;}
    _Scalar* data() const{return m_data;}
    _Scalar& operator()(size_t row, size_t col){return m_data[row * m_col + col];}
    _Scalar operator()(size_t row, size_t col) const{return m_data[row * m_col + col];}
protected:
    size_t m_row;
    size_t m_col;
    _Scalar* m_data;
};

typedef Matrix<double> MatrixXd;

template <typename _Scalar>
inline Matrix<_Scalar>::Matrix()
    : m_row{0}
    , m_col{0}
    , m_data{nullptr}
{}

template <typename _Scalar>
inline Matrix<_Scalar>::~Matrix()
{
    if(m_data)
        free(m_data);
}

template <typename _Scalar>
inline Matrix<_Scalar>::Matrix(const Matrix& other)
    : m_row{other.m_row}
    , m_col{other.m_col}
    , m_data{nullptr}
{
    if (other.m_data) {
        this->m_data = (_Scalar*)malloc(other.size() * sizeof(_Scalar));
        memcpy(m_data, other.m_data, other.size() * sizeof(_Scalar));
    }
}

template <typename _Scalar>
inline Matrix<_Scalar>& Matrix<_Scalar>::operator=(const Matrix& other)
{
    if(this != &other){
        resize(other.row(), other.col());
        if (other.m_data) {
            memcpy(m_data, other.m_data, other.size() * sizeof(_Scalar));
        }
    }
    return *this;
}

template <typename _Scalar>
inline Matrix<_Scalar>::Matrix(Matrix&& other)
    : m_row{other.m_row}
    , m_col{other.m_col}
    , m_data{other.m_data}
{
    other.m_row = 0;
    other.m_col = 0;
    other.m_data = nullptr;
}

template <typename _Scalar>
inline Matrix<_Scalar>& Matrix<_Scalar>::operator=(Matrix&& other)
{
    if(this != &other){
        std::swap(m_row, other.m_row);
        std::swap(m_col, other.m_col);
        std::swap(m_data, other.m_data);
    }
    return *this;
}

template <typename _Scalar>
inline void Matrix<_Scalar>::resize(size_t row, size_t col)
{
    if(m_data)
        free(m_data);
    m_row = row;
    m_col = col;
    m_data = (_Scalar*)malloc(row * col * sizeof(_Scalar));
}


template <typename _Scalar>
inline void Matrix<_Scalar>::setZero()
{
    if(m_data)
        memset(m_data, 0, size() * sizeof(_Scalar));
}


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
    static Self Zero(){return Self{0,0,0,0,0,0,0,0,0};}
    
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
    void setIdentity(){*this = Identity();}
    Self& operator*=(const Self& other);

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
A_ALWAYS_INLINE typename MatrixMN<_Scalar, 3, 3>::Self &MatrixMN<_Scalar, 3, 3>::operator*=(const Self &other)
{
    using namespace _AST math;
    *this = operator*(*this, other);
    return *this;
}

/*! @} */

AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Matrix3d)



