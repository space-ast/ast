///
/// @file      Vector.hpp
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
 

template<typename Scalar, size_t N>
class VectorN
{
public:
    constexpr const Scalar* data() const {return m_data;}
    constexpr Scalar* data() {return m_data;}
    constexpr size_t size() const{return N;}
    Scalar operator[](size_t idx) const{return m_data[idx];}
    Scalar& operator[](size_t idx) {return m_data[idx];}

    // 支持基于范围的for循环
    Scalar* begin() noexcept{ return m_data; }
    Scalar* end() noexcept{ return m_data + N; }
    const Scalar* begin() const noexcept{ return m_data; }
    const Scalar* end() const noexcept{ return m_data + N; }
    const Scalar* cbegin() const noexcept{ return m_data; }
    const Scalar* cend() const noexcept{ return m_data + N; }
public:
    Scalar m_data[N]; // 不要直接访问m_data，设为public仅为了实现聚合初始化
};



AST_NAMESPACE_END


AST_DECL_TYPE_ALIAS(Vector3d)
