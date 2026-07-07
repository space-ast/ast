///
/// @file      testHPOPEarth_SRP.cpp
/// @brief     太阳辐射压力专项测试
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
    
    // @todo: 计算精度还需要提高，可能是光照比例的原因，目前光照比例没有考虑椭球扁率等情况
    EXPECT_NEAR(pos[0],  posExpect[0], 2e-3);
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


GTEST_MAIN();
