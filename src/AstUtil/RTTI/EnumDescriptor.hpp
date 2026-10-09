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

/// @brief 枚举描述符数据
class AST_UTIL_API EnumDescriptorData
{   
public:
    using NumberType = int;                                // 枚举值类型
    using ValueList = Span<std::pair<NumberType, char*>>;  // 枚举值列表
public:
    EnumDescriptorData() = default;
    ~EnumDescriptorData() = default;

    EnumDescriptorData(ValueList values) : values_(values) {}

public:
    const char* nameOf(NumberType value) const;

    NumberType valueOf(StringView name) const;

    NumberType valueOf(StringView name, bool& found) const;

private:
    ValueList values_;  ///< 枚举值列表
};

/// @brief 枚举描述符
template<typename EnumType>
class EnumDescriptor: public EnumDescriptorData
{
public:
    using EnumDescriptorData::EnumDescriptorData;
    static_assert(std::is_enum<EnumType>::value, "EnumType must be an enum type");
    static_assert(sizeof(NumberType) >= sizeof(EnumType), "sizeof(NumberType) must be at least sizeof(EnumType) in order to store EnumType");

    const char* nameOf(EnumType value) const
    {
        return EnumDescriptorData::nameOf(static_cast<NumberType>(value));
    }

    EnumType valueOf(StringView name) const
    {
        return static_cast<EnumType>(EnumDescriptorData::valueOf(name));
    }

    EnumType valueOf(StringView name, bool& found) const
    {
        return static_cast<EnumType>(EnumDescriptorData::valueOf(name, found));
    }
};

template<typename EnumType>
EnumDescriptor<EnumType>& aEnumDescriptor();


/// @brief 定义一个枚举项（枚举值 + 名称）
/// @details 仅用于 AST_ENUM_DESCRIPTOR() 的枚举项列表中
/// @param Enumerator 枚举项名称（不带枚举类型限定符）
/// @param Name       枚举项的名称字符串
#define AST_ENUM_VALUE(Enumerator, Name)  {static_cast<EnumDescriptorData::NumberType>(_EnumType::Enumerator), const_cast<char*>(Name)}

/// @brief 定义枚举描述符，即 aEnumDescriptor<EnumType>() 的特化
/// @param EnumType 枚举类型
/// @param ...      枚举项列表，每一项由 AST_ENUM_VALUE() 给出
#define AST_ENUM_DESCRIPTOR(EnumType, ...) \
    template<> \
    EnumDescriptor<EnumType>& aEnumDescriptor<EnumType>() \
    { \
        using _EnumType = EnumType; \
        static_assert(sizeof(EnumType) <= sizeof(EnumDescriptorData::NumberType), "size of " #EnumType " must be less than or equal to size of EnumDescriptorData::NumberType"); \
        static std::pair<EnumDescriptorData::NumberType, char*> _values[] = {__VA_ARGS__}; \
        static EnumDescriptor<EnumType> descriptor{_values}; \
        return descriptor; \
    }


/*! @} */

AST_NAMESPACE_END
