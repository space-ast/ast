///
/// @file      OpUnaryPredefined.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-22
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
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstScript/ScriptAPI.hpp"

AST_NAMESPACE_BEGIN

class Value;

/// @brief 执行一元运算
/// @param op 运算类型
/// @param value 运算数
/// @return 运算结果
Value* opunary(OpUnaryType op, Value* value);

/// @brief 获取一元运算函数指针
/// @param op 运算类型
/// @param type 运算数类型
/// @return 运算函数指针
AST_SCRIPT_CAPI OpUnaryFunc opunary_get_func(OpUnaryType op, Class* type);


AST_NAMESPACE_END