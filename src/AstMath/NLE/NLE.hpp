///
/// @file      NLE.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-28
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

/*!
    @ingroup Math
    @defgroup NLE 非线性方程求解
    @brief 提供非线性方程求解相关功能
    @details
    本模块提供了非线性方程求解功能，包括：
    - secant 方法
    - ridder 方法
    - brent 方法
    - bisection 方法
*/

#include "AstMath/SecantSolver.hpp"
#include "AstMath/RidderSolver.hpp"
#include "AstMath/BrentSolver.hpp"
#include "AstMath/BisectionSolver.hpp"