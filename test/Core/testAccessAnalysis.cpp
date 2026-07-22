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

/// @brief 根据配置文件参数构建 Satellite 对象
/// @param name 卫星名称
/// @return 配置完成的 Satellite 指针
static Satellite* CreateSatelliteFromParams(const std::string& name)
{
    auto sat = new Satellite();
    sat->setName(name);

    // 创建二体运动模型
    auto motion = MotionTwoBody::New();

    // 创建开普勒轨道状态
    auto state = StateKeplerian::New();

    // 设置轨道根数（与 Satellite1.sa 文件一致）
    // RadiusOfPerigee = 6678137.0 m
    // Eccentricity     = 0.0
    // Inclination      = 28.5 deg
    // RightAscension   = 0.0 deg
    // ArgOfPerigee     = 0.0 deg
    // TrueAnomaly      = 0.0 deg
    ModOrbElem elem{};
    elem.rp_     = 6678137.0;               // 近地点半径 (m)
    elem.e_      = 0.0;                     // 偏心率
    elem.i_      = deg2rad(28.5);           // 倾角 (rad)
    elem.raan_   = deg2rad(0.0);            // 升交点赤经 (rad)
    elem.argper_ = deg2rad(0.0);            // 近地点辐角 (rad)
    elem.trueA_  = deg2rad(0.0);            // 真近点角 (rad)

    // 获取地球天体
    CelestialBody* earth = aGetEarth();

    // 创建 ICRF 坐标系
    auto frame = earth->makeFrameICRF();

    // 设置状态历元：22 Jul 2026 04:00:00 UTC
    TimePoint epoch = TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0);
    state->setFrame(frame.get());
    state->setState(elem);
    state->setStateEpoch(epoch);

    // 配置运动模型
    motion->setInitialState(state);
    motion->setPropagationFrame(frame.get());

    // 设置分析时间区间（与 .sa 文件一致）
    TimePoint startTime = TimePoint::FromUTC(2026, 7, 22, 4, 0, 0.0);
    TimePoint stopTime  = TimePoint::FromUTC(2026, 7, 23, 4, 0, 0.0);
    auto interval = EventIntervalExplicit::New(startTime, stopTime);
    motion->setInterval(interval);

    // 设置步长 60 秒（与 .sa 文件 TimeStep 一致）
    motion->setStepSize(60.0);

    // 将运动模型附加到卫星
    sat->setMotionProfile(motion);

    return sat;
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
    Satellite* sat = CreateSatelliteFromParams("Satellite1");
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
    Satellite* sat = CreateSatelliteFromParams("Satellite1");
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

    // ---- 调试：精细采样过境时间附近 ----
    {
        // 第一个过境时间附近（基准: 04:01:47 附近）
        TimePoint t0 = TimePoint::FromUTC(2026, 7, 22, 4, 1, 0.0);
        for (int s = 0; s <= 120; ++s) {
            TimePoint t = t0 + s * 1.0;
            double v = constraint.evaluate(t);
            if (v >= -10.0) { // 只打印接近零的值
                printf("DEBUG t=04:01:%02d  margin=%.15f  satisfied=%d\n",
                       s, v, v >= 0.0 ? 1 : 0);
            }
        }
        // 第二过境附近（基准: 05:36:13）
        t0 = TimePoint::FromUTC(2026, 7, 22, 5, 36, 0.0);
        for (int s = 0; s <= 30; ++s) {
            TimePoint t = t0 + s * 1.0;
            double v = constraint.evaluate(t);
            if (v >= -10.0) {
                printf("DEBUG t=05:36:%02d  margin=%.15f  satisfied=%d\n",
                       s, v, v >= 0.0 ? 1 : 0);
            }
        }
    }
    // ---- 调试结束 ----

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

// ==================== 测试入口 ====================

GTEST_MAIN()
