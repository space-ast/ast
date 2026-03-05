///
/// @file      AxesTransform.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-04
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

/*!
    @addtogroup Coordinate
    @{
*/

class Axes;


/// @brief      根轴系
AST_CORE_CAPI Axes* aAxesRoot();

/// @brief      国际天球参考系轴系（ICRF）
AST_CORE_CAPI Axes* aAxesICRF();

/// @brief      地球固连参考系轴系（ECF）
AST_CORE_CAPI Axes* aAxesECF();

/// @brief      J2000 轴系
AST_CORE_CAPI Axes* aAxesJ2000();

/// @brief       MOD 轴系
AST_CORE_CAPI Axes* aAxesMOD();

/// @brief       TOD 轴系
AST_CORE_CAPI Axes* aAxesTOD();

/// @brief       GTOD 轴系
AST_CORE_CAPI Axes* aAxesGTOD();


/*! @} */

AST_NAMESPACE_END
