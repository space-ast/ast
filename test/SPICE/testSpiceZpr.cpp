///
/// @file      testSpiceZpr.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-02
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

#define _USE_MATH_DEFINES
#include "AstSPICE/SpiceZpr.h"
#include "AstTest/Test.h"
#include "AstUtil/Literals.hpp"
#include <cmath>
#include <cstdio>
#include <array>
#include <vector>

#ifndef AST_NO_CSPICE
#include "SpiceUsr.h"

AST_USING_NAMESPACE
using namespace _AST literals;

TEST(SpiceZpr, axisar)
{
    static const struct {
        double axis[3];
        double angle;
    } testData[] = {
        {{1.0, 0.0, 0.0}, M_PI / 2.0},
        {{1.0, 1.0, 0.0}, M_PI / 4.0},
        {{-5.0, 0.0, 1.0}, M_PI / 8.0}
    };
    
    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
    {
        const auto& data = testData[i];
        double axis[3] = {data.axis[0], data.axis[1], data.axis[2]};
        double angle = data.angle;

        double r1[3][3], r2[3][3];
        axisar(axis, angle, r1);
        axisar_c(axis, angle, r2);
        // printf("r1 = \n");
        // printf("%lf, %lf, %lf\n", r1[0][0], r1[0][1], r1[0][2]);
        // printf("%lf, %lf, %lf\n", r1[1][0], r1[1][1], r1[1][2]);
        // printf("%lf, %lf, %lf\n", r1[2][0], r1[2][1], r1[2][2]);
        // printf("r2 = \n");
        // printf("%lf, %lf, %lf\n", r2[0][0], r2[0][1], r2[0][2]);
        // printf("%lf, %lf, %lf\n", r2[1][0], r2[1][1], r2[1][2]);
        // printf("%lf, %lf, %lf\n", r2[2][0], r2[2][1], r2[2][2]);
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                EXPECT_NEAR(r1[j][k], r2[j][k], 1e-12);
            }
        }
    }
}


TEST(SpiceZpr, azlrec)
{
    static const struct {
        double range;
        double az;
        double el;
    } testData[] = {
        {1.0, 0.0, 0.0},
        {10.0, M_PI / 4.0, M_PI / 4.0},
        {5.0, M_PI / 2.0, M_PI / 2.0},
        {0.000,    0.000_deg,    0.000_deg},
        {1.000,    0.000_deg,    0.000_deg},
        {1.000,  270.000_deg,    0.000_deg},
        {1.000,    0.000_deg,  -90.000_deg},
        {1.000,  180.000_deg,    0.000_deg},
        {1.000,   90.000_deg,    0.000_deg},
        {1.000,    0.000_deg,   90.000_deg},
        {1.414,  315.000_deg,    0.000_deg},
        {1.414,    0.000_deg,  -45.000_deg},
        {1.414,  270.000_deg,  -45.000_deg},
        {1.732,  315.000_deg,  -35.264_deg},
    };
    
    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
    {
        const auto& data = testData[i];
        double range = data.range;
        double az = data.az;
        double el = data.el;
        
        static const struct {
            bool azccw;
            bool elplsz;
        } testData2[] = {
            {true, true},
            {false, true},
            {true, false},
            {false, false},
        };
        for (size_t j = 0; j < sizeof(testData2) / sizeof(testData2[0]); j++)
        {
            const auto& data = testData2[j];
            bool azccw = data.azccw;
            bool elplsz = data.elplsz;
            double rectan1[3], rectan2[3];
            azlrec(range, az, el, azccw, elplsz, rectan1);
            azlrec_c(range, az, el, azccw, elplsz, rectan2);
            
            for (int j = 0; j < 3; j++)
            {
                EXPECT_NEAR(rectan1[j], rectan2[j], 1e-12);
            }
        }
    }
}

TEST(SpiceZpr, b1900)
{
    EXPECT_EQ(b1900(), b1900_c());
}

TEST(SpiceZpr, b1950)
{
    EXPECT_EQ(b1950(), b1950_c());
}


// -----------------------------------------------------------


TEST(SpiceZpr, dpr)
{
    EXPECT_EQ(dpr(), dpr_c());
}



TEST(SpiceZpr, latrec)
{
    static const struct {
        double radius;
        double lon;
        double lat;
    } testData[] = {
        {1.0, 0.0, 0.0},
        {5.0, M_PI / 4.0, M_PI / 4.0},
        {10.0, M_PI / 2.0, M_PI / 2.0},
        {0.000,    0.000_deg,    0.000_deg},
        {1.000,    0.000_deg,    0.000_deg},
        {1.000,   90.000_deg,    0.000_deg},
        {1.000,    0.000_deg,   90.000_deg},
        {1.000,  180.000_deg,    0.000_deg},
        {1.000,  -90.000_deg,    0.000_deg},
        {1.000,    0.000_deg,  -90.000_deg},
        {1.414,   45.000_deg,    0.000_deg},
        {1.414,    0.000_deg,   45.000_deg},
        {1.414,   90.000_deg,   45.000_deg},
        {1.732,   45.000_deg,   35.264_deg},
    };
    
    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
    {
        const auto& data = testData[i];
        double radius = data.radius;
        double lon = data.lon;
        double lat = data.lat;
        
        double rectan1[3], rectan2[3];
        latrec(radius, lon, lat, rectan1);
        latrec_c(radius, lon, lat, rectan2);
        
        // printf("latrec test %zu:\n", i+1);
        // printf("radius: %lf, lon: %lf, lat: %lf\n", radius, lon, lat);
        // printf("rectan1: %lf, %lf, %lf\n", rectan1[0], rectan1[1], rectan1[2]);
        // printf("rectan2: %lf, %lf, %lf\n", rectan2[0], rectan2[1], rectan2[2]);
        
        for (int j = 0; j < 3; j++)
        {
            EXPECT_NEAR(rectan1[j], rectan2[j], 1e-12);
        }
    }
}

TEST(SpiceZpr, j2000)
{
    EXPECT_EQ(j2000(), j2000_c());
}


TEST(SpiceZpr, j2100)
{
    EXPECT_EQ(j2100(), j2100_c());
}

#endif

GTEST_MAIN()