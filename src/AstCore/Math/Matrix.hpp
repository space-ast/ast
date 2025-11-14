///
/// @file      Matrix.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 根据 Apache 许可证 2.0 版本（"许可证"）授权；
/// 除非符合许可证，否则不得使用此文件。
/// 您可以在以下网址获取许可证副本：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 除非适用法律要求或书面同意，按"原样"分发的软件
/// 没有任何明示或暗示的担保或条件。
/// 请参阅许可证了解特定语言的权限和限制。
 
#pragma once
 
#include "AstGlobal.hpp"
 
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



