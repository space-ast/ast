///
/// @file      OrbitGeometry.hpp
/// @brief     轨道几何相关计算函数
/// @details   计算轨道面/轨道路径之间的几何关系，如法向量、半径、基向量、交线距离等。
/// @author    axel
/// @date      2026-08-19
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
    @ingroup   Core
    @defgroup  OrbitGeometry 轨道几何
    @brief     轨道面/轨道路径之间的几何关系计算。
*/

/*!
    @addtogroup OrbitGeometry
    @{
*/

/// @brief 计算轨道面单位法向量（惯性参考系）
/// @details 由轨道倾角与升交点赤经唯一确定轨道面姿态，法向量为
///           n = (sin i · sin Ω, -sin i · cos Ω, cos i)，即轨道角动量方向。
/// @param inc  轨道倾角 [rad]
/// @param raan 升交点赤经 [rad]
/// @param normal 输出：轨道面单位法向量
AST_CORE_CAPI void aOrbitPlaneNormal(double inc, double raan, Vector3d& normal);


/// @brief 计算真近点角处的轨道半径 [m]
/// @details 椭圆轨道极坐标方程 r = a(1-e²)/(1 + e·cosν)。
///          ν=0 为近拱点半径 a(1-e)，ν=π 为远拱点半径 a(1+e)。
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity  偏心率
/// @param trueAnomaly  真近点角 [rad]
/// @return 当前真近点角处到中心天体的距离 [m]
AST_CORE_CAPI double aOrbitRadius(double semiMajorAxis, double eccentricity, double trueAnomaly);


/// @brief 计算轨道 perifocal 基向量（近拱点/垂直方向）在惯性系中的表达
/// @see https://orbital-mechanics.space/classical-orbital-elements/perifocal-frame.html
/// @details perifocal 坐标系（P、Q、W）附着于轨道：P 指向近拱点，Q 在轨道面内
///          沿运动方向超前 P 90°，W = P × Q 为轨道面法向。
///          仅使用经典根数 i/raan/argper。
/// @param e 经典根数
/// @param p 输出：近拱点方向单位向量 P
/// @param q 输出：轨道面内超前 P 90° 的单位向量 Q
AST_CORE_CAPI void aOrbitPerifocalBasis(const OrbElem& e, Vector3d& p, Vector3d& q);


/// @brief 计算两轨道面公共交线对应的两个路径距离 [m]
/// @details 公共交线方向 L = n1 × n2（n 为轨道面单位法向）。交线在 +L 与 -L 两个方向
///          上各与两条轨道相交一次，得到两个距离值 distPlus、distMinus。
///          两轨道路径最短距离 = min(distPlus, distMinus)。
///          仅使用经典根数 a/e/i/raan/argper，与真近点角及引力常数无关。
/// @param orb1 轨道1经典根数
/// @param orb2 轨道2经典根数
/// @param distPlus  输出：+L 方向两交点距离 [m]
/// @param distMinus 输出：-L 方向两交点距离 [m]
/// @return eNoError 成功；两平面近共面（交线退化）返回 eErrorInvalidParam
AST_CORE_CAPI errc_t aOrbitNodeLineDistances(const OrbElem& orb1, const OrbElem& orb2, double& distPlus, double& distMinus);

                                             
/// @brief 计算两轨道路径之间的最短距离 [m]
/// @details 基于公共交线法；共面/近共面退化时改用一维共线假设的数值回退。
/// @param orb1 轨道1经典根数
/// @param orb2 轨道2经典根数
/// @param dist 输出：最短距离 [m]
/// @return eNoError 成功
AST_CORE_CAPI errc_t aOrbitPathMinDistance(const OrbElem& orb1, const OrbElem& orb2, double& dist);



/// @brief 计算本对象相对于另一对象轨道面距离在指定距离阈值内的时间区间
/// @param epoch 本对象的轨道历元
/// @param orbit 本对象的轨道根数
/// @param other 另一对象的轨道根数（仅用 i/raan 定义其轨道面，与历元无关）
/// @param gm 引力常数 [m^3/s^2]
/// @param threshold 到轨道面距离阈值 [m]
/// @param window 分析窗口：只生成落在窗口内的区间
/// @param result 输出：所有在距离阈值内的时间区间
/// @return eNoError 成功
AST_CORE_CAPI errc_t aOrbitPlaneProximityIntervals(
     const TimePoint& epoch,     
     const OrbElem& orbit,       
     const OrbElem& other,       
     double gm,                  
     double threshold,           
     const TimeInterval& window,
     TimeIntervalList& result
);

/// @brief 计算给定时刻的真近点角（开普勒二体线性推进）
/// @details 由 epoch 处平近点角加 n·Δt 线性推进，再转换为真近点角。
///          返回主值范围 (-π, π]。
/// @param epoch 轨道历元
/// @param orbit 轨道根数
/// @param gm 引力常数 [m^3/s^2]
/// @param time 查询时刻
/// @return 查询时刻的真近点角 [rad]
AST_CORE_CAPI double aOrbitTrueAnomaly(
    const TimePoint& epoch,
    const OrbElem& orbit,
    double gm,
    const TimePoint& time
);

/// @brief 计算给定时刻的轨道位置
/// @param epoch 轨道历元
/// @param orbit 轨道根数
/// @param gm 引力常数 [m^3/s^2]
/// @param time 查询时刻
/// @param position 输出：查询时刻的轨道位置
AST_CORE_CAPI void aOrbitPosition(
    const TimePoint& epoch,     
    const OrbElem& orbit,       
    double gm,                  
    const TimePoint& time,      
    Vector3d& position
);
AST_CORE_CAPI void aOrbitPosition2(
    const TimePoint& epoch,     
    const OrbElem& orbit,       
    double gm,                  
    const TimePoint& time,      
    Vector3d& position
);
AST_CORE_CAPI void aOrbitPosition3(
    const TimePoint& epoch,     
    const OrbElem& orbit,       
    double gm,                  
    const TimePoint& time,      
    Vector3d& position
);

/// @brief 计算给定时刻本对象到另一对象轨道面的有符号距离 [m]
/// @details 有符号距离 d = r·n2，为位置矢量在另一对象轨道面单位法向 n2 上的投影。
///          正值在 +n2 侧，负值在 -n2 侧，绝对值为垂直距离。
///          由 epoch 处轨道根数经开普勒二体传播得到真近点角，再计算面距。
/// @param epoch 本对象的轨道历元
/// @param orbit 本对象的轨道根数
/// @param other 另一对象的轨道根数（仅用 i/raan 定义其轨道面，与历元无关）
/// @param gm 引力常数 [m^3/s^2]
/// @param time 查询时刻
/// @param dist 输出：到轨道面的有符号距离 [m]
/// @return eNoError 成功
AST_CORE_CAPI errc_t aOrbitPlaneDistance(
     const TimePoint& epoch,
     const OrbElem& orbit,
     const OrbElem& other,
     double gm,
     const TimePoint& time,
     double& dist
);

/*! @} */

AST_NAMESPACE_END
