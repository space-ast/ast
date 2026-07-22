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
#include "ast/RunTime.hpp"
#include "ast/EOP.hpp"
#include <clocale>
#include <cmath>
#include <cstdio>

AST_USING_NAMESPACE

using namespace ast::literals;

// ==================== 测试辅助：构建卫星 ====================

/// @brief 根据轨道参数构建 Satellite 对象
/// @param name       卫星名称
/// @param rp         近地点半径 (m)
/// @param ecc        偏心率
/// @param incDeg     倾角（度）
/// @param raanDeg    升交点赤经（度）
/// @param argPeriDeg 近地点辐角（度）
/// @param trueADeg   真近点角（度）
/// @param epoch      状态历元
/// @return 配置完成的 Satellite 指针
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

    TimePoint stopTime = epoch + 86400.0; // 24 小时
    auto interval = EventIntervalExplicit::New(epoch, stopTime);
    motion->setInterval(interval);
    motion->setStepSize(60.0);

    // 将运动模型附加到卫星
    sat->setMotionProfile(motion);

    return sat;
}

/// @brief 构建 Satellite1（对应 Satellite1.sa）
static Satellite* CreateSatellite1()
{
    return CreateSatellite("Satellite1",
        6678137.0, 0.0, 28.5, 0.0, 0.0, 0.0,
        TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0));
}

/// @brief 构建 Satellite2（对应 Satellite2.sa）
static Satellite* CreateSatellite2()
{
    return CreateSatellite("Satellite2",
        1.0e7, 0.0, 58.5, 50.0, 0.0, 0.0,
        TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0));
}

// ==================== 测试辅助：构建地面站 ====================

/// @brief 根据配置文件参数构建 Facility 对象
/// @param name       地面站名称
/// @param latDeg     纬度（度）
/// @param lonDeg     经度（度）
/// @param altMeter   高度（米）
/// @return 配置完成的 Facility 指针
static Facility* CreateFacilityFromParams(
    const std::string& name, double latDeg, double lonDeg, double altMeter)
{
    auto facility = new Facility();
    facility->setName(name);

    // 设置位置（Facility 构造函数已自动调用 setBody(aGetEarth())）
    facility->setLatitude(deg2rad(latDeg));
    facility->setLongitude(deg2rad(lonDeg));
    facility->setAltitude(altMeter);

    return facility;
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

// ==================== 测试用例 1：卫星对地面站1 ====================

TEST_F(AccessAnalysisTest, Satellite1ToFacility1)
{
    // 1. 构建卫星
    Satellite* sat = CreateSatellite1();
    ASSERT_NE(sat, nullptr);
    printf("Created satellite: %s\n", sat->getName().c_str());

    // 2. 生成星历
    errc_t rc = sat->generateEphemeris();
    ASSERT_EQ(rc, eNoError);
    printf("Ephemeris generated successfully.\n");

    // 3. 构建地面站（对应 Facility1.f: lat=40.03861°N, lon=75.5966°W, alt=0m）
    Facility* fac = CreateFacilityFromParams("Facility1", 4.0038609999999998e+01, -7.5596599999999995e+01, 0.0);
    ASSERT_NE(fac, nullptr);
    printf("Created facility: %s (lat=%.6f°, lon=%.6f°)\n",
           fac->getName().c_str(),
           fac->latitude() * 180.0 / kPI,
           fac->longitude() * 180.0 / kPI);

    // 4. 获取地球天体
    CelestialBody* earth = aGetEarth();
    ASSERT_NE(earth, nullptr);

    // 5. 定义分析时间区间
    TimePoint start = TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0);
    TimePoint stop  = TimePoint::FromUTC(2026, 7, 23, 4, 0, 0.0);
    TimeInterval analysisInterval(start, stop);
    printf("Analysis interval: %s\n", analysisInterval.toString().c_str());

    // 6. 创建视线遮挡约束
    BodyObstructionConstraint constraint(sat, fac, earth);

    // 7. 创建固定步进步进器（60 秒步长）
    FixedStepStepper stepper(60.0);

    // 8. 创建访问评估器
    AccessEvaluator evaluator;
    evaluator.setConstraint(&constraint);
    evaluator.setStepper(&stepper);

    // 9. 计算访问时段
    TimeIntervalList result;
    rc = evaluator.evaluate(analysisInterval, result);
    ASSERT_EQ(rc, eNoError);

    // 10. 打印结果
    printf("\n=== Access Intervals: Satellite1 -> Facility1 ===\n");
    printf("Found %zu access intervals:\n", result.size());

    double totalDuration = 0.0;
    for (size_t i = 0; i < result.size(); ++i)
    {
        TimeInterval ti = result[i];
        printf("  [%zu] %s  (duration = %.3f s)\n",
               i, ti.toString().c_str(), ti.duration());
        totalDuration += ti.duration();
    }
    printf("Total access time: %.3f s (%.2f min)\n\n",
           totalDuration, totalDuration / 60.0);

    // 11. 与基准结果对比（容差：时间 0.01s，时长 0.1s）
    struct BaselineInterval {
        int       index;
        TimePoint start;
        TimePoint stop;
        double    duration;
    };
    static const BaselineInterval kBaseline[] = {
        {1, TimePoint::FromUTC(2026, 7, 22, 13, 17, 50.5968),
            TimePoint::FromUTC(2026, 7, 22, 13, 23, 43.8780), 353.2811},
        {2, TimePoint::FromUTC(2026, 7, 22, 14, 52, 26.0666),
            TimePoint::FromUTC(2026, 7, 22, 14, 59, 26.0345), 419.9679},
        {3, TimePoint::FromUTC(2026, 7, 22, 16, 28,  1.2828),
            TimePoint::FromUTC(2026, 7, 22, 16, 34,  9.4526), 368.1698},
    };
    static const double kExpectedTotal = 1141.4188;
    static const double kTimeTol      = 0.001;  // 1ms
    static const double kDurTol       = 0.01;   // 10ms

    ASSERT_EQ(result.size(), 3u) << "Interval count mismatch";

    for (size_t i = 0; i < result.size(); ++i) {
        TimeInterval ti = result[i];
        const auto& bl = kBaseline[i];

        double startDiff = fabs(ti.start() - bl.start);
        double stopDiff  = fabs(ti.stop() - bl.stop);
        double durDiff   = fabs(ti.duration() - bl.duration);

        EXPECT_LT(startDiff, kTimeTol)
            << "Interval " << bl.index << " start mismatch: computed="
            << ti.start().toString() << " baseline=" << bl.start.toString();
        EXPECT_LT(stopDiff, kTimeTol)
            << "Interval " << bl.index << " stop mismatch: computed="
            << ti.stop().toString() << " baseline=" << bl.stop.toString();
        EXPECT_LT(durDiff, kDurTol)
            << "Interval " << bl.index << " duration mismatch: computed="
            << ti.duration() << " baseline=" << bl.duration;
    }

    EXPECT_NEAR(totalDuration, kExpectedTotal, kDurTol)
        << "Total duration mismatch";

    // 清理
    delete fac;
    delete sat;
}

// ==================== 测试用例 2：卫星对地面站2 ====================

TEST_F(AccessAnalysisTest, Satellite1ToFacility2)
{
    // 1. 构建卫星
    Satellite* sat = CreateSatellite1();
    ASSERT_NE(sat, nullptr);

    // 2. 生成星历
    errc_t rc = sat->generateEphemeris();
    ASSERT_EQ(rc, eNoError);

    // 3. 构建地面站（对应 Facility2.f: lat=20.0°N, lon=11.666667°E, alt=0m）
    Facility* fac = CreateFacilityFromParams("Facility2", 20.0, 1.1666666666666666e+01, 0.0);
    ASSERT_NE(fac, nullptr);
    printf("Created facility: %s (lat=%.6f°, lon=%.6f°)\n",
           fac->getName().c_str(),
           fac->latitude() * 180.0 / kPI,
           fac->longitude() * 180.0 / kPI);

    // 4. 获取地球天体
    CelestialBody* earth = aGetEarth();
    ASSERT_NE(earth, nullptr);

    // 5. 定义分析时间区间
    TimePoint start = TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0);
    TimePoint stop  = TimePoint::FromUTC(2026, 7, 23, 4, 0, 0.0);
    TimeInterval analysisInterval(start, stop);

    // 6. 创建约束、步进器、评估器
    BodyObstructionConstraint constraint(sat, fac, earth);
    FixedStepStepper stepper(60.0);
    AccessEvaluator evaluator;
    evaluator.setConstraint(&constraint);
    evaluator.setStepper(&stepper);

    // 7. 计算访问时段
    TimeIntervalList result;
    rc = evaluator.evaluate(analysisInterval, result);
    ASSERT_EQ(rc, eNoError);

    // 8. 打印结果
    printf("\n=== Access Intervals: Satellite1 -> Facility2 ===\n");
    printf("Found %zu access intervals:\n", result.size());

    double totalDuration = 0.0;
    for (size_t i = 0; i < result.size(); ++i)
    {
        TimeInterval ti = result[i];
        printf("  [%zu] %s  (duration = %.4f s)\n",
               i, ti.toString().c_str(), ti.duration());
        totalDuration += ti.duration();
    }
    printf("Total access time: %.4f s (%.2f min)\n\n",
           totalDuration, totalDuration / 60.0);

    // 9. 与基准结果对比（容差：时间 0.001s，时长 0.01s）
    struct BaselineInterval {
        int       index;
        TimePoint start;
        TimePoint stop;
        double    duration;
    };
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
    static const double kExpectedTotal = 3539.5400;
    static const double kTimeTol      = 0.001;  // 1ms
    static const double kDurTol       = 0.01;   // 10ms

    ASSERT_EQ(result.size(), 7u) << "Interval count mismatch";

    for (size_t i = 0; i < result.size(); ++i) {
        TimeInterval ti = result[i];
        const auto& bl = kBaseline[i];

        double startDiff = fabs(ti.start() - bl.start);
        double stopDiff  = fabs(ti.stop() - bl.stop);
        double durDiff   = fabs(ti.duration() - bl.duration);

        EXPECT_LT(startDiff, kTimeTol)
            << "Interval " << bl.index << " start mismatch: computed="
            << ti.start().toString() << " baseline=" << bl.start.toString();
        EXPECT_LT(stopDiff, kTimeTol)
            << "Interval " << bl.index << " stop mismatch: computed="
            << ti.stop().toString() << " baseline=" << bl.stop.toString();
        EXPECT_LT(durDiff, kDurTol)
            << "Interval " << bl.index << " duration mismatch: computed="
            << ti.duration() << " baseline=" << bl.duration;
    }

    EXPECT_NEAR(totalDuration, kExpectedTotal, kDurTol)
        << "Total duration mismatch";

    // 清理
    delete fac;
    delete sat;
}

// ==================== 测试用例 3：卫星对卫星 ====================

TEST_F(AccessAnalysisTest, Satellite1ToSatellite2)
{
    // 1. 构建两颗卫星
    Satellite* sat1 = CreateSatellite1();
    Satellite* sat2 = CreateSatellite2();
    ASSERT_NE(sat1, nullptr);
    ASSERT_NE(sat2, nullptr);
    printf("Created satellite: %s\n", sat1->getName().c_str());
    printf("Created satellite: %s\n", sat2->getName().c_str());

    // 2. 生成星历
    errc_t rc = sat1->generateEphemeris();
    ASSERT_EQ(rc, eNoError);
    rc = sat2->generateEphemeris();
    ASSERT_EQ(rc, eNoError);
    printf("Ephemeris generated successfully.\n");

    // 3. 获取地球天体
    CelestialBody* earth = aGetEarth();
    ASSERT_NE(earth, nullptr);

    // 4. 定义分析时间区间
    TimePoint start = TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0);
    TimePoint stop  = TimePoint::FromUTC(2026, 7, 23, 4, 0, 0.0);
    TimeInterval analysisInterval(start, stop);

    // 5. 创建约束、步进器、评估器
    BodyObstructionConstraint constraint(sat1, sat2, earth);
    FixedStepStepper stepper(60.0);
    AccessEvaluator evaluator;
    evaluator.setConstraint(&constraint);
    evaluator.setStepper(&stepper);

    // 6. 计算访问时段
    TimeIntervalList result;
    rc = evaluator.evaluate(analysisInterval, result);
    ASSERT_EQ(rc, eNoError);

    // 7. 打印结果
    printf("\n=== Access Intervals: Satellite1 -> Satellite2 ===\n");
    printf("Found %zu access intervals:\n", result.size());

    double totalDuration = 0.0;
    for (size_t i = 0; i < result.size(); ++i)
    {
        TimeInterval ti = result[i];
        printf("  [%zu] %s  (duration = %.4f s)\n",
               i, ti.toString().c_str(), ti.duration());
        totalDuration += ti.duration();
    }
    printf("Total access time: %.4f s (%.2f min)\n\n",
           totalDuration, totalDuration / 60.0);

    // 8. 与基准结果对比（容差：时间 0.001s，时长 0.01s）
    struct BaselineInterval {
        int       index;
        TimePoint start;
        TimePoint stop;
        double    duration;
    };
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
    static const double kExpectedTotal = 32116.7356;
    static const double kTimeTol      = 0.001;  // 1ms
    static const double kDurTol       = 0.01;   // 10ms

    ASSERT_EQ(result.size(), 8u) << "Interval count mismatch";

    for (size_t i = 0; i < result.size(); ++i) {
        TimeInterval ti = result[i];
        const auto& bl = kBaseline[i];

        double startDiff = fabs(ti.start() - bl.start);
        double stopDiff  = fabs(ti.stop() - bl.stop);
        double durDiff   = fabs(ti.duration() - bl.duration);

        EXPECT_LT(startDiff, kTimeTol)
            << "Interval " << bl.index << " start mismatch: computed="
            << ti.start().toString() << " baseline=" << bl.start.toString();
        EXPECT_LT(stopDiff, kTimeTol)
            << "Interval " << bl.index << " stop mismatch: computed="
            << ti.stop().toString() << " baseline=" << bl.stop.toString();
        EXPECT_LT(durDiff, kDurTol)
            << "Interval " << bl.index << " duration mismatch: computed="
            << ti.duration() << " baseline=" << bl.duration;
    }

    EXPECT_NEAR(totalDuration, kExpectedTotal, kDurTol)
        << "Total duration mismatch";

    // 清理
    delete sat1;
    delete sat2;
}

// ==================== 测试用例 4：传感器对地面站 ====================

TEST_F(AccessAnalysisTest, Sensor2ToFacility1)
{
    // 1. 构建 Satellite2 和 Facility1
    Satellite* sat2 = CreateSatellite2();
    Facility* fac1 = CreateFacilityFromParams("Facility1", 4.0038609999999998e+01, -7.5596599999999995e+01, 0.0);
    ASSERT_NE(sat2, nullptr);
    ASSERT_NE(fac1, nullptr);

    // 2. 生成星历
    errc_t rc = sat2->generateEphemeris();
    ASSERT_EQ(rc, eNoError);

    // 3. 获取地球天体
    CelestialBody* earth = aGetEarth();
    ASSERT_NE(earth, nullptr);

    // 4. 创建传感器坐标系（以 Satellite2 为原点，ICRF 轴系）
    auto sensorFrame = FrameAssembly::New(sat2, aAxesICRF());

    // 5. 创建视场：简单圆锥，半锥角 45°
    FOVSimpleCone fov;
    fov.setConeAngle(deg2rad(45.0));

    // 6. 定义分析时间区间
    TimePoint start = TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0);
    TimePoint stop  = TimePoint::FromUTC(2026, 7, 23, 4, 0, 0.0);
    TimeInterval analysisInterval(start, stop);

    FixedStepStepper stepper(60.0);

    // 7. FOV 约束
    FieldOfViewConstraint fovConstraint(sensorFrame, fac1, &fov);
    AccessEvaluator fovEval;
    fovEval.setConstraint(&fovConstraint);
    fovEval.setStepper(&stepper);
    TimeIntervalList fovResult;
    rc = fovEval.evaluate(analysisInterval, fovResult);
    ASSERT_EQ(rc, eNoError);

    // 8. 视线遮挡约束
    BodyObstructionConstraint losConstraint(sat2, fac1, earth);
    AccessEvaluator losEval;
    losEval.setConstraint(&losConstraint);
    losEval.setStepper(&stepper);
    TimeIntervalList losResult;
    rc = losEval.evaluate(analysisInterval, losResult);
    ASSERT_EQ(rc, eNoError);

    // 9. 求交集（同时满足 FOV 和 LOS）
    TimeIntervalList result = fovResult.intersect(losResult);

    // 10. 打印结果
    printf("\n=== Access Intervals: Sensor2 -> Facility1 (FOV & LOS) ===\n");
    printf("FOV intervals: %zu, LOS intervals: %zu, combined: %zu\n",
           fovResult.size(), losResult.size(), result.size());

    double totalDuration = 0.0;
    for (size_t i = 0; i < result.size(); ++i)
    {
        TimeInterval ti = result[i];
        printf("  [%zu] %s  (duration = %.4f s)\n",
               i, ti.toString().c_str(), ti.duration());
        totalDuration += ti.duration();
    }
    printf("Total access time: %.4f s (%.2f min)\n\n",
           totalDuration, totalDuration / 60.0);

    // 11. 与基准结果对比
    struct BaselineInterval {
        int       index;
        TimePoint start;
        TimePoint stop;
        double    duration;
    };
    static const BaselineInterval kBaseline[] = {
        {1, TimePoint::FromUTC(2026, 7, 22, 12, 11, 58.6366),
            TimePoint::FromUTC(2026, 7, 22, 12, 14, 32.7797), 154.143},
        {2, TimePoint::FromUTC(2026, 7, 23,  0, 47,  5.0274),
            TimePoint::FromUTC(2026, 7, 23,  0, 49, 35.0256), 149.998},
    };
    static const double kExpectedTotal = 304.141;
    static const double kTimeTol      = 0.001;
    static const double kDurTol       = 0.01;

    ASSERT_EQ(result.size(), 2u) << "Interval count mismatch";

    for (size_t i = 0; i < result.size(); ++i) {
        TimeInterval ti = result[i];
        const auto& bl = kBaseline[i];

        double startDiff = fabs(ti.start() - bl.start);
        double stopDiff  = fabs(ti.stop() - bl.stop);
        double durDiff   = fabs(ti.duration() - bl.duration);

        EXPECT_LT(startDiff, kTimeTol)
            << "Interval " << bl.index << " start mismatch";
        EXPECT_LT(stopDiff, kTimeTol)
            << "Interval " << bl.index << " stop mismatch";
        EXPECT_LT(durDiff, kDurTol)
            << "Interval " << bl.index << " duration mismatch";
    }

    EXPECT_NEAR(totalDuration, kExpectedTotal, kDurTol)
        << "Total duration mismatch";

    // 清理
    delete fac1;
    delete sat2;
}

// ==================== 测试用例 5：传感器对地面站（AndConstraint 方式）====================

TEST_F(AccessAnalysisTest, Sensor2ToFacility1_AndConstraint)
{
    // 1. 构建 Satellite2 和 Facility1
    Satellite* sat2 = CreateSatellite2();
    Facility* fac1 = CreateFacilityFromParams("Facility1", 4.0038609999999998e+01, -7.5596599999999995e+01, 0.0);
    ASSERT_NE(sat2, nullptr);
    ASSERT_NE(fac1, nullptr);

    // 2. 生成星历
    errc_t rc = sat2->generateEphemeris();
    ASSERT_EQ(rc, eNoError);

    // 3. 获取地球天体
    CelestialBody* earth = aGetEarth();
    ASSERT_NE(earth, nullptr);

    // 4. 创建传感器坐标系
    auto sensorFrame = FrameAssembly::New(sat2, aAxesICRF());

    // 5. FOV 约束（堆分配：AndConstraint 的 SharedPtr 会接管生命周期）
    auto pFov = new FOVSimpleCone();
    pFov->setConeAngle(deg2rad(45.0));
    auto pFovConstraint = new FieldOfViewConstraint(sensorFrame, fac1, pFov);

    // 6. LOS 约束（堆分配）
    auto pLosConstraint = new BodyObstructionConstraint(sat2, fac1, earth);

    // 7. And 组合约束
    AndConstraint combined;
    combined.add(pFovConstraint);
    combined.add(pLosConstraint);

    // 8. 步进器、评估器
    FixedStepStepper stepper(60.0);
    AccessEvaluator evaluator;
    evaluator.setConstraint(&combined);
    evaluator.setStepper(&stepper);

    // 9. 分析时间区间
    TimePoint start = TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0);
    TimePoint stop  = TimePoint::FromUTC(2026, 7, 23, 4, 0, 0.0);
    TimeInterval analysisInterval(start, stop);

    // 10. 计算访问时段
    TimeIntervalList result;
    rc = evaluator.evaluate(analysisInterval, result);
    ASSERT_EQ(rc, eNoError);

    // 11. 打印结果
    printf("\n=== Sensor2 -> Facility1 (AndConstraint) ===\n");
    printf("Found %zu access intervals:\n", result.size());

    double totalDuration = 0.0;
    for (size_t i = 0; i < result.size(); ++i)
    {
        TimeInterval ti = result[i];
        printf("  [%zu] %s  (duration = %.4f s)\n",
               i, ti.toString().c_str(), ti.duration());
        totalDuration += ti.duration();
    }
    printf("Total access time: %.4f s (%.2f min)\n\n",
           totalDuration, totalDuration / 60.0);

    // 12. 与基准结果对比
    struct BaselineInterval {
        int       index;
        TimePoint start;
        TimePoint stop;
        double    duration;
    };
    static const BaselineInterval kBaseline[] = {
        {1, TimePoint::FromUTC(2026, 7, 22, 12, 11, 58.6366),
            TimePoint::FromUTC(2026, 7, 22, 12, 14, 32.7797), 154.143},
        {2, TimePoint::FromUTC(2026, 7, 23,  0, 47,  5.0274),
            TimePoint::FromUTC(2026, 7, 23,  0, 49, 35.0256), 149.998},
    };
    static const double kExpectedTotal = 304.141;
    static const double kTimeTol      = 0.001;
    static const double kDurTol       = 0.01;

    ASSERT_EQ(result.size(), 2u) << "Interval count mismatch";

    for (size_t i = 0; i < result.size(); ++i) {
        TimeInterval ti = result[i];
        const auto& bl = kBaseline[i];

        EXPECT_LT(fabs(ti.start() - bl.start), kTimeTol)
            << "Interval " << bl.index << " start mismatch";
        EXPECT_LT(fabs(ti.stop() - bl.stop), kTimeTol)
            << "Interval " << bl.index << " stop mismatch";
        EXPECT_LT(fabs(ti.duration() - bl.duration), kDurTol)
            << "Interval " << bl.index << " duration mismatch";
    }

    EXPECT_NEAR(totalDuration, kExpectedTotal, kDurTol)
        << "Total duration mismatch";

    // 清理
    delete fac1;
    delete sat2;
}

// ==================== 测试入口 ====================

GTEST_MAIN()
