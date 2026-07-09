///
/// @file      OrbitParam.hpp
/// @brief     轨道参数转换与计算接口
/// @details   提供各种轨道参数之间的转换函数，包括近地点/远地点高度/半径、周期、平均角速度、偏心率等参数的相互转换。
/// @author    axel
/// @date      19.11.2025
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Orbit
    @{
*/


/*!
轨道参数与其常用缩写

|     轨道参数      |                 全称              | 常用缩写      |
| -----------------| --------------------------------- | -------       |
| 远地点高度        | apoapsis altitude                 | apoAlt        |
| 近地点高度        | periapsis altitude                | periAlt       |
| 远地点半径        | apoapsis radius                   | apoRad        |
| 近地点半径        | periapsis radius                  | periRad       |
| 轨道周期          | period                            | period        |
| 平均角速度        | mean motion                       | meanMotion    |
| 偏心率            | eccentricity                      | ecc           |
| 长半轴            | semi-major axis                   | sma           |
| 近地点幅角        | argument of perigee               | argPeri       |
| 升交点赤经        | right ascension of ascending node | raan          |
| 真近点角          | true anomaly                     | trueAnomaly    |
| 平近点角          | mean anomaly                     | meanAnomaly    |
| 偏近点角          | eccentric anomaly                | eccAnomaly     |
| 纬度幅角          | argument of latitude             | `argLat`，`AOP`|
| 真近点经度        | true longitude                   | trueLong       |
| 近地点经度        | longitude of perigee             | longPeri       |   
| 过近心点后时间    | time past periapsis              | `TPP`,`timePastPeri` |
| 过升交点后时间    | time past ascending node         | `TPAN`,`timePastAscNode` |
| 扁率             | flattening factor                | flatFactor      |
| 引力参数          | gravitational parameter          | gm            |
| 天体半径          | body radius                      | bodyRadius    |
| 天体自转角速度     | body rotation rate              | bodyRotRate   |    

peri- 近
apo-  远
ge- 地球
apsis 拱点

periapsis: 近拱点
perigee: 近地点，绕地球（Geo） 转
perihelion: 近日点，绕太阳（Helios） 转
perilune或Periselene: 近月点，绕月球（Luna/Selene） 转
perijove: 近木点，绕木星（Jove/Jupiter） 转
periastron: 近星点，绕恒星（Aster）转（通常在双星系统中使用）
*/


/// @brief 远地点高度转换为远地点半径
/// @param apogeeAlt 远地点高度 [m]
/// @param bodyRadius 中心天体半径 [m]
/// @return 远地点半径 [m]
inline double    aApoAltToApoRad    (double apogeeAlt, double bodyRadius);

/// @brief 远地点高度转换为平均角速度
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aApoAltToMeanMotion  (double apogeeAlt, double eccentricity, double bodyRadius, double gm);

/// @brief 远地点高度转换为近地点高度
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aApoAltToPeriAlt   (double apogeeAlt, double eccentricity, double bodyRadius);

/// @brief 远地点高度转换为近地点半径
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 近地点半径 [m]
AST_CORE_CAPI double    aApoAltToPeriRad   (double apogeeAlt, double eccentricity, double bodyRadius);

/// @brief 远地点高度转换为轨道周期
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aApoAltToPeriod    (double apogeeAlt, double eccentricity, double bodyRadius, double gm);

/// @brief 远地点高度转换为长半轴
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 长半轴 [m]
AST_CORE_CAPI double    aApoAltToSMA    (double apogeeAlt, double eccentricity, double bodyRadius);

/// @brief 远地点半径转换为远地点高度
/// @param apogeeRad 远地点半径 [m]
/// @param bodyRadius 中心天体半径 [m]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aApoRadToApoAlt    (double apogeeRad, double bodyRadius);

/// @brief 远地点半径转换为平均角速度
/// @param apogeeRad 远地点半径 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aApoRadToMeanMotion  (double apogeeRad, double eccentricity, double gm);

/// @brief 远地点半径转换为近地点高度
/// @param apogeeRad 远地点半径 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aApoRadToPeriAlt   (double apogeeRad, double eccentricity, double bodyRadius);

/// @brief 远地点半径转换为轨道周期
/// @param apogeeRad 远地点半径 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aApoRadToPeriod    (double apogeeRad, double eccentricity, double gm);

/// @brief 远地点半径转换为近地点半径
/// @param apogeeRad 远地点半径 [m]
/// @param eccentricity 偏心率
/// @return 近地点半径 [m]
AST_CORE_CAPI double    aApoRadToPeriRad   (double apogeeRad, double eccentricity);


/// @brief 远地点半径转换为偏心率
/// @param apogeeRad 远地点半径 [m]
/// @param semiMajorAxis 长半轴 [m]
/// @return 偏心率
inline double aApoRadToEcc(double apogeeRad, double semiMajorAxis);

/// @brief 远地点半径转换为长半轴
/// @param apogeeRad 远地点半径 [m]
/// @param eccentricity 偏心率
/// @return 长半轴 [m]
AST_CORE_CAPI double    aApoRadToSMA    (double apogeeRad, double eccentricity);

/// @brief 偏近点角转换为平近点角
/// @param eccAnomaly 偏近点角 [rad]
/// @param eccentricity 偏心率
/// @return 平近点角 [rad]
AST_CORE_CAPI double    aEccToMean         (double eccAnomaly, double eccentricity);

/// @brief 偏近点角转换为真近点角
/// @param eccAnomaly 偏近点角 [rad]
/// @param eccentricity 偏心率
/// @return 真近点角 [rad]
AST_CORE_CAPI double    aEccToTrue         (double eccAnomaly, double eccentricity);

/// @brief 偏近点角转换为过升交点后时间
/// @param eccAnomaly 偏近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 过升交点后时间 [s]
AST_CORE_CAPI double    aEccToTimePastAscNode         (double eccAnomaly, double argPeri, double semiMajorAxis, double eccentricity, double gm);

/// @brief 偏近点角转换为过近心点后时间
/// @param eccAnomaly 偏近点角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 过近心点后时间 [s]
AST_CORE_CAPI double    aEccToTimePastPeri          (double eccAnomaly, double semiMajorAxis, double eccentricity, double gm);

/// @brief 平均角速度转换为远地点高度
/// @param meanMotion 平均角速度 [rad/s]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aMeanMotionToApoAlt  (double meanMotion, double eccentricity, double bodyRadius, double gm);

/// @brief 平均角速度转换为远地点半径
/// @param meanMotion 平均角速度 [rad/s]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aMeanMotionToApoRad  (double meanMotion, double eccentricity, double gm);

/// @brief 平均角速度转换为近地点高度
/// @param meanMotion 平均角速度 [rad/s]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aMeanMotionToPeriAlt (double meanMotion, double eccentricity, double bodyRadius, double gm);

/// @brief 平均角速度转换为近地点半径
/// @param meanMotion 平均角速度 [rad/s]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点半径 [m]
AST_CORE_CAPI double    aMeanMotionToPeriRad (double meanMotion, double eccentricity, double gm);

/// @brief 平均角速度转换为轨道周期
/// @param meanMotn 平均角速度 [rad/s]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aMeanMotionToPeriod  (double meanMotn);

/// @brief 平均角速度转换为长半轴
/// @param meanMotn 平均角速度 [rad/s]
/// @param gm 引力参数 [m^3/s^2]
/// @return 长半轴 [m]
AST_CORE_CAPI double    aMeanMotionToSMA  (double meanMotn, double gm);

/// @brief 平近点角转换为偏近点角（使用牛顿迭代法）
/// @param meanAnomaly 平近点角 [rad]
/// @param eccentricity 偏心率
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 偏近点角 [rad]
AST_CORE_CAPI double    aMeanToEcc         (double meanAnomaly, double eccentricity, double eps = 1e-14, int maxIter = 100);

/// @brief 平近点角转换为过升交点后时间
/// @param meanAnomaly 平近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 过升交点后时间 [s]
AST_CORE_CAPI double    aMeanToTimePastAscNode        (double meanAnomaly, double argPeri, double semiMajorAxis, double eccentricity, double gm);

/// @brief 平近点角转换为过近心点后时间
/// @param meanAnomaly 平近点角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 过近心点后时间 [s]
AST_CORE_CAPI double    aMeanToTimePastPeri         (double meanAnomaly, double semiMajorAxis, double gm);

/// @brief 平近点角转换为真近点角
/// @param meanAnomaly 平近点角 [rad]
/// @param eccentricity 偏心率
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 真近点角 [rad]
AST_CORE_CAPI double    aMeanToTrue        (double meanAnomaly, double eccentricity, double eps = 1e-14, int maxIter = 100);

/// @brief 近地点高度转换为远地点高度
/// @param perigeeAlt 近地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aPeriAltToApoAlt   (double perigeeAlt, double eccentricity, double bodyRadius);

/// @brief 近地点高度转换为远地点半径
/// @param perigeeAlt 近地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aPeriAltToApoRad   (double perigeeAlt, double eccentricity, double bodyRadius);

/// @brief 近地点高度转换为平均角速度
/// @param perigeeAlt 近地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aPeriAltToMeanMotion (double perigeeAlt, double eccentricity, double bodyRadius, double gm);

/// @brief 近地点高度转换为近地点半径
/// @param perigeeAlt 近地点高度 [m]
/// @param bodyRadius 中心天体半径 [m]
/// @return 近地点半径 [m]
AST_CORE_CAPI double    aPeriAltToPeriRad  (double perigeeAlt, double bodyRadius);

/// @brief 近地点高度转换为轨道周期
/// @param perigeeAlt 近地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aPeriAltToPeriod   (double perigeeAlt, double eccentricity, double bodyRadius, double gm);

/// @brief 近地点高度转换为长半轴
/// @param perigeeAlt 近地点高度 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 长半轴 [m]
AST_CORE_CAPI double    aPeriAltToSMA   (double perigeeAlt, double eccentricity, double bodyRadius);

/// @brief 近地点半径转换为远地点高度
/// @param perigeeRad 近地点半径 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aPeriRadToApoAlt   (double perigeeRad, double eccentricity, double bodyRadius);

/// @brief 近地点半径转换为远地点半径
/// @param perigeeRad 近地点半径 [m]
/// @param eccentricity 偏心率
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aPeriRadToApoRad   (double perigeeRad, double eccentricity);

/// @brief 近地点半径转换为平均角速度
/// @param perigeeRad 近地点半径 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aPeriRadToMeanMotion (double perigeeRad, double eccentricity, double gm);

/// @brief 近地点半径转换为近地点高度
/// @param perigeeRad 近地点半径 [m]
/// @param bodyRadius 中心天体半径 [m]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aPeriRadToPeriAlt  (double perigeeRad, double bodyRadius);

/// @brief 近地点半径转换为轨道周期
/// @param perigeeRad 近地点半径 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aPeriRadToPeriod   (double perigeeRad, double eccentricity, double gm);

/// @brief 近地点半径转换为长半轴
/// @param perigeeRad 近地点半径 [m]
/// @param eccentricity 偏心率
/// @return 长半轴 [m]
AST_CORE_CAPI double    aPeriRadToSMA   (double perigeeRad, double eccentricity);


/// @brief 近地点半径转换为偏心率
/// @param perigeeRad 近地点半径 [m]
/// @param semiMajorAxis 长半轴 [m]
/// @return 偏心率
inline double aPeriRadToEcc(double perigeeRad, double semiMajorAxis);

/// @brief 轨道周期转换为远地点高度
/// @param period 轨道周期 [s]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aPeriodToApoAlt    (double period, double eccentricity, double bodyRadius, double gm);

/// @brief 轨道周期转换为远地点半径
/// @param period 轨道周期 [s]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aPeriodToApoRad    (double period, double eccentricity, double gm);

/// @brief 轨道周期转换为平均角速度
/// @param period 轨道周期 [s]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aPeriodToMeanMotion  (double period);

/// @brief 轨道周期转换为近地点高度
/// @param period 轨道周期 [s]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aPeriodToPeriAlt   (double period, double eccentricity, double bodyRadius, double gm);

/// @brief 轨道周期转换为近地点半径
/// @param period 轨道周期 [s]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点半径 [m]
AST_CORE_CAPI double    aPeriodToPeriRad   (double period, double eccentricity, double gm);

/// @brief 轨道周期转换为长半轴
/// @param period 轨道周期 [s]
/// @param gm 引力参数 [m^3/s^2]
/// @return 长半轴 [m]
AST_CORE_CAPI double    aPeriodToSMA    (double period, double gm);

/// @brief 根据近地点和远地点半径计算偏心率
/// @param perigeeRad 近地点半径 [m]
/// @param apogeeRad 远地点半径 [m]
/// @return 偏心率
AST_CORE_CAPI double aPeriRadApoRadToEcc        (double perigeeRad, double apogeeRad);

/// @brief 根据近地点和远地点高度计算偏心率
/// @param perigeeAlt 近地点高度 [m]
/// @param apogeeAlt 远地点高度 [m]
/// @param bodyRadius 中心天体半径 [m]
/// @return 偏心率
AST_CORE_CAPI double aPeriAltApoAltToEcc        (double perigeeAlt, double apogeeAlt, double bodyRadius);

/// @brief 长半轴转换为远地点高度
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aSMAToApoAlt    (double semiMajorAxis, double eccentricity, double bodyRadius);

/// @brief 长半轴转换为远地点半径
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aSMAToApoRad    (double semiMajorAxis, double eccentricity);

/// @brief 长半轴转换为平均角速度
/// @param semiMajorAxis 长半轴 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aSMAToMeanMotion  (double semiMajorAxis, double gm);

/// @brief 长半轴转换为近地点高度
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param bodyRadius 中心天体半径 [m]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aSMAToPeriAlt   (double semiMajorAxis, double eccentricity, double bodyRadius);

/// @brief 长半轴转换为近地点半径
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @return 近地点半径 [m]
inline double    aSMAToPeriRad   (double semiMajorAxis, double eccentricity);

/// @brief 长半轴转换为轨道周期
/// @param semiMajorAxis 长半轴 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aSMAToPeriod    (double semiMajorAxis, double gm);

/// @brief 长半轴转换为短半轴
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @return 短半轴 [m]
AST_CORE_CAPI double    aSMAToSMinAx    (double semiMajorAxis, double eccentricity);

/// @brief 长半轴转换为半通径参数
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @return 半通径 [m]
AST_CORE_CAPI double    aSMAToSParam    (double semiMajorAxis, double eccentricity);

/// @brief 短半轴转换为长半轴
/// @param semiminorAxis 短半轴 [m]
/// @param eccentricity 偏心率
/// @return 长半轴 [m]
AST_CORE_CAPI double    aSMinAxToSMA    (double semiminorAxis, double eccentricity);

/// @brief 过升交点后时间转换为偏近点角
/// @param TimePastAscNode 过升交点后时间 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 偏近点角 [rad]
AST_CORE_CAPI double    aTimePastAscNodeToEcc         (double TimePastAscNode, double argPeri, double semiMajorAxis,  double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);

/// @brief 过升交点后时间转换为平近点角
/// @param TimePastAscNode 过升交点后时间 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 平近点角 [rad]
AST_CORE_CAPI double    aTimePastAscNodeToMean        (double TimePastAscNode, double argPeri, double semiMajorAxis,  double eccentricity, double gm);

/// @brief 过升交点后时间转换为过近心点后时间
/// @param TimePastAscNode 过升交点后时间 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 过近心点后时间 [s]
AST_CORE_CAPI double    aTimePastAscNodeToTimePastPeri         (double TimePastAscNode, double argPeri, double semiMajorAxis,  double eccentricity, double gm);

/// @brief 过升交点后时间转换为真近点角
/// @param TimePastAscNode 过升交点后时间 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 真近点角 [rad]
AST_CORE_CAPI double    aTimePastAscNodeToTrue        (double TimePastAscNode, double argPeri, double semiMajorAxis,  double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);

/// @brief 过近心点后时间转换为偏近点角
/// @param TimePastPeri 过近心点后时间 [s]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 偏近点角 [rad]
AST_CORE_CAPI double    aTimePastPeriToEcc          (double TimePastPeri, double semiMajorAxis, double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);

/// @brief 过近心点后时间转换为平近点角
/// @param TimePastPeri 过近心点后时间 [s]
/// @param semiMajorAxis 长半轴 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 平近点角 [rad]
AST_CORE_CAPI double    aTimePastPeriToMean         (double TimePastPeri, double semiMajorAxis, double gm);

/// @brief 过近心点后时间转换为真近点角
/// @param TimePastPeri 过近心点后时间 [s]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 真近点角 [rad]
AST_CORE_CAPI double    aTimePastPeriToTrue         (double TimePastPeri, double semiMajorAxis, double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);

/// @brief 过近心点后时间转换为过升交点后时间
/// @param TimePastPeri 过近心点后时间 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 过升交点后时间 [s]
AST_CORE_CAPI double    aTimePastPeriToTimePastAscNode         (double TimePastPeri, double argPeri, double semiMajorAxis,  double eccentricity, double gm);

/// @brief 真近点角转换为偏近点角
/// @param trueAnomaly 真近点角 [rad]
/// @param eccentricity 偏心率
/// @return 偏近点角 [rad]
AST_CORE_CAPI double    aTrueToEcc         (double trueAnomaly, double eccentricity);

/// @brief 真近点角转换为平近点角
/// @param trueAnomaly 真近点角 [rad]
/// @param eccentricity 偏心率
/// @return 平近点角 [rad]
AST_CORE_CAPI double    aTrueToMean        (double trueAnomaly, double eccentricity);

/// @brief 真近点角转换为过升交点后时间
/// @param trueAnomaly 真近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 过升交点后时间 [s]
AST_CORE_CAPI double    aTrueToTimePastAscNode        (double trueAnomaly, double argPeri, double semiMajorAxis,  double eccentricity, double gm);

/// @brief 真近点角转换为过近心点后时间
/// @param trueAnomaly 真近点角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 过近心点后时间 [s]
AST_CORE_CAPI double    aTrueToTimePastPeri         (double trueAnomaly, double semiMajorAxis, double eccentricity, double gm);

/// @brief 真近点角转换为纬度幅角
/// @param trueAnomaly 真近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @return 纬度幅角 [rad]
inline double    aTrueToArgLat      (double trueAnomaly, double argPeri);

/// @brief 纬度幅角转换为真近点角
/// @param argLat 纬度幅角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @return 真近点角 [rad]
inline double aArgLatToTrue(double argLat, double argPeri);

/// @brief 真近点角转换为真近点经度
/// @param trueAnomaly 真近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @param raan 升交点赤经 [rad]
/// @return 真近点经度 [rad]
A_ALWAYS_INLINE double    aTrueToTrueLong    (double trueAnomaly, double argPeri, double raan){return trueAnomaly + argPeri + raan;}

/// @brief 近地点幅角转换为近地点经度
/// @param argPeri 近地点幅角 [rad]
/// @param raan 升交点赤经 [rad]
/// @return 近地点经度 [rad]
A_ALWAYS_INLINE double    aArgPeriToLongPeri (double argPeri, double raan){return argPeri + raan;}


/// @brief 升交点赤经转换为升交点经度
/// @param raan 升交点赤经 [rad]
/// @param inertialAxes 定义升交点的惯性系
/// @param timeOfAscNodePassage 过升交点时刻
/// @param bodyFixedAxes 天体固连系
/// @return 升交点经度 [rad]
AST_CORE_CAPI double aRAANToLAN(double raan, Axes* inertialAxes, const TimePoint& timeOfAscNodePassage, Axes* bodyFixedAxes);


/// @brief 升交点经度转换为升交点赤经
/// @param lan 升交点经度 [rad]
/// @param bodyFixedAxes 天体固连系
/// @param timeOfAscNodePassage 过升交点时刻
/// @param inertialAxes 定义升交点的惯性系
/// @return 升交点赤经 [rad]
AST_CORE_CAPI double aLANToRAAN(double lan, Axes* bodyFixedAxes, const TimePoint& timeOfAscNodePassage, Axes* inertialAxes);


/// @brief 计算地面轨迹重复所需的轨道长半轴
/// @param daysToRepeat 重复天数
/// @param revsToRepeat 重复圈数
/// @param gm 引力参数 [m^3/s^2]
/// @param bodyRotRate 中心天体自转角速度 [rad/s]
/// @return 满足地面轨迹重复条件的轨道长半轴 [m]
AST_CORE_CAPI double    aRepeatGroundTrackSMA     (int daysToRepeat, int revsToRepeat, double gm, double bodyRotRate);

/// @brief 偏心率转换为扁率
/// @param eccentricity 偏心率
/// @return 扁率
AST_CORE_CAPI double    aEccToFlat         (double eccentricity);

/// @brief 扁率转换为偏心率
/// @param flatFactor 扁率
/// @return 偏心率
AST_CORE_CAPI double    aFlatToEcc         (double flatFactor);


inline double aApoRadToEcc(double apogeeRad, double semiMajorAxis)
{
    return (apogeeRad - semiMajorAxis) / semiMajorAxis;
}

inline double aPeriRadToEcc(double perigeeRad, double semiMajorAxis)
{
    return (semiMajorAxis - perigeeRad) / semiMajorAxis;
}

inline double aApoAltToApoRad(double apogeeAlt, double bodyRadius)
{
    return apogeeAlt + bodyRadius;
}

inline double aSMAToPeriRad(double semiMajorAxis, double eccentricity)
{
    return semiMajorAxis * (1 - eccentricity);
}

inline double aTrueToArgLat(double trueAnomaly, double argPeri)
{
    return trueAnomaly + argPeri;
}

inline double aArgLatToTrue(double argLat, double argPeri)
{
    return argLat - argPeri;
}

AST_NAMESPACE_END