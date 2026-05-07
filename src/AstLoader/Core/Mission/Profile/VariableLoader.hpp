///
/// @file      VariableLoader.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-01
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


/// @brief 加载参数
/// @param value 值
/// @param var 参数变量
/// @return 错误码
errc_t aLoadParameter(const Value& value, Variable& var);


/// @brief 加载计算对象
/// @param value 值
/// @param var 计算对象变量
/// @param scope 作用域
/// @return 错误码
errc_t aLoadCalcObject(const Value& value, Variable& var, Object* scope);


/// @brief 加载属性
/// @param value 值
/// @param var 属性变量
/// @param scope 作用域
/// @return 错误码
errc_t aLoadAttribute(const Value& value, Variable& var, Object* scope);


/*! @} */

AST_NAMESPACE_END
