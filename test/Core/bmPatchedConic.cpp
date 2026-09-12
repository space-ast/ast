///
/// @file      bmPatchedConic.cpp
/// @brief     HPOP 与地月双二体(圆锥曲线拼接)预报的效率对比
/// @details   同一个地月转移算例下，比较三种预报方法的单次耗时：
///              - aPatchedConicPropEarthToMoon  解析拼接预报(两段二体 + 月球星历查表)
///              - HPOP                         数值积分，力模型取地球点质量 + 月球三体，
///                                             是拼接模型"两段二体"的数值对应物，用于隔离
///                                             "解析预报 vs 数值积分"本身的代价
///              - HPOP                         数值积分，力模型取 WGS84 8x8 + 日月三体，
///                                             即工程上实际使用的精度档位
///            三者推进到同一终端时刻(由拼接预报给出的近月点时刻确定)，故耗时差异只来自
///            预报方法本身，而不来自被积时段长度的不同。
/// @note      终端状态差异不作断言，只在末尾打印；拼接模型本身是近似，两者不应严格相等，
///            打印出来只为确认两种方法确实在解同一个算例。
/// @author    axel
/// @date      2026-09-10
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

#include "ast/PatchedConic.hpp"
#include "ast/HPOP.hpp"
#include "ast/HPOPForceModel.hpp"
#include "ast/RunTime.hpp"
#include "ast/RunTimeSolarSystem.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/Frame.hpp"
#include "ast/FrameTransform.hpp"
#include "ast/KinematicTransform.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/SpacecraftParam.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Constants.h"
#include "ast/Literals.hpp"
#include <benchmark/benchmark.h>
#include <clocale>
#include <cmath>
#include <cstdio>
#include <map>
#include <string>

AST_USING_NAMESPACE

namespace
{

// ---------------------------------------------------------------------------
// 基准算例
// ---------------------------------------------------------------------------

/// @brief 已知的地月转移注入初值（与 examples/core/lunar/lunar_transfer_analysis.cpp 一致）
/// @note  该初值在地心惯性系下给出；地球的惯性轴系与 ICRF 重合，故其分量与地心 ICRF 相同
CartState transferInitState()
{
    return CartState{
        6159968.33397138, 2393641.1690744, 922487.447464321,
        -2872.54217681962, 9257.74498172147, -4877.87739768414};
}

/// @brief 用月球在给定时刻的瞬时密切轨道半长轴计算地月影响球半径 [m]
double earthMoonSoiFromOsculating(const TimePoint& tp)
{
    Vector3d pos, vel;
    aGetMoon()->getPosVelIn(aGetEarth()->getFrameInertial(), tp, pos, vel);
    ModOrbElem ele;
    aCartToModOrbElem(pos, vel, aGetEarth()->getGM(), ele);
    return aSOIRadius(aGetEarth()->getGM(), aGetMoon()->getGM(), ele.getSMA());
}

/// @brief 全体基准共用的算例：同一初值、同一影响球半径、同一终端时刻
struct BenchmarkCase
{
    CartState initState;      ///< 初始状态（地心惯性系）[m, m/s]
    TimePoint initTime;       ///< 出发时刻
    double    soiRadius{0.0}; ///< 月球影响球半径 [m]
    TimePoint targetTime;     ///< 终端时刻，取拼接预报给出的近月点时刻
    double    tof{0.0};       ///< 预报时长 [s]
};

BenchmarkCase g_case;

// ---------------------------------------------------------------------------
// 被测对象与测量结果
// ---------------------------------------------------------------------------

/// @note 用裸指针而非全局对象：HPOP 是 RTTI 对象，其析构(destruct)依赖运行时对象表，
///       在静态析构阶段执行既不可控也不必要，故统一在 main 中显式释放
HPOP* g_hpopTwoBodyMoon = nullptr; ///< 地球点质量 + 月球三体
HPOP* g_hpopFullModel   = nullptr; ///< WGS84 8x8 + 日月三体

/// @brief 截获实测耗时、供末尾汇总的控制台报告器
/// @details 汇总表要用各基准的耗时，而 google benchmark 并不通过 State 把本轮结果暴露出来，
///          故在报告器里截获它自己测得的 real_time。耗时因此只有"框架测量"这一个来源——
///          基准循环里不再另行计时，免得出现两个可能对不上的数。
class CollectingReporter : public benchmark::ConsoleReporter
{
public:
    void ReportRuns(const std::vector<Run>& reports) override
    {
        for (const auto& run : reports)
        {
            const bool isAggregate = !run.aggregate_name.empty();
            // 使用 --benchmark_repetitions 时会给出 mean/median/stddev 等多个聚合行，只取 mean
            if (isAggregate && run.aggregate_name != "mean")
                continue;
            const std::string name = run.benchmark_name();
            // 无重复时只有原始行；有重复时用 mean 覆盖原始行
            if (isAggregate || ns_.count(name) == 0)
                ns_[name] = run.GetAdjustedRealTime();   // 单次耗时 [ns]
        }
        ConsoleReporter::ReportRuns(reports);
    }

    /// @brief 取某基准的单次耗时 [ns]；该基准未运行时返回 NaN
    double ns(const std::string& name) const
    {
        auto it = ns_.find(name);
        return it == ns_.end() ? kNaN : it->second;
    }

private:
    std::map<std::string, double> ns_;
};

// ---------------------------------------------------------------------------
// 力模型
// ---------------------------------------------------------------------------

/// @brief 与双二体模型等价的力模型：地球点质量 + 月球点质量三体
/// @note  拼接模型把动力学拆成"地球二体段"和"月球二体段"两段解析解，其数值对应物正是
///        同一个中心点质量叠加月球三体引力。用该力模型可以把"解析 vs 数值"的代价单独
///        隔离出来，不额外引入引力场高阶项等摄动。
HPOPForceModel twoBodyMoonForceModel()
{
    HPOPForceModel fm;
    fm.setCentralBody(aGetEarth());
    fm.pointMass();
    fm.addThirdBody(aGetMoon());   // 三体引力类型默认为点质量
    fm.useDrag(false);
    fm.useSRP(false);
    return fm;
}

/// @brief 工程常用的高精度力模型：WGS84 8x8 引力场 + 日月三体
HPOPForceModel fullForceModel()
{
    HPOPForceModel fm;
    fm.setCentralBody(aGetEarth());
    fm.gravity().model_     = "WGS84";
    fm.gravity().maxDegree_ = 8;
    fm.gravity().maxOrder_  = 8;
    fm.addThirdBody(aGetMoon());
    fm.addThirdBody(aGetSun());
    fm.useDrag(false);
    fm.useSRP(false);
    return fm;
}

// ---------------------------------------------------------------------------
// 算例与被测对象的准备
// ---------------------------------------------------------------------------

/// @brief 建立算例与被测对象
/// @details 必须在对所有基准计时之前完成：HPOP 的引力场加载、方程构建、积分器分配、
///          以及月球星历的首次解析都是"一次性"开销，若落在计时区间内会淹没真实差异。
/// @return 错误码
errc_t prepare()
{
    g_case.initState = transferInitState();
    g_case.initTime  = "2017-12-31 20:51:50.678"_utc;
    g_case.soiRadius = earthMoonSoiFromOsculating(g_case.initTime);

    // 终端时刻取拼接预报给出的近月点时刻，保证三种方法预报的时长严格相同
    CartState terminalState{};
    errc_t err = aPatchedConicPropEarthToMoon(g_case.soiRadius, g_case.initTime,
                                            g_case.initState, g_case.targetTime, terminalState);
    if (err != eNoError)
        return err;
    g_case.tof = g_case.targetTime - g_case.initTime;

    g_hpopTwoBodyMoon = new HPOP();
    g_hpopTwoBodyMoon->setForceModel(twoBodyMoonForceModel());
    g_hpopTwoBodyMoon->setSpacecraftParam(SpacecraftParam{});

    g_hpopFullModel = new HPOP();
    g_hpopFullModel->setForceModel(fullForceModel());
    g_hpopFullModel->setSpacecraftParam(SpacecraftParam{});

    // 预热：各跑一遍，触发一次性初始化（加载重力场、构建方程、分配积分器）
    {
        TimePoint targetTime = g_case.targetTime;
        CartState state      = g_case.initState;
        err = g_hpopTwoBodyMoon->propagate(g_case.initTime, targetTime, state);
        if (err != eNoError)
            return err;
    }
    {
        TimePoint targetTime = g_case.targetTime;
        CartState state      = g_case.initState;
        err = g_hpopFullModel->propagate(g_case.initTime, targetTime, state);
        if (err != eNoError)
            return err;
    }
    return eNoError;
}

/// @brief 释放被测对象
void cleanup()
{
    delete g_hpopTwoBodyMoon;
    g_hpopTwoBodyMoon = nullptr;
    delete g_hpopFullModel;
    g_hpopFullModel = nullptr;
}

// ---------------------------------------------------------------------------
// 基准
// ---------------------------------------------------------------------------

/// @brief 解析拼接预报：地月转移从注入到近月点
void bmPatchedConic(benchmark::State& state)
{
    const BenchmarkCase& c = g_case;
    double sink = 0.0;
    for (auto _ : state)
    {
        TimePoint outTime{};
        CartState outState{};
        errc_t err = aPatchedConicPropEarthToMoon(c.soiRadius, c.initTime, c.initState, outTime, outState);

        // 把输出量累加进 sink，避免整段计算被优化掉
        sink += err;
    }
    benchmark::DoNotOptimize(sink);
}

/// @brief HPOP 数值积分：地球点质量 + 月球三体（与拼接模型同物理）
void bmHpopTwoBodyMoon(benchmark::State& state)
{
    const BenchmarkCase& c = g_case;
    double sink = 0.0;
    for (auto _ : state)
    {
        // propagate 的终端时刻与状态是 in-out 参数，每轮必须复位
        TimePoint targetTime = c.targetTime;
        CartState sc         = c.initState;
        errc_t err = g_hpopTwoBodyMoon->propagate(c.initTime, targetTime, sc);

        sink += err;
    }
    benchmark::DoNotOptimize(sink);
}

/// @brief HPOP 数值积分：WGS84 8x8 + 日月三体（工程常用精度）
void bmHpopFullModel(benchmark::State& state)
{
    const BenchmarkCase& c = g_case;
    double sink = 0.0;
    for (auto _ : state)
    {
        TimePoint targetTime = c.targetTime;
        CartState sc         = c.initState;
        errc_t err = g_hpopFullModel->propagate(c.initTime, targetTime, sc);

        sink += err;
    }
    benchmark::DoNotOptimize(sink);
}

/// @brief 参照基准：月球星历的单次查询耗时
/// @details 拼接预报的扫描段要反复查询月球位置，HPOP 的右端函数同样每步都要查；
///          该耗时用于解释二者的比值从何而来。
void bmMoonEphemeris(benchmark::State& state)
{
    const BenchmarkCase& c = g_case;
    Frame* earthFrame   = aGetEarth()->getFrameInertial();
    CelestialBody* moon = aGetMoon();
    double sink = 0.0;
    for (auto _ : state)
    {
        Vector3d pos, vel;
        errc_t err = moon->getPosVelIn(earthFrame, c.initTime, pos, vel);
        sink += err;
    }
    benchmark::DoNotOptimize(sink);
}

// ---------------------------------------------------------------------------
// 结果汇总
// ---------------------------------------------------------------------------

/// @brief 打印一行对比数据
void printRow(const char* label, const CollectingReporter& reporter,
              const char* benchName, double baseNs)
{
    const double ns    = reporter.ns(benchName);
    const double ratio = (std::isnan(ns) || std::isnan(baseNs) || baseNs <= 0.0)
                       ? kNaN : ns / baseNs;
    printf("  %s：%.1f ns/次（相对拼接 %.2fx）\n", label, ns, ratio);
}

/// @brief 打印耗时对比表
/// @note  中文与 ASCII 混排时按字节数用 printf 对齐必然错位，故不排成列表，逐行内联给出
/// @param reporter 已截获各基准实测耗时的报告器
void printSummary(const CollectingReporter& reporter)
{
    const double baseNs = reporter.ns("bmPatchedConic");

    printf("\n");
    printf("================ HPOP 与 aPatchedConicPropEarthToMoon 效率对比 ================\n");
    printf("算例：地月转移，出发时刻 %s，预报时长 %.4f 天\n",
           g_case.initTime.toString().c_str(), g_case.tof / kDayToSec);
    printf("----------------------------------------------------------------------------\n");
    printRow("aPatchedConicPropEarthToMoon 解析拼接", reporter, "bmPatchedConic",    baseNs);
    printRow("HPOP 点质量+月球三体 数值积分",       reporter, "bmHpopTwoBodyMoon", baseNs);
    printRow("HPOP WGS84 8x8+日月三体 数值积分",    reporter, "bmHpopFullModel",   baseNs);
    printRow("月球星历单次查询（参照）",            reporter, "bmMoonEphemeris",   baseNs);
    printf("----------------------------------------------------------------------------\n");
}

/// @brief 打印两模型终端状态的差异
/// @note  拼接预报输出在月心惯性系，HPOP 输出在地心惯性系，必须先把前者转回地心惯性系
///        再相减，否则是在比较不同坐标系下的向量。此处只打印不断言：拼接模型是近似模型，
///        两者本就不应严格相等。
void printTerminalStateComparison()
{
    const BenchmarkCase& c = g_case;

    TimePoint tPC{};
    CartState scPC{};                       // 月心惯性系
    if (aPatchedConicPropEarthToMoon(c.soiRadius, c.initTime, c.initState, tPC, scPC) != eNoError)
        return;

    TimePoint tHP = c.targetTime;
    CartState scHP = c.initState;           // 地心惯性系
    if (g_hpopTwoBodyMoon->propagate(c.initTime, tHP, scHP) != eNoError)
        return;

    KinematicTransform kt;
    Frame* moonFrame  = aGetMoon()->getFrameInertial();
    Frame* earthFrame = aGetEarth()->getFrameInertial();
    if (aFrameTransform(moonFrame, earthFrame, tPC, kt) != eNoError)
        return;
    CartState scPCInEarth;
    kt.transformPositionVelocity(scPC.pos(), scPC.vel(), scPCInEarth.pos(), scPCInEarth.vel());

    // 两者的终端时刻由构造相同(均为拼接预报给出的近月点时刻)，故可直接在地心惯性系下相减。
    // 先各自给出到月心的距离，确认两者都落在月球附近；再给出同一时刻的位置差。
    // @note 位置差对拼接模型的到达时刻误差极其敏感：近月段相对速度约 2.5 km/s，
    //       终端时刻差 1 小时即对应约 9000 km 的沿迹差。故该差值只作量级参照，不作断言。
    Vector3d moonPosPC, moonVelPC, moonPosHP, moonVelHP;
    if (aGetMoon()->getPosVelIn(earthFrame, tPC, moonPosPC, moonVelPC) != eNoError)
        return;
    if (aGetMoon()->getPosVelIn(earthFrame, tHP, moonPosHP, moonVelHP) != eNoError)
        return;

    printf("\n终端状态（地心惯性系，时刻 %s）：\n", tHP.toString().c_str());
    printf("  拼接预报      r = %s\n", scPCInEarth.pos().toString().c_str());
    printf("                到月心距离 = %10.3f km\n",
           (scPCInEarth.pos() - moonPosPC).norm() / 1000.0);
    printf("  HPOP 点质量   r = %s\n", scHP.pos().toString().c_str());
    printf("                到月心距离 = %10.3f km\n",
           (scHP.pos() - moonPosHP).norm() / 1000.0);
    printf("  同一时刻之差  |dr| = %.3f km   |dv| = %.6f km/s\n",
           (scHP.pos() - scPCInEarth.pos()).norm() / 1000.0,
           (scHP.vel() - scPCInEarth.vel()).norm() / 1000.0);
    printf("  说明：差值主要由拼接模型的到达时刻误差贡献——近月段相对速度约 2.5 km/s，\n"
           "        时刻差 1 小时即对应约 9000 km 的沿迹差。\n");
}

} // namespace

BENCHMARK(bmPatchedConic);
BENCHMARK(bmHpopTwoBodyMoon);
BENCHMARK(bmHpopFullModel);
BENCHMARK(bmMoonEphemeris);

/// @note 不用 BENCHMARK_MAIN()：被测函数依赖 aInitialize() 装载的太阳系数据，
///       且对比表需要在全部基准跑完之后汇总打印，故自定义 main
int main(int argc, char** argv)
{
    setlocale(LC_ALL, ".UTF-8");
    aInitialize();
 
    const errc_t err = prepare();
    if (err != eNoError)
    {
        fprintf(stderr, "基准算例建立失败，err = %d\n", err);
        cleanup();
        aUninitialize();
        return 1;
    }

    benchmark::Initialize(&argc, argv);

    // 报告器既负责正常的控制台输出，也顺便把各基准的耗时收集下来供末尾汇总
    CollectingReporter reporter;
    benchmark::RunSpecifiedBenchmarks(&reporter);
    benchmark::Shutdown();

    printSummary(reporter);
    printTerminalStateComparison();

    cleanup();
    aUninitialize();
    return 0;
}
