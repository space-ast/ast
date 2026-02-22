///
/// @file      RunTimeXYS.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
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
    @addtogroup Data
    @{
*/



/// @brief 获取给定时间点的IAU 理论XYS值（IERS 2010规范）
/// @param tp 时间点
/// @param xys iau xys数据
AST_CORE_API void aTheoreticalXYS_IERS2010(const TimePoint& tp, array3d& xys);


/// @brief 获取给定时间点的IAU 理论XYS值（IERS 2010规范）
/// @param t 时间点（距离J2000.0 TT的儒略日世纪数）
/// @param xys iau xys数据
AST_CORE_API void aTheoreticalXYS_IERS2010(double t, array3d& xys);
AST_CORE_API void aTheoreticalXYS_IERS2010_Cache(double t, array3d& xys);


/// @brief 获取给定时间点的IAU 理论XYS值（IERS 2010规范）
/// @param jdTT 时间点的儒略日数（TT）
/// @param xys iau xys数据
AST_CORE_CAPI void aTheoreticalXYS_IERS2010_TT(const JulianDate& jdTT, array3d& xys);


/// @brief 获取给定时间点的IAU 理论XYS值（预计算数据）
AST_CORE_CAPI err_t aTheoreticalXYS_IERS2010Precomputed(const TimePoint& tp, array3d& xys);



/// @brief 获取给定时间点的IAU 理论XYS值
/// @param tp 时间点
/// @param xys iau xys数据
AST_CORE_CAPI void aTheoreticalXYS(const TimePoint& tp, array3d& xys);


/// @brief 获取给定时间点的IAU 理论XYS值的修正项
/// @param tp 时间点
/// @param xyCorrection iau xys修正项
AST_CORE_CAPI void aXYCorrection(const TimePoint& tp, array2d& xyCorrection);


/// @brief 获取给定时间点的IAU XYS值（IERS 2010规范，包含修正项）
/// @param tp 时间点
/// @param xys iau xys数据
AST_CORE_CAPI void aXYS_IERS2010(const TimePoint& tp, array3d& xys);


/// @brief 获取给定时间点的IAU XYS值（包含修正项）
/// @param tp 时间点
/// @param xys iau xys数据
AST_CORE_CAPI void aXYS(const TimePoint& tp, array3d& xys);


/*! @} */

AST_NAMESPACE_END
