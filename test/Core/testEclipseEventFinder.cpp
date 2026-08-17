///
/// @file      testEclipseEventFinder.cpp
/// @brief     日食事件求解器测试
/// @details   构建 LEO 圆轨道卫星，用 EclipseEventFinder 求解一天的日食事件，
///            校验事件结构、本影数量/时长及光照比。
/// @author    axel
/// @date      2026-08-15
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

#include "ast/EclipseEventFinder.hpp"
#include "ast/ConeEclipseCalculator.hpp"
#include "ast/Satellite.hpp"
#include "ast/MotionTwoBody.hpp"
#include "ast/StateKeplerian.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/EventIntervalExplicit.hpp"
#include "ast/RunTime.hpp"
#include "ast/RunTimeSolarSystem.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Literals.hpp"
#include "ast/Test.hpp"
#include "ast/MathDegree.hpp"
#include "ast/Vector.hpp"
#include "ast/EOP.hpp"
#include <clocale>
#include <cmath>
#include <cstdio>
#include <vector>

AST_USING_NAMESPACE

using namespace ast::literals;

// ==================== 测试辅助 ====================

static TimeInterval DefaultInterval()
{
    return {
        TimePoint::FromUTC(2026, 8, 13, 4, 0, 0.0),
        TimePoint::FromUTC(2026, 8, 14, 4, 0, 0.0)
    };
}

// ==================== 基准数据 ====================

/// @brief 构造 2026 年的 UTC 时刻
static TimePoint UTC(int mon, int day, int h, int m, double s)
{
    return TimePoint::FromUTC(2026, mon, day, h, m, s);
}

/// @brief对比容差
static const double kTimeTol = 1.5;  // 边界时刻 [s]

/// @brief 构造一个基准日食事件
static EclipseEvent MakeExpected(const TimePoint& penStart, const TimePoint& umbStart,
                                 const TimePoint& umbStop, const TimePoint& penStop)
{
    EclipseEvent ev;
    ev.penumbraStart_    = penStart;
    ev.umbraStart_       = umbStart;
    ev.umbraStop_        = umbStop;
    ev.penumbraStop_     = penStop;
    ev.maxShadowRatio_   = 1.0;
    ev.timeAtMaxShadow_  = umbStart;   // 本影事件最大遮蔽在本影进入时刻首次达到
    ev.hasUmbra_         = true;
    return ev;
}

static std::vector<EclipseEvent> ExpectedEclipses()
{
    std::vector<EclipseEvent> ex;
    ex.reserve(17);
    ex.push_back(MakeExpected(UTC(8,13, 4, 0, 0.000), UTC(8,13, 4, 0, 0.000), UTC(8,13, 4, 8,11.676), UTC(8,13, 4, 8,19.609)));
    ex.push_back(MakeExpected(UTC(8,13, 5, 2,11.278), UTC(8,13, 5, 2,19.249), UTC(8,13, 5,38,43.766), UTC(8,13, 5,38,51.692)));
    ex.push_back(MakeExpected(UTC(8,13, 6,32,43.370), UTC(8,13, 6,32,51.335), UTC(8,13, 7, 9,15.848), UTC(8,13, 7, 9,23.778)));
    ex.push_back(MakeExpected(UTC(8,13, 8, 3,15.455), UTC(8,13, 8, 3,23.425), UTC(8,13, 8,39,47.936), UTC(8,13, 8,39,55.865)));
    ex.push_back(MakeExpected(UTC(8,13, 9,33,47.546), UTC(8,13, 9,33,55.514), UTC(8,13,10,10,20.025), UTC(8,13,10,10,27.951)));
    ex.push_back(MakeExpected(UTC(8,13,11, 4,19.632), UTC(8,13,11, 4,27.603), UTC(8,13,11,40,52.111), UTC(8,13,11,41, 0.038)));
    ex.push_back(MakeExpected(UTC(8,13,12,34,51.721), UTC(8,13,12,34,59.689), UTC(8,13,13,11,24.193), UTC(8,13,13,11,32.127)));
    ex.push_back(MakeExpected(UTC(8,13,14, 5,23.809), UTC(8,13,14, 5,31.778), UTC(8,13,14,41,56.284), UTC(8,13,14,42, 4.209)));
    ex.push_back(MakeExpected(UTC(8,13,15,35,55.898), UTC(8,13,15,36, 3.866), UTC(8,13,16,12,28.370), UTC(8,13,16,12,36.296)));
    ex.push_back(MakeExpected(UTC(8,13,17, 6,27.990), UTC(8,13,17, 6,35.955), UTC(8,13,17,43, 0.457), UTC(8,13,17,43, 8.387)));
    ex.push_back(MakeExpected(UTC(8,13,18,37, 0.078), UTC(8,13,18,37, 8.047), UTC(8,13,19,13,32.539), UTC(8,13,19,13,40.473)));
    ex.push_back(MakeExpected(UTC(8,13,20, 7,32.163), UTC(8,13,20, 7,40.136), UTC(8,13,20,44, 4.625), UTC(8,13,20,44,12.555)));
    ex.push_back(MakeExpected(UTC(8,13,21,38, 4.252), UTC(8,13,21,38,12.221), UTC(8,13,22,14,36.713), UTC(8,13,22,14,44.646)));
    ex.push_back(MakeExpected(UTC(8,13,23, 8,36.343), UTC(8,13,23, 8,44.312), UTC(8,13,23,45, 8.799), UTC(8,13,23,45,16.733)));
    ex.push_back(MakeExpected(UTC(8,14, 0,39, 8.432), UTC(8,14, 0,39,16.398), UTC(8,14, 1,15,40.889), UTC(8,14, 1,15,48.819)));
    ex.push_back(MakeExpected(UTC(8,14, 2, 9,40.518), UTC(8,14, 2, 9,48.489), UTC(8,14, 2,46,12.972), UTC(8,14, 2,46,20.902)));
    ex.push_back(MakeExpected(UTC(8,14, 3,40,12.606), UTC(8,14, 3,40,20.578), UTC(8,14, 4, 0, 0.000), UTC(8,14, 4, 0, 0.000)));
    return ex;
}

static void Verify(const std::vector<EclipseEvent>& events, const std::vector<EclipseEvent>& expected)
{
    ASSERT_EQ(events.size(), expected.size()) << "eclipse event count mismatch";

    for (size_t i = 0; i < events.size(); ++i)
    {
        const auto& ev = events[i];
        const auto& ex = expected[i];

        EXPECT_NEAR(ev.penumbraStart_ - ex.penumbraStart_, 0.0, kTimeTol) << "event " << i << " penStart";
        EXPECT_NEAR(ev.umbraStart_    - ex.umbraStart_,    0.0, kTimeTol) << "event " << i << " umbStart";
        EXPECT_NEAR(ev.umbraStop_     - ex.umbraStop_,     0.0, kTimeTol) << "event " << i << " umbStop";
        EXPECT_NEAR(ev.penumbraStop_  - ex.penumbraStop_,  0.0, kTimeTol) << "event " << i << " penStop";
        EXPECT_NEAR(ev.maxShadowRatio_ - ex.maxShadowRatio_, 0.0, 0.001) << "event " << i << " maxShadow";
        EXPECT_NEAR(ev.timeAtMaxShadow_ - ex.timeAtMaxShadow_, 0.0, kTimeTol) << "event " << i << " maxShadowTime";
    }
}

/// @brief 用双锥模型直接计算某时刻的光照比（作为独立参考）
static double LightingRatioAt(Point* sat, Frame* frame, const TimePoint& t)
{
    ConeEclipseCalculator calc;
    calc.setLightSource(aGetSun());
    calc.addOccultingBody(aGetEarth());
    Vector3d pos;
    if (sat->getPosIn(frame, t, pos) != eNoError)
        return 1.0; // 获取失败，保守视为全光照
    return calc.getLightingRatio(t, pos, frame);
}

// ==================== 测试 Fixture ====================

class EclipseEventFinderTest : public testing::Test
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

// ==================== 测试：LEO 一天日食事件 ====================

TEST_F(EclipseEventFinderTest, LeoOrbitOneDay)
{
    // 构建 LEO 圆轨道卫星（~300 km 高度，i=28.5°）
    auto sat = new Satellite();
    sat->setName("Satellite1");

    auto motion = MotionTwoBody::New();
    auto state  = StateKeplerian::New();

    ModOrbElem elem{};
    elem.rp_     = 6678137.0;
    elem.e_      = 0.0;
    elem.i_      = 28.5_deg;
    elem.raan_   = 0.0;
    elem.argper_ = 0.0;
    elem.trueA_  = 0.0;

    CelestialBody* earth = aGetEarth();
    auto frame = earth->makeFrameICRF();

    state->setFrame(frame.get());
    state->setState(elem);
    state->setStateEpoch(DefaultInterval().start());

    motion->setInitialState(state);
    motion->setPropagationFrame(frame.get());
    motion->setInterval(EventIntervalExplicit::New(DefaultInterval().start(), DefaultInterval().stop()));
    motion->setStepSize(60.0);

    sat->setMotionProfile(motion);
    ASSERT_EQ(sat->generateEphemeris(), eNoError);

    // 求解日食事件（仅地球遮挡）
    EclipseEventFinder finder;
    finder.setPoint(sat);
    finder.setLightSource(aGetSun());
    finder.setOccultingBodies({aGetEarth()});
    finder.setStepSize(5.0);

    std::vector<EclipseEvent> events;
    ASSERT_EQ(finder.find(DefaultInterval(), events), eNoError);

    ASSERT_GT(events.size(), 0u) << "expected at least one eclipse event";

    // 打印求解结果
    std::printf("\n===== Eclipse Events (%zu) =====\n", events.size());
    for (size_t i = 0; i < events.size(); ++i)
    {
        const auto& ev = events[i];
        std::printf("[%2zu] body=%s hasUmbra=%d shadow=%.1f%% "
                    "      pen=%s  umbIn=%s  umbOut=%s  penOut=%s timeAtMaxShadow=%s\n",
                    i,
                    ev.obstruction_ ? ev.obstruction_->getName().c_str() : "(null)",
                    ev.hasUmbra_ ? 1 : 0,
                    ev.maxShadowRatio_ * 100.0,
                    ev.penumbraStart_.toString(3).c_str(),
                    ev.umbraStart_.toString(3).c_str(),
                    ev.umbraStop_.toString(3).c_str(),
                    ev.penumbraStop_.toString(3).c_str(),
                    ev.timeAtMaxShadow_.toString(3).c_str()
                );
    }
    std::printf("==============================\n\n");

    int    umbraCount     = 0;
    int    fullUmbraCount = 0;
    // 与基准逐项对比
    Verify(events, ExpectedEclipses());

    // 结构性校验（边界顺序 + 遮挡体），并记录首个本影事件用于光照比 sanity
    const EclipseEvent* firstUmbra = nullptr;
    for (const auto& ev : events)
    {
        // 边界顺序：半影进入 <= 本影进入 <= 本影退出 <= 半影退出
        EXPECT_LE(ev.penumbraStart_ - ev.umbraStart_, 0.0) << "penumbraStart > umbraStart";
        EXPECT_LE(ev.umbraStart_    - ev.umbraStop_,  0.0) << "umbraStart > umbraStop";
        EXPECT_LE(ev.umbraStop_     - ev.penumbraStop_, 0.0) << "umbraStop > penumbraStop";

        // 遮挡体应为地球
        ASSERT_TRUE(ev.obstruction_);
        EXPECT_EQ(ev.obstruction_.get(), aGetEarth());

        if (ev.hasUmbra_)
        {
            if (!firstUmbra) firstUmbra = &ev;
            ++umbraCount;
            double umbraDur = ev.umbraStop_ - ev.umbraStart_;
            EXPECT_GT(umbraDur, 0.0)      << "umbra duration must be positive";
            EXPECT_LE(umbraDur, 3000.0)   << "umbra duration too long";
            if (umbraDur > 1800.0) ++fullUmbraCount; // 完整本影约 2186 s
            EXPECT_GE(ev.maxShadowRatio_, 0.999) << "umbra event should be ~100% shadow";
        }
    }

    // LEO 一天约 16 次本影，其中绝大多数为完整本影（~2186 s），首尾两个可能被区间裁剪
    EXPECT_GE(umbraCount, 14) << "too few umbra events";
    EXPECT_LE(umbraCount, 18) << "too many umbra events";
    EXPECT_GE(fullUmbraCount, 12) << "expected most umbra events to be full (~2186 s)";
    EXPECT_LE(fullUmbraCount, 16) << "too many full umbra events";

    ASSERT_NE(firstUmbra, nullptr) << "expected at least one umbra event";

    // 本影中点应为全阴影（ratio ~ 0）
    {
        TimePoint mid = firstUmbra->umbraStart_ + (firstUmbra->umbraStop_ - firstUmbra->umbraStart_) * 0.5;
        double r = LightingRatioAt(sat, frame.get(), mid);
        EXPECT_LT(r, 0.01) << "umbra midpoint should be fully shadowed, got " << r;
    }

    // 半影退出后 60 s 应为全光照（ratio ~ 1）
    {
        TimePoint t = firstUmbra->penumbraStop_ + 60.0;
        double r = LightingRatioAt(sat, frame.get(), t);
        EXPECT_GT(r, 0.9) << "just after eclipse should be sunlit, got " << r;
    }

    delete sat;
}

GTEST_MAIN()
