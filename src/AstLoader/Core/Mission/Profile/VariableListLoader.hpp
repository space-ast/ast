///
/// @file      VariableListLoader.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-30
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class VariableList;


/// @brief 加载参数列表
/// @param value 值
/// @param list 参数列表
/// @return 错误码
/// @note 该函数会清空已有参数列表
errc_t aLoadParameters(const Value& value, VariableList& list);


/// @brief 加载属性列表
/// @param value 值
/// @param list 属性列表
/// @param scope 作用域
/// @return 错误码
/// @note 该函数会清空已有属性列表
errc_t aLoadAttributes(const Value& value, VariableList& list, Object* scope);


/// @brief 加载计算对象列表
/// @param value 值
/// @param list 计算对象列表
/// @param scope 作用域
/// @return 错误码
/// @note 该函数会清空已有计算对象列表
errc_t aLoadCalcObjects(const Value& value, VariableList& list, Object* scope);


/*! @} */

AST_NAMESPACE_END


