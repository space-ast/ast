///
/// @file      Vector.hpp
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
