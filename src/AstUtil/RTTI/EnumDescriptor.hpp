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
    using NumberType = int;                                             // 枚举值类型
    using ValueList = std::vector<std::pair<NumberType, std::string>>;  // 枚举值列表
public:
    EnumDescriptor() = default;
    ~EnumDescriptor() = default;

    EnumDescriptor(const ValueList& values) : values_(values) {}

public:
    const std::string* nameOf(NumberType value) const;

    const NumberType* valueOf(StringView name) const;
private:
    ValueList values_;  ///< 枚举值列表
};


/*! @} */

AST_NAMESPACE_END
