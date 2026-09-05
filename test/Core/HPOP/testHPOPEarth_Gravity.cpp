///
/// @file      testHPOPEarth_Gravity.cpp
/// @brief     地球重力场专项测试（纯引力，无摄动）
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


/// @brief 测试方程初始化与维度
/// @details
/// 验证 HPOPEquation 的 initialize(forceModel, spacecraftParam, frame) 流程：
///   - 设置 WGS84 重力模型后初始化，状态量维度应为 6（3位置 + 3速度）
///   - 启用月球引力后用新 equation 重新初始化，维度保持 6 不变（摄动力不增加状态量个数）
TEST_F(HPOPTest, HPOPEquation)
{
    HPOPEquation equation;
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "WGS84";
    errc_t err = equation.initialize(forcemodel, SpacecraftParam{}, nullptr);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(equation.getDimension(), 6);

    // 配置变更：equation 不再持有配置，需用新 equation 重建图（HPOP 通过置空 equation_ 达到同样效果）
    forcemodel.useMoonGravity(true);
    HPOPEquation equation2;
    errc_t err2 = equation2.initialize(forcemodel, SpacecraftParam{}, nullptr);
    EXPECT_EQ(err2, 0);
    EXPECT_EQ(equation2.getDimension(), 6);
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


GTEST_MAIN();
