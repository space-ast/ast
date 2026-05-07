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
#include <string.h>

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
    Matrix(size_t row, size_t col);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other);
    ~Matrix();
    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other);
    void resize(size_t row, size_t col);
    void setZero();
    size_t size() const{return row_ * col_;}
    size_t row() const{return row_;}
    size_t col() const{return col_;}
    _Scalar* data() const{return data_;}
    _Scalar& operator()(size_t row, size_t col){return data_[row * col_ + col];}
    _Scalar operator()(size_t row, size_t col) const{return data_[row * col_ + col];}
protected:
    size_t row_;
    size_t col_;
    _Scalar* data_;
};

typedef Matrix<double> MatrixXd;

template <typename _Scalar>
inline Matrix<_Scalar>::Matrix()
    : row_{0}
    , col_{0}
    , data_{nullptr}
{}

template <typename _Scalar>
inline Matrix<_Scalar>::Matrix(size_t row, size_t col)
    : row_{row}
    , col_{col}
    , data_{nullptr}
{
    data_ = (_Scalar*)malloc(row * col * sizeof(_Scalar));
    setZero();
}

template <typename _Scalar>
inline Matrix<_Scalar>::~Matrix()
{
    if(data_)
        free(data_);
}

template <typename _Scalar>
inline Matrix<_Scalar>::Matrix(const Matrix& other)
    : row_{other.row()}
    , col_{other.col()}
    , data_{nullptr}
{
    if (other.data_) {
        this->data_ = (_Scalar*)malloc(other.size() * sizeof(_Scalar));
        if(this->data_)
        {
            memcpy(data_, other.data_, other.size() * sizeof(_Scalar));
        }
        else
        {
            // aError("failed to copy Matrix");
            data_ = nullptr;
            row_ = 0;
            col_ = 0;
        }
    }
}

template <typename _Scalar>
inline Matrix<_Scalar>& Matrix<_Scalar>::operator=(const Matrix& other)
{
    if(this != &other){
        resize(other.row(), other.col());
        if (other.data_) {
            memcpy(data_, other.data_, other.size() * sizeof(_Scalar));
        }
    }
    return *this;
}

template <typename _Scalar>
inline Matrix<_Scalar>::Matrix(Matrix&& other)
    : row_{other.row()}
    , col_{other.col()}
    , data_{other.data()}
{
    other.row_ = 0;
    other.col_ = 0;
    other.data_ = nullptr;
}

template <typename _Scalar>
inline Matrix<_Scalar>& Matrix<_Scalar>::operator=(Matrix&& other)
{
    if(this != &other){
        std::swap(row_, other.row_);
        std::swap(col_, other.col_);
        std::swap(data_, other.data_);
    }
    return *this;
}

template <typename _Scalar>
inline void Matrix<_Scalar>::resize(size_t row, size_t col)
{
    if(data_)
        free(data_);
    row_ = row;
    col_ = col;
    data_ = (_Scalar*)malloc(row * col * sizeof(_Scalar));
}


template <typename _Scalar>
inline void Matrix<_Scalar>::setZero()
{
    if(data_)
        memset(data_, 0, size() * sizeof(_Scalar));
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
    Scalar data_[Row][Col];  ///< 矩阵元素；不初始化，确保类型为聚合类型
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



