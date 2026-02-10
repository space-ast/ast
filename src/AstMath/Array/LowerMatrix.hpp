///
/// @file      LowerMatrix.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-14
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
#include <malloc.h>     // for malloc, free
#include <string.h>     // for memset

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Array
    @{
*/



/// @brief     下三角矩阵
template<typename _Scalar>
class LowerMatrix
{
public:
    LowerMatrix()  : rowcol_{0}, data_{nullptr} {}
    LowerMatrix(const LowerMatrix& other);
    LowerMatrix& operator=(const LowerMatrix& other);
    LowerMatrix(LowerMatrix&& other) noexcept;
    LowerMatrix& operator=(LowerMatrix&& other) noexcept;
    ~LowerMatrix();
    void resize(size_t rowcol);
    size_t row() const { return rowcol_; }
    size_t col() const { return rowcol_; }
    _Scalar operator()(size_t row, size_t col) const noexcept {return data_[index(row, col)]; }
    _Scalar& operator()(size_t row, size_t col)  noexcept {return data_[index(row, col)]; }
    void setZero() noexcept;
protected:
    static constexpr size_t index(size_t row, size_t col) noexcept { return row*(row+1)/2 + col; }
    static constexpr size_t capacity(size_t nrowcol) noexcept { return nrowcol * (nrowcol + 1) / 2; }
protected:
    size_t      rowcol_;        ///< 矩阵行数/列数
    _Scalar*    data_;          ///< 矩阵数据
};

template <typename _Scalar>
inline LowerMatrix<_Scalar>::~LowerMatrix()
{
    if(data_)
        free(data_);
}

template <typename _Scalar>
inline void LowerMatrix<_Scalar>::resize(size_t rowcol)
{
    if (rowcol > rowcol_)
    {
        if(data_)
            free(data_);
        data_ = (_Scalar*)malloc(sizeof(_Scalar) * capacity(rowcol));
    }
    rowcol_ = rowcol;
}

template <typename _Scalar>
inline LowerMatrix<_Scalar>::LowerMatrix(const LowerMatrix &other)
    :LowerMatrix()
{
    resize(other.rowcol_);
    memcpy(data_, other.data_, sizeof(_Scalar) * capacity(rowcol_));
}

template <typename _Scalar>
inline LowerMatrix<_Scalar> &LowerMatrix<_Scalar>::operator=(const LowerMatrix &other)
{
    if(this != &other)
    {
        resize(other.rowcol_);
        memcpy(data_, other.data_, sizeof(_Scalar) * capacity(rowcol_));
    }
    return *this;
}

template <typename _Scalar>
inline LowerMatrix<_Scalar>::LowerMatrix(LowerMatrix &&other) noexcept
{
    rowcol_ = other.rowcol_;
    data_ = other.data_;
    other.rowcol_ = 0;
    other.data_ = nullptr;
}

template <typename _Scalar>
inline LowerMatrix<_Scalar> &LowerMatrix<_Scalar>::operator=(LowerMatrix &&other) noexcept
{
    if(this != &other)
    {
        std::swap(rowcol_, other.rowcol_);
        std::swap(data_, other.data_);
    }
    return *this;
}
template <typename _Scalar>
inline void LowerMatrix<_Scalar>::setZero() noexcept
{ 
    memset(data_, 0, sizeof(_Scalar) * capacity(rowcol_)); 
}


typedef LowerMatrix<double> LowerMatrixd;

/*! @} */

AST_NAMESPACE_END

