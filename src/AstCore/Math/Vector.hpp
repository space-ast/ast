#pragma once
#include "AstTypesForward.h"

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
