///
/// @file      ODEStepHandler.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

AST_NAMESPACE_BEGIN

enum EODEAction
{
    eContinue = 0,
    eStop = 1,
};

/// @brief ODE积分步长处理类
class ODEStepHandler
{
public:
    virtual ~ODEStepHandler() = default;

    /// @brief 积分步长处理函数
    /// @param y 当前状态向量
    /// @param x 当前积分变量
    virtual EODEAction handleStep(const double* y, double x) = 0;

    /// 兼容性处理
    A_ALWAYS_INLINE
    EODEAction handleStep(double x, const double* y){return handleStep(y, x);}
};

AST_NAMESPACE_END
