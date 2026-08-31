///
/// @file      testHPOPMoon_Gravity.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-08-31
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
#include "ast/Resolve.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/Literals.hpp"


AST_USING_NAMESPACE


class HPOPMoonGravityTest : public ::testing::Test
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


TEST_F(HPOPMoonGravityTest, GLGM2_20_20)
{
    auto moon = "Moon"_body;
    HPOP hpop;
    SpacecraftParam spacecraftParam;
    HPOPForceModel forceModel;
    forceModel.setCentralBody(moon);
    forceModel.gravity().model_ = "GLGM2";
    forceModel.gravity().maxDegree_ = 20;
    forceModel.gravity().maxOrder_ = 20;
    hpop.setForceModel(forceModel);
    hpop.setSpacecraftParam(spacecraftParam);
    hpop.setPropagationFrame("Moon ICRF"_frame);

    auto startTime = "2029-10-07 19:25:24"_utc;
    auto stopTime  = "2029-10-12 13:11:44"_utc;
    CartState initState{-1193.88677_km, 1400.059587_km, 605.2966457_km, 1.2525667_km/s, 0.9161693_km/s, 0.3560754_km/s};
    auto state = initState;
    errc_t rc = hpop.propagate(startTime, stopTime, state.pos(), state.vel());
    EXPECT_EQ(rc, eNoError);
    CartState expectedState{1695443_m, -914709_m, -399116_m, -795.867_m/s, -1256.948_m/s, -493.587_m/s};
    EXPECT_NEAR(state.pos().x(), expectedState.pos().x(), 1);
    EXPECT_NEAR(state.pos().y(), expectedState.pos().y(), 1);
    EXPECT_NEAR(state.pos().z(), expectedState.pos().z(), 1);
    EXPECT_NEAR(state.vel().x(), expectedState.vel().x(), 1e-3);
    EXPECT_NEAR(state.vel().y(), expectedState.vel().y(), 1e-3);
    EXPECT_NEAR(state.vel().z(), expectedState.vel().z(), 1e-3);

    ModOrbElem orbElem;
    aCartToModOrbElem(state.pos(), state.vel(), moon->getGM(), orbElem);
    double periAlt = orbElem.getPeriAlt(moon->getRadius());
    double apoAlt = orbElem.getApoAlt (moon->getRadius());
    printf("state:   %s\n",    state.toString().c_str());
    printf("orbElem: %s\n",    orbElem.toString().c_str());
    printf("periAlt: %.15g\n", periAlt);
    printf("apoAlt:  %.15g\n", apoAlt);
    EXPECT_NEAR(periAlt, 175238_m, 1);
    EXPECT_NEAR(apoAlt, 230011_m, 1);
}


GTEST_MAIN()