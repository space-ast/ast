///
/// @file      DataAPI.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-24
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


/// @brief 获取UTC时间的闰秒数
/// @param jdUTC 
/// @return 
AST_CORE_CAPI double aLeapSecondUTC(double jdUTC);

/// @brief 获取UTC时间的闰秒数（MJD）
/// @param mjdUTC 
/// @return 
AST_CORE_CAPI double aLeapSecondUTCMJD(double mjdUTC);

/// @brief 获取JPL DE星历数据的位置和速度（ICRF）
/// @param time 
/// @param target 
/// @param referenceBody 
/// @param pos 
/// @param vel 
/// @return 
AST_CORE_CAPI err_t aJplDeGetPosVelICRF(
    const TimePoint& time, 
    int target, 
    int referenceBody, 
    Vector3d& pos,
    Vector3d& vel
);

/// @brief 获取JPL DE星历数据的位置（ICRF）
/// @param time 
/// @param target 
/// @param referenceBody 
/// @param pos 
/// @return 
AST_CORE_CAPI err_t aJplDeGetPosICRF(
    const TimePoint& time,
    int target,
    int referenceBody,
    Vector3d& pos
);


/// @brief 获取JPL DE星历数据的中章动角数据
/// @param time 
/// @param dpsi 
/// @param deps 
/// @return 
AST_CORE_CAPI err_t aJplDeGetNutation(
    const TimePoint& time,
    double& dpsi,
    double& deps
);

/// @brief 获取JPL DE星历数据的月面天平动相关角度
/// @param time 
/// @param ang  - 从ICRF到月球惯性主轴坐标系的转角(rad)
///             - [omega,i,u],[进动角,章动角,自转角],按照313顺序旋转
/// @return 
AST_CORE_API err_t aJplDeGetLibration(
    const TimePoint& time,
    Vector3d& ang
);


/// @brief 获取JPL DE星历数据的月面天平动相关角度（Euler）
/// @param time 
/// @param ang  - 从ICRF到月球惯性主轴坐标系的转角(rad)
///             - [omega,i,u],[进动角,章动角,自转角],按照313顺序旋转
/// @return 
AST_CORE_API err_t aJplDeGetLibration(
    const TimePoint& time,
    Euler& ang
);


/// @brief 获取JPL DE星历数据的版本号
/// @return 星历版本号
AST_CORE_CAPI int aJplDeNum();


/// @brief 打开JPL DE星历数据文件
/// @param filepath 
/// @return 
AST_CORE_CAPI err_t aJplDeOpen(const char* filepath);



/// @brief 关闭JPL DE星历数据文件
AST_CORE_CAPI void aJplDeClose();


/// @brief 获取UT1时间与UTC时间的差值（秒）
/// @param jdUTC 协调世界时（UTC）的儒略日数
/// @return UT1时间与UTC时间的差值（秒）
AST_CORE_CAPI double aUT1MinusUTC_UTC(const JulianDate& jdUTC);


struct PoleMotion
{
    double x, y;            ///< 极移
};

/// @brief 获取给定时间点的极移
/// @param tp 时间点
/// @param x 极移x
/// @param y 极移y
AST_CORE_CAPI void aPoleMotion(const TimePoint& tp, double& x, double& y);


/// @brief 获取给定UTC时间的极移
/// @param jdUTC 协调世界时（UTC）的儒略日数
/// @param x 极移x
/// @param y 极移y
AST_CORE_CAPI void aPoleMotionUTC(const JulianDate& jdUTC, double& x, double& y);


/// @brief 获取给定时间点的极移
/// @param tp 时间点
/// @param pm 极移
A_ALWAYS_INLINE void aPoleMotion(const TimePoint& tp, PoleMotion& pm)
{
    return aPoleMotion(tp, pm.x, pm.y);
}



/// @brief 获取给定时间点的LOD（秒）
/// @param tp 时间点
/// @return LOD（秒）
AST_CORE_CAPI double aLOD(const TimePoint& tp);


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



AST_NAMESPACE_END
