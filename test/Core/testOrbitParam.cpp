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
#include "AstUtil/Constants.h"
#include <cstdio>

AST_USING_NAMESPACE

// 测试常量
const double CB_RADIUS = 6378137.0; // 地球赤道半径 [m]
const double GM = 3.986004418e14;    // 地球引力参数 [m^3/s^2]
const double EPS = 1e-7;            // 精度要求

/// @brief 测试远地点/近地点高度与半径转换
TEST(OrbitParam, AltitudeRadiusConversion)
{
    printf("测试: 远地点/近地点高度 ←→ 半径\n");
    
    // 测试远地点高度 ←→ 远地点半径
    {
        double apogeeAlt = 42164000.0; // 地球同步轨道高度
        double apogeeRad = aApoAltToApoRad(apogeeAlt, CB_RADIUS);
        double apogeeAltBack = aApoRadToApoAlt(apogeeRad, CB_RADIUS);
        EXPECT_NEAR(apogeeAlt, apogeeAltBack, EPS);
    }
    
    // 测试近地点高度 ←→ 近地点半径
    {
        double perigeeAlt = 200000.0; // 低轨道近地点高度
        double perigeeRad = aPeriAltToPeriRad(perigeeAlt, CB_RADIUS);
        double perigeeAltBack = aPeriRadToPeriAlt(perigeeRad, CB_RADIUS);
        EXPECT_NEAR(perigeeAlt, perigeeAltBack, EPS);
    }
    
    // 测试近地点/远地点高度与半径互转
    {
        double perigeeAlt = 200000.0;
        double eccentricity = 0.05;
        
        double apogeeAlt = aPeriAltToApoAlt(perigeeAlt, eccentricity, CB_RADIUS);
        double perigeeAltBack = aApoAltToPeriAlt(apogeeAlt, eccentricity, CB_RADIUS);
        EXPECT_NEAR(perigeeAlt, perigeeAltBack, EPS);
    }
    
}

/// @brief 测试近点角转换
TEST(OrbitParam, AnomalyConversion)
{
    printf("测试: 近点角转换\n");
    
    double eccentricity = 0.01;
    
    // 测试偏近点角 ←→ 平近点角
    {
        double eccAnomaly = 1.0;      // 偏近点角 [rad]
        double meanAnomaly = aEccToMean(eccAnomaly, eccentricity);
        double eccAnomalyBack = aMeanToEcc(meanAnomaly, eccentricity);
        EXPECT_NEAR(eccAnomaly, eccAnomalyBack, EPS);
    }
    
    // 测试偏近点角 ←→ 真近点角
    {
        double eccAnomaly = 1.0;      // 偏近点角 [rad]
        double trueAnomaly = aEccToTrue(eccAnomaly, eccentricity);
        double eccAnomalyBack = aTrueToEcc(trueAnomaly, eccentricity);
        EXPECT_NEAR(eccAnomaly, eccAnomalyBack, EPS);
    }
    
    // 测试真近点角 ←→ 平近点角
    {
        double trueAnomaly = 1.0;     // 真近点角 [rad]
        double meanAnomaly = aTrueToMean(trueAnomaly, eccentricity);
        double trueAnomalyBack = aMeanToTrue(meanAnomaly, eccentricity);
        EXPECT_NEAR(trueAnomaly, trueAnomalyBack, EPS);
    }
    
}

/// @brief 测试轨道要素转换
TEST(OrbitParam, OrbitalElementConversion)
{
    printf("测试: 轨道要素转换\n");
    
    // 测试长半轴 ←→ 平均角速度
    {
        double semiMajorAxis = 42164000.0 + CB_RADIUS; // 地球同步轨道长半轴
        double meanMotion = aSMajAxToMeanMotn(semiMajorAxis, GM);
        double semiMajorAxisBack = aMeanMotnToSMajAx(meanMotion, GM);
        EXPECT_NEAR(semiMajorAxis, semiMajorAxisBack, EPS);
    }
    
    // 测试长半轴 ←→ 轨道周期
    {
        double semiMajorAxis = 42164000.0 + CB_RADIUS; // 地球同步轨道长半轴
        double period = aSMajAxToPeriod(semiMajorAxis, GM);
        double semiMajorAxisBack = aPeriodToSMajAx(period, GM);
        EXPECT_NEAR(semiMajorAxis, semiMajorAxisBack, EPS);
    }
    
    // 测试平均角速度 ←→ 轨道周期
    {
        double meanMotion = 7.2921158553e-5; // 地球自转角速度
        double period = aMeanMotnToPeriod(meanMotion);
        double meanMotionBack = aPeriodToMeanMotn(period);
        EXPECT_NEAR(meanMotion, meanMotionBack, EPS);
    }
    
    // 测试近地点/远地点高度 ←→ 长半轴
    {
        double perigeeAlt = 200000.0;  // 近地点高度
        double eccentricity = 0.05;    // 偏心率
        
        double semiMajorAxis = aPeriAltToSMajAx(perigeeAlt, eccentricity, CB_RADIUS);
        double perigeeAltBack = aSMajAxToPeriAlt(semiMajorAxis, eccentricity, CB_RADIUS);
        EXPECT_NEAR(perigeeAlt, perigeeAltBack, EPS);
        
        double apogeeAlt = aPeriAltToApoAlt(perigeeAlt, eccentricity, CB_RADIUS);
        double semiMajorAxisFromApo = aApoAltToSMajAx(apogeeAlt, eccentricity, CB_RADIUS);
        double apogeeAltBack = aSMajAxToApoAlt(semiMajorAxisFromApo, eccentricity, CB_RADIUS);
        EXPECT_NEAR(apogeeAlt, apogeeAltBack, EPS);
    }
    
}

/// @brief 测试其他转换
TEST(OrbitParam, OtherConversions)
{
    printf("测试: 其他转换\n");
    
    // 测试偏心率 ←→ 扁平率
    {
        double eccentricity = 0.08;    // 偏心率
        double flatFactor = aEccToFlat(eccentricity);
        double eccentricityBack = aFlatToEcc(flatFactor);
        EXPECT_NEAR(eccentricity, eccentricityBack, EPS);
    }
    
    // 测试近地点幅角 ←→ 近地点经度
    {
        double argPeri = 1.5;  // 近地点幅角 [rad]
        double raan = 0.8;     // 升交点赤经 [rad]
        
        double longPeri = aArgPeriToLongPeri(argPeri, raan);
        // 注意：由于缺少直接从近地点经度转回的函数，这里只验证计算结果的合理性
        EXPECT_NEAR(longPeri, argPeri + raan, EPS);
    }
    
}

/// @brief 测试特定转换场景
TEST(OrbitParam, SpecificScenarios)
{
    printf("测试: 特定转换场景\n");
    
    // 测试同步轨道参数转换
    {
        double semiMajorAxis = 42164000.0; // 地球同步轨道长半轴
        double eccentricity = 0.0;
        
        double period = aSMajAxToPeriod(semiMajorAxis, GM);
        double expectedPeriod = 23 * 3600 + 56 * 60 + 4.0916; // 恒星日
        EXPECT_NEAR(period, expectedPeriod, 1.0); // 允许1秒误差
    }
    
    // 测试偏心率和扁率的边界情况
    {
        double eccentricity = 0.0; // 圆轨道
        double flatFactor = aEccToFlat(eccentricity);
        EXPECT_NEAR(flatFactor, 0.0, EPS);
        
        double eccentricityBack = aFlatToEcc(flatFactor);
        EXPECT_NEAR(eccentricity, eccentricityBack, EPS);
    }
    
}


// 测试抛物线
TEST(OrbitParam, Parabola)
{
    double e = 1;
    // mean and ecc
    {
        double meanA = 0.2;
        double eccA = aMeanToEcc(meanA, e);
        double meanA2 = aEccToMean(eccA, e);
        EXPECT_NEAR(meanA, meanA2, 1e-14);
    }
    {
        double meanA = 1.4;
        double eccA = aMeanToEcc(meanA, e);
        double meanA2 = aEccToMean(eccA, e);
        EXPECT_NEAR(meanA, meanA2, 1e-14);
    }
    // ecc and true
    {
        double eccA = 0.2;
        double trueA = aEccToTrue(eccA, e);
        double eccA2 = aTrueToEcc(trueA, e);
        EXPECT_NEAR(eccA, eccA2, 1e-14);
    }
    {
        double eccA = 1.7;
        double trueA = aEccToTrue(eccA, e);
        double eccA2 = aTrueToEcc(trueA, e);
        EXPECT_NEAR(eccA, eccA2, 1e-14);
    }

    // true and mean
    {
        double meanA = 0.2;
        double trueA = aMeanToTrue(meanA, e);
        double meanA2 = aTrueToMean(trueA, e);
        EXPECT_NEAR(meanA, meanA2, 1e-14);
    }
    {
        double meanA = 1.6;
        double trueA = aMeanToTrue(meanA, e);
        double meanA2 = aTrueToMean(trueA, e);
        EXPECT_NEAR(meanA, meanA2, 1e-14);
    }
}


/// @brief 测试时间相关转换函数
TEST(OrbitParam, TimeConversions)
{
    printf("测试: 时间相关转换函数\n");
    
    double eccentricity = 0.01; // 小偏心率椭圆轨道
    double semiMajorAxis = 7000000.0; // 长半轴
    double argPeri = 1.0; // 近地点幅角
    
    // 测试平近点角 ←→ 过近心点时间
    {
        double meanAnomaly = 0.5; // 平近点角
        double tpp = aMeanToTPP(meanAnomaly, semiMajorAxis, GM);
        double meanAnomalyBack = aTPPToMean(tpp, semiMajorAxis, GM);
        EXPECT_NEAR(meanAnomaly, meanAnomalyBack, EPS);
    }
    
    // 测试偏近点角 ←→ 过近心点时间
    {
        double eccAnomaly = 0.5; // 偏近点角
        double tpp = aEccToTPP(eccAnomaly, semiMajorAxis, eccentricity, GM);
        double eccAnomalyBack = aTPPToEcc(tpp, semiMajorAxis, eccentricity, GM);
        EXPECT_NEAR(eccAnomaly, eccAnomalyBack, EPS);
    }
    
    // 测试真近点角 ←→ 过近心点时间
    {
        double trueAnomaly = 0.5; // 真近点角
        double tpp = aTrueToTPP(trueAnomaly, semiMajorAxis, eccentricity, GM);
        double trueAnomalyBack = aTPPToTrue(tpp, semiMajorAxis, eccentricity, GM);
        EXPECT_NEAR(trueAnomaly, trueAnomalyBack, EPS);
    }
    
    // 测试过升交点时间 ←→ 过近心点时间
    {
        double tpp = 1000.0; // 过近心点时间
        double tpan = aTPPToTPAN(tpp, argPeri, semiMajorAxis, eccentricity, GM);
        double tppBack = aTPANToTPP(tpan, argPeri, semiMajorAxis, eccentricity, GM);
        EXPECT_NEAR(tpp, tppBack, EPS);
    }
    
    // 测试过升交点时间 ←→ 平近点角
    {
        double tpan = 5000.0; // 过升交点时间
        double meanAnomaly = aTPANToMean(tpan, argPeri, semiMajorAxis, eccentricity, GM);
        // 这里需要通过其他函数组合来验证
        double tpp = aTPANToTPP(tpan, argPeri, semiMajorAxis, eccentricity, GM);
        double meanAnomalyVerify = aTPPToMean(tpp, semiMajorAxis, GM);
        EXPECT_NEAR(meanAnomaly, meanAnomalyVerify, EPS);
    }
    
    // 测试过升交点时间 ←→ 偏近点角
    {
        double tpan = 5000.0; // 过升交点时间
        double eccAnomaly = aTPANToEcc(tpan, argPeri, semiMajorAxis, eccentricity, GM);
        // 验证逻辑
        double tpp = aTPANToTPP(tpan, argPeri, semiMajorAxis, eccentricity, GM);
        double eccAnomalyVerify = aTPPToEcc(tpp, semiMajorAxis, eccentricity, GM);
        EXPECT_NEAR(eccAnomaly, eccAnomalyVerify, EPS);
    }
    
    // 测试过升交点时间 ←→ 真近点角
    {
        double tpan = 5000.0; // 过升交点时间
        double trueAnomaly = aTPANToTrue(tpan, argPeri, semiMajorAxis, eccentricity, GM);
        // 验证逻辑
        double tpp = aTPANToTPP(tpan, argPeri, semiMajorAxis, eccentricity, GM);
        double trueAnomalyVerify = aTPPToTrue(tpp, semiMajorAxis, eccentricity, GM);
        EXPECT_NEAR(trueAnomaly, trueAnomalyVerify, EPS);
    }
    
    // 测试真近点角 ←→ 过升交点时间
    {
        double trueAnomaly = 0.5; // 真近点角
        double tpan = aTrueToTPAN(trueAnomaly, argPeri, semiMajorAxis, eccentricity, GM);   // 过升交点后时间
        // 验证逻辑
        double tppFromTrue = aTrueToTPP(trueAnomaly, semiMajorAxis, eccentricity, GM);      // 过近心点后时间
        double tppFromArgPeri = aTrueToTPP(argPeri, semiMajorAxis, eccentricity, GM);       // 升交点到近心点时间
        double tpanVerify = tppFromTrue + tppFromArgPeri;
        EXPECT_NEAR(tpan, tpanVerify, EPS);
    }
}

/// @brief 测试轨道几何参数转换
TEST(OrbitParam, GeometricParams)
{
    printf("测试: 轨道几何参数转换\n");
    
    // 测试长半轴 ←→ 半短轴
    {
        double semiMajorAxis = 7000000.0; // 长半轴
        double eccentricity = 0.01; // 偏心率
        
        double semiMinorAxis = aSMajAxToSMinAx(semiMajorAxis, eccentricity);
        double semiMajorAxisBack = aSMinAxToSMajAx(semiMinorAxis, eccentricity);
        EXPECT_NEAR(semiMajorAxis, semiMajorAxisBack, EPS);
        
        // 验证几何关系：b = a * sqrt(1 - e^2)
        double expectedSemiMinorAxis = semiMajorAxis * sqrt(1.0 - eccentricity * eccentricity);
        EXPECT_NEAR(semiMinorAxis, expectedSemiMinorAxis, EPS);
    }
    
    // 测试长半轴 ←→ 半通径
    {
        double semiMajorAxis = 7000000.0; // 长半轴
        double eccentricity = 0.01; // 偏心率
        
        double semiParam = aSMajAxToSParam(semiMajorAxis, eccentricity);
        // 验证几何关系：p = a * (1 - e^2)
        double expectedSemiParam = semiMajorAxis * (1.0 - eccentricity * eccentricity);
        EXPECT_NEAR(semiParam, expectedSemiParam, EPS);
    }
    
    // 测试近地点/远地点半径 ←→ 偏心率
    {
        double perigeeRad = 6778000.0; // 近地点半径
        double apogeeRad = 7000000.0;  // 远地点半径
        
        double eccentricity = aRadiiToEcc(perigeeRad, apogeeRad);
        // 验证偏心率计算：e = (Ra - Rp) / (Ra + Rp)
        double expectedEccentricity = (apogeeRad - perigeeRad) / (apogeeRad + perigeeRad);
        EXPECT_NEAR(eccentricity, expectedEccentricity, EPS);
    }
}

/// @brief 测试角度相关转换函数
TEST(OrbitParam, AngleConversions)
{
    printf("测试: 角度相关转换函数\n");
    
    // 测试真近点角 + 近地点幅角 ←→ 幅角
    {
        double trueAnomaly = 1.0; // 真近点角
        double argPeri = 0.5;     // 近地点幅角
        
        double argLat = aTrueToArgLat(trueAnomaly, argPeri);
        // 验证幅角计算：u = f + ω
        double expectedArgLat = trueAnomaly + argPeri;
        EXPECT_NEAR(argLat, expectedArgLat, EPS);
    }
    
    // 测试真近点角 + 近地点幅角 + 升交点赤经 ←→ 真经度
    {
        double trueAnomaly = 1.0; // 真近点角
        double argPeri = 0.5;     // 近地点幅角
        double raan = 0.8;        // 升交点赤经
        
        double trueLong = aTrueToTrueLong(trueAnomaly, argPeri, raan);
        // 验证真经度计算：l = f + ω + Ω
        double expectedTrueLong = trueAnomaly + argPeri + raan;
        EXPECT_NEAR(trueLong, expectedTrueLong, EPS);
    }
    
    // 测试近地点幅角 + 升交点赤经 ←→ 近地点经度（已在其他测试中有部分覆盖，这里补充更多情况）
    {
        double argPeri = 1.0;     // 近地点幅角
        double raan = 0.8;        // 升交点赤经
        
        double longPeri = aArgPeriToLongPeri(argPeri, raan);
        // 验证近地点经度计算：π = ω + Ω
        double expectedLongPeri = argPeri + raan;
        EXPECT_NEAR(longPeri, expectedLongPeri, EPS);
        
        // 测试边界值
        double argPeri2 = kPI;    // 近地点幅角为π
        double raan2 = kPI;       // 升交点赤经为π
        double longPeri2 = aArgPeriToLongPeri(argPeri2, raan2);
        double expectedLongPeri2 = argPeri2 + raan2;
        EXPECT_NEAR(longPeri2, expectedLongPeri2, EPS);
    }
}

/// @brief 测试地面轨迹重复函数
TEST(OrbitParam, GroundTrackRepeat)
{
    printf("测试: 地面轨迹重复函数\n");
    
    // 地球自转角速度 [rad/s]
    const double EARTH_ROT_RATE = 7.2921158553e-5;
    
    // 测试太阳同步轨道近似（16圈/天）
    {
        int daysToRepeat = 1;
        int revsToRepeat = 16;
        
        double semiMajorAxis = aRepeatGrndTrk(daysToRepeat, revsToRepeat, GM, EARTH_ROT_RATE);
        
        // 计算平均角速度
        double meanMotion = aSMajAxToMeanMotn(semiMajorAxis, GM);
        
        // 验证轨道周期与地球自转的关系
        double orbitalPeriod = aMeanMotnToPeriod(meanMotion);
        double earthDayInSeconds = 24 * 3600.0;
        
        // 检查在一天内是否完成约16圈
        double expectedPeriod = earthDayInSeconds / revsToRepeat;
        EXPECT_NEAR(orbitalPeriod, expectedPeriod, expectedPeriod * 0.01); // 允许1%误差
    }
    
    // 测试2天重复周期（例如某些遥感卫星）
    {
        int daysToRepeat = 2;
        int revsToRepeat = 31; // 2天31圈
        
        double semiMajorAxis = aRepeatGrndTrk(daysToRepeat, revsToRepeat, GM, EARTH_ROT_RATE);
        
        // 计算平均角速度
        double meanMotion = aSMajAxToMeanMotn(semiMajorAxis, GM);
        
        // 验证轨道周期
        double orbitalPeriod = aMeanMotnToPeriod(meanMotion);
        double totalTime = daysToRepeat * 24 * 3600.0;
        
        // 检查在指定天数内是否完成指定圈数
        double expectedPeriod = totalTime / revsToRepeat;
        EXPECT_NEAR(orbitalPeriod, expectedPeriod, expectedPeriod * 0.01); // 允许1%误差
    }
}

GTEST_MAIN()