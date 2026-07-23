///
/// @file      testAccessAnalysis.cpp
/// @brief     卫星对地面站可见性分析测试
/// @details   通过代码构建卫星和地面站对象，调用可见性分析框架计算访问时段。
///            对象参数与配置文件严格一致。
/// @author    axel
/// @date      2026-07-22
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "ast/BodyObstructionConstraint.hpp"
#include "ast/FieldOfViewConstraint.hpp"
#include "ast/FOVSimpleCone.hpp"
#include "ast/AndConstraint.hpp"
#include "ast/AccessEvaluator.hpp"
#include "ast/FixedStepStepper.hpp"
#include "ast/AccessStepper.hpp"
#include "ast/Facility.hpp"
#include "ast/Satellite.hpp"
#include "ast/Mover.hpp"
#include "ast/MotionTwoBody.hpp"
#include "ast/StateKeplerian.hpp"
#include "ast/State.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/EventIntervalExplicit.hpp"
#include "ast/EventTimeExplicit.hpp"
#include "ast/FrameAssembly.hpp"
#include "ast/BuiltinAxes.hpp"
#include "ast/RunTime.hpp"
#include "ast/RunTimeSolarSystem.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/TimeIntervalList.hpp"
#include "ast/Literals.hpp"
#include "ast/Test.hpp"
#include "ast/MathDegree.hpp"
#include "ast/EOP.hpp"
#include <clocale>
#include <cmath>
#include <cstdio>
#include <vector>

AST_USING_NAMESPACE

using namespace ast::literals;

// ==================== 测试辅助类型与常量 ====================

/// @brief 基准访问区间
struct BaselineInterval {
    int       index;
    TimePoint start;
    TimePoint stop;
    double    duration;
};

/// @brief 基准对比容差
static const double kTimeTol = 0.001;  // 1ms
static const double kDurTol  = 0.01;   // 10ms

/// @brief 默认分析时间区间（22 Jul 2026 04:00 — 23 Jul 2026 04:00 UTC）
static TimeInterval DefaultInterval()
{
    return {
        TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0),
        TimePoint::FromUTC(2026, 7, 23, 4, 0, 0.0)
    };
}

/// @brief 与基准结果逐项对比
static void VerifyBaseline(const TimeIntervalList& result,
                           const BaselineInterval* baseline, size_t count,
                           double expectedTotal)
{
    ASSERT_EQ(result.size(), count) << "Interval count mismatch";

    double total = 0.0;
    for (size_t i = 0; i < result.size(); ++i) {
        TimeInterval ti = result[i];
        const auto& bl = baseline[i];
        total += ti.duration();

        EXPECT_LT(fabs(ti.start() - bl.start), kTimeTol)
            << "Interval " << bl.index << " start mismatch";
        EXPECT_LT(fabs(ti.stop() - bl.stop), kTimeTol)
            << "Interval " << bl.index << " stop mismatch";
        EXPECT_LT(fabs(ti.duration() - bl.duration), kDurTol)
            << "Interval " << bl.index << " duration mismatch";
    }
    EXPECT_NEAR(total, expectedTotal, kDurTol) << "Total duration mismatch";
}

/// @brief 打印访问时段
static double PrintIntervals(const char* title, const TimeIntervalList& result, int prec)
{
    printf("\n=== %s ===\n", title);
    printf("Found %zu access intervals:\n", result.size());
    double total = 0.0;
    for (size_t i = 0; i < result.size(); ++i) {
        TimeInterval ti = result[i];
        printf("  [%zu] %s  (duration = %.*f s)\n",
               i, ti.toString().c_str(), prec, ti.duration());
        total += ti.duration();
    }
    printf("Total access time: %.*f s (%.2f min)\n\n", prec, total, total / 60.0);
    return total;
}

/// @brief 用单约束计算访问时段
static TimeIntervalList EvaluateAccess(AccessConstraint* constraint)
{
    FixedStepStepper stepper(60.0);
    AccessEvaluator evaluator;
    evaluator.setConstraint(constraint);
    evaluator.setStepper(&stepper);

    TimeIntervalList result;
    evaluator.evaluate(DefaultInterval(), result);
    return result;
}

// ==================== 测试辅助：构建对象 ====================

static Satellite* CreateSatellite(
    const std::string& name,
    double rp, double ecc, double incDeg, double raanDeg, double argPeriDeg, double trueADeg,
    const TimePoint& epoch)
{
    auto sat = new Satellite();
    sat->setName(name);

    // 创建二体运动模型
    auto motion = MotionTwoBody::New();

    // 创建开普勒轨道状态
    auto state = StateKeplerian::New();

    ModOrbElem elem{};
    elem.rp_     = rp;
    elem.e_      = ecc;
    elem.i_      = deg2rad(incDeg);
    elem.raan_   = deg2rad(raanDeg);
    elem.argper_ = deg2rad(argPeriDeg);
    elem.trueA_  = deg2rad(trueADeg);

    // 获取地球天体
    CelestialBody* earth = aGetEarth();

    // 创建 ICRF 坐标系
    auto frame = earth->makeFrameICRF();

    state->setFrame(frame.get());
    state->setState(elem);
    state->setStateEpoch(epoch);

    // 配置运动模型
    motion->setInitialState(state);
    motion->setPropagationFrame(frame.get());

    TimePoint stopTime = epoch + 86400.0;
    motion->setInterval(EventIntervalExplicit::New(epoch, stopTime));
    motion->setStepSize(60.0);

    // 将运动模型附加到卫星
    sat->setMotionProfile(motion);

    return sat;
}

static Satellite* CreateSatellite1()
{
    return CreateSatellite("Satellite1",
        6678137.0, 0.0, 28.5, 0.0, 0.0, 0.0,
        TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0));
}

static Satellite* CreateSatellite2()
{
    return CreateSatellite("Satellite2",
        1.0e7, 0.0, 58.5, 50.0, 0.0, 0.0,
        TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0));
}

static Facility* CreateFacility(const std::string& name, double latDeg, double lonDeg, double altMeter)
{
    auto f = new Facility();
    f->setName(name);
    f->setLatitude(deg2rad(latDeg));
    f->setLongitude(deg2rad(lonDeg));
    f->setAltitude(altMeter);
    return f;
}

static Facility* CreateFacility1()
{
    return CreateFacility("Facility1", 4.0038609999999998e+01, -7.5596599999999995e+01, 0.0);
}

static Facility* CreateFacility2()
{
    return CreateFacility("Facility2", 20.0, 1.1666666666666666e+01, 0.0);
}

// ==================== 测试 Fixture ====================

class AccessAnalysisTest : public testing::Test
{
public:
    void SetUp() override
    {
        setlocale(LC_ALL, ".UTF-8");
        aInitialize();
        aDataContext_GetEOP()->unload();
    }
    void TearDown() override
    {
        aUninitialize();
    }
};

// ==================== 测试 1：Satellite1 → Facility1 ====================

TEST_F(AccessAnalysisTest, Satellite1ToFacility1)
{
    Satellite* sat = CreateSatellite1();
    Facility*  fac = CreateFacility1();
    ASSERT_EQ(sat->generateEphemeris(), eNoError);

    BodyObstructionConstraint constraint(sat, fac, aGetEarth());
    TimeIntervalList result = EvaluateAccess(&constraint);
    PrintIntervals("Satellite1 -> Facility1", result, 3);

    static const BaselineInterval kBaseline[] = {
        {1, TimePoint::FromUTC(2026, 7, 22, 13, 17, 50.5968),
            TimePoint::FromUTC(2026, 7, 22, 13, 23, 43.8780), 353.2811},
        {2, TimePoint::FromUTC(2026, 7, 22, 14, 52, 26.0666),
            TimePoint::FromUTC(2026, 7, 22, 14, 59, 26.0345), 419.9679},
        {3, TimePoint::FromUTC(2026, 7, 22, 16, 28,  1.2828),
            TimePoint::FromUTC(2026, 7, 22, 16, 34,  9.4526), 368.1698},
    };
    VerifyBaseline(result, kBaseline, 3, 1141.4188);

    delete fac;
    delete sat;
}

// ==================== 测试 2：Satellite1 → Facility2 ====================

TEST_F(AccessAnalysisTest, Satellite1ToFacility2)
{
    Satellite* sat = CreateSatellite1();
    Facility*  fac = CreateFacility2();
    ASSERT_EQ(sat->generateEphemeris(), eNoError);

    BodyObstructionConstraint constraint(sat, fac, aGetEarth());
    TimeIntervalList result = EvaluateAccess(&constraint);
    PrintIntervals("Satellite1 -> Facility2", result, 4);

    static const BaselineInterval kBaseline[] = {
        {1, TimePoint::FromUTC(2026, 7, 22,  4,  1, 47.0393),
            TimePoint::FromUTC(2026, 7, 22,  4,  8, 39.2264), 412.1870},
        {2, TimePoint::FromUTC(2026, 7, 22,  5, 36, 13.4007),
            TimePoint::FromUTC(2026, 7, 22,  5, 45, 22.8149), 549.4141},
        {3, TimePoint::FromUTC(2026, 7, 22,  7, 12, 16.6308),
            TimePoint::FromUTC(2026, 7, 22,  7, 21,  5.9310), 529.3002},
        {4, TimePoint::FromUTC(2026, 7, 22,  8, 48, 50.1641),
            TimePoint::FromUTC(2026, 7, 22,  8, 56, 57.7461), 487.5820},
        {5, TimePoint::FromUTC(2026, 7, 22, 10, 24, 57.0230),
            TimePoint::FromUTC(2026, 7, 22, 10, 33, 24.9100), 507.8870},
        {6, TimePoint::FromUTC(2026, 7, 22, 12,  0, 37.5973),
            TimePoint::FromUTC(2026, 7, 22, 12,  9, 48.7444), 551.1471},
        {7, TimePoint::FromUTC(2026, 7, 22, 13, 36, 43.4926),
            TimePoint::FromUTC(2026, 7, 22, 13, 45,  5.5152), 502.0226},
    };
    VerifyBaseline(result, kBaseline, 7, 3539.5400);

    delete fac;
    delete sat;
}

// ==================== 测试 3：Satellite1 → Satellite2 ====================

TEST_F(AccessAnalysisTest, Satellite1ToSatellite2)
{
    Satellite* sat1 = CreateSatellite1();
    Satellite* sat2 = CreateSatellite2();
    ASSERT_EQ(sat1->generateEphemeris(), eNoError);
    ASSERT_EQ(sat2->generateEphemeris(), eNoError);

    BodyObstructionConstraint constraint(sat1, sat2, aGetEarth());
    TimeIntervalList result = EvaluateAccess(&constraint);
    PrintIntervals("Satellite1 -> Satellite2", result, 4);

    static const BaselineInterval kBaseline[] = {
        {1, TimePoint::FromUTC(2026, 7, 22,  4,  0,  0.0000),
            TimePoint::FromUTC(2026, 7, 22,  4, 51, 48.5809), 3108.5809},
        {2, TimePoint::FromUTC(2026, 7, 22,  7,  2, 32.1665),
            TimePoint::FromUTC(2026, 7, 22,  8, 19, 24.0335), 4611.8670},
        {3, TimePoint::FromUTC(2026, 7, 22, 10, 27,  8.8420),
            TimePoint::FromUTC(2026, 7, 22, 11, 30, 16.3041), 3787.4621},
        {4, TimePoint::FromUTC(2026, 7, 22, 13, 45, 18.4471),
            TimePoint::FromUTC(2026, 7, 22, 15,  0,  0.4138), 4481.9667},
        {5, TimePoint::FromUTC(2026, 7, 22, 16, 57, 31.7290),
            TimePoint::FromUTC(2026, 7, 22, 18, 12, 46.1887), 4514.4596},
        {6, TimePoint::FromUTC(2026, 7, 22, 20, 27, 37.8986),
            TimePoint::FromUTC(2026, 7, 22, 21, 28, 45.3587), 3667.4601},
        {7, TimePoint::FromUTC(2026, 7, 22, 23, 38, 35.9752),
            TimePoint::FromUTC(2026, 7, 23,  0, 55, 28.6028), 4612.6276},
        {8, TimePoint::FromUTC(2026, 7, 23,  3,  4, 27.6884),
            TimePoint::FromUTC(2026, 7, 23,  4,  0,  0.0000), 3332.3116},
    };
    VerifyBaseline(result, kBaseline, 8, 32116.7356);

    delete sat1;
    delete sat2;
}

// ==================== Sensor2 基准 ====================

static const BaselineInterval kSensor2Baseline[] = {
    {1, TimePoint::FromUTC(2026, 7, 22, 12, 11, 58.6366),
        TimePoint::FromUTC(2026, 7, 22, 12, 14, 32.7797), 154.143},
    {2, TimePoint::FromUTC(2026, 7, 23,  0, 47,  5.0274),
        TimePoint::FromUTC(2026, 7, 23,  0, 49, 35.0256), 149.998},
};

// ==================== 测试 4：Sensor2 → Facility1（交集法）====================

TEST_F(AccessAnalysisTest, Sensor2ToFacility1)
{
    Satellite* sat2 = CreateSatellite2();
    Facility*  fac1 = CreateFacility1();
    ASSERT_EQ(sat2->generateEphemeris(), eNoError);

    auto sensorFrame = FrameAssembly::New(sat2, aAxesICRF());
    FOVSimpleCone fov;
    fov.setConeAngle(deg2rad(45.0));

    FixedStepStepper stepper(60.0);

    FieldOfViewConstraint fovConstraint(sensorFrame, fac1, &fov);
    AccessEvaluator fovEval;
    fovEval.setConstraint(&fovConstraint);
    fovEval.setStepper(&stepper);
    TimeIntervalList fovResult;
    fovEval.evaluate(DefaultInterval(), fovResult);

    BodyObstructionConstraint losConstraint(sat2, fac1, aGetEarth());
    AccessEvaluator losEval;
    losEval.setConstraint(&losConstraint);
    losEval.setStepper(&stepper);
    TimeIntervalList losResult;
    losEval.evaluate(DefaultInterval(), losResult);

    TimeIntervalList result = fovResult.intersect(losResult);

    PrintIntervals("Sensor2 -> Facility1 (FOV & LOS)", result, 4);
    printf("FOV: %zu, LOS: %zu, combined: %zu\n",
           fovResult.size(), losResult.size(), result.size());
    VerifyBaseline(result, kSensor2Baseline, 2, 304.141);

    delete fac1;
    delete sat2;
}

// ==================== 测试 5：Sensor2 → Facility1（AndConstraint）====================

TEST_F(AccessAnalysisTest, Sensor2ToFacility1_AndConstraint)
{
    Satellite* sat2 = CreateSatellite2();
    Facility*  fac1 = CreateFacility1();
    ASSERT_EQ(sat2->generateEphemeris(), eNoError);

    auto sensorFrame = FrameAssembly::New(sat2, aAxesICRF());
    auto pFov = new FOVSimpleCone();
    pFov->setConeAngle(deg2rad(45.0));
    auto pFovConstraint = new FieldOfViewConstraint(sensorFrame, fac1, pFov);
    auto pLosConstraint = new BodyObstructionConstraint(sat2, fac1, aGetEarth());

    AndConstraint combined;
    combined.add(pFovConstraint);
    combined.add(pLosConstraint);

    TimeIntervalList result = EvaluateAccess(&combined);
    PrintIntervals("Sensor2 -> Facility1 (AndConstraint)", result, 4);
    VerifyBaseline(result, kSensor2Baseline, 2, 304.141);

    delete fac1;
    delete sat2;
}

// ==================== 测试 6：Sensor2 → Facility1（向量 aEvaluateAccess）====================

TEST_F(AccessAnalysisTest, Sensor2ToFacility1_VectorEval)
{
    Satellite* sat2 = CreateSatellite2();
    Facility*  fac1 = CreateFacility1();
    ASSERT_EQ(sat2->generateEphemeris(), eNoError);

    auto sensorFrame = FrameAssembly::New(sat2, aAxesICRF());
    FOVSimpleCone fov;
    fov.setConeAngle(deg2rad(45.0));
    FieldOfViewConstraint fovConstraint(sensorFrame, fac1, &fov);
    BodyObstructionConstraint losConstraint(sat2, fac1, aGetEarth());

    std::vector<AccessConstraint*> constraints = {&fovConstraint, &losConstraint};
    FixedStepStepper stepper(60.0);
    TimeIntervalList result;
    aEvaluateAccess(constraints, &stepper, DefaultInterval(), result);

    PrintIntervals("Sensor2 -> Facility1 (vector aEvaluateAccess)", result, 4);
    VerifyBaseline(result, kSensor2Baseline, 2, 304.141);

    delete fac1;
    delete sat2;
}

GTEST_MAIN()
