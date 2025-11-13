#pragma once
#include "AstMacro.h"

AST_NAMESPACE_BEGIN

template<typename Scalar>
class Matrix
{
public:
    Matrix()
        : m_row{0}
        , m_col{0}
        , m_data{nullptr}
    {}
    size_t size() const{
        return m_row * m_col;
    }
    size_t row() const{
        return m_row;
    }
    size_t col() const{
        return m_col;
    }
    Scalar* data() const{
        return m_data;
    }
protected:
    size_t m_row;
    size_t m_col;
    Scalar* m_data;
};

template<typename Scalar, size_t Row, size_t Col>
class MatrixMN
{
public:
    constexpr size_t size() const{return Row*Col;}
    constexpr size_t row() const{return Row;}
    constexpr size_t col() const{return Col;}
    constexpr Scalar* data() const{return m_data[0];}
    Scalar operator[](size_t idx) const{return m_data[0][idx];}
protected:
    Scalar m_data[Row][Col];
};


typedef MatrixMN<double, 3, 3> Matrix3d;



AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Matrix3d)



