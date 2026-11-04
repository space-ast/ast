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
#include "ast/TimePoint.hpp"
#include "ast/SpacecraftState.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/Literals.hpp"
#include "ast/ScStateCalcLongDriftRate.hpp"
#include "ast/ScStateCalcAngle.hpp"
#include "ast/ScStateCalcSemiMajorAxis.hpp"
#include "ast/ScStateCalcInclination.hpp"
#include "ast/ScStateCalcEccAnomaly.hpp"
#include "ast/ScStateCalcAltitudeOfApoapsis.hpp"
#include "ast/ScStateCalcTimePastAscNode.hpp"
#include "ast/ScStateCalcDifferenceOtherSegment.hpp"
#include "ast/ScStateCalcDeltaFromMaster.hpp"
#include "ast/ScStateCalcTrueAnomaly.hpp"
#include "ast/ScStateCalcEpoch.hpp"
#include "ast/OrbitParam.hpp"
#include "ast/EphemerisTwoBody.hpp"
#include "ast/InitialState.hpp"
#include "ast/VectorFixed.hpp"
#include "ast/AxesICRF.hpp"

#include <cmath>

AST_USING_NAMESPACE
using namespace literals;

namespace{

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
TEST(ScStateCalcLongDriftRateTest, LongDriftRateCircularEquatorial)
{
    auto scState = makeOrbitState(42164170.0, 0.0, 0.0);
    // 标称值：参考实现计算结果
    EXPECT_NEAR(calcLongDriftRate(*scState), 5.42564416615085e-09, 1e-12);
}

/// @brief 小偏心率静止轨道
TEST(ScStateCalcLongDriftRateTest, LongDriftRateSmallEccentricity)
{
    auto scState = makeOrbitState(42164170.0, 0.0005, 0.0);
    EXPECT_NEAR(calcLongDriftRate(*scState), 5.425646537287449e-09, 1e-12);
}

/// @brief 倾角 30 度，验证结果对倾角的依赖
TEST(ScStateCalcLongDriftRateTest, LongDriftRateInclined)
{
    auto scState = makeOrbitState(42164170.0, 0.0, 30.0_deg);
    EXPECT_NEAR(calcLongDriftRate(*scState), 3.0788914680312304e-09, 1e-12);
}

/// @brief 半长轴比名义值低 20 km，应为明显向东漂移（正值）
TEST(ScStateCalcLongDriftRateTest, LongDriftRateLowerSemiMajorAxis)
{
    auto scState = makeOrbitState(42144170.0, 0.0, 0.0);
    double value = calcLongDriftRate(*scState);
    EXPECT_NEAR(value, 5.734916870419346e-08, 1e-12);
    EXPECT_GT(value, 0.0);
}

/// @brief 由笛卡尔状态计算（含 12 度倾角、极小偏心率）
TEST(ScStateCalcLongDriftRateTest, LongDriftRateFromCartesianState)
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
TEST(ScStateCalcLongDriftRateTest, LongDriftRateMoon)
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
TEST(ScStateCalcLongDriftRateTest, LongDriftRateDefaultBodyIgnoresStateFrame)
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

/// @brief 双曲线轨道没有长期的经度漂移率
TEST(ScStateCalcLongDriftRateTest, LongDriftRateHyperbolic)
{
    const double radius = 7000000.0;
    const double escapeVelocity = std::sqrt(2.0 * aGetEarth()->getGM() / radius);
    CartState cartState{{radius, 0.0, 0.0}, {0.0, escapeVelocity * 1.1, 0.0}};

    SharedPtr<SpacecraftState> scState = SpacecraftState::NewDefault();
    scState->setFrame(aGetEarth()->getFrameInertial());
    ASSERT_EQ(scState->setState(cartState), eNoError);
    scState->setStateEpoch(TimePoint::FromUTC(2026, 1, 1, 0, 0, 0));

    ScStateCalcLongDriftRate calc;
    double value = 0.0;
    EXPECT_EQ(calc.calculate(*scState, value), eErrorInvalidParam);
}

/// @brief 两向量垂直，夹角为 π/2
TEST(ScStateCalcAngleTest, AnglePerpendicular)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    VectorFixed vector1, vector2;
    vector1.setAxes(aAxesICRF());
    vector1.setValue({1.0, 0.0, 0.0});
    vector2.setAxes(aAxesICRF());
    vector2.setValue({0.0, 2.0, 0.0});

    ScStateCalcAngle calc;
    calc.setVector1(&vector1);
    calc.setVector2(&vector2);
    double value = 0.0;
    ASSERT_EQ(calc.calculate(*scState, value), eNoError);
    EXPECT_NEAR(value, kPI / 2.0, 1e-15);
}

/// @brief 两向量平行，夹角为 0
TEST(ScStateCalcAngleTest, AngleParallel)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    VectorFixed vector1, vector2;
    vector1.setAxes(aAxesICRF());
    vector1.setValue({1.0, -2.0, 3.0});
    vector2.setAxes(aAxesICRF());
    vector2.setValue({2.0, -4.0, 6.0});

    ScStateCalcAngle calc;
    calc.setVector1(&vector1);
    calc.setVector2(&vector2);
    double value = 0.0;
    ASSERT_EQ(calc.calculate(*scState, value), eNoError);
    EXPECT_NEAR(value, 0.0, 1e-15);
}

/// @brief 两向量反向，夹角为 π
TEST(ScStateCalcAngleTest, AngleAntiParallel)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    VectorFixed vector1, vector2;
    vector1.setAxes(aAxesICRF());
    vector1.setValue({1.0, 0.0, 0.0});
    vector2.setAxes(aAxesICRF());
    vector2.setValue({-3.0, 0.0, 0.0});

    ScStateCalcAngle calc;
    calc.setVector1(&vector1);
    calc.setVector2(&vector2);
    double value = 0.0;
    ASSERT_EQ(calc.calculate(*scState, value), eNoError);
    EXPECT_NEAR(value, kPI, 1e-15);
}

/// @brief 夹角为 60 度，验证结果单位为弧度
TEST(ScStateCalcAngleTest, AngleSixtyDegrees)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    VectorFixed vector1, vector2;
    vector1.setAxes(aAxesICRF());
    vector1.setValue({1.0, 0.0, 0.0});
    vector2.setAxes(aAxesICRF());
    vector2.setValue({1.0, std::sqrt(3.0), 0.0});

    ScStateCalcAngle calc;
    calc.setVector1(&vector1);
    calc.setVector2(&vector2);
    double value = 0.0;
    ASSERT_EQ(calc.calculate(*scState, value), eNoError);
    EXPECT_NEAR(value, 60.0_deg, 1e-15);
}

/// @brief 零向量没有确定的方向，与 aAngle 一致返回 0
TEST(ScStateCalcAngleTest, AngleZeroVector)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    VectorFixed vector1, vector2;
    vector1.setAxes(aAxesICRF());
    vector1.setValue({1.0, 0.0, 0.0});
    vector2.setAxes(aAxesICRF());
    vector2.setValue({0.0, 0.0, 0.0});

    ScStateCalcAngle calc;
    calc.setVector1(&vector1);
    calc.setVector2(&vector2);
    double value = 0.0;
    EXPECT_EQ(calc.calculate(*scState, value), eNoError);
    EXPECT_NEAR(value, 0.0, 1e-15);
}

/// @brief 未配置向量时应返回空指针错误
TEST(ScStateCalcAngleTest, AngleMissingVector)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    ScStateCalcAngle calc;
    double value = 0.0;
    EXPECT_EQ(calc.calculate(*scState, value), eErrorNullPtr);
}

/// @brief 状态没有历元时无法确定向量的取值时刻
TEST(ScStateCalcAngleTest, AngleWithoutStateEpoch)
{
    SharedPtr<SpacecraftState> scState = SpacecraftState::NewDefault();
    ASSERT_NE(scState->getOrbitState(), nullptr);
    scState->getOrbitState()->setStateEpoch(nullptr);
    VectorFixed vector1, vector2;
    vector1.setAxes(aAxesICRF());
    vector2.setAxes(aAxesICRF());
    vector1.setValue({1.0, 0.0, 0.0});
    vector2.setValue({0.0, 1.0, 0.0});

    ScStateCalcAngle calc;
    calc.setVector1(&vector1);
    calc.setVector2(&vector2);
    double value = 0.0;
    EXPECT_EQ(calc.calculate(*scState, value), eErrorNullPtr);
}

/// @brief 第一个向量缺少参考轴系时无法确定投影目标
TEST(ScStateCalcAngleTest, AngleVector1WithoutAxes)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    VectorFixed vector1, vector2;
    vector2.setAxes(aAxesICRF());
    vector1.setValue({1.0, 0.0, 0.0});
    vector2.setValue({0.0, 1.0, 0.0});

    ScStateCalcAngle calc;
    calc.setVector1(&vector1);
    calc.setVector2(&vector2);
    double value = 0.0;
    EXPECT_EQ(calc.calculate(*scState, value), eErrorNullPtr);
}

/// @brief 第二个向量缺少参考轴系时无法转换到第一个向量的轴系
TEST(ScStateCalcAngleTest, AngleVector2WithoutAxes)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    VectorFixed vector1, vector2;
    vector1.setAxes(aAxesICRF());
    vector1.setValue({1.0, 0.0, 0.0});
    vector2.setValue({0.0, 1.0, 0.0});

    ScStateCalcAngle calc;
    calc.setVector1(&vector1);
    calc.setVector2(&vector2);
    double value = 0.0;
    EXPECT_EQ(calc.calculate(*scState, value), eErrorNullPtr);
}

/// @brief 历元计算量返回状态的历元（自 J2000 起的秒数）
TEST(ScStateCalcEpochTest, EpochFromState)
{
    const TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    ScStateCalcEpoch calc;
    double value = 0.0;
    EXPECT_EQ(calc.calculate(*scState, value), eNoError);
    EXPECT_NEAR(value, aTimePointToEpochSecond(epoch), 1e-9);
}

/// @brief 状态没有历元时应向上传递错误
TEST(ScStateCalcEpochTest, EpochWithoutStateEpoch)
{
    SharedPtr<SpacecraftState> scState = SpacecraftState::NewDefault();
    ASSERT_NE(scState->getOrbitState(), nullptr);
    scState->getOrbitState()->setStateEpoch(nullptr);
    ScStateCalcEpoch calc;
    double value = 0.0;
    EXPECT_EQ(calc.calculate(*scState, value), eErrorNullPtr);
}

/// @brief 计算半长轴
double calcSemiMajorAxis(const SpacecraftState& scState, CelestialBody* body = nullptr)
{
    ScStateCalcSemiMajorAxis calc;
    if(body)
        calc.setBody(body);
    double value = 0.0;
    EXPECT_EQ(calc.calculate(scState, value), eNoError);
    return value;
}

/// @brief 圆轨道半长轴
TEST(ScStateCalcSemiMajorAxisTest, SemiMajorAxisCircular)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0);
    EXPECT_NEAR(calcSemiMajorAxis(*scState), 7000000.0, 1e-6);
}

/// @brief 椭圆轨道半长轴，验证与偏心率、倾角无关
TEST(ScStateCalcSemiMajorAxisTest, SemiMajorAxisElliptical)
{
    auto scState = makeOrbitState(7000000.0, 0.1, 30.0_deg);
    EXPECT_NEAR(calcSemiMajorAxis(*scState), 7000000.0, 1e-6);
}

/// @brief 计算量自身指定中心天体为月球
TEST(ScStateCalcSemiMajorAxisTest, SemiMajorAxisMoon)
{
    // 月球惯性系下的圆轨道
    auto moonState = makeOrbitState(88463000.0, 0.0, 0.0, aGetMoon());

    // 同一物理状态在地球惯性系下的表示，计算时应自动转换回月球惯性系
    CartState cartInEarth;
    ASSERT_EQ(moonState->getStateIn(aGetEarth()->getFrameInertial(), cartInEarth), eNoError);
    SharedPtr<SpacecraftState> earthState = SpacecraftState::NewDefault();
    earthState->setFrame(aGetEarth()->getFrameInertial());
    ASSERT_EQ(earthState->setState(cartInEarth), eNoError);
    earthState->setStateEpoch(TimePoint::FromUTC(2026, 1, 1, 0, 0, 0));

    EXPECT_NEAR(calcSemiMajorAxis(*moonState, aGetMoon()), 88463000.0, 1e-3);
    // 与输入状态所在坐标系无关（坐标转换会引入微小的数值差）
    EXPECT_NEAR(calcSemiMajorAxis(*earthState, aGetMoon()), calcSemiMajorAxis(*moonState, aGetMoon()), 1e-3);
}

/// @brief 未配置中心天体时使用默认天体（地球）
TEST(ScStateCalcSemiMajorAxisTest, SemiMajorAxisDefaultBody)
{
    auto earthState = makeOrbitState(7000000.0, 0.0, 0.0);
    EXPECT_NEAR(calcSemiMajorAxis(*earthState), calcSemiMajorAxis(*earthState, aGetEarth()), 1e-9);
}

/// @brief 由给定半径的圆轨道构造主航天器星历，历元与 makeOrbitState 一致
SharedPtr<EphemerisTwoBody> makeMasterEphemeris(double radius)
{
    const TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    const double gm = aGetEarth()->getGM();
    const double velocity = std::sqrt(gm / radius);
    CartState state{{radius, 0.0, 0.0}, {0.0, velocity, 0.0}};
    return EphemerisTwoBody::New(aGetEarth()->getFrameInertial(), gm, epoch, state);
}

/// @brief 当前值减主航天器值
TEST(ScStateCalcDeltaFromMasterTest, CurrentMinusMaster)
{
    auto currentState = makeOrbitState(9000000.0, 0.0, 0.0);
    auto master = makeMasterEphemeris(8000000.0);

    SharedPtr<ScStateCalcSemiMajorAxis> calculation = new ScStateCalcSemiMajorAxis();
    SharedPtr<ScStateCalcDeltaFromMaster> delta = new ScStateCalcDeltaFromMaster();
    delta->setCalculation(calculation.get());
    delta->setReference(master.get());

    double value = 0.0;
    ASSERT_EQ(delta->calculate(*currentState, value), eNoError);
    EXPECT_NEAR(value, 1.0e6, 1e-6);
    EXPECT_EQ(delta->getDimension(), EDimension::eLength);
}

/// @brief 主航天器按当前状态的历元取值，而非其星历历元
TEST(ScStateCalcDeltaFromMasterTest, UsesCurrentEpoch)
{
    auto currentState = makeOrbitState(9000000.0, 0.0, 0.0);
    const TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 10, 0);
    currentState->setStateEpoch(epoch);

    auto master = makeMasterEphemeris(8000000.0);
    Vector3d masterPos, masterVel;
    ASSERT_EQ(master->getPosVel(epoch, masterPos, masterVel), eNoError);
    ModOrbElem masterElem{};
    ASSERT_EQ(aCartToModOrbElem(masterPos, masterVel, aGetEarth()->getGM(), masterElem), eNoError);

    SharedPtr<ScStateCalcTrueAnomaly> calculation = new ScStateCalcTrueAnomaly();
    SharedPtr<ScStateCalcDeltaFromMaster> delta = new ScStateCalcDeltaFromMaster();
    delta->setCalculation(calculation.get());
    delta->setReference(master.get());

    // 当前状态在圆轨道上，真近点角为 0
    double value = 0.0;
    ASSERT_EQ(delta->calculate(*currentState, value), eNoError);
    EXPECT_NEAR(value, -masterElem.getTrueAnomaly(), 1e-12);
    EXPECT_GT(std::abs(value), 1e-3);
}

/// @brief 未配置参考计算量或主航天器时应返回空指针错误
TEST(ScStateCalcDeltaFromMasterTest, MissingInputs)
{
    auto currentState = makeOrbitState(9000000.0, 0.0, 0.0);
    SharedPtr<ScStateCalcDeltaFromMaster> delta = new ScStateCalcDeltaFromMaster();
    double value = 0.0;
    EXPECT_EQ(delta->calculate(*currentState, value), eErrorNullPtr);

    delta->setCalculation(new ScStateCalcSemiMajorAxis());
    EXPECT_EQ(delta->calculate(*currentState, value), eErrorNullPtr);
}

/// @brief 将指定段的输出状态设置为给定半长轴的圆轨道
void setSegmentFinalState(Segment& segment, double sma)
{
    auto outputState = segment.getOutputState();
    ASSERT_NE(outputState, nullptr);
    outputState->setFrame(aGetEarth()->getFrameInertial());
    outputState->setState(ModOrbElem{sma, 0.0, 0.0, 0.0, 0.0, 0.0});
    outputState->setStateEpoch(TimePoint::FromUTC(2026, 1, 1, 0, 0, 0));
}

/// @brief 当前值减另一轨道段的最终值（默认顺序与默认段状态）
TEST(ScStateCalcDifferenceOtherSegmentTest, CurrentMinusFinal)
{
    auto currentState = makeOrbitState(9000000.0, 0.0, 0.0);
    auto otherInitialState = makeOrbitState(7000000.0, 0.0, 0.0);
    SharedPtr<InitialState> otherSegment = new InitialState();
    setSegmentFinalState(*otherSegment, 8000000.0);
    otherSegment->setInputState(otherInitialState.get());

    SharedPtr<ScStateCalcSemiMajorAxis> calculation = new ScStateCalcSemiMajorAxis();
    SharedPtr<ScStateCalcDifferenceOtherSegment> difference = new ScStateCalcDifferenceOtherSegment();
    difference->setCalculation(calculation.get());
    difference->setOtherSegment(otherSegment.get());

    double value = 0.0;
    ASSERT_EQ(difference->calculate(*currentState, value), eNoError);
    EXPECT_NEAR(value, 1.0e6, 1e-6);
    EXPECT_EQ(difference->getDimension(), EDimension::eLength);
}

/// @brief 当前值减另一轨道段的初始值
TEST(ScStateCalcDifferenceOtherSegmentTest, CurrentMinusInitial)
{
    auto currentState = makeOrbitState(9000000.0, 0.0, 0.0);
    auto otherInitialState = makeOrbitState(7000000.0, 0.0, 0.0);
    SharedPtr<InitialState> otherSegment = new InitialState();
    setSegmentFinalState(*otherSegment, 8000000.0);
    otherSegment->setInputState(otherInitialState.get());

    SharedPtr<ScStateCalcSemiMajorAxis> calculation = new ScStateCalcSemiMajorAxis();
    SharedPtr<ScStateCalcDifferenceOtherSegment> difference = new ScStateCalcDifferenceOtherSegment();
    difference->setCalculation(calculation.get());
    difference->setOtherSegment(otherSegment.get());
    difference->setSegmentStateToUse(ESegmentStateToUse::eInitial);

    double value = 0.0;
    ASSERT_EQ(difference->calculate(*currentState, value), eNoError);
    EXPECT_NEAR(value, 2.0e6, 1e-6);
}

/// @brief 另一轨道段的值减当前值
TEST(ScStateCalcDifferenceOtherSegmentTest, SegmentMinusCurrent)
{
    auto currentState = makeOrbitState(9000000.0, 0.0, 0.0);
    auto otherInitialState = makeOrbitState(7000000.0, 0.0, 0.0);
    SharedPtr<InitialState> otherSegment = new InitialState();
    setSegmentFinalState(*otherSegment, 8000000.0);
    otherSegment->setInputState(otherInitialState.get());

    SharedPtr<ScStateCalcSemiMajorAxis> calculation = new ScStateCalcSemiMajorAxis();
    SharedPtr<ScStateCalcDifferenceOtherSegment> difference = new ScStateCalcDifferenceOtherSegment();
    difference->setCalculation(calculation.get());
    difference->setOtherSegment(otherSegment.get());
    difference->setDifferenceOrderToUse(ESegmentDifferenceOrder::eSegmentMinusCurrent);

    double value = 0.0;
    ASSERT_EQ(difference->calculate(*currentState, value), eNoError);
    EXPECT_NEAR(value, -1.0e6, 1e-6);
}

/// @brief 未配置参考计算量或另一轨道段时应返回空指针错误
TEST(ScStateCalcDifferenceOtherSegmentTest, MissingInputs)
{
    auto currentState = makeOrbitState(9000000.0, 0.0, 0.0);
    SharedPtr<ScStateCalcSemiMajorAxis> calculation = new ScStateCalcSemiMajorAxis();
    SharedPtr<ScStateCalcDifferenceOtherSegment> difference = new ScStateCalcDifferenceOtherSegment();

    double value = 0.0;
    EXPECT_EQ(difference->calculate(*currentState, value), eErrorNullPtr);

    difference->setCalculation(calculation.get());
    EXPECT_EQ(difference->calculate(*currentState, value), eErrorNullPtr);
}

/// @brief 由修正轨道根数构造指定天体惯性系下的航天器状态
SharedPtr<SpacecraftState> makeOrbitState(double sma, double ecc, double inc,
    double raan, double argPeri, double trueA, CelestialBody* body = nullptr)
{
    if(!body)
        body = aGetEarth();
    SharedPtr<SpacecraftState> scState = SpacecraftState::NewDefault();
    scState->setFrame(body->getFrameInertial());
    ModOrbElem orbElem{sma * (1.0 - ecc), ecc, inc, raan, argPeri, trueA};
    scState->setState(orbElem);
    scState->setStateEpoch(TimePoint::FromUTC(2026, 1, 1, 0, 0, 0));
    return scState;
}

/// @brief 计算轨道倾角
double calcInclination(const SpacecraftState& scState, Frame* frame = nullptr)
{
    ScStateCalcInclination calc;
    if(frame)
        calc.setFrame(frame);
    double value = 0.0;
    EXPECT_EQ(calc.calculate(scState, value), eNoError);
    return value;
}

/// @brief 计算偏近点角
double calcEccAnomaly(const SpacecraftState& scState, CelestialBody* body = nullptr)
{
    ScStateCalcEccAnomaly calc;
    if(body)
        calc.setBody(body);
    double value = 0.0;
    EXPECT_EQ(calc.calculate(scState, value), eNoError);
    return value;
}

/// @brief 计算远地点高度
double calcAltitudeOfApoapsis(const SpacecraftState& scState, CelestialBody* body = nullptr)
{
    ScStateCalcAltitudeOfApoapsis calc;
    if(body)
        calc.setBody(body);
    double value = 0.0;
    EXPECT_EQ(calc.calculate(scState, value), eNoError);
    return value;
}

/// @brief 计算过升交点后经过的时间
double calcTimePastAscNode(const SpacecraftState& scState, Frame* frame = nullptr)
{
    ScStateCalcTimePastAscNode calc;
    if(frame)
        calc.setFrame(frame);
    double value = 0.0;
    EXPECT_EQ(calc.calculate(scState, value), eNoError);
    return value;
}

/// @brief 椭圆轨道的倾角
TEST(ScStateCalcInclinationTest, InclinationElliptical)
{
    auto scState = makeOrbitState(7000000.0, 0.1, 30.0_deg, 45.0_deg, 60.0_deg, 90.0_deg);
    EXPECT_NEAR(calcInclination(*scState), 30.0_deg, 1e-12);
}

/// @brief 逆行轨道的倾角
TEST(ScStateCalcInclinationTest, InclinationRetrograde)
{
    auto scState = makeOrbitState(7000000.0, 0.1, 120.0_deg, 45.0_deg, 60.0_deg, 90.0_deg);
    EXPECT_NEAR(calcInclination(*scState), 120.0_deg, 1e-12);
}

/// @brief 显式配置参考系
TEST(ScStateCalcInclinationTest, InclinationExplicitFrame)
{
    auto scState = makeOrbitState(7000000.0, 0.1, 30.0_deg, 45.0_deg, 60.0_deg, 90.0_deg);
    EXPECT_NEAR(calcInclination(*scState, aGetEarth()->getFrameInertial()), 30.0_deg, 1e-12);
}

/// @brief 倾角的量纲为角度
TEST(ScStateCalcInclinationTest, InclinationDimension)
{
    ScStateCalcInclination calc;
    EXPECT_EQ(calc.getDimension(), EDimension::eAngle);
}

/// @brief 椭圆轨道的偏近点角
TEST(ScStateCalcEccAnomalyTest, EccAnomalyElliptical)
{
    const double ecc = 0.1;
    const double trueA = 60.0_deg;
    auto scState = makeOrbitState(7000000.0, ecc, 30.0_deg, 45.0_deg, 60.0_deg, trueA);
    EXPECT_NEAR(calcEccAnomaly(*scState), aTrueToEcc(trueA, ecc), 1e-12);
}

/// @brief 圆轨道的偏近点角与真近点角相同
TEST(ScStateCalcEccAnomalyTest, EccAnomalyCircular)
{
    auto scState = makeOrbitState(7000000.0, 0.0, 0.0, 0.0, 0.0, 60.0_deg);
    EXPECT_NEAR(calcEccAnomaly(*scState), 60.0_deg, 1e-12);
}

/// @brief 计算量自身指定中心天体为月球
TEST(ScStateCalcEccAnomalyTest, EccAnomalyMoon)
{
    const double ecc = 0.05;
    const double trueA = 90.0_deg;
    auto moonState = makeOrbitState(88463000.0, ecc, 30.0_deg, 45.0_deg, 60.0_deg, trueA, aGetMoon());

    // 同一物理状态在地球惯性系下的表示，计算时应自动转换回月球惯性系
    CartState cartInEarth;
    ASSERT_EQ(moonState->getStateIn(aGetEarth()->getFrameInertial(), cartInEarth), eNoError);
    SharedPtr<SpacecraftState> earthState = SpacecraftState::NewDefault();
    earthState->setFrame(aGetEarth()->getFrameInertial());
    ASSERT_EQ(earthState->setState(cartInEarth), eNoError);
    earthState->setStateEpoch(TimePoint::FromUTC(2026, 1, 1, 0, 0, 0));

    EXPECT_NEAR(calcEccAnomaly(*moonState, aGetMoon()), aTrueToEcc(trueA, ecc), 1e-12);
    EXPECT_NEAR(calcEccAnomaly(*earthState, aGetMoon()), calcEccAnomaly(*moonState, aGetMoon()), 1e-8);
}

/// @brief 偏近点角的量纲为角度
TEST(ScStateCalcEccAnomalyTest, EccAnomalyDimension)
{
    ScStateCalcEccAnomaly calc;
    EXPECT_EQ(calc.getDimension(), EDimension::eAngle);
}

/// @brief 椭圆轨道的远地点高度
TEST(ScStateCalcAltitudeOfApoapsisTest, AltitudeOfApoapsisElliptical)
{
    const double sma = 7000000.0;
    const double ecc = 0.1;
    auto scState = makeOrbitState(sma, ecc, 30.0_deg, 45.0_deg, 60.0_deg, 90.0_deg);
    EXPECT_NEAR(calcAltitudeOfApoapsis(*scState),
        sma * (1.0 + ecc) - aGetEarth()->getRadius(), 1e-6);
}

/// @brief 圆轨道的远地点高度
TEST(ScStateCalcAltitudeOfApoapsisTest, AltitudeOfApoapsisCircular)
{
    const double sma = 7000000.0;
    auto scState = makeOrbitState(sma, 0.0, 0.0, 0.0, 0.0, 0.0);
    EXPECT_NEAR(calcAltitudeOfApoapsis(*scState),
        sma - aGetEarth()->getRadius(), 1e-6);
}

/// @brief 计算量自身指定中心天体为月球，高度按月球半径计算
TEST(ScStateCalcAltitudeOfApoapsisTest, AltitudeOfApoapsisMoon)
{
    const double sma = 88463000.0;
    const double ecc = 0.05;
    auto moonState = makeOrbitState(sma, ecc, 30.0_deg, 45.0_deg, 60.0_deg, 90.0_deg, aGetMoon());
    EXPECT_NEAR(calcAltitudeOfApoapsis(*moonState, aGetMoon()),
        sma * (1.0 + ecc) - aGetMoon()->getRadius(), 1e-3);
}

/// @brief 远地点高度的量纲为长度
TEST(ScStateCalcAltitudeOfApoapsisTest, AltitudeOfApoapsisDimension)
{
    ScStateCalcAltitudeOfApoapsis calc;
    EXPECT_EQ(calc.getDimension(), EDimension::eLength);
}

/// @brief 椭圆轨道的过升交点后经过的时间
TEST(ScStateCalcTimePastAscNodeTest, TimePastAscNodeElliptical)
{
    const double sma = 7000000.0;
    const double ecc = 0.1;
    const double argPeri = 30.0_deg;
    const double trueA = 60.0_deg;
    auto scState = makeOrbitState(sma, ecc, 30.0_deg, 45.0_deg, argPeri, trueA);
    EXPECT_NEAR(calcTimePastAscNode(*scState),
        aTrueToTimePastAscNode(trueA, argPeri, sma, ecc, aGetEarth()->getGM()), 1e-6);
}

/// @brief 位于升交点时为 0
TEST(ScStateCalcTimePastAscNodeTest, TimePastAscNodeAtAscendingNode)
{
    const double argPeri = 30.0_deg;
    auto scState = makeOrbitState(7000000.0, 0.1, 30.0_deg, 45.0_deg, argPeri, -argPeri);
    EXPECT_NEAR(calcTimePastAscNode(*scState), 0.0, 1e-6);
}

/// @brief 显式配置参考系
TEST(ScStateCalcTimePastAscNodeTest, TimePastAscNodeExplicitFrame)
{
    auto scState = makeOrbitState(7000000.0, 0.1, 30.0_deg, 45.0_deg, 30.0_deg, 60.0_deg);
    EXPECT_NEAR(calcTimePastAscNode(*scState, aGetEarth()->getFrameInertial()),
        calcTimePastAscNode(*scState), 1e-12);
}

/// @brief 过升交点后经过的时间的量纲为时间
TEST(ScStateCalcTimePastAscNodeTest, TimePastAscNodeDimension)
{
    ScStateCalcTimePastAscNode calc;
    EXPECT_EQ(calc.getDimension(), EDimension::eTime);
}

GTEST_MAIN()
