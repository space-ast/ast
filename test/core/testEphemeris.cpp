///
/// @file      testEphemeris.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-24
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "ast/EphemerisLagrangeVar.hpp"
#include "ast/BuiltinFrame.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/RunTime.hpp"
#include "ast/Test.h"


AST_USING_NAMESPACE

class EphemerisTest : public testing::Test
{
public:
    void SetUp() override
    {
        aInitialize();
    }
    void TearDown() override
    {
        aUninitialize();
    }
};

TEST_F(EphemerisTest, EphemerisLagrangeVar_Empty)
{
    EphemerisLagrangeVar ephem;
    TimePoint tp = TimePoint::FromUTC(2026, 3, 24, 0, 0, 0);
    Vector3d pos, vel;
    errc_t rc = ephem.getPos(tp, pos);
    EXPECT_TRUE(rc != eNoError);
    rc = ephem.getPosVel(tp, pos, vel);
    EXPECT_TRUE(rc != eNoError);
}

TEST_F(EphemerisTest, EphemerisLagrangeVar_GetPosVel)
{
    EphemerisLagrangeVar ephem;
    ephem.setFrame(aGetEarth()->makeFrameICRF());
    const auto epoch = TimePoint::FromUTC(2026, 3, 24, 0, 0, 0);
    ephem.setEpoch(epoch);
    ephem.setTimes({0, 60, 120});
    ephem.setPositions({{0, 0, 0}, {1, 0, 0}, {0, 1, 0}});
    ephem.setVelocities({{0, 0, 0}, {0, 0, 1}, {0, 1, 0}});
    // 测试正常的插值
    {
        TimePoint tp = epoch + 61;
        Vector3d pos, vel;
        Vector3d pos_expected = {1, 0, 0};
        Vector3d vel_expected = {0, 0, 1};
        errc_t rc = ephem.getPos(tp, pos);
        EXPECT_TRUE(rc == eNoError);
        printf("pos = %s\n", pos.toString().c_str());
        EXPECT_NEAR(pos[0], pos_expected[0], 1e-2);
        EXPECT_NEAR(pos[1], pos_expected[1], 1e-2);
        EXPECT_NEAR(pos[2], pos_expected[2], 1e-2);

        rc = ephem.getPosVel(tp, pos, vel);
        EXPECT_TRUE(rc == eNoError);
        printf("pos = %s\n", pos.toString().c_str());
        printf("vel = %s\n", vel.toString().c_str());
        EXPECT_NEAR(pos[0], pos_expected[0], 1e-2);
        EXPECT_NEAR(pos[1], pos_expected[1], 1e-2);
        EXPECT_NEAR(pos[2], pos_expected[2], 1e-2);
        EXPECT_NEAR(vel[0], vel_expected[0], 1e-2);
        EXPECT_NEAR(vel[1], vel_expected[1], 1e-2);
        EXPECT_NEAR(vel[2], vel_expected[2], 1e-2);
    }

    // 测试在准确的时间点上获取位置和速度是否正确
    {
        TimePoint tp = epoch + 60;
        Vector3d pos, vel;
        Vector3d pos_expected = {1, 0, 0};
        Vector3d vel_expected = {0, 0, 1};
        errc_t rc = ephem.getPos(tp, pos);
        EXPECT_TRUE(rc == eNoError);
        EXPECT_EQ(pos[0], pos_expected[0]);
        EXPECT_EQ(pos[1], pos_expected[1]);
        EXPECT_EQ(pos[2], pos_expected[2]);
        
        rc = ephem.getPosVel(tp, pos, vel);
        EXPECT_TRUE(rc == eNoError);
        EXPECT_EQ(pos[0], pos_expected[0]);
        EXPECT_EQ(pos[1], pos_expected[1]);
        EXPECT_EQ(pos[2], pos_expected[2]);
        EXPECT_EQ(vel[0], vel_expected[0]);
        EXPECT_EQ(vel[1], vel_expected[1]);
        EXPECT_EQ(vel[2], vel_expected[2]);
    }

    // 测试在时间点外获取位置和速度是否返回错误
    {
        Vector3d pos, vel;
        TimePoint tp = epoch + 120.01;
        errc_t rc = ephem.getPos(tp, pos);
        EXPECT_TRUE(rc != eNoError);
        rc = ephem.getPosVel(tp, pos, vel);
        EXPECT_TRUE(rc != eNoError);

        tp = epoch - 0.001;
        rc = ephem.getPos(tp, pos);
        EXPECT_TRUE(rc != eNoError);
        rc = ephem.getPosVel(tp, pos, vel);
        EXPECT_TRUE(rc != eNoError);
    }

    // 测试在时间点的边界处获取位置和速度是否正确
    {
        TimePoint tp = epoch;
        Vector3d pos, vel;
        errc_t rc = ephem.getPos(tp, pos);
        EXPECT_TRUE(rc == eNoError);
        rc = ephem.getPosVel(tp, pos, vel);
        EXPECT_TRUE(rc == eNoError);
        Vector3d pos_expected = {0, 1, 0};
        Vector3d vel_expected = {0, 1, 0};

        tp = epoch + 120;
        rc = ephem.getPos(tp, pos);
        EXPECT_EQ(pos[0], pos_expected[0]);
        EXPECT_EQ(pos[1], pos_expected[1]);
        EXPECT_EQ(pos[2], pos_expected[2]);
        EXPECT_TRUE(rc == eNoError);
        
        rc = ephem.getPosVel(tp, pos, vel);
        EXPECT_TRUE(rc == eNoError);
        EXPECT_EQ(pos[0], pos_expected[0]);
        EXPECT_EQ(pos[1], pos_expected[1]);
        EXPECT_EQ(pos[2], pos_expected[2]);
        EXPECT_EQ(vel[0], vel_expected[0]);
        EXPECT_EQ(vel[1], vel_expected[1]);
        EXPECT_EQ(vel[2], vel_expected[2]);
    }
}


GTEST_MAIN()
