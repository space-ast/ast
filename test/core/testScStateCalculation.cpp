///
/// @file      testScStateCalculation.cpp
/// @brief     ScStateCalculation 计算量测试
/// @author    axel
/// @date      2026-10-29
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

#include "ast/Test.hpp"
#include "ast/RunTime.hpp"
#include "ast/EOP.hpp"
#include "ast/TimePoint.hpp"
#include "ast/SpacecraftState.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/Literals.hpp"
#include "ast/ScStateCalcLongDriftRate.hpp"

#include <cmath>

AST_USING_NAMESPACE
using namespace literals;

namespace{

class ScStateCalculationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        aInitialize();
        aDataContext_GetEOP()->unload();
    }
    void TearDown() override
    {
        aUninitialize();
    }
};

/// @brief 由经典轨道根数构造指定天体惯性系下的航天器状态
/// @param sma 半长轴
/// @param ecc 偏心率
/// @param inc 倾角
/// @param body 中心天体，为空时使用地球
SharedPtr<SpacecraftState> makeOrbitState(double sma, double ecc, double inc, CelestialBody* body = nullptr)
{
    if(!body)
        body = aGetEarth();
    SharedPtr<SpacecraftState> scState = SpacecraftState::NewDefault();
    scState->setFrame(body->getFrameInertial());
    ModOrbElem orbElem{sma * (1.0 - ecc), ecc, inc, 0.0, 0.0, 0.0};
    scState->setState(orbElem);
    scState->setStateEpoch(TimePoint::FromUTC(2026, 1, 1, 0, 0, 0));
    return scState;
}

/// @brief 计算经度漂移率
double calcLongDriftRate(const SpacecraftState& scState)
{
    ScStateCalcLongDriftRate calc;
    double value = 0.0;
    EXPECT_EQ(calc.calculate(scState, value), eNoError);
    return value;
}

} // namespace

/// @brief 圆赤道静止轨道附近（半长轴取名义静止轨道半长轴）
TEST_F(ScStateCalculationTest, LongDriftRateCircularEquatorial)
{
    auto scState = makeOrbitState(42164170.0, 0.0, 0.0);
    // 标称值：参考实现计算结果
    EXPECT_NEAR(calcLongDriftRate(*scState), 5.42564416615085e-09, 1e-12);
}

/// @brief 小偏心率静止轨道
TEST_F(ScStateCalculationTest, LongDriftRateSmallEccentricity)
{
    auto scState = makeOrbitState(42164170.0, 0.0005, 0.0);
    EXPECT_NEAR(calcLongDriftRate(*scState), 5.425646537287449e-09, 1e-12);
}

/// @brief 倾角 30 度，验证结果对倾角的依赖
TEST_F(ScStateCalculationTest, LongDriftRateInclined)
{
    auto scState = makeOrbitState(42164170.0, 0.0, 30.0_deg);
    EXPECT_NEAR(calcLongDriftRate(*scState), 3.0788914680312304e-09, 1e-12);
}

/// @brief 半长轴比名义值低 20 km，应为明显向东漂移（正值）
TEST_F(ScStateCalculationTest, LongDriftRateLowerSemiMajorAxis)
{
    auto scState = makeOrbitState(42144170.0, 0.0, 0.0);
    double value = calcLongDriftRate(*scState);
    EXPECT_NEAR(value, 5.734916870419346e-08, 1e-12);
    EXPECT_GT(value, 0.0);
}

/// @brief 由笛卡尔状态计算（含 12 度倾角、极小偏心率）
TEST_F(ScStateCalculationTest, LongDriftRateFromCartesianState)
{
    SharedPtr<SpacecraftState> scState = SpacecraftState::NewDefault();
    scState->setFrame(aGetEarth()->makeFrameICRF());
    CartState cartState{
        {42005.80521701823_km, -3594.723497458691_km, -764.0820667742561_km},
        {0.2679676403189557_km/s, 2.99595277706601_km/s, 0.6368094212189054_km/s}
    };
    ASSERT_EQ(scState->setState(cartState), eNoError);
    scState->setStateEpoch(TimePoint::FromUTC(2026, 10, 26, 4, 0, 37));
    EXPECT_NEAR(calcLongDriftRate(*scState), -4.0755095465408973e-10, 1e-12);
}

/// @brief 计算量自身指定中心天体为月球（默认配置之外的配置）
TEST_F(ScStateCalculationTest, LongDriftRateMoon)
{
    // 月面静止轨道半径附近，圆赤道轨道（月球惯性系）
    auto moonState = makeOrbitState(88463000.0, 0.0, 0.0, aGetMoon());

    // 同一物理状态在地球惯性系下的表示，计算时应自动转换回月球惯性系
    CartState cartInEarth;
    ASSERT_EQ(moonState->getStateIn(aGetEarth()->getFrameInertial(), cartInEarth), eNoError);
    SharedPtr<SpacecraftState> earthState = SpacecraftState::NewDefault();
    earthState->setFrame(aGetEarth()->getFrameInertial());
    ASSERT_EQ(earthState->setState(cartInEarth), eNoError);
    earthState->setStateEpoch(TimePoint::FromUTC(2026, 1, 1, 0, 0, 0));

    ScStateCalcLongDriftRate calc;
    calc.setBody(aGetMoon());
    double moonValue = 0.0;
    double earthValue = 0.0;
    ASSERT_EQ(calc.calculate(*moonState, moonValue), eNoError);
    ASSERT_EQ(calc.calculate(*earthState, earthValue), eNoError);
    // 标称值：参考实现计算结果
    EXPECT_NEAR(moonValue, -3.4814499213389033e-10, 1e-11);
    // 与输入状态所在坐标系无关
    EXPECT_NEAR(earthValue, moonValue, 1e-15);
}

/// @brief 未配置中心天体时使用默认天体（地球），不读取状态所在坐标系
TEST_F(ScStateCalculationTest, LongDriftRateDefaultBodyIgnoresStateFrame)
{
    const double sma = 88463000.0;
    const double velocity = std::sqrt(aGetMoon()->getGM() / sma);
    CartState cartState{{sma, 0.0, 0.0}, {0.0, velocity, 0.0}};

    // 状态置于月球惯性系
    SharedPtr<SpacecraftState> scState = SpacecraftState::NewDefault();
    scState->setFrame(aGetMoon()->getFrameInertial());
    ASSERT_EQ(scState->setState(cartState), eNoError);
    scState->setStateEpoch(TimePoint::FromUTC(2026, 1, 1, 0, 0, 0));

    ScStateCalcLongDriftRate defaultCalc;    // 未配置，应使用默认天体（地球）
    ScStateCalcLongDriftRate earthCalc;
    earthCalc.setBody(aGetEarth());

    double defaultValue = 0.0;
    double earthValue = 0.0;
    ASSERT_EQ(defaultCalc.calculate(*scState, defaultValue), eNoError);
    ASSERT_EQ(earthCalc.calculate(*scState, earthValue), eNoError);
    EXPECT_NEAR(defaultValue, earthValue, 1e-15);
}

GTEST_MAIN()
