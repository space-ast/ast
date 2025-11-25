///
/// @file      OrbitParam.hpp
/// @brief     轨道参数转换与计算接口
/// @details   提供各种轨道参数之间的转换函数，包括近地点/远地点高度/半径、周期、平均角速度、偏心率等参数的相互转换。
/// @author    jinke18
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


/// @brief 远地点高度转换为远地点半径
/// @param apogeeAlt 远地点高度 [m]
/// @param cbRadius 中心天体半径 [m]
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aApoAltToApoRad    (double apogeeAlt, double cbRadius);

/// @brief 远地点高度转换为平均角速度
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aApoAltToMeanMotn  (double apogeeAlt, double eccentricity, double cbRadius, double gm);

/// @brief 远地点高度转换为近地点高度
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aApoAltToPeriAlt   (double apogeeAlt, double eccentricity, double cbRadius);

/// @brief 远地点高度转换为近地点半径
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @return 近地点半径 [m]
AST_CORE_CAPI double    aApoAltToPeriRad   (double apogeeAlt, double eccentricity, double cbRadius);

/// @brief 远地点高度转换为轨道周期
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aApoAltToPeriod    (double apogeeAlt, double eccentricity, double cbRadius, double gm);

/// @brief 远地点高度转换为长半轴
/// @param apogeeAlt 远地点高度 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @return 长半轴 [m]
AST_CORE_CAPI double    aApoAltToSMajAx    (double apogeeAlt, double eccentricity, double cbRadius);

/// @brief 远地点半径转换为远地点高度
/// @param apogeeRad 远地点半径 [m]
/// @param cbRadius 中心天体半径 [m]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aApoRadToApoAlt    (double apogeeRad, double cbRadius);

/// @brief 远地点半径转换为平均角速度
/// @param apogeeRad 远地点半径 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aApoRadToMeanMotn  (double apogeeRad, double eccentricity, double gm);

/// @brief 远地点半径转换为近地点高度
/// @param apogeeRad 远地点半径 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aApoRadToPeriAlt   (double apogeeRad, double eccentricity, double cbRadius);

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

/// @brief 远地点半径转换为长半轴
/// @param apogeeRad 远地点半径 [m]
/// @param eccentricity 偏心率
/// @return 长半轴 [m]
AST_CORE_CAPI double    aApoRadToSMajAx    (double apogeeRad, double eccentricity);

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

/// @brief 偏近点角转换为近地点幅角时刻
/// @param eccAnomaly 偏近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点幅角时刻 [s]
AST_CORE_CAPI double    aEccToTPAN         (double eccAnomaly, double argPeri, double semiMajorAxis, double eccentricity, double gm);

/// @brief 偏近点角转换为近地点通过时刻
/// @param eccAnomaly 偏近点角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点通过时刻 [s]
AST_CORE_CAPI double    aEccToTPP          (double eccAnomaly, double semiMajorAxis, double eccentricity, double gm);

/// @brief 平均角速度转换为远地点高度
/// @param meanMotion 平均角速度 [rad/s]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aMeanMotnToApoAlt  (double meanMotion, double eccentricity, double cbRadius, double gm);

/// @brief 平均角速度转换为远地点半径
/// @param meanMotion 平均角速度 [rad/s]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aMeanMotnToApoRad  (double meanMotion, double eccentricity, double gm);

/// @brief 平均角速度转换为近地点高度
/// @param meanMotion 平均角速度 [rad/s]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aMeanMotnToPeriAlt (double meanMotion, double eccentricity, double cbRadius, double gm);

/// @brief 平均角速度转换为近地点半径
/// @param meanMotion 平均角速度 [rad/s]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点半径 [m]
AST_CORE_CAPI double    aMeanMotnToPeriRad (double meanMotion, double eccentricity, double gm);

/// @brief 平均角速度转换为轨道周期
/// @param meanMotn 平均角速度 [rad/s]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aMeanMotnToPeriod  (double meanMotn);

/// @brief 平均角速度转换为长半轴
/// @param meanMotn 平均角速度 [rad/s]
/// @param gm 引力参数 [m^3/s^2]
/// @return 长半轴 [m]
AST_CORE_CAPI double    aMeanMotnToSMajAx  (double meanMotn, double gm);

/// @brief 平近点角转换为偏近点角（使用牛顿迭代法）
/// @param meanAnomaly 平近点角 [rad]
/// @param eccentricity 偏心率
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 偏近点角 [rad]
AST_CORE_CAPI double    aMeanToEcc         (double meanAnomaly, double eccentricity, double eps = 1e-14, int maxIter = 100);

/// @brief 平近点角转换为近地点幅角时刻
/// @param meanAnomaly 平近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点幅角时刻 [s]
AST_CORE_CAPI double    aMeanToTPAN        (double meanAnomaly, double argPeri, double semiMajorAxis, double eccentricity, double gm);

/// @brief 平近点角转换为近地点通过时刻
/// @param meanAnomaly 平近点角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点通过时刻 [s]
AST_CORE_CAPI double    aMeanToTPP         (double meanAnomaly, double semiMajorAxis, double gm);

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
/// @param cbRadius 中心天体半径 [m]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aPeriAltToApoAlt   (double perigeeAlt, double eccentricity, double cbRadius);

/// @brief 近地点高度转换为远地点半径
/// @param perigeeAlt 近地点高度 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aPeriAltToApoRad   (double perigeeAlt, double eccentricity, double cbRadius);

/// @brief 近地点高度转换为平均角速度
/// @param perigeeAlt 近地点高度 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aPeriAltToMeanMotn (double perigeeAlt, double eccentricity, double cbRadius, double gm);

/// @brief 近地点高度转换为近地点半径
/// @param perigeeAlt 近地点高度 [m]
/// @param cbRadius 中心天体半径 [m]
/// @return 近地点半径 [m]
AST_CORE_CAPI double    aPeriAltToPeriRad  (double perigeeAlt, double cbRadius);

/// @brief 近地点高度转换为轨道周期
/// @param perigeeAlt 近地点高度 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aPeriAltToPeriod   (double perigeeAlt, double eccentricity, double cbRadius, double gm);

/// @brief 近地点高度转换为长半轴
/// @param perigeeAlt 近地点高度 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @return 长半轴 [m]
AST_CORE_CAPI double    aPeriAltToSMajAx   (double perigeeAlt, double eccentricity, double cbRadius);

/// @brief 近地点半径转换为远地点高度
/// @param perigeeRad 近地点半径 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aPeriRadToApoAlt   (double perigeeRad, double eccentricity, double cbRadius);

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
AST_CORE_CAPI double    aPeriRadToMeanMotn (double perigeeRad, double eccentricity, double gm);

/// @brief 近地点半径转换为近地点高度
/// @param perigeeRad 近地点半径 [m]
/// @param cbRadius 中心天体半径 [m]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aPeriRadToPeriAlt  (double perigeeRad, double cbRadius);

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
AST_CORE_CAPI double    aPeriRadToSMajAx   (double perigeeRad, double eccentricity);

/// @brief 轨道周期转换为远地点高度
/// @param period 轨道周期 [s]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aPeriodToApoAlt    (double period, double eccentricity, double cbRadius, double gm);

/// @brief 轨道周期转换为远地点半径
/// @param period 轨道周期 [s]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aPeriodToApoRad    (double period, double eccentricity, double gm);

/// @brief 轨道周期转换为平均角速度
/// @param period 轨道周期 [s]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aPeriodToMeanMotn  (double period);

/// @brief 轨道周期转换为近地点高度
/// @param period 轨道周期 [s]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aPeriodToPeriAlt   (double period, double eccentricity, double cbRadius, double gm);

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
AST_CORE_CAPI double    aPeriodToSMajAx    (double period, double gm);

/// @brief 根据近地点和远地点半径计算偏心率
/// @param perigeeRad 近地点半径 [m]
/// @param apogeeRad 远地点半径 [m]
/// @return 偏心率
AST_CORE_CAPI double    aRadiiToEcc        (double perigeeRad, double apogeeRad);

/// @brief 长半轴转换为远地点高度
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @return 远地点高度 [m]
AST_CORE_CAPI double    aSMajAxToApoAlt    (double semiMajorAxis, double eccentricity, double cbRadius);

/// @brief 长半轴转换为远地点半径
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @return 远地点半径 [m]
AST_CORE_CAPI double    aSMajAxToApoRad    (double semiMajorAxis, double eccentricity);

/// @brief 长半轴转换为平均角速度
/// @param semiMajorAxis 长半轴 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 平均角速度 [rad/s]
AST_CORE_CAPI double    aSMajAxToMeanMotn  (double semiMajorAxis, double gm);

/// @brief 长半轴转换为近地点高度
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param cbRadius 中心天体半径 [m]
/// @return 近地点高度 [m]
AST_CORE_CAPI double    aSMajAxToPeriAlt   (double semiMajorAxis, double eccentricity, double cbRadius);

/// @brief 长半轴转换为近地点半径
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @return 近地点半径 [m]
AST_CORE_CAPI double    aSMajAxToPeriRad   (double semiMajorAxis, double eccentricity);

/// @brief 长半轴转换为轨道周期
/// @param semiMajorAxis 长半轴 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 轨道周期 [s]
AST_CORE_CAPI double    aSMajAxToPeriod    (double semiMajorAxis, double gm);

/// @brief 长半轴转换为短半轴
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @return 短半轴 [m]
AST_CORE_CAPI double    aSMajAxToSMinAx    (double semiMajorAxis, double eccentricity);

/// @brief 长半轴转换为半通径参数
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @return 半通径 [m]
AST_CORE_CAPI double    aSMajAxToSParam    (double semiMajorAxis, double eccentricity);

/// @brief 短半轴转换为长半轴
/// @param semiminorAxis 短半轴 [m]
/// @param eccentricity 偏心率
/// @return 长半轴 [m]
AST_CORE_CAPI double    aSMinAxToSMajAx    (double semiminorAxis, double eccentricity);

/// @brief 近地点幅角时刻转换为偏近点角
/// @param TPAN 近地点幅角时刻 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 偏近点角 [rad]
AST_CORE_CAPI double    aTPANToEcc         (double TPAN, double argPeri, double semiMajorAxis,  double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);

/// @brief 近地点幅角时刻转换为平近点角
/// @param TPAN 近地点幅角时刻 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 平近点角 [rad]
AST_CORE_CAPI double    aTPANToMean        (double TPAN, double argPeri, double semiMajorAxis,  double eccentricity, double gm);

/// @brief 近地点幅角时刻转换为近地点通过时刻
/// @param TPAN 近地点幅角时刻 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点通过时刻 [s]
AST_CORE_CAPI double    aTPANToTPP         (double TPAN, double argPeri, double semiMajorAxis,  double eccentricity, double gm);

/// @brief 近地点幅角时刻转换为真近点角
/// @param TPAN 近地点幅角时刻 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 真近点角 [rad]
AST_CORE_CAPI double    aTPANToTrue        (double TPAN, double argPeri, double semiMajorAxis,  double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);

/// @brief 近地点通过时刻转换为偏近点角
/// @param TPP 近地点通过时刻 [s]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 偏近点角 [rad]
AST_CORE_CAPI double    aTPPToEcc          (double TPP, double semiMajorAxis, double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);

/// @brief 近地点通过时刻转换为平近点角
/// @param TPP 近地点通过时刻 [s]
/// @param semiMajorAxis 长半轴 [m]
/// @param gm 引力参数 [m^3/s^2]
/// @return 平近点角 [rad]
AST_CORE_CAPI double    aTPPToMean         (double TPP, double semiMajorAxis, double gm);

/// @brief 近地点通过时刻转换为真近点角
/// @param TPP 近地点通过时刻 [s]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @param eps 迭代精度，默认1e-14
/// @param maxIter 最大迭代次数，默认100
/// @return 真近点角 [rad]
AST_CORE_CAPI double    aTPPToTrue         (double TPP, double semiMajorAxis, double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);

/// @brief 近地点通过时刻转换为近地点幅角时刻
/// @param TPP 近地点通过时刻 [s]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点幅角时刻 [s]
AST_CORE_CAPI double    aTPPToTPAN         (double TPP, double argPeri, double semiMajorAxis,  double eccentricity, double gm);

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

/// @brief 真近点角转换为近地点幅角时刻
/// @param trueAnomaly 真近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点幅角时刻 [s]
AST_CORE_CAPI double    aTrueToTPAN        (double trueAnomaly, double argPeri, double semiMajorAxis,  double eccentricity, double gm);

/// @brief 真近点角转换为近地点通过时刻
/// @param trueAnomaly 真近点角 [rad]
/// @param semiMajorAxis 长半轴 [m]
/// @param eccentricity 偏心率
/// @param gm 引力参数 [m^3/s^2]
/// @return 近地点通过时刻 [s]
AST_CORE_CAPI double    aTrueToTPP         (double trueAnomaly, double semiMajorAxis, double eccentricity, double gm);

/// @brief 真近点角转换为幅角
/// @param trueAnomaly 真近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @return 幅角 [rad]
AST_CORE_CAPI double    aTrueToArgLat      (double trueAnomaly, double argPeri);

/// @brief 真近点角转换为真近点经度
/// @param trueAnomaly 真近点角 [rad]
/// @param argPeri 近地点幅角 [rad]
/// @param raan 升交点赤经 [rad]
/// @return 真近点经度 [rad]
AST_CORE_CAPI double    aTrueToTrueLong    (double trueAnomaly, double argPeri, double raan);

/// @brief 近地点幅角转换为近地点经度
/// @param argPeri 近地点幅角 [rad]
/// @param raan 升交点赤经 [rad]
/// @return 近地点经度 [rad]
AST_CORE_CAPI double    aArgPeriToLongPeri (double argPeri, double raan);

/// @brief 计算地面轨迹重复所需的轨道长半轴
/// @param daysToRepeat 重复天数
/// @param revsToRepeat 重复圈数
/// @param gm 引力参数 [m^3/s^2]
/// @param cbRotRate 中心天体自转角速度 [rad/s]
/// @return 满足地面轨迹重复条件的轨道长半轴 [m]
AST_CORE_CAPI double    aRepeatGrndTrk     (int daysToRepeat, int revsToRepeat, double gm, double cbRotRate);

/// @brief 偏心率转换为扁平率
/// @param eccentricity 偏心率
/// @return 扁平率
AST_CORE_CAPI double    aEccToFlat         (double eccentricity);

/// @brief 扁平率转换为偏心率
/// @param flatFactor 扁平率
/// @return 偏心率
AST_CORE_CAPI double    aFlatToEcc         (double flatFactor);


AST_NAMESPACE_END