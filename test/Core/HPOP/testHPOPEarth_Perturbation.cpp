///
/// @file      testHPOPEarth_Perturbation.cpp
/// @brief     摄动力综合测试（阻力、三体、月球场景）
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

#include "ast/HPOP.hpp"
#include "ast/HPOPEquation.hpp"
#include "ast/EOP.hpp"
#include "ast/RunTime.hpp"
#include "ast/Test.hpp"
#include "ast/Vector.hpp"
#include "ast/Environment.hpp"
#include "ast/ODEVarStepIntegrator.hpp"
#include "ast/SpacecraftParam.hpp"
#include "ast/Literals.hpp"
#include "ast/DetectorPeriapsis.hpp"
#include "ast/Resolve.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/FrameTransform.hpp"
#include "ast/Transform.hpp"
#include "ast/KinematicTransform.hpp"

AST_USING_NAMESPACE
using namespace literals;
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


/// @brief 测试地月转移，反向预报
TEST_F(HPOPTest, Earth_Moon_Transfer_Backward_Propagation)
{
    HPOPForceModel forceModel;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useDrag(true);
    forceModel.drag().atmDensityModel_ = EAtmDensityModel::eNRLMSISE2000;
    forceModel.drag().f10p7Average_ = 15.0;
    forceModel.drag().f10p7Daily_ = 15.0;
    forceModel.drag().kp_ = 3;

    forceModel.useSRP(true);
    forceModel.srp().sunPosition_ = ESunPosition::eTrue;
    forceModel.srp().shadowModel_ = EShadowModel::eDualCone;

    forceModel.addThirdBody("Moon");
    forceModel.addThirdBody("Sun");

    SpacecraftParam scParam;
    scParam.setDryMass(200_kg);
    scParam.setFuelMass(26000_kg);
    scParam.setCd(2.2);
    scParam.setDragArea(50_m2);
    scParam.setCr(1);
    scParam.setSrpArea(50_m2);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    propagator.setSpacecraftParam(scParam);

    auto start = "2029-10-01 00:00:00"_utc;
    auto end = start - 10_day;

    auto detector = aMakeShared<DetectorPeriapsis>();
    detector->setBody("Earth");
    propagator.addEventDetector(detector.get());

    ModOrbElem modOrbElem{1937400, 1.264390077164752, 156.132018119242332_deg, 173.10502120709711_deg, 108.963351860038912_deg, 0};
    double gmMoon = aGetMoon()->getGM();
    CartState moonJ2000;
    aModOrbElemToCart(modOrbElem, gmMoon, moonJ2000.pos(), moonJ2000.vel());
    KinematicTransform transform;
    aFrameTransform("Moon J2000"_frame, "Earth ICRF"_frame, start, transform);
    CartState earthICRF;
    transform.transformPositionVelocity(moonJ2000.pos(), moonJ2000.vel(), earthICRF.pos(), earthICRF.vel());
    err = propagator.propagate(start, end, earthICRF.pos(), earthICRF.vel());

    printf("start: %s\n", start.toString().c_str());
    printf("end  : %s\n", end.toString().c_str());
    printf("pos  : %s\n", earthICRF.pos().toString().c_str());
    printf("vel  : %s\n", earthICRF.vel().toString().c_str());

    Vector3d posExpected{1574552.8400255216, -5990072.842589266, -2403805.4378060703};
    Vector3d velExpected{10249.204432921466, 1350.1713349226482, 3348.9770740081763};

    EXPECT_NEAR(earthICRF.pos()[0], posExpected[0], 1);
    EXPECT_NEAR(earthICRF.pos()[1], posExpected[1], 1);
    EXPECT_NEAR(earthICRF.pos()[2], posExpected[2], 1);
    EXPECT_NEAR(earthICRF.vel()[0], velExpected[0], 1e-3);
    EXPECT_NEAR(earthICRF.vel()[1], velExpected[1], 1e-3);
    EXPECT_NEAR(earthICRF.vel()[2], velExpected[2], 1e-3);

}


/// @brief 测试完整摄动力模型：重力场 + 太阳 + 月球三体引力
/// @details
/// 综合测试地球非球形引力（EGM2008 21×21）、太阳第三体摄动和月球第三体摄动。
///
/// 初始条件为近圆 LEO 轨道（半径 ≈ 6900 km）。
/// 力模型：EGM2008(21,21) + 太阳点质量三体 + 月球点质量三体。
TEST_F(HPOPTest, With_Sun_Moon_ThirdBody)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    ThirdBodyForce thirdBodyForce;
    thirdBodyForce.setAttractionType(EBodyAttractionType::ePointMass);
    thirdBodyForce.pointMass().gmSource_ = EGMSource::eBodyGravity;

    thirdBodyForce.setBody(aGetSun());
    forceModel.addThirdBody(thirdBodyForce);

    thirdBodyForce.setBody(aGetMoon());
    forceModel.addThirdBody(thirdBodyForce);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    EXPECT_EQ(err, 0);
    auto integrator = propagator.getIntegrator();
    auto varStepIntegrator = aobject_cast<ODEVarStepIntegrator*>(integrator);
    if(varStepIntegrator)
    {
        varStepIntegrator->setUseFixedStep(true);
        varStepIntegrator->setStepSize(60);
    }
    auto start = TimePoint::FromUTC(2026, 6, 9, 0, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 7, 9, 0, 0, 0);
    Vector3d pos{6.8440021143593639e+06, 6.8440021143593476e+05, -8.0763129517436028e-10};
    Vector3d vel{ -6.6493233842545351e+02, 6.6493233842545542e+03, 3.6463909105223511e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    Vector3d posExpect{ -5.5995469216931257e+06, -3.0816759444764131e+06, 2.5246500543340887e+06};
    Vector3d velExpect{ 4.2866120729498525e+03, -5.8469115306180220e+03, 2.3388688727148760e+03};
    EXPECT_NEAR(pos[0],  posExpect[0], 3e-4);
    EXPECT_NEAR(pos[1],  posExpect[1], 3e-4);
    EXPECT_NEAR(pos[2],  posExpect[2], 2e-4);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-6);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-6);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-6);
}


/// @brief 测试地月二体 + 月球三体引力摄动
/// @details
/// 地球和月球均视为点质量，航天器在地月系统中运动。
///
/// 力模型：地球点质量引力 + 月球第三体摄动（使用三体公式：直接项 − 间接项）。
TEST_F(HPOPTest, MoonThirdBody)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity(true);
    forcemodel.setMoonGravity(4.90280030555540e12);
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
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

/// @brief 测试月球自由返回轨道
/// @details
/// 自由返回轨道是一种绕月后自动返回地球的轨道，无需中途机动，
///
/// 力模型：地球点质量 + 月球点质量三体摄动，JPL DE 星历。
TEST_F(HPOPTest, MoonFreeReturn)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity(true);
    forcemodel.setMoonGravity(4.90280030555540e12);
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
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


/// @brief 测试月球返回轨道
/// @details
/// 从月球附近出发返回地球的轨道弧段，约 3.1 天。
///
/// 力模型：地球点质量 + 月球点质量三体摄动。
TEST_F(HPOPTest, MoonReturn)
{
    // aInitialize();
    HPOPForceModel forceModel;
    forceModel.useMoonGravity(true);
    forceModel.setMoonGravity(4.902800305555400390625e12);
    forceModel.gravity().maxDegree_ = 0;
    forceModel.gravity().maxOrder_ = 0;
    forceModel.gravity().model_ = "JGM3";
    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    EXPECT_EQ(err, 0);
    auto start = TimePoint::FromUTC(2028, 6, 27, 10, 52, 43);
    auto end   = TimePoint::FromUTC(2028, 6, 30, 13, 20, 0);
    Vector3d pos{ -344102148.522, 133236881.64935, 31537732.121};
    Vector3d vel{ 155.6, 879.050, 602.803};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());
    Vector3d posExpect{ -7744298.936267, 2645786.33049, 736890.01309 };
    Vector3d velExpect{ 9071.93747, -3461.80637, -920.620795};
    EXPECT_NEAR(pos[0],  posExpect[0], 2e-3);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-3);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-3);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-5);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-5);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-5);
}


GTEST_MAIN();
