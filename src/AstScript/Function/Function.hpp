///
/// @file      Function.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-19
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
#include "AstScript/Macro.hpp"

AST_NAMESPACE_BEGIN

/// @brief 函数对象基类
/// @details
/// 函数对象用于表示脚本中的函数定义，可以包含参数和函数体。
/// 函数是一种特殊的值对象
/// 函数的特点是“可以被调用，且会将输入的值转换为另一个值”
/// 因为值也是一种表达式，所以函数是一种特殊的宏，其输出的表达式是一个值对象
/// @ingroup Script
class Function: public Macro
{
public:
    AST_EXPR(Function)

    using Macro::Macro;
    ~Function() override = default;
};



AST_NAMESPACE_END
