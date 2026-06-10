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
    propagator.addEventDetector(detector);

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

/// @brief 测试对重力场长期变化率的支持
/// @details
/// 在EGM2008 21×21基础上叠加长期变化率。
TEST_F(HPOPTest, Gravity_SecularVariation)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = true;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

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
    auto end   = TimePoint::FromUTC(2026, 7, 23, 0, 0, 0);
    Vector3d pos{6.9281370000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.5063736481185788e+03, 3.9094236978331351e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());



    Vector3d posExpect{  -2.7860997530027227e+06, 5.8800552383734034e+06, 2.3653234244502061e+06 };
    Vector3d velExpect{  -5.7912724289295857e+03, -3.9317279131837313e+03, 2.9464110271406303e+03};
    EXPECT_NEAR(pos[0],  posExpect[0], 2e-4);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-4);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-7);
    EXPECT_NEAR(vel[1],  velExpect[1], 2e-7);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-7);
}


/// @brief 测试太阳辐射压力（考虑圆柱阴影模型，真太阳位置）
/// @details
/// 在EGM2008 21×21基础上叠加太阳光压摄动。
/// 使用最简单的 SRP 配置：检测阴影，并使用真太阳位置
TEST_F(HPOPTest, SRP_WithCylindricalShadow_TrueSun)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useSRP(true);
    forceModel.srp().shadowModel_ = EShadowModel::eCylindrical;
    forceModel.srp().sunPosition_ = ESunPosition::eTrue;

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setSrpArea(20);
    scParam.setCr(1);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    propagator.setSpacecraftParam(scParam);
    EXPECT_EQ(err, 0);
    auto integrator = propagator.getIntegrator();
    auto varStepIntegrator = aobject_cast<ODEVarStepIntegrator*>(integrator);
    if(varStepIntegrator)
    {
        varStepIntegrator->setUseFixedStep(true);
        varStepIntegrator->setStepSize(60);
    }
    auto start = TimePoint::FromUTC(2026, 6, 9, 0, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 6, 13, 0, 0, 0);
    Vector3d pos{6.9281370000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.5063736481185788e+03, 3.9094236978331351e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());


    Vector3d posExpect{  1.2117973367595519e+06, 5.8671057852713587e+06, 3.4797049140907731e+06 };
    Vector3d velExpect{  -7.2773055453958086e+03, 1.9899676170590501e+03, -8.1031903062816559e+02};
    EXPECT_NEAR(pos[0],  posExpect[0], 2e-3);
    EXPECT_NEAR(pos[1],  posExpect[1], 3e-3);
    EXPECT_NEAR(pos[2],  posExpect[2], 2e-3);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-5);
    EXPECT_NEAR(vel[1],  velExpect[1], 2e-5);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-5);
}


/// @brief 测试太阳辐射压力（考虑圆锥阴影模型，真太阳位置）
/// @details
/// 在EGM2008 21×21基础上叠加太阳光压摄动。
/// 使用最简单的 SRP 配置：检测阴影，并使用真太阳位置
TEST_F(HPOPTest, SRP_WithConeShadow_TrueSun)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useSRP(true);
    forceModel.srp().shadowModel_ = EShadowModel::eDualCone;
    forceModel.srp().sunPosition_ = ESunPosition::eTrue;

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setSrpArea(20);
    scParam.setCr(1);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    propagator.setSpacecraftParam(scParam);
    EXPECT_EQ(err, 0);
    auto integrator = propagator.getIntegrator();
    auto varStepIntegrator = aobject_cast<ODEVarStepIntegrator*>(integrator);
    if(varStepIntegrator)
    {
        varStepIntegrator->setUseFixedStep(true);
        varStepIntegrator->setStepSize(60);
    }
    auto start = TimePoint::FromUTC(2026, 6, 9, 0, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 6, 13, 0, 0, 0);
    Vector3d pos{6.9281370000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.5063736481185788e+03, 3.9094236978331351e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());

    Vector3d posExpect{  1.2117982390472749e+06, 5.8671055494347895e+06, 3.4797050199101889e+06};
    Vector3d velExpect{  -7.2773053572113140e+03, 1.9899684932434402e+03, -8.1031850638668527e+02};
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-3);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-3);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-3);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-6);
    EXPECT_NEAR(vel[1],  velExpect[1], 2e-6);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-6);
}



/// @brief 测试太阳辐射压力（考虑圆锥阴影模型，视太阳位置）
/// @details
/// 在EGM2008 21×21基础上叠加太阳光压摄动。
/// 使用最简单的 SRP 配置：检测阴影，并使用视太阳位置（考虑光行差和像差）。
TEST_F(HPOPTest, SRP_WithConeShadow_ApparentSunToBody)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useSRP(true);
    forceModel.srp().shadowModel_ = EShadowModel::eDualCone;
    forceModel.srp().sunPosition_ = ESunPosition::eApparentSunToTrueCB;

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setSrpArea(20);
    scParam.setCr(1);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    propagator.setSpacecraftParam(scParam);
    EXPECT_EQ(err, 0);
    auto integrator = propagator.getIntegrator();
    auto varStepIntegrator = aobject_cast<ODEVarStepIntegrator*>(integrator);
    if(varStepIntegrator)
    {
        varStepIntegrator->setUseFixedStep(true);
        varStepIntegrator->setStepSize(60);
    }
    auto start = TimePoint::FromUTC(2026, 6, 9, 0, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 6, 13, 0, 0, 0);
    Vector3d pos{6.9281370000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.5063736481185788e+03, 3.9094236978331351e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());

    Vector3d posExpect{  1.2117982591632833e+06, 5.8671055414197138e+06, 3.4797050209281933e+06};
    Vector3d velExpect{  -7.2773053560503795e+03, 1.9899685137763549e+03, -8.1031849486451665e+02};
    EXPECT_NEAR(pos[0],  posExpect[0], 5e-1);
    EXPECT_NEAR(pos[1],  posExpect[1], 3e-1);
    EXPECT_NEAR(pos[2],  posExpect[2], 2e-1);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-4);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-3);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-3);
}



/// @brief 测试太阳辐射压力（无阴影模型，视太阳位置）
/// @details
/// 在EGM2008 21×21基础上叠加太阳光压摄动。
/// 使用最简单的 SRP 配置：不检测阴影，并使用视太阳位置（考虑光行差和像差）。
TEST_F(HPOPTest, SRP_NoShadow_ApparentSun)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useSRP(true);
    forceModel.srp().shadowModel_ = EShadowModel::eNone;
    forceModel.srp().sunPosition_ = ESunPosition::eApparent;

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setSrpArea(20);
    scParam.setCr(1);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    propagator.setSpacecraftParam(scParam);
    EXPECT_EQ(err, 0);
    auto integrator = propagator.getIntegrator();
    auto varStepIntegrator = aobject_cast<ODEVarStepIntegrator*>(integrator);
    if(varStepIntegrator)
    {
        varStepIntegrator->setUseFixedStep(true);
        varStepIntegrator->setStepSize(60);
    }
    auto start = TimePoint::FromUTC(2026, 6, 9, 0, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 7, 17, 0, 0, 0);
    Vector3d pos{6.8781370000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.6900903321018868e+03, 3.6324226769107227e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());


    Vector3d posExpect{ -4.9951497747736117e+06, -3.5746254555597445e+06, 3.0854579753351789e+06};
    Vector3d velExpect{   3.8356051445650824e+03, -6.4541844858277245e+03, -1.2740514873863249e+03};
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-3);
    EXPECT_NEAR(pos[1],  posExpect[1], 2e-3);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-3);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-5);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-6);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-6);
}

/// @brief 测试太阳辐射压力（无阴影模型，视太阳位置）
/// @details
/// 在EGM2008 21×21基础上叠加太阳光压摄动。
/// 使用最简单的 SRP 配置：不检测阴影，并使用视太阳位置（考虑光行差和像差）。
TEST_F(HPOPTest, SRP_NoShadow_ApparentSunToBody)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useSRP(true);
    forceModel.srp().shadowModel_ = EShadowModel::eNone;
    forceModel.srp().sunPosition_ = ESunPosition::eApparentSunToTrueCB;

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setSrpArea(20);
    scParam.setCr(1);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    propagator.setSpacecraftParam(scParam);
    EXPECT_EQ(err, 0);
    auto integrator = propagator.getIntegrator();
    auto varStepIntegrator = aobject_cast<ODEVarStepIntegrator*>(integrator);
    if(varStepIntegrator)
    {
        varStepIntegrator->setUseFixedStep(true);
        varStepIntegrator->setStepSize(60);
    }
    auto start = TimePoint::FromUTC(2026, 6, 9, 0, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 7, 17, 0, 0, 0);
    Vector3d pos{6.8781370000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.6900903321018868e+03, 3.6324226769107227e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());

    Vector3d posExpect{  -4.9951606036705822e+06, -3.5746072424644586e+06, 3.0854615806196616e+06 };
    Vector3d velExpect{  3.8355878481494951e+03, -6.4541968622990571e+03, -1.2740407535364818e+03 };
    EXPECT_NEAR(pos[0],  posExpect[0], 2e-3);
    EXPECT_NEAR(pos[1],  posExpect[1], 2e-3);
    EXPECT_NEAR(pos[2],  posExpect[2], 2e-3);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-5);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-5);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-5);
}


/// @brief 测试太阳辐射压力（无阴影模型，真太阳位置）
/// @details
/// 在EGM2008 21×21基础上叠加太阳光压摄动。
/// 使用最简单的 SRP 配置：不检测阴影，并使用几何太阳位置（不考虑光行差）。
TEST_F(HPOPTest, SRP_NoShadow_TrueSun)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useSRP(true);
    forceModel.srp().shadowModel_ = EShadowModel::eNone;
    forceModel.srp().sunPosition_ = ESunPosition::eTrue;

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setSrpArea(20);
    scParam.setCr(1);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    propagator.setSpacecraftParam(scParam);
    EXPECT_EQ(err, 0);
    auto integrator = propagator.getIntegrator();
    auto varStepIntegrator = aobject_cast<ODEVarStepIntegrator*>(integrator);
    if(varStepIntegrator)
    {
        varStepIntegrator->setUseFixedStep(true);
        varStepIntegrator->setStepSize(60);
    }
    auto start = TimePoint::FromUTC(2026, 6, 9, 0, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 6, 17, 0, 0, 0);
    Vector3d pos{6.8781370000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.6900903321018868e+03, 3.6324226769107227e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());

    Vector3d posExpect{ 6.2333481674869442e+06, 4.2166379211770219e+05, 2.8615712739877473e+06};
    Vector3d velExpect{ -1.2876976995850798e+03, 7.3033485417655947e+03, 1.7416876627816571e+03};
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-5);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-5);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-7);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-8);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-8);
}


/// @brief 测试大气阻力摄动（近似海拔模式）
/// @details
/// 在 EGM2008(21,21) 重力场基础上叠加 NRLMSISE-2000 大气阻力模型。
/// 使用近似海拔（useApproxAltForDrag=true）
///
/// 航天器参数：质量 1000 kg，阻力面积 20 m²，阻力系数 2.2。
/// 空间天气参数：F10.7=150（日均值与平均值相同），Kp=3（中等地磁活动）。
TEST_F(HPOPTest, Drag_ApproximateAltitude)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useDrag(true);
    forceModel.drag().atmDensityModel_ = EAtmDensityModel::eNRLMSISE2000;
    forceModel.drag().useApproxAltForDrag_ = true;
    forceModel.drag().f10p7Average_ = 150;
    forceModel.drag().f10p7Daily_ = 150;
    forceModel.drag().kp_ = 3.0;

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setDragArea(20);
    scParam.setCd(2.2);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forceModel);
    propagator.setSpacecraftParam(scParam);
    EXPECT_EQ(err, 0);
    auto integrator = propagator.getIntegrator();
    auto varStepIntegrator = aobject_cast<ODEVarStepIntegrator*>(integrator);
    if(varStepIntegrator)
    {
        varStepIntegrator->setUseFixedStep(true);
        varStepIntegrator->setStepSize(60);
    }
    auto start = TimePoint::FromUTC(2026, 6, 9, 0, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 6, 17, 0, 0, 0);
    Vector3d pos{6.8781370000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.6900903321018868e+03, 3.6324226769107227e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());

    Vector3d posExpect{ 6.0718101518087089e+06, 1.1238242325263233e+06, 3.0126906601675297e+06};
    Vector3d velExpect{ -2.0190679823240823e+03, 7.2126951799608260e+03, 1.3912689672060314e+03};
    EXPECT_NEAR(pos[0],  posExpect[0], 1);
    EXPECT_NEAR(pos[1],  posExpect[1], 1);
    EXPECT_NEAR(pos[2],  posExpect[2], 1);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-3);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-3);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-3);
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




/// @brief 测试方程初始化与维度
/// @details
/// 验证 HPOPEquation 的 setForceModel() 和 initialize() 流程：
///   - 设置 WGS84 重力模型后初始化，状态量维度应为 6（3位置 + 3速度）
///   - 启用月球引力后重新初始化，维度保持 6 不变（摄动力不增加状态量个数）
TEST_F(HPOPTest, HPOPEquation)
{
    HPOPEquation equation;
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "WGS84";
    int ndim;
    errc_t err;
    err = equation.setForceModel(forcemodel);
    equation.initialize();
    ndim = equation.getDimension();
    EXPECT_EQ(err, 0);
    EXPECT_EQ(ndim, 6);

    forcemodel.useMoonGravity(true);
    err = equation.setForceModel(forcemodel);
    equation.initialize();
    ndim = equation.getDimension();
    EXPECT_EQ(err, 0);
    EXPECT_EQ(ndim, 6);
}

/// @brief 测试纯二体引力预报
/// @details
/// 使用 JGM3 重力场 degree=0（仅使用 GM，退化为点质量二体问题），
/// 预报 24 小时。初始状态为近圆 LEO 轨道（高度约 300 km）。
///
/// 力模型：仅地球中心引力（点质量），无 J2、三体、阻力等摄动。
/// 期望值来自解析二体预报的参考结果。
TEST_F(HPOPTest, TwoBody)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity(false);
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
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

/// @brief 测试 J2 摄动（重力场 degree=2, order=0）
/// @details
/// 在二体引力基础上叠加 J2 带谐项（地球扁率）摄动。
/// J2 是地球非球形引力的主导项，引起升交点赤经和近地点幅角的长期进动。
///
/// 力模型：JGM3(2,0)，24 小时预报。
TEST_F(HPOPTest, OnlyGravity_2_0)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity(false);
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_ = 0;

    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
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

/// @brief 测试高阶重力场（70×50）短期预报
/// @details
/// 使用 JGM3 重力场 70 阶 50 次球谐展开，24 小时预报。
///
/// 注意：order=50 < degree=70，只使用了田谐项的子集，
TEST_F(HPOPTest, OnlyGravity_70_50)
{
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity(false);
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 70;
    forcemodel.gravity().maxOrder_ = 50;
    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
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

/// @brief 测试中阶重力场（20×20）30天长期预报
/// @details
/// 使用 JGM3 重力场 20×20 球谐展开，30 天预报。
///
/// 验证预报器在长期积分中的数值稳定性。
TEST_F(HPOPTest, OnlyGravity_20_20)
{
    // if(!aIsCI()){
    //     GTEST_SKIP();
    // }
    HPOPForceModel forcemodel;
    forcemodel.useMoonGravity(false);
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 20;
    forcemodel.gravity().maxOrder_ = 20;
    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
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

