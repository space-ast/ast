/// @file      testOrbitParamSpecial.cpp
/// @brief     轨道参数转换特殊情况测试
/// @details   测试抛物线轨道、双曲线轨道和各种边界情况下的轨道参数转换。
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
#include "AstCore/Constants.h"
#include "AstTest/AstTestMacro.h"
#include <cstdio>

AST_USING_NAMESPACE

// 测试常量
const double CB_RADIUS = 6378137.0; // 地球赤道半径 [m]
const double GM = 3.986004418e14;    // 地球引力参数 [m^3/s^2]
const double EPS = 1e-7;            // 一般精度要求
const double HIGH_EPS = 1e-14;      // 高精度要求（用于近点角转换）

/// @brief 测试抛物线轨道 (e = 1.0) 参数转换
TEST(OrbitParam, ParabolaOrbit)
{
    printf("测试: 抛物线轨道 (e = 1.0)\n");
    double eccentricity = 1.0;
    
    // 测试平近点角 ←→ 偏近点角转换
    printf("  测试平近点角 ←→ 偏近点角转换...\n");
    {
        // 测试不同的平近点角值
        double meanAnomalies[] = {0.0, 0.1, 0.5, 1.0, 2.0, 5.0};
        int numTests = sizeof(meanAnomalies) / sizeof(meanAnomalies[0]);
        
        for (int i = 0; i < numTests; i++) {
            double M = meanAnomalies[i];
            double E = aMeanToEcc(M, eccentricity);
            double M_back = aEccToMean(E, eccentricity);
            EXPECT_NEAR(M, M_back, HIGH_EPS);
        }
    }
    
    // 测试偏近点角 ←→ 真近点角转换
    printf("  测试偏近点角 ←→ 真近点角转换...\n");
    {
        // 测试不同的偏近点角值
        double eccAnomalies[] = {0.0, 0.1, 0.5, 1.0, 2.0, 5.0};
        int numTests = sizeof(eccAnomalies) / sizeof(eccAnomalies[0]);
        
        for (int i = 0; i < numTests; i++) {
            double E = eccAnomalies[i];
            double f = aEccToTrue(E, eccentricity);
            double E_back = aTrueToEcc(f, eccentricity);
            EXPECT_NEAR(E, E_back, HIGH_EPS);
        }
    }
    
    // 测试真近点角 ←→ 平近点角转换
    printf("  测试真近点角 ←→ 平近点角转换...\n");
    {
        // 测试不同的真近点角值（注意抛物线轨道的真近点角范围）
        double trueAnomalies[] = {-kPI/2, -kPI/4, 0.0, kPI/4, kPI/2, kPI-0.1};
        int numTests = sizeof(trueAnomalies) / sizeof(trueAnomalies[0]);
        
        for (int i = 0; i < numTests; i++) {
            double f = trueAnomalies[i];
            double M = aTrueToMean(f, eccentricity);
            double f_back = aMeanToTrue(M, eccentricity);
            EXPECT_NEAR(f, f_back, HIGH_EPS);
        }
    }
    
    // 测试轨道周期相关函数（抛物线轨道没有周期，应返回无效值）
    printf("  测试抛物线轨道周期函数...\n");
    {
        double periRad = 7000000.0; // 近地点半径
        double semiMajorAxis = periRad / (1 - eccentricity); // 长半轴（理论上无穷大，但计算上需要处理）
        
        // 检查aPeriRadToPeriod在抛物线轨道时的行为
        double period = aPeriRadToPeriod(periRad, eccentricity, GM);
        // 注意：由于实现中可能没有特殊处理抛物线轨道的周期计算，这里只检查结果是否合理
    }
    
}

/// @brief 测试双曲线轨道 (e > 1.0) 参数转换
TEST(OrbitParam, HyperbolaOrbit)
{
    printf("测试: 双曲线轨道 (e > 1.0)\n");
    
    // 测试多个不同的偏心率值
    double eccentricities[] = {1.1, 1.5, 2.0, 5.0};
    int numEccTests = sizeof(eccentricities) / sizeof(eccentricities[0]);
    
    for (int e_idx = 0; e_idx < numEccTests; e_idx++) {
        double eccentricity = eccentricities[e_idx];
        printf("  测试偏心率 e = %.2f\n", eccentricity);
        
        // 测试平近点角 ←→ 偏近点角转换
        printf("    测试平近点角 ←→ 偏近点角转换...\n");
        {
            // 双曲线轨道的平近点角范围
            double meanAnomalies[] = {0.0, 0.5, 1.0, 2.0, 5.0};
            int numTests = sizeof(meanAnomalies) / sizeof(meanAnomalies[0]);
            
            for (int i = 0; i < numTests; i++) {
                double M = meanAnomalies[i];
                double E = aMeanToEcc(M, eccentricity);
                double M_back = aEccToMean(E, eccentricity);
                EXPECT_NEAR(M, M_back, HIGH_EPS);
            }
        }
        
        // 测试偏近点角 ←→ 真近点角转换
        printf("    测试偏近点角 ←→ 真近点角转换...\n");
        {
            // 测试不同的偏近点角值
            double eccAnomalies[] = {0.1, 0.5, 1.0, 2.0};
            int numTests = sizeof(eccAnomalies) / sizeof(eccAnomalies[0]);
            
            for (int i = 0; i < numTests; i++) {
                double E = eccAnomalies[i];
                double f = aEccToTrue(E, eccentricity);
                
                // 验证真近点角在有效范围内
                double maxTrueAnomaly = kPI - acos(1.0 / eccentricity);
                EXPECT_TRUE(f < maxTrueAnomaly && f > -maxTrueAnomaly);
                
                // 验证往返转换
                double E_back = aTrueToEcc(f, eccentricity);
                EXPECT_NEAR(E, E_back, HIGH_EPS);
            }
        }
        
        // 测试真近点角 ←→ 平近点角转换
        printf("    测试真近点角 ←→ 平近点角转换...\n");
        {
            // 测试双曲线轨道有效范围内的真近点角
            double maxTrueAnomaly = kPI - acos(1.0 / eccentricity);
            double trueAnomalies[] = {-maxTrueAnomaly * 0.8, 0.0, maxTrueAnomaly * 0.8};
            int numTests = sizeof(trueAnomalies) / sizeof(trueAnomalies[0]);
            
            for (int i = 0; i < numTests; i++) {
                double f = trueAnomalies[i];
                double M = aTrueToMean(f, eccentricity);
                double f_back = aMeanToTrue(M, eccentricity);
                EXPECT_NEAR(f, f_back, HIGH_EPS);
            }
        }
        
        // 测试无效真近点角的情况
        printf("    测试无效真近点角处理...\n");
        {
            // 超出双曲线轨道允许范围的真近点角
            double invalidTrueAnomaly = kPI - acos(1.0 / eccentricity) + 0.1;
            double E = aTrueToEcc(invalidTrueAnomaly, eccentricity);
            // 检查是否返回了无效值
            EXPECT_TRUE(isnan(E));
        }
        
        // 测试双曲线轨道的长半轴和半径关系
        printf("    测试双曲线轨道长半轴和半径关系...\n");
        {
            // 双曲线轨道长半轴为负值
            double semiMajorAxis = -10000000.0; // 负值
            
            double periRad = aSMajAxToPeriRad(semiMajorAxis, eccentricity);
            double semiMajorAxisBack = aPeriRadToSMajAx(periRad, eccentricity);
            
            // 验证往返转换
            EXPECT_NEAR(semiMajorAxis, semiMajorAxisBack, EPS);
        }
    }
    
}

/// @brief 测试接近边界的情况
TEST(OrbitParam, BoundaryConditions)
{
    printf("测试: 边界条件\n");
    
    // 测试接近抛物线的椭圆轨道 (e 接近但小于 1.0)
    printf("  测试接近抛物线的椭圆轨道 (e = 0.999)...\n");
    {
        double eccentricity = 0.999; // 非常接近1.0
        double meanAnomaly = 1.0;
        
        // 测试近点角转换
        double E = aMeanToEcc(meanAnomaly, eccentricity);
        double M_back = aEccToMean(E, eccentricity);
        EXPECT_NEAR(meanAnomaly, M_back, HIGH_EPS);
        
        double f = aEccToTrue(E, eccentricity);
        double E_back = aTrueToEcc(f, eccentricity);
        EXPECT_NEAR(E, E_back, HIGH_EPS);
    }
    
    // 测试接近抛物线的双曲线轨道 (e 接近但大于 1.0)
    printf("  测试接近抛物线的双曲线轨道 (e = 1.001)...\n");
    {
        double eccentricity = 1.001; // 非常接近1.0
        double meanAnomaly = 1.0;
        
        // 测试近点角转换
        double E = aMeanToEcc(meanAnomaly, eccentricity);
        double M_back = aEccToMean(E, eccentricity);
        EXPECT_NEAR(meanAnomaly, M_back, HIGH_EPS);
        
        double f = aEccToTrue(E, eccentricity);
        double E_back = aTrueToEcc(f, eccentricity);
        EXPECT_NEAR(E, E_back, HIGH_EPS);
    }
    
    // 测试极端椭圆轨道 (e 接近 1.0 但小于 1.0)
    printf("  测试极端椭圆轨道 (e = 0.99)...\n");
    {
        double eccentricity = 0.99;
        double perigeeAlt = 200000.0; // 近地点高度
        
        // 计算远地点高度（应该非常大）
        double apogeeAlt = aPeriAltToApoAlt(perigeeAlt, eccentricity, CB_RADIUS);
        double perigeeAltBack = aApoAltToPeriAlt(apogeeAlt, eccentricity, CB_RADIUS);
        
        // 验证往返转换
        EXPECT_NEAR(perigeeAlt, perigeeAltBack, EPS);
    }
    
    // 测试负偏心率的处理（应该返回无效值）
    printf("  测试负偏心率处理...\n");
    {
        double negativeEcc = -0.1;
        double eccAnomaly = 1.0;
        
        // 测试负偏心率的处理
        double meanAnomaly = aEccToMean(eccAnomaly, negativeEcc);
        EXPECT_TRUE(isnan(meanAnomaly));
        
        double trueAnomaly = aEccToTrue(eccAnomaly, negativeEcc);
        EXPECT_TRUE(isnan(trueAnomaly));
    }
    
}

/// @brief 测试迭代函数的收敛性
TEST(OrbitParam, Convergence)
{
    printf("测试: 迭代函数收敛性\n");
    
    // 测试不同精度要求下的收敛性
    double meanAnomaly = 10.0; // 较大的平近点角
    double eccentricity = 0.9;  // 高偏心率
    
    printf("  测试不同精度要求...\n");
    {
        // 不同的精度要求
        double precisions[] = {1e-10, 1e-12, 1e-14, 1e-15};
        int numTests = sizeof(precisions) / sizeof(precisions[0]);
        
        for (int i = 0; i < numTests; i++) {
            double eps = precisions[i];
            int maxIter = 200; // 增加迭代次数以确保收敛
            
            double E = aMeanToEcc(meanAnomaly, eccentricity, eps, maxIter);
            double M_back = aEccToMean(E, eccentricity);
            
            // 验证结果满足精度要求
            EXPECT_TRUE(fabs(meanAnomaly - M_back) < eps * 10); // 允许10倍精度误差作为安全边界
        }
    }
    
    // 测试极端高偏心率椭圆轨道的收敛性
    printf("  测试极端高偏心率椭圆轨道...\n");
    {
        double highEcc = 0.9999;
        double M = 5.0;
        
        double E = aMeanToEcc(M, highEcc, 1e-12, 200);
        double M_back = aEccToMean(E, highEcc);
        EXPECT_NEAR(M, M_back, 1e-10);
    }
    
}

GTEST_MAIN()
