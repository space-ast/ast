///
/// @file      IteratePredefined.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-26
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



/// @brief 获取迭代函数指针
/// @param type 迭代对象类型
/// @return 迭代函数指针
IterateFunc iterate_get_func(Class* type);


/// @brief 迭代器开始
/// @param container 迭代对象
/// @param index 迭代索引
/// @return 迭代器开始值
Value* iterate_begin(Value* container, int& index);

/// @brief 迭代器下一个
/// @param container 迭代对象
/// @param index 迭代索引
/// @return 迭代器下一个值
Value* iterate_next(Value* container, int& index);


AST_NAMESPACE_END
