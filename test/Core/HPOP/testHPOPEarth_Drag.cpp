///
/// @file      testHPOPEarth_Drag.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-24
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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


/// @brief 测试大气阻力摄动（MSIS86 模型）
TEST_F(HPOPTest, Drag_MSIS86)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useDrag(true);
    forceModel.drag().atmDensityModel_ = EAtmDensityModel::eMSIS1986;
    forceModel.drag().useApproxAltForDrag_ = false;
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
    Vector3d pos{6.8781400000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.6900888731120158e+03, 3.6324218847438560e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());


    Vector3d posExpect{ 6.0735523802171685e+06,  1.1177155532492716e+06,  3.0115244635127960e+06 };
    Vector3d velExpect{ -2.0127449234117348e+03, 7.2138267230884767e+03, 1.3944090160026465e+03};
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-5);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-5);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-7);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-7);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-7);
}


/// @brief 测试大气阻力摄动（MSISE90 模型）
TEST_F(HPOPTest, Drag_MSISE90)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useDrag(true);
    forceModel.drag().atmDensityModel_ = EAtmDensityModel::eMSISE1990;
    forceModel.drag().useApproxAltForDrag_ = false;
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
    Vector3d pos{6.8781400000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.6900888731120158e+03, 3.6324218847438560e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());


    Vector3d posExpect{ 6.0704715211070320e+06,  1.1286418571327669e+06,  3.0136259849788244e+06 };
    Vector3d velExpect{ -2.0240508646207834e+03,  7.2117524559037665e+03,  1.3887784485845330e+03};
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-5);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-5);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-7);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-7);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-7);
}



/// @brief 测试大气阻力摄动（NRLMSISE2000 模型）
TEST_F(HPOPTest, Drag_NRLMSISE2000)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useDrag(true);
    forceModel.drag().atmDensityModel_ = EAtmDensityModel::eNRLMSISE2000;
    forceModel.drag().useApproxAltForDrag_ = false;
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
    Vector3d pos{6.8781400000000000e+06, 0.0, 0.0};
    Vector3d vel{0.0000000000000000e+00, 6.6900888731120158e+03, 3.6324218847438560e+03};
    err = propagator.propagate(start, end, pos, vel);
    EXPECT_EQ(err, 0);
    printf("end: %s\n", end.toString().c_str());
    printf("pos: %s\n", pos.toString().c_str());
    printf("vel: %s\n", vel.toString().c_str());


    Vector3d posExpect{ 6.0727343209944004e+06,  1.1205317606959976e+06,  3.0120554995662356e+06 };
    Vector3d velExpect{ -2.0156613271822021e+03,  7.2133223946848639e+03,  1.3929662087240408e+03};
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-4);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-4);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-7);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-7);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-7);
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
    EXPECT_NEAR(pos[0],  posExpect[0], 1e-4);
    EXPECT_NEAR(pos[1],  posExpect[1], 1e-4);
    EXPECT_NEAR(pos[2],  posExpect[2], 1e-4);
    EXPECT_NEAR(vel[0],  velExpect[0], 1e-7);
    EXPECT_NEAR(vel[1],  velExpect[1], 1e-7);
    EXPECT_NEAR(vel[2],  velExpect[2], 1e-7);
}




GTEST_MAIN()

