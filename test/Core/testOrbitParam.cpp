/// @file      testOrbitParam.cpp
/// @brief     轨道参数转换往返测试
/// @details   测试轨道参数之间的双向转换精度，验证A转到B再转回A的一致性。
/// @author    jinke18
/// @date      19.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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

#include "AstCore/OrbitParam.hpp"
#include "AstTest/AstTestMacro.h"
#include <cstdio>

AST_USING_NAMESPACE

// 测试常量
const double CB_RADIUS = 6378137.0; // 地球赤道半径 [m]
const double GM = 3.986004418e14;    // 地球引力参数 [m^3/s^2]
const double EPS = 1e-7;            // 精度要求

/// @brief 测试远地点/近地点高度与半径转换
int testAltitudeRadiusConversion()
{
    printf("测试: 远地点/近地点高度 ↔ 半径\n");
    
    // 测试远地点高度 ↔ 远地点半径
    {
        double apogeeAlt = 42164000.0; // 地球同步轨道高度
        double apogeeRad = aApoAltToApoRad(apogeeAlt, CB_RADIUS);
        double apogeeAltBack = aApoRadToApoAlt(apogeeRad, CB_RADIUS);
        ASSERT_NEAR(apogeeAlt, apogeeAltBack, EPS);
    }
    
    // 测试近地点高度 ↔ 近地点半径
    {
        double perigeeAlt = 200000.0; // 低轨道近地点高度
        double perigeeRad = aPeriAltToPeriRad(perigeeAlt, CB_RADIUS);
        double perigeeAltBack = aPeriRadToPeriAlt(perigeeRad, CB_RADIUS);
        ASSERT_NEAR(perigeeAlt, perigeeAltBack, EPS);
    }
    
    // 测试近地点/远地点高度与半径互转
    {
        double perigeeAlt = 200000.0;
        double eccentricity = 0.05;
        
        double apogeeAlt = aPeriAltToApoAlt(perigeeAlt, eccentricity, CB_RADIUS);
        double perigeeAltBack = aApoAltToPeriAlt(apogeeAlt, eccentricity, CB_RADIUS);
        ASSERT_NEAR(perigeeAlt, perigeeAltBack, EPS);
    }
    
    return 0;
}

/// @brief 测试近点角转换
int testAnomalyConversion()
{
    printf("测试: 近点角转换\n");
    
    double eccentricity = 0.01;
    
    // 测试偏近点角 ↔ 平近点角
    {
        double eccAnomaly = 1.0;      // 偏近点角 [rad]
        double meanAnomaly = aEccToMean(eccAnomaly, eccentricity);
        double eccAnomalyBack = aMeanToEcc(meanAnomaly, eccentricity);
        ASSERT_NEAR(eccAnomaly, eccAnomalyBack, EPS);
    }
    
    // 测试偏近点角 ↔ 真近点角
    {
        double eccAnomaly = 1.0;      // 偏近点角 [rad]
        double trueAnomaly = aEccToTrue(eccAnomaly, eccentricity);
        double eccAnomalyBack = aTrueToEcc(trueAnomaly, eccentricity);
        ASSERT_NEAR(eccAnomaly, eccAnomalyBack, EPS);
    }
    
    // 测试真近点角 ↔ 平近点角
    {
        double trueAnomaly = 1.0;     // 真近点角 [rad]
        double meanAnomaly = aTrueToMean(trueAnomaly, eccentricity);
        double trueAnomalyBack = aMeanToTrue(meanAnomaly, eccentricity);
        ASSERT_NEAR(trueAnomaly, trueAnomalyBack, EPS);
    }
    
    return 0;
}

/// @brief 测试轨道要素转换
int testOrbitalElementConversion()
{
    printf("测试: 轨道要素转换\n");
    
    // 测试长半轴 ↔ 平均角速度
    {
        double semiMajorAxis = 42164000.0 + CB_RADIUS; // 地球同步轨道长半轴
        double meanMotion = aSMajAxToMeanMotn(semiMajorAxis, GM);
        double semiMajorAxisBack = aMeanMotnToSMajAx(meanMotion, GM);
        ASSERT_NEAR(semiMajorAxis, semiMajorAxisBack, EPS);
    }
    
    // 测试长半轴 ↔ 轨道周期
    {
        double semiMajorAxis = 42164000.0 + CB_RADIUS; // 地球同步轨道长半轴
        double period = aSMajAxToPeriod(semiMajorAxis, GM);
        double semiMajorAxisBack = aPeriodToSMajAx(period, GM);
        ASSERT_NEAR(semiMajorAxis, semiMajorAxisBack, EPS);
    }
    
    // 测试平均角速度 ↔ 轨道周期
    {
        double meanMotion = 7.2921158553e-5; // 地球自转角速度
        double period = aMeanMotnToPeriod(meanMotion);
        double meanMotionBack = aPeriodToMeanMotn(period);
        ASSERT_NEAR(meanMotion, meanMotionBack, EPS);
    }
    
    // 测试近地点/远地点高度 ↔ 长半轴
    {
        double perigeeAlt = 200000.0;  // 近地点高度
        double eccentricity = 0.05;    // 偏心率
        
        double semiMajorAxis = aPeriAltToSMajAx(perigeeAlt, eccentricity, CB_RADIUS);
        double perigeeAltBack = aSMajAxToPeriAlt(semiMajorAxis, eccentricity, CB_RADIUS);
        ASSERT_NEAR(perigeeAlt, perigeeAltBack, EPS);
        
        double apogeeAlt = aPeriAltToApoAlt(perigeeAlt, eccentricity, CB_RADIUS);
        double semiMajorAxisFromApo = aApoAltToSMajAx(apogeeAlt, eccentricity, CB_RADIUS);
        double apogeeAltBack = aSMajAxToApoAlt(semiMajorAxisFromApo, eccentricity, CB_RADIUS);
        ASSERT_NEAR(apogeeAlt, apogeeAltBack, EPS);
    }
    
    return 0;
}

/// @brief 测试其他转换
int testOtherConversions()
{
    printf("测试: 其他转换\n");
    
    // 测试偏心率 ↔ 扁平率
    {
        double eccentricity = 0.08;    // 偏心率
        double flatFactor = aEccToFlat(eccentricity);
        double eccentricityBack = aFlatToEcc(flatFactor);
        ASSERT_NEAR(eccentricity, eccentricityBack, EPS);
    }
    
    // 测试近地点幅角 ↔ 近地点经度
    {
        double argPeri = 1.5;  // 近地点幅角 [rad]
        double raan = 0.8;     // 升交点赤经 [rad]
        
        double longPeri = aArgPeriToLongPeri(argPeri, raan);
        // 注意：由于缺少直接从近地点经度转回的函数，这里只验证计算结果的合理性
        ASSERT_NEAR(longPeri, argPeri + raan, EPS);
    }
    
    return 0;
}

/// @brief 测试特定转换场景
int testSpecificScenarios()
{
    printf("测试: 特定转换场景\n");
    
    // 测试同步轨道参数转换
    {
        double semiMajorAxis = 42164000.0; // 地球同步轨道长半轴
        double eccentricity = 0.0;
        
        double period = aSMajAxToPeriod(semiMajorAxis, GM);
        double expectedPeriod = 23 * 3600 + 56 * 60 + 4.0916; // 恒星日
        ASSERT_NEAR(period, expectedPeriod, 1.0); // 允许1秒误差
    }
    
    // 测试偏心率和扁率的边界情况
    {
        double eccentricity = 0.0; // 圆轨道
        double flatFactor = aEccToFlat(eccentricity);
        ASSERT_NEAR(flatFactor, 0.0, EPS);
        
        double eccentricityBack = aFlatToEcc(flatFactor);
        ASSERT_NEAR(eccentricity, eccentricityBack, EPS);
    }
    
    return 0;
}

int main()
{
    printf("开始轨道参数转换往返测试...\n");
    
    int rc = 0;
    rc |= testAltitudeRadiusConversion();
    rc |= testAnomalyConversion();
    rc |= testOrbitalElementConversion();
    rc |= testOtherConversions();
    rc |= testSpecificScenarios();
    
    if (rc == 0) {
        printf("所有往返测试完成，全部通过！\n");
    } else {
        printf("测试完成，但有失败项！\n");
    }
    
    return rc;
}