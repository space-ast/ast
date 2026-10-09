///
/// @file      EnumDescriptor.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-08
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AstUtil/Span.hpp"
#include <string>       // for std::string
#include <vector>       // for std::vector
#include <utility>      // for std::pair


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 枚举描述符
class AST_UTIL_API EnumDescriptor
{   
public:
    using NumberType = int;                                // 枚举值类型
    using ValueList = Span<std::pair<NumberType, char*>>;  // 枚举值列表
public:
    EnumDescriptor() = default;
    ~EnumDescriptor() = default;

    EnumDescriptor(ValueList values) : values_(values) {}
    EnumDescriptor(ValueList&& values) : values_(std::move(values)) {}

public:
    const char* nameOf(NumberType value) const;

    const NumberType* valueOf(StringView name) const;

    template<typename EnumType>
    const char* nameOf(EnumType value) const
    {
        static_assert(std::is_enum<EnumType>::value, "EnumType must be an enum type");
        static_assert(sizeof(NumberType) >= sizeof(EnumType), "sizeof(NumberType) must be at least sizeof(EnumType) in order to store EnumType");
        return nameOf(static_cast<NumberType>(value));
    }

    template<typename EnumType>
    const EnumType* valueOf(StringView name) const
    {
        static_assert(std::is_enum<EnumType>::value, "EnumType must be an enum type");
        static_assert(sizeof(NumberType) >= sizeof(EnumType), "sizeof(NumberType) must be at least sizeof(EnumType) in order to store EnumType");
        return valueOf(static_cast<NumberType>(name));
    }
private:
    ValueList values_;  ///< 枚举值列表
};


/*! @} */

AST_NAMESPACE_END
