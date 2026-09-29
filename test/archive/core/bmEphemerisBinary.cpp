///
/// @file      bmEphemerisBinary.cpp
/// @brief     对比 EphemerisLagrangeVar 与 EphemerisBinary 的查询性能
/// @details   在不同数据规模下按 4 种访问模式进行 benchmark：
///              - Sequential Stored   : 顺序遍历存储点（精确命中）
///              - Sequential Interp   : 顺序遍历非存储点（Lagrange 插值）
///              - Random Stored       : 随机存储点查询
///              - Random Interp       : 随机非存储点查询
/// @author    axel
/// @date      2026-06-29
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include "ast/EphemerisBinary.hpp"
#include "ast/EphemerisLagrangeVar.hpp"
#include "ast/BuiltinFrame.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/Resolve.hpp"
#include "ast/RunTime.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Vector.hpp"
#include "ast/Test.h"

#include <benchmark/benchmark.h>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>
#include <string>
#include <algorithm>

AST_USING_NAMESPACE

namespace {

// =============================================================================
// 数据规模定义
// =============================================================================

struct ScaleParams {
    size_t      pointCount;     // 星历点总数
    double      step;           // 时间步长 (秒)
    const char* label;          // 规模标签
};

const ScaleParams kScales[] = {
    {  100,   60.0,  "Small"  },   // 100 点  — 全部在窗口缓存内
    { 1000,   60.0,  "Medium" },   // 1000 点 — ~4 个窗口
    {10000,   60.0,  "Large"  },   // 10000 点 — ~40 个窗口
};

constexpr size_t kRandomQueryCount = 1000;

// =============================================================================
// 数据构造
// =============================================================================

/// @brief 创建包含测试数据的 EphemerisLagrangeVar
EphemerisLagrangeVar* createSource(size_t count, double step)
{
    auto ephem = new EphemerisLagrangeVar();
    auto frame = aResolveFrame("EarthICRF");
    assert(frame != nullptr);
    ephem->setFrame(frame);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);
    ephem->setEpoch(epoch);

    std::vector<double> times(count);
    std::vector<Vector3d> positions(count);
    std::vector<Vector3d> velocities(count);

    for (size_t i = 0; i < count; ++i)
    {
        double t = i * step;
        times[i] = t;
        positions[i] = {
            7.0e6 + 7.5e3 * t,
            1.0e3 * std::sin(t * 0.001),
            1.0e3 * std::cos(t * 0.001)
        };
        velocities[i] = {
            7.5e3,
            1.0 * std::cos(t * 0.001),
            -1.0 * std::sin(t * 0.001)
        };
    }
    ephem->setTimes(std::move(times));
    ephem->setPositions(std::move(positions));
    ephem->setVelocities(std::move(velocities));

    return ephem;
}

/// @brief 通过文件往返创建 EphemerisBinary
EphemerisBinary* createBinary(const Ephemeris* source, const std::string& filepath)
{
    auto binary = EphemerisBinary::New();
    errc_t rc = binary->saveFrom(source, filepath);
    if (rc != eNoError) {
        delete binary;
        return nullptr;
    }
    delete binary;

    auto loaded = EphemerisBinary::New();
    rc = loaded->open(filepath);
    if (rc != eNoError) {
        delete loaded;
        return nullptr;
    }
    return loaded;
}

/// @brief 生成一批随机存储点索引
std::vector<size_t> randomStoredIndices(size_t pointCount, size_t queryCount)
{
    std::mt19937 rng(42);
    std::uniform_int_distribution<size_t> dist(0, pointCount - 1);
    std::vector<size_t> indices(queryCount);
    for (size_t i = 0; i < queryCount; ++i)
        indices[i] = dist(rng);
    return indices;
}

/// @brief 生成一批随机非存储点查询时间（相对历元秒），位于区间内但不命中存储点
std::vector<double> randomInterpDeltas(double stopTime, double step, size_t queryCount)
{
    std::mt19937 rng(42);
    // 在每个间隙内随机选取位置，大部分偏中点附近（模拟典型用法）
    std::uniform_real_distribution<double> frac(0.1, 0.9);
    std::uniform_int_distribution<size_t> gapDist(0, static_cast<size_t>(stopTime / step) - 1);
    std::vector<double> deltas(queryCount);
    for (size_t i = 0; i < queryCount; ++i)
    {
        size_t gap = gapDist(rng);
        double tStored = gap * step;
        deltas[i] = tStored + frac(rng) * step;
    }
    return deltas;
}

} // namespace

// =============================================================================
// Benchmark fixture — 每种规模一个 fixture
// =============================================================================

template <size_t ScaleIdx>
class EphemFixture : public benchmark::Fixture
{
public:
    void SetUp(const benchmark::State&) override
    {
        aInitialize();

        const auto& params = kScales[ScaleIdx];

        lagrange_ = createSource(params.pointCount, params.step);
        assert(lagrange_);

        char fname[64];
        std::snprintf(fname, sizeof(fname), "bm_ephem_%zu.bin", params.pointCount);
        filepath_ = fname;

        binary_ = createBinary(lagrange_, filepath_);
        assert(binary_);

        epoch_ = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

        double stopTime = (params.pointCount - 1) * params.step;

        // 预构造顺序存储点查询时间
        storedSeq_.resize(params.pointCount);
        for (size_t i = 0; i < params.pointCount; ++i)
            storedSeq_[i] = epoch_ + i * params.step;

        // 预构造顺序插值查询时间（每个间隙的中点）
        size_t gapCount = params.pointCount - 1;
        interpSeq_.resize(gapCount);
        for (size_t i = 0; i < gapCount; ++i)
            interpSeq_[i] = epoch_ + i * params.step + params.step * 0.5;

        // 预构造随机存储点索引 → 查询时间
        auto storedIndices = randomStoredIndices(params.pointCount, kRandomQueryCount);
        storedRnd_.resize(kRandomQueryCount);
        for (size_t i = 0; i < kRandomQueryCount; ++i)
            storedRnd_[i] = epoch_ + storedIndices[i] * params.step;

        // 预构造随机非存储点查询时间
        auto interpDeltas = randomInterpDeltas(stopTime, params.step, kRandomQueryCount);
        interpRnd_.resize(kRandomQueryCount);
        for (size_t i = 0; i < kRandomQueryCount; ++i)
            interpRnd_[i] = epoch_ + interpDeltas[i];
    }

    void TearDown(const benchmark::State &) override
    {
        delete lagrange_;
        delete binary_;
        aUninitialize();
        std::remove(filepath_.c_str());
    }

protected:
    EphemerisLagrangeVar* lagrange_ = nullptr;
    EphemerisBinary*      binary_ = nullptr;
    TimePoint             epoch_;
    std::string           filepath_;

    // 预构造查询点
    std::vector<TimePoint> storedSeq_;
    std::vector<TimePoint> interpSeq_;
    std::vector<TimePoint> storedRnd_;
    std::vector<TimePoint> interpRnd_;
};

// =============================================================================
// 宏：为每种尺度和模式分别生成 Lagrange 和 Binary 的 benchmark
// =============================================================================

#define BM_LAGRANGE(ScaleIdx, Mode, QueryVec)                              \
    BENCHMARK_TEMPLATE_F(EphemFixture, BM_Lagrange_##Mode, ScaleIdx)       \
    (benchmark::State & state)                                             \
    {                                                                      \
        const auto& tps = this->QueryVec;                                  \
        Vector3d pos, vel;                                                 \
        for (auto _ : state)                                               \
        {                                                                  \
            for (const auto& tp : tps)                                     \
            {                                                              \
                this->lagrange_->getPosVel(tp, pos, vel);                  \
                benchmark::DoNotOptimize(pos);                             \
            }                                                              \
        }                                                                  \
        state.SetItemsProcessed(state.iterations() * tps.size());          \
    }

#define BM_BINARY(ScaleIdx, Mode, QueryVec)                                \
    BENCHMARK_TEMPLATE_F(EphemFixture, BM_Binary_##Mode, ScaleIdx)         \
    (benchmark::State & state)                                             \
    {                                                                      \
        const auto& tps = this->QueryVec;                                  \
        Vector3d pos, vel;                                                 \
        for (auto _ : state)                                               \
        {                                                                  \
            for (const auto& tp : tps)                                     \
            {                                                              \
                this->binary_->getPosVel(tp, pos, vel);                    \
                benchmark::DoNotOptimize(pos);                             \
            }                                                              \
        }                                                                  \
        state.SetItemsProcessed(state.iterations() * tps.size());          \
    }

// =============================================================================
// 实例化: 3 规模 × 4 模式 × 2 星历类型 = 24 个 benchmark
// =============================================================================

// --- Small (100 points) ---
BM_LAGRANGE(0, Small_SeqStored,  storedSeq_)
BM_BINARY  (0, Small_SeqStored,  storedSeq_)
BM_LAGRANGE(0, Small_SeqInterp,  interpSeq_)
BM_BINARY  (0, Small_SeqInterp,  interpSeq_)
BM_LAGRANGE(0, Small_RndStored,  storedRnd_)
BM_BINARY  (0, Small_RndStored,  storedRnd_)
BM_LAGRANGE(0, Small_RndInterp,  interpRnd_)
BM_BINARY  (0, Small_RndInterp,  interpRnd_)

// --- Medium (1000 points) ---
BM_LAGRANGE(1, Medium_SeqStored, storedSeq_)
BM_BINARY  (1, Medium_SeqStored, storedSeq_)
BM_LAGRANGE(1, Medium_SeqInterp, interpSeq_)
BM_BINARY  (1, Medium_SeqInterp, interpSeq_)
BM_LAGRANGE(1, Medium_RndStored, storedRnd_)
BM_BINARY  (1, Medium_RndStored, storedRnd_)
BM_LAGRANGE(1, Medium_RndInterp, interpRnd_)
BM_BINARY  (1, Medium_RndInterp, interpRnd_)

// --- Large (10000 points) ---
BM_LAGRANGE(2, Large_SeqStored,  storedSeq_)
BM_BINARY  (2, Large_SeqStored,  storedSeq_)
BM_LAGRANGE(2, Large_SeqInterp,  interpSeq_)
BM_BINARY  (2, Large_SeqInterp,  interpSeq_)
BM_LAGRANGE(2, Large_RndStored,  storedRnd_)
BM_BINARY  (2, Large_RndStored,  storedRnd_)
BM_LAGRANGE(2, Large_RndInterp,  interpRnd_)
BM_BINARY  (2, Large_RndInterp,  interpRnd_)

// =============================================================================

BENCHMARK_MAIN();
