///
/// @file      AgentUtils.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-27
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
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class JsonValue;

/// @brief 将对象转换为简化的JSON格式
/// @details 包含对象的名称、类型、ID和父作用域ID
/// @param obj 对象指针
/// @return 简化的JSON对象
AST_AGENT_API JsonValue aObjectToBriefJson(Object* obj);

/// @brief 将对象转换为JSON值
/// @param object 对象
/// @param maxDepth 最大递归深度
/// @return JSON值
AST_AGENT_API JsonValue aObjectToJson(Object* object, int maxDepth = 20);


/// @brief 获取类的JSON Schema
/// @param cls 类指针
/// @param maxDepth 最大递归深度
/// @return 类的JSON Schema
AST_AGENT_API JsonValue aClassJsonSchema(Class* cls, int maxDepth = 20);


/// @brief 设置对象属性
/// @param object 对象
/// @param attr 属性名
/// @param valueJson 属性值JSON值
/// @return 错误码
/// @note 支持整数、浮点数、布尔值和字符串属性
AST_AGENT_API errc_t aObject_SetAttribute(Object& object, StringView attr, const JsonValue& valueJson);



/*! @} */

AST_NAMESPACE_END
