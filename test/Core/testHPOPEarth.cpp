///
/// @file      testHPOP.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-20
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

#include "AstCore/HPOP.hpp"
#include "AstCore/HPOPEquation.hpp"
#include "AstCore/EOP.hpp"
#include "AstCore/RunTime.hpp"
#include "AstTest/Test.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Environment.hpp"

AST_USING_NAMESPACE

class HPOPTest : public ::testing::Test
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

TEST_F(HPOPTest, HPOPEquation)
{
    HPOPEquation equation;
    HPOPForceModel forcemodel;
    int ndim;
    err_t err;
    err = equation.setForceModel(forcemodel);
    ndim = equation.getDimension();
    EXPECT_EQ(err, 0);
    EXPECT_EQ(ndim, 6);

    forcemodel.useMoonGravity_ = true;
    err = equation.setForceModel(forcemodel);
    ndim = equation.getDimension();
    EXPECT_EQ(err, 0);
    EXPECT_EQ(ndim, 6);
}

TEST_F(HPOPTest, TwoBody)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity_ = false;
    forcemodel.gravity_.model_ = "JGM3";
    forcemodel.gravity_.degree_ = 0;
    forcemodel.gravity_.order_ = 0;
    HPOP propagator;
    err_t err = propagator.setForceModel(forcemodel);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2026, 1, 20, 0, 0, 0);
    auto end = TimePoint::FromUTC(2026, 1, 21, 0, 0, 0);
    Vector3d pos{6678137, 0, 0};
    Vector3d vel{0, 6789.53029, 3686.414173};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    EXPECT_NEAR(pos[0],  5596646.758676, 1e-4);
    EXPECT_NEAR(pos[1], -3201966.117956, 1e-4);
    EXPECT_NEAR(pos[2],  -1738525.75573, 1e-4);
    EXPECT_NEAR(vel[0],  4215.063570733, 1e-7);
    EXPECT_NEAR(vel[1],  5690.000472941, 1e-7);
    EXPECT_NEAR(vel[2],  3089.418191229, 1e-7);
}

TEST_F(HPOPTest, OnlyGravity_2_0)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity_ = false;
    forcemodel.gravity_.model_ = "JGM3";
    forcemodel.gravity_.degree_ = 2;
    forcemodel.gravity_.order_ = 0;

    HPOP propagator;
    err_t err = propagator.setForceModel(forcemodel);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2026, 1, 20, 0, 0, 0);
    auto end = TimePoint::FromUTC(2026, 1, 21, 0, 0, 0);
    Vector3d pos{6678137, 0, 0};
    Vector3d vel{0, 6789.53029, 3686.414173};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    EXPECT_NEAR(pos[0],  6304667.00836, 1e-4);
    EXPECT_NEAR(pos[1], -2092242.66016, 1e-4);
    EXPECT_NEAR(pos[2],  -678674.87954, 1e-4);
    EXPECT_NEAR(vel[0], 2503.448146081, 1e-7);
    EXPECT_NEAR(vel[1], 6362.232926361, 1e-7);
    EXPECT_NEAR(vel[2], 3599.248709178, 1e-7);
}

TEST_F(HPOPTest, OnlyGravity_70_50)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity_ = false;
    forcemodel.gravity_.model_ = "JGM3";
    forcemodel.gravity_.degree_ = 70;
    forcemodel.gravity_.order_ = 50;
    HPOP propagator;
    err_t err = propagator.setForceModel(forcemodel);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2026, 1, 20, 0, 0, 0);
    auto end = TimePoint::FromUTC(2026, 1, 21, 0, 0, 0);
    Vector3d pos{6678137, 0, 0};
    Vector3d vel{0, 6789.53029, 3686.414173};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    EXPECT_NEAR(pos[0],  6302382.63957, 1e-2);
    EXPECT_NEAR(pos[1], -2096352.94648, 1e-1);
    EXPECT_NEAR(pos[2],  -679973.96122, 1e-2);
    EXPECT_NEAR(vel[0],  2508.5830626, 1e-4);
    EXPECT_NEAR(vel[1],  6361.2154510, 1e-5);
    EXPECT_NEAR(vel[2],  3599.3483256, 1e-5);
}

TEST_F(HPOPTest, OnlyGravity_20_20)
{
    if(!aIsCI()){
        GTEST_SKIP();
    }
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity_ = false;
    forcemodel.gravity_.model_ = "JGM3";
    forcemodel.gravity_.degree_ = 20;
    forcemodel.gravity_.order_ = 20;
    HPOP propagator;
    err_t err = propagator.setForceModel(forcemodel);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2026, 1, 20, 0, 0, 0);
    auto end = TimePoint::FromUTC(2026, 2, 19, 0, 0, 0);
    double duration = end - start;
    double delta = duration - 30 * 86400.;
    printf("duration: %f\n", duration);
    printf("delta: %f\n", delta);
    Vector3d pos{6678137, 0, 0};
    Vector3d vel{0, 6789.53029, 3686.414173};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    Vector3d posExpect{-3751978.1406987, 5486879.688504, -634647.012079};
    Vector3d velExpect{-5821.980921923, -3557.877945309, 3625.945661598};
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-1);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-1);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-1);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-4);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-4);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-5);
}

TEST_F(HPOPTest, MoonThirdBody)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity_ = true;
    forcemodel.moonGravity_ = 4.90280030555540e12;
    forcemodel.gravity_.model_ = "JGM3";
    forcemodel.gravity_.degree_ = 0;
    forcemodel.gravity_.order_ = 0;
    HPOP propagator;
    err_t err = propagator.setForceModel(forcemodel);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2028, 6, 24, 17, 32, 54.47325613);
    auto end   = TimePoint::FromUTC(2028, 6, 27, 11, 7,  45.21180341);
    Vector3d pos{ 6354014.813902841881, -1376122.664308371721, -781613.513150640414};
    Vector3d vel{ 1746.646187542246, 10168.796191792746, -3704.293552428442};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    Vector3d posExpect{ -343181741.5553657, 132364959.4210277, 31096923.2826433145 };
    Vector3d velExpect{ 615.919587695, 1171.545013541995, 749.472303573647 };
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-4);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-4);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-7);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-7);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-8);
}

TEST_F(HPOPTest, MoonFreeReturn)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity_ = true;
    forcemodel.moonGravity_ = 4.90280030555540e12;
    forcemodel.gravity_.model_ = "JGM3";
    forcemodel.gravity_.degree_ = 0;
    forcemodel.gravity_.order_ = 0;
    HPOP propagator;
    err_t err = propagator.setForceModel(forcemodel);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2028, 6, 24, 17, 36, 59.83014802);
    auto end   = TimePoint::FromUTC(2028, 6, 30,  2, 59, 52.66388130);
    Vector3d pos{ 6354566.839031312615, -1373571.305321738357, -781613.513150649029};
    Vector3d vel{ 1742.576871508756, 10169.577066826638, -3704.322833361938};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    Vector3d posExpect{ 5533457.951033574529, -3028079.859784354921, -1237950.223206555471 };
    Vector3d velExpect{ 5206.902914248729, 6554.495771193961, 7241.520277290054};
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-2);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-2);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-2);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-5);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-6);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-6);
}

GTEST_MAIN();

