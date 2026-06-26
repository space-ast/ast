///
/// @file      testHPOP_ThirdBodyGravity.cpp
/// @brief     三体重力场函数块单元测试
/// @details   ~
/// @author    axel
/// @date      2026-06-24
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
#include "ast/SpacecraftParam.hpp"
#include "ast/Literals.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/FrameTransform.hpp"

AST_USING_NAMESPACE
using namespace literals;

class BlockThirdBodyGravityTest : public ::testing::Test
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

/// @brief 测试 degree=0 时 BlockThirdBodyGravity 退化为 BlockThirdBodyPointMass
/// @details
/// 当重力场阶数为 0 时，球谐展开仅包含中心项，此时三体重力场应退化为
/// 点质量引力。本测试分别创建 BlockThirdBodyGravity（degree=0）和
/// BlockThirdBodyPointMass 配置的 HPOP，比较短时间预报结果是否一致。
TEST_F(BlockThirdBodyGravityTest, PointMassDegeneracy)
{
    // 构造 degree=0 重力场模式的三体力模型（仅月球，日球用点质量）
    HPOPForceModel forceModelGravity;
    forceModelGravity.gravity().model_ = "EGM2008";
    forceModelGravity.gravity().maxDegree_ = 0;
    forceModelGravity.gravity().maxOrder_ = 0;

    forceModelGravity.addThirdBody("Sun");

    auto* moonForceGravity = forceModelGravity.addThirdBody("Moon");
    moonForceGravity->gravity().model_ = "GL0420A";
    moonForceGravity->gravity().maxDegree_ = 0;     // 零阶 = 点质量退化
    moonForceGravity->gravity().maxOrder_ = 0;
    moonForceGravity->setAttractionType(EBodyAttractionType::eGravity);


    // 构造点质量模式的力模型（默认行为，用于对比基准）
    HPOPForceModel forceModelPointMass;
    forceModelPointMass.gravity().model_ = "EGM2008";
    forceModelPointMass.gravity().maxDegree_ = 0;
    forceModelPointMass.gravity().maxOrder_ = 0;
    forceModelPointMass.addThirdBody("Sun");
    auto thirdBody = forceModelPointMass.addThirdBody("Moon");
    
    thirdBody->pointMass().gmSource_ = EGMSource::eSpecifiedValue;
    aGetGravityParameter(*thirdBody->body(), "GL0420A", thirdBody->pointMass().specifiedGM_);

    SpacecraftParam scParam;
    scParam.setDryMass(1000_kg);

    HPOP propGravity;
    HPOP propPointMass;

    errc_t err = propGravity.setForceModel(forceModelGravity);
    ASSERT_EQ(err, eNoError);
    propGravity.setSpacecraftParam(scParam);

    err = propPointMass.setForceModel(forceModelPointMass);
    ASSERT_EQ(err, eNoError);
    propPointMass.setSpacecraftParam(scParam);

    // 设置初始状态：LEO 轨道
    auto epoch = "2029-10-01 00:00:00"_utc;
    OrbElem oe{6800e3, 0.001, 51.6_deg, 0, 0, 0};
    double gmEarth = aGetEarth()->getGM();
    CartState cartState;
    aOrbElemToCart(oe, gmEarth, cartState.pos(), cartState.vel());

    auto dt = 10_day;  

    // 使用重力场模式的预报
    CartState stateGravity = cartState;
    auto end1 = epoch + dt;
    err = propGravity.propagate(epoch, end1, stateGravity.pos(), stateGravity.vel());
    ASSERT_EQ(err, eNoError);

    // 使用点质量模式的预报
    CartState statePointMass = cartState;
    auto end2 = epoch + dt;
    err = propPointMass.propagate(epoch, end2, statePointMass.pos(), statePointMass.vel());
    ASSERT_EQ(err, eNoError);

    // degree=0 时两者结果应接近一致
    double posDiff = (stateGravity.pos() - statePointMass.pos()).norm();
    // 这里的误差是因为重力场加速度需要先转换为固连系，然后再从固连系转回来，计算中会产生舍入误差
    double absTol = 2e-5;  // 绝对容差 ~200nm

    EXPECT_NEAR(posDiff, 0.0, absTol)
        << "degree=0 gravity field should degenerate to point-mass result";
}

/// @brief 测试 degree>0 时非球形项产生可测差异
/// @details
/// 使用月球的非球形重力场（degree>0）计算三体摄动，结果应与点质量
/// 存在可测量的差异，验证非球形项确实被计算在内。
TEST_F(BlockThirdBodyGravityTest, NonSphericalEffect)
{
    // 构造非球形重力场模式的力模型
    HPOPForceModel forceModelGravity;
    forceModelGravity.gravity().model_ = "EGM2008";
    forceModelGravity.gravity().maxDegree_ = 0;
    forceModelGravity.gravity().maxOrder_ = 0;

    auto* moonForce = forceModelGravity.addThirdBody("Moon");
    moonForce->gravity().model_ = "GL0420A";
    moonForce->gravity().maxDegree_ = 4;      // 非零阶
    moonForce->gravity().maxOrder_ = 4;
    moonForce->setAttractionType(EBodyAttractionType::eGravity);

    // 点质量对比模型
    HPOPForceModel forceModelPointMass;
    forceModelPointMass.gravity().model_ = "EGM2008";
    forceModelPointMass.gravity().maxDegree_ = 0;
    forceModelPointMass.gravity().maxOrder_ = 0;

    auto thirdBody = forceModelPointMass.addThirdBody("Moon");
    thirdBody->pointMass().gmSource_ = EGMSource::eSpecifiedValue;
    aGetGravityParameter(*thirdBody->body(), "GL0420A", thirdBody->pointMass().specifiedGM_);

    SpacecraftParam scParam;
    scParam.setDryMass(1000_kg);

    HPOP propGravity;
    HPOP propPointMass;

    errc_t err = propGravity.setForceModel(forceModelGravity);
    ASSERT_EQ(err, eNoError);
    propGravity.setSpacecraftParam(scParam);

    err = propPointMass.setForceModel(forceModelPointMass);
    ASSERT_EQ(err, eNoError);
    propPointMass.setSpacecraftParam(scParam);

    // 初始状态：LEO 轨道
    auto epoch = "2029-10-01 00:00:00"_utc;
    OrbElem oe{6800e3, 0.001, 51.6_deg, 0, 0, 0};
    double gmEarth = aGetEarth()->getGM();
    CartState cartState;
    aOrbElemToCart(oe, gmEarth, cartState.pos(), cartState.vel());

    auto dt = 10_day;  

    CartState stateGravity = cartState;
    auto end1 = epoch + dt;
    err = propGravity.propagate(epoch, end1, stateGravity.pos(), stateGravity.vel());
    ASSERT_EQ(err, eNoError);

    CartState statePointMass = cartState;
    auto end2 = epoch + dt;
    err = propPointMass.propagate(epoch, end2, statePointMass.pos(), statePointMass.vel());
    ASSERT_EQ(err, eNoError);

    // 非球形项应产生可测量差异（>> 浮点舍入误差）
    double posDiff = (stateGravity.pos() - statePointMass.pos()).norm();

    EXPECT_GT(posDiff, 1e-4)
        << "non-spherical gravity should produce measurable difference from point-mass";
}

GTEST_MAIN();
