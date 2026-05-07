///
/// @file      testHPOPMars.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-25
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


#include "AstCore/HPOP.hpp"
#include "AstCore/HPOPEquation.hpp"
#include "AstCore/EOP.hpp"
#include "AstCore/RunTime.hpp"
#include "AstTest/Test.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Environment.hpp"
#include "AstUtil/Literals.hpp"


AST_USING_NAMESPACE

class HPOPMarsTest : public ::testing::Test
{
    void SetUp() override
    {
        aInitialize();
        aDataContext_GetEOP()->unload();  // 卸载EOP数据，确保测试不受EOP修正数据的影响
    }

    void TearDown() override
    {
        aUninitialize();
    }
};



TEST_F(HPOPMarsTest, TwoBody)
{
    auto mars = aGetMars();
    EXPECT_TRUE(mars!=nullptr);
    auto marsInertial = mars->makeFrameInertial();
    EXPECT_TRUE(marsInertial!=nullptr);

    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity(false);
    forcemodel.gravity().model_ = "MRO110C";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
    EXPECT_EQ(err, 0);
    err = propagator.setPropagationFrame(marsInertial);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
    auto end = TimePoint::FromUTC(2026, 3, 23, 0, 0, 0);
    Vector3d pos{6792.4_km, 0, 0};
    Vector3d vel{0, 1.775_km/sec, 1.776_km/sec};
    
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    Vector3d posExpected, velExpected;
    posExpected = {
        5864637.6677002646028996, 
        2422296.1111970371566713,
        2423660.7850625044666231 
    };
    velExpected = {
        -1266.8319855301369898, 
        1532.5529584566465928, 
        1533.4163685740882102
    };
    EXPECT_NEAR(pos[0],  posExpected[0], 1e-4);
    EXPECT_NEAR(pos[1],  posExpected[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpected[2], 1e-4);
    EXPECT_NEAR(vel[0],  velExpected[0], 1e-8);
    EXPECT_NEAR(vel[1],  velExpected[1], 1e-8);
    EXPECT_NEAR(vel[2],  velExpected[2], 1e-8);
}

TEST_F(HPOPMarsTest, OnlyGravity_2_0)
{
    auto mars = aGetMars();
    EXPECT_TRUE(mars!=nullptr);
    auto marsInertial = mars->makeFrameInertial();
    EXPECT_TRUE(marsInertial!=nullptr);

    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity(false);
    forcemodel.gravity().model_ = "MRO110C";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_ = 0;
    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
    EXPECT_EQ(err, 0);
    err = propagator.setPropagationFrame(marsInertial);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
    auto end = TimePoint::FromUTC(2026, 3, 23, 0, 0, 0);
    Vector3d pos{6792.4_km, 0, 0};
    Vector3d vel{0, 1.775_km/sec, 1.776_km/sec};
    
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    Vector3d posExpected, velExpected;
    posExpected = {
        5762406.5368412779644132,
        2493772.5030095814727247,
        2588691.7993344967253506
    };
    velExpected = {
        -1329.5026203141746919, 
        1516.8968305568528194, 
        1495.4697573930666294
    };
    EXPECT_NEAR(pos[0],  posExpected[0], 1e-4);
    EXPECT_NEAR(pos[1],  posExpected[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpected[2], 1e-4);
    EXPECT_NEAR(vel[0],  velExpected[0], 1e-8);
    EXPECT_NEAR(vel[1],  velExpected[1], 1e-8);
    EXPECT_NEAR(vel[2],  velExpected[2], 1e-8);
}



TEST_F(HPOPMarsTest, OnlyGravity_70_10)
{
    auto mars = aGetMars();
    EXPECT_TRUE(mars!=nullptr);
    auto marsInertial = mars->makeFrameInertial();
    EXPECT_TRUE(marsInertial!=nullptr);

    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity(false);
    forcemodel.gravity().model_ = "MRO110C";
    forcemodel.gravity().maxDegree_ = 70;
    forcemodel.gravity().maxOrder_ = 10;
    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
    EXPECT_EQ(err, 0);
    err = propagator.setPropagationFrame(marsInertial);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
    auto end = TimePoint::FromUTC(2026, 3, 23, 0, 0, 0);
    Vector3d pos{6792.4_km, 0, 0};
    Vector3d vel{0, 1.775_km/sec, 1.776_km/sec};
    
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    Vector3d posExpected, velExpected;
    posExpected = {
        5756936.6053937152028084,
        2500508.6478170398622751,
        2595254.3992940662428737
    };
    velExpected = {
        -1332.9202227406155998,
        1515.2390413241971601,
        1493.7296638859563700
    };
    EXPECT_NEAR(pos[0],  posExpected[0], 1e-4);
    EXPECT_NEAR(pos[1],  posExpected[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpected[2], 1e-4);
    EXPECT_NEAR(vel[0],  velExpected[0], 1e-8);
    EXPECT_NEAR(vel[1],  velExpected[1], 1e-8);
    EXPECT_NEAR(vel[2],  velExpected[2], 1e-8);
}

GTEST_MAIN()
