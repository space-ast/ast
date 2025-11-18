///
/// @file      Matrix.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
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
 
#include "AstGlobal.hpp"
 
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
    constexpr Scalar* data() const{return data_[0];}
    Scalar operator[](size_t idx) const{return data_[0][idx];}
    Scalar operator()(size_t idx) const {return data_[0][idx]; }
    Scalar& operator()(size_t idx)  {return data_[0][idx]; }
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
    static Self Identify(){return Self{1,0,0,0,1,0,0,0,1};}

    constexpr size_t size() const{return Row*Col;}
    constexpr size_t row() const{return Row;}
    constexpr size_t col() const{return Col;}
    constexpr Scalar* data() const{return data_[0];}
    Scalar operator[](size_t idx) const{return data_[0][idx];}
    Scalar operator()(size_t idx) const {return data_[0][idx]; }
    Scalar& operator()(size_t idx)  {return data_[0][idx]; }
    Scalar operator()(size_t row, size_t col) const {return data_[row][col]; }
    Scalar& operator()(size_t row, size_t col) {return data_[row][col]; }

public:
    Scalar data_[Row][Col];
};


typedef MatrixMN<double, 3, 3> Matrix3d;



AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Matrix3d)



