///
/// @file      ObjectProtocol.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
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
#include "AstUtil/StringView.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

#define _AST_OBJ_CALL static


/// @brief 对象协议
class AST_UTIL_API ObjectProtocol
{
public:
    /// @brief 设置对象属性字符串值
    /// @details id 对象索引/对象ID
    /// @param name 属性名
    /// @param value 属性值
    /// @return 错误码
    _AST_OBJ_CALL errc_t setAttrString(ObjectId id, StringView name, StringView value);

    /// @brief 获取对象属性字符串值
    /// @details id 对象索引/对象ID
    /// @param name 属性名
    /// @param value 属性值
    /// @return 错误码
    _AST_OBJ_CALL errc_t getAttrString(ObjectId id, StringView name, std::string& value);

    /// @brief 创建新对象
    /// @details typeName 对象类型名
    /// @return 对象ID
    _AST_OBJ_CALL ObjectId newObject(StringView typeName);
};

/*! @} */

AST_NAMESPACE_END


