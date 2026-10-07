///
/// @file      testEphemerisBinary.cpp
/// @brief     测试 EphemerisBinary 文件后备星历的功能
/// @details   测试 saveFrom/open 往返、位置速度查询、区间查询、边界条件及大数据量场景。
///            测试用例基于接口契约编写，不依赖具体实现细节。
/// @author    axel
/// @date      2026-06-29
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

#include "ast/EphemerisBinary.hpp"
#include "ast/EphemerisLagrangeVar.hpp"
#include "ast/BuiltinFrame.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/Resolve.hpp"
#include "ast/RunTime.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Test.h"
#include <fstream>
#include <cmath>
#include <memory>

AST_USING_NAMESPACE

namespace {

/// @brief 浮点比较容差 — 存储点精确命中
constexpr double kTolExact = 1e-12;

/// @brief 浮点比较容差 — 插值结果
constexpr double kTolInterp = 1e-9;

/// @brief 浮点比较容差 — 时间区间跨度
constexpr double kTolTime = 1.0;

} // namespace

class EphemerisBinaryTest : public testing::Test
{
public:
    void SetUp() override
    {
        aInitialize();
    }
    void TearDown() override
    {
        aUninitialize();
        std::remove(tempFile_.c_str());
    }

    /// @brief 创建带有测试数据的 EphemerisLagrangeVar
    /// @param count 数据点数（允许 0）
    /// @param step  时间步长 (秒)
    static EphemerisLagrangeVar* createSourceData(size_t count, double step = 60.0)
    {
        auto ephem = new EphemerisLagrangeVar();

        // 使用 aResolveFrame 获取坐标系，其名称应能往返序列化后重新解析。
        // 若往返失败则说明 aResolveFrame / Frame 命名存在不一致 bug。
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

    /// @brief 便捷：saveFrom → 返回已 open 的星历（调用者负责 delete）
    EphemerisBinary* roundTrip(const Ephemeris* source)
    {
        auto binary = EphemerisBinary::New();
        errc_t rc = binary->saveFrom(source, tempFile_);
        if (rc != eNoError)
        {
            delete binary;
            return nullptr;
        }
        delete binary;

        auto loaded = EphemerisBinary::New();
        rc = loaded->open(tempFile_);
        if (rc != eNoError)
        {
            delete loaded;
            return nullptr;
        }
        return loaded;
    }

protected:
    std::string tempFile_ = "test_ephem_binary_tmp.bin";
};

// =============================================================================
// 基本功能测试
// =============================================================================

/// @brief saveFrom(nullptr) 应返回空指针错误
TEST_F(EphemerisBinaryTest, DumpFrom_NullSource)
{
    auto binary = EphemerisBinary::New();
    errc_t rc = binary->saveFrom(nullptr, tempFile_);
    EXPECT_EQ(rc, eErrorNullPtr);
    delete binary;
}

/// @brief open 不存在的文件应返回错误
TEST_F(EphemerisBinaryTest, Open_NonExistentFile)
{
    auto binary = EphemerisBinary::New();
    errc_t rc = binary->open("__nonexistent_file__.bin");
    EXPECT_NE(rc, eNoError);
    delete binary;
}

/// @brief open 空文件应返回错误
TEST_F(EphemerisBinaryTest, Open_EmptyFile)
{
    {
        std::ofstream file(tempFile_, std::ios::binary | std::ios::trunc);
        file.close();
    }

    auto binary = EphemerisBinary::New();
    errc_t rc = binary->open(tempFile_);
    EXPECT_NE(rc, eNoError);
    delete binary;
}

/// @brief 未初始化的星历查询应返回错误
TEST_F(EphemerisBinaryTest, Empty_GetInterval)
{
    auto binary = EphemerisBinary::New();
    TimeInterval interval;
    errc_t rc = binary->getInterval(interval);
    EXPECT_NE(rc, eNoError);

    auto tp = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);
    Vector3d pos, vel;
    rc = binary->getPos(tp, pos);
    EXPECT_NE(rc, eNoError);
    rc = binary->getPosVel(tp, pos, vel);
    EXPECT_NE(rc, eNoError);

    delete binary;
}

/// @brief EphemerisBinary::New() 应返回非空指针，初始状态无坐标系
TEST_F(EphemerisBinaryTest, New_ReturnsValidPointer)
{
    auto binary = EphemerisBinary::New();
    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getFrame(), nullptr);
    delete binary;
}

/// @brief 0 数据点源星历：saveFrom 应返回错误（无数据可写）
TEST_F(EphemerisBinaryTest, DumpFrom_ZeroPoints)
{
    EphemerisLagrangeVar* source = createSourceData(0, 60.0);
    ASSERT_NE(source, nullptr);

    auto binary = EphemerisBinary::New();
    errc_t rc = binary->saveFrom(source, tempFile_);
    // 空数据应拒绝保存
    EXPECT_NE(rc, eNoError);

    delete source;
    delete binary;
}

// =============================================================================
// saveFrom / open 往返测试
// =============================================================================

/// @brief saveFrom 后对象应正确恢复坐标系、Frame 信息和时间区间
TEST_F(EphemerisBinaryTest, RoundTrip_FrameAndInterval)
{
    EphemerisLagrangeVar* source = createSourceData(10, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr) << "往返失败：dump → open 应能正确恢复星历";

    // 坐标系应非空
    Frame* frame = loaded->getFrame();
    ASSERT_NE(frame, nullptr);
    // 恢复的 Frame 应具有可解析的名称（与源 Frame 一致）
    EXPECT_FALSE(frame->getName().empty());

    // 时间区间：起始应为历元，跨度应覆盖全部数据点
    TimeInterval interval;
    errc_t rc = loaded->getInterval(interval);
    EXPECT_EQ(rc, eNoError);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);
    EXPECT_NEAR(interval.getStart() - epoch, 0.0, kTolTime);
    // 10 个点，步长 60s，跨度为 0 到 540s
    EXPECT_NEAR(interval.getStop() - epoch, 540.0, kTolTime);

    delete source;
    delete loaded;
}

/// @brief 在存储点上精确查询应返回原始数据（含起点、中间点、终点）
TEST_F(EphemerisBinaryTest, GetPosVel_ExactAtStoredPoints)
{
    EphemerisLagrangeVar* source = createSourceData(10, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    // 起点 (t=0, index=0)
    {
        TimePoint tp = epoch + 0.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(pos[0], 7.0e6, kTolExact);
        EXPECT_NEAR(pos[1], 0.0, kTolExact);
        EXPECT_NEAR(pos[2], 1.0e3, kTolExact);
        EXPECT_NEAR(vel[0], 7.5e3, kTolExact);
    }

    // 中间点 (t=120, index=2)
    {
        TimePoint tp = epoch + 120.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);

        double expectedX  = 7.0e6 + 7.5e3 * 120.0;
        double expectedY  = 1.0e3 * std::sin(120.0 * 0.001);
        double expectedZ  = 1.0e3 * std::cos(120.0 * 0.001);
        double expectedVY = 1.0 * std::cos(120.0 * 0.001);
        double expectedVZ = -1.0 * std::sin(120.0 * 0.001);

        EXPECT_NEAR(pos[0], expectedX,  kTolExact);
        EXPECT_NEAR(pos[1], expectedY,  kTolExact);
        EXPECT_NEAR(pos[2], expectedZ,  kTolExact);
        EXPECT_NEAR(vel[0], 7.5e3,     kTolExact);
        EXPECT_NEAR(vel[1], expectedVY, kTolExact);
        EXPECT_NEAR(vel[2], expectedVZ, kTolExact);
    }

    // 终点 (t=540, index=9) — 精确命中存储点，不需要插值
    {
        TimePoint tp = epoch + 540.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);

        double expectedX = 7.0e6 + 7.5e3 * 540.0;
        EXPECT_NEAR(pos[0], expectedX, kTolExact);
        EXPECT_NEAR(vel[0], 7.5e3,     kTolExact);
    }

    delete source;
    delete loaded;
}

/// @brief getPos (仅位置) 接口应正常工作
TEST_F(EphemerisBinaryTest, GetPos_Only)
{
    EphemerisLagrangeVar* source = createSourceData(10, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);
    TimePoint tp = epoch + 120.0;
    Vector3d pos;
    errc_t rc = loaded->getPos(tp, pos);
    EXPECT_EQ(rc, eNoError);

    double expectedX = 7.0e6 + 7.5e3 * 120.0;
    EXPECT_NEAR(pos[0], expectedX, kTolExact);

    delete source;
    delete loaded;
}

// =============================================================================
// 边界测试
// =============================================================================

/// @brief 区间外（第一个点之前、最后一个点之后）查询应返回越界错误
TEST_F(EphemerisBinaryTest, OutOfRange_BeforeFirstAndAfterLast)
{
    EphemerisLagrangeVar* source = createSourceData(10, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);
    Vector3d pos, vel;

    // 第一个点之前
    {
        TimePoint tp = epoch - 0.001;
        errc_t rc = loaded->getPos(tp, pos);
        EXPECT_EQ(rc, eErrorOutOfRange);
        rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eErrorOutOfRange);
    }

    // 最后一个点之后
    {
        TimePoint tp = epoch + 540.001;
        errc_t rc = loaded->getPos(tp, pos);
        EXPECT_EQ(rc, eErrorOutOfRange);
        rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eErrorOutOfRange);
    }

    delete source;
    delete loaded;
}

/// @brief 仅有一个数据点时，精确命中应返回该点数据，偏离则越界
TEST_F(EphemerisBinaryTest, SinglePoint_ExactMatch)
{
    EphemerisLagrangeVar* source = createSourceData(1, 0.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    // 精确命中唯一点 — 直接返回存储值，无需插值
    {
        TimePoint tp = epoch + 0.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(pos[0], 7.0e6, kTolExact);
        EXPECT_NEAR(vel[0], 7.5e3, kTolExact);
    }

    // 偏离唯一点 — 越界
    {
        TimePoint tp = epoch + 0.001;
        Vector3d pos;
        errc_t rc = loaded->getPos(tp, pos);
        EXPECT_EQ(rc, eErrorOutOfRange);
    }

    delete source;
    delete loaded;
}

/// @brief 两个数据点：起点和终点都应可以精确查询
TEST_F(EphemerisBinaryTest, TwoPoints_BothEndpoints)
{
    EphemerisLagrangeVar* source = createSourceData(2, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    // 起点 (t=0)
    {
        TimePoint tp = epoch + 0.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(pos[0], 7.0e6, kTolExact);
        EXPECT_NEAR(vel[0], 7.5e3, kTolExact);
    }

    // 终点 (t=60) — 精确命中
    {
        TimePoint tp = epoch + 60.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);
        double expectedX = 7.0e6 + 7.5e3 * 60.0;
        EXPECT_NEAR(pos[0], expectedX, kTolExact);
        EXPECT_NEAR(vel[0], 7.5e3,     kTolExact);
    }

    delete source;
    delete loaded;
}

// =============================================================================
// 大数据量场景测试
// =============================================================================

/// @brief 大数据量下前后随机访问正常工作
TEST_F(EphemerisBinaryTest, LargeData_RandomAccess)
{
    const size_t NUM_POINTS = 1000;  // 足够大以超出任何合理的内部缓存
    EphemerisLagrangeVar* source = createSourceData(NUM_POINTS, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    // 前面 (index ~ 10)
    {
        TimePoint tp = epoch + 600.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(pos[0], 7.0e6 + 7.5e3 * 600.0, kTolExact);
    }

    // 中间 (index ~ 250)
    {
        TimePoint tp = epoch + 15000.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(pos[0], 7.0e6 + 7.5e3 * 15000.0, kTolExact);
    }

    // 后面 (index ~ 833)
    {
        TimePoint tp = epoch + 50000.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);
        EXPECT_DOUBLE_EQ(pos[0], 7.0e6 + 7.5e3 * 50000.0);
    }

    // 终点 (最后一个存储点，精确匹配)
    {
        double tLast = (NUM_POINTS - 1) * 60.0;
        TimePoint tp = epoch + tLast;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(pos[0], 7.0e6 + 7.5e3 * tLast, kTolExact);
    }

    // 回到前面 (非连续访问，验证任意方向均可正确查询)
    {
        TimePoint tp = epoch + 300.0;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(pos[0], 7.0e6 + 7.5e3 * 300.0, kTolExact);
    }

    delete source;
    delete loaded;
}

/// @brief 大数据量顺序遍历所有存储点 (模拟轨道传播场景)
TEST_F(EphemerisBinaryTest, LargeData_SequentialScan)
{
    const size_t NUM_POINTS = 500;
    EphemerisLagrangeVar* source = createSourceData(NUM_POINTS, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    for (size_t i = 0; i < NUM_POINTS; ++i)
    {
        double t = i * 60.0;
        TimePoint tp = epoch + t;
        Vector3d pos, vel;
        errc_t rc = loaded->getPosVel(tp, pos, vel);
        ASSERT_EQ(rc, eNoError) << "Failed at index " << i;

        double expectedX = 7.0e6 + 7.5e3 * t;
        EXPECT_NEAR(pos[0], expectedX, kTolExact) << "Position mismatch at index " << i;
        EXPECT_NEAR(vel[0], 7.5e3,     kTolExact) << "Velocity mismatch at index " << i;
    }

    delete source;
    delete loaded;
}

// =============================================================================
// 插值精度测试
// =============================================================================

/// @brief 两点之间的插值：EphemerisBinary 和源 EphemerisLagrangeVar 往返一致
TEST_F(EphemerisBinaryTest, Interpolation_BetweenPoints)
{
    EphemerisLagrangeVar* source = createSourceData(10, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    // 在非存储点 (t=90s, t=30s, t=510s) 处对比两种星历的插值结果
    double offsets[] = {90.0, 30.0, 510.0};
    for (double off : offsets)
    {
        TimePoint tp = epoch + off;
        Vector3d posSrc, velSrc, posBin, velBin;
        errc_t rcSrc = source->getPosVel(tp, posSrc, velSrc);
        errc_t rcBin = loaded->getPosVel(tp, posBin, velBin);
        EXPECT_EQ(rcSrc, eNoError)  << "source getPosVel failed at t=" << off;
        EXPECT_EQ(rcBin, eNoError)  << "binary getPosVel failed at t=" << off;

        // 往返后同一 Lagrange 算法同一数据，结果应 bit-identical
        for (int k = 0; k < 3; ++k)
        {
            EXPECT_DOUBLE_EQ(posSrc[k], posBin[k]) << "pos[" << k << "] mismatch at t=" << off;
            EXPECT_DOUBLE_EQ(velSrc[k], velBin[k]) << "vel[" << k << "] mismatch at t=" << off;
        }
    }

    delete source;
    delete loaded;
}

/// @brief 密集扫描：在整个区间内以 1s 步长遍历，验证 EphemerisBinary 与 EphemerisLagrangeVar 的 5 阶插值完全一致
TEST_F(EphemerisBinaryTest, Interpolation_DenseSweep)
{
    EphemerisLagrangeVar* source = createSourceData(10, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    // 从 0.5s 开始，以 1s 步长扫描整个区间，覆盖所有插值区间
    // 存储点为 t = 0, 60, 120, ..., 540，跳过存储点本身
    for (double t = 0.5; t < 540.0; t += 1.0)
    {
        TimePoint tp = epoch + t;
        Vector3d posSrc, velSrc, posBin, velBin;
        errc_t rcSrc = source->getPosVel(tp, posSrc, velSrc);
        errc_t rcBin = loaded->getPosVel(tp, posBin, velBin);
        ASSERT_EQ(rcSrc, eNoError)  << "source getPosVel failed at t=" << t;
        ASSERT_EQ(rcBin, eNoError)  << "binary getPosVel failed at t=" << t;

        for (int k = 0; k < 3; ++k)
        {
            EXPECT_DOUBLE_EQ(posSrc[k], posBin[k])
                << "pos[" << k << "] mismatch at t=" << t;
            EXPECT_DOUBLE_EQ(velSrc[k], velBin[k])
                << "vel[" << k << "] mismatch at t=" << t;
        }
    }

    delete source;
    delete loaded;
}

/// @brief 边界附近插值：在区间首尾的间隙内密集采样，验证窗口约束下的一致性
TEST_F(EphemerisBinaryTest, Interpolation_NearBoundaries)
{
    EphemerisLagrangeVar* source = createSourceData(10, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    // 首部区间 (0, 60)：以 0.1s 步长采样，插值窗口受左边界约束
    for (double t = 0.1; t < 60.0; t += 0.1)
    {
        TimePoint tp = epoch + t;
        Vector3d posSrc, velSrc, posBin, velBin;
        ASSERT_EQ(source->getPosVel(tp, posSrc, velSrc), eNoError);
        ASSERT_EQ(loaded->getPosVel(tp, posBin, velBin), eNoError);
        for (int k = 0; k < 3; ++k)
        {
            EXPECT_DOUBLE_EQ(posSrc[k], posBin[k])
                << "pos[" << k << "] mismatch near start at t=" << t;
            EXPECT_DOUBLE_EQ(velSrc[k], velBin[k])
                << "vel[" << k << "] mismatch near start at t=" << t;
        }
    }

    // 尾部区间 (480, 540)：以 0.1s 步长采样，插值窗口受右边界约束
    for (double t = 480.1; t < 540.0; t += 0.1)
    {
        TimePoint tp = epoch + t;
        Vector3d posSrc, velSrc, posBin, velBin;
        ASSERT_EQ(source->getPosVel(tp, posSrc, velSrc), eNoError);
        ASSERT_EQ(loaded->getPosVel(tp, posBin, velBin), eNoError);
        for (int k = 0; k < 3; ++k)
        {
            EXPECT_DOUBLE_EQ(posSrc[k], posBin[k])
                << "pos[" << k << "] mismatch near end at t=" << t;
            EXPECT_DOUBLE_EQ(velSrc[k], velBin[k])
                << "vel[" << k << "] mismatch near end at t=" << t;
        }
    }

    delete source;
    delete loaded;
}

/// @brief 最少点数 (6 点 = 5 阶 + 1)：在所有相邻点之间的中点处验证插值一致
TEST_F(EphemerisBinaryTest, Interpolation_MinOrderPoints)
{
    // 恰好 6 个点，是完整 5 阶 Lagrange 插值所需的最少点数
    EphemerisLagrangeVar* source = createSourceData(6, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    // 在每一对相邻存储点之间的中点处进行插值
    // 6 个点产生 5 个间隙，中点分别为 t = 30, 90, 150, 210, 270
    for (int i = 0; i < 5; ++i)
    {
        double t = i * 60.0 + 30.0;
        TimePoint tp = epoch + t;
        Vector3d posSrc, velSrc, posBin, velBin;
        ASSERT_EQ(source->getPosVel(tp, posSrc, velSrc), eNoError);
        ASSERT_EQ(loaded->getPosVel(tp, posBin, velBin), eNoError);

        for (int k = 0; k < 3; ++k)
        {
            EXPECT_DOUBLE_EQ(posSrc[k], posBin[k])
                << "pos[" << k << "] mismatch at midpoint t=" << t;
            EXPECT_DOUBLE_EQ(velSrc[k], velBin[k])
                << "vel[" << k << "] mismatch at midpoint t=" << t;
        }
    }

    delete source;
    delete loaded;
}

/// @brief 大数据量插值：1000 点数据时在多个非存储点处对比插值结果
/// @details 跨越 fillWindow 边界，验证从文件读取的窗口数据与源数据一致，
///           从而确保 5 阶 Lagrange 插值结果与 EphemerisLagrangeVar 完全一致。
TEST_F(EphemerisBinaryTest, Interpolation_LargeData)
{
    const size_t NUM_POINTS = 1000;
    EphemerisLagrangeVar* source = createSourceData(NUM_POINTS, 60.0);
    ASSERT_NE(source, nullptr);

    EphemerisBinary* loaded = roundTrip(source);
    ASSERT_NE(loaded, nullptr);

    auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);

    // 在不同区段选取非存储点进行插值对比，覆盖窗口缓存未命中场景
    // 存储点步长为 60s，取偏移 ~30s 作为每个区间的中点
    double offsets[] = {
        30.0,       // 首部区间中点 (index ~ 0,   窗口 0)
        6030.0,     // 前部区间中点 (index ~ 100, 窗口 0)
        15030.0,    // 中部区间中点 (index ~ 250, 窗口 0)
        30030.0,    // 中后区间中点 (index ~ 500, fillWindow 重新加载)
        50030.0,    // 后部区间中点 (index ~ 833, fillWindow 重新加载)
        59910.0,    // 接近尾部区间中点 (index ~ 998, fillWindow 重新加载)
    };

    for (double off : offsets)
    {
        TimePoint tp = epoch + off;
        Vector3d posSrc, velSrc, posBin, velBin;
        errc_t rcSrc = source->getPosVel(tp, posSrc, velSrc);
        errc_t rcBin = loaded->getPosVel(tp, posBin, velBin);
        EXPECT_EQ(rcSrc, eNoError)  << "source getPosVel failed at t=" << off;
        EXPECT_EQ(rcBin, eNoError)  << "binary getPosVel failed at t=" << off;

        // 同一数据经同一 Lagrange 算法，结果应 bit-identical
        for (int k = 0; k < 3; ++k)
        {
            EXPECT_DOUBLE_EQ(posSrc[k], posBin[k])
                << "pos[" << k << "] mismatch at t=" << off;
            EXPECT_DOUBLE_EQ(velSrc[k], velBin[k])
                << "vel[" << k << "] mismatch at t=" << off;
        }
    }

    delete source;
    delete loaded;
}

/// @brief 多次 saveFrom/open：同一实例重复操作的行为应一致
TEST_F(EphemerisBinaryTest, RepeatedOpen)
{
    EphemerisLagrangeVar* source = createSourceData(5, 60.0);
    ASSERT_NE(source, nullptr);

    // 第一次 saveFrom
    auto binary = EphemerisBinary::New();
    errc_t rc = binary->saveFrom(source, tempFile_);
    ASSERT_EQ(rc, eNoError);
    delete binary;

    // 第一次 open 并查询
    {
        auto loaded = EphemerisBinary::New();
        rc = loaded->open(tempFile_);
        ASSERT_EQ(rc, eNoError);

        auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);
        TimePoint tp = epoch + 120.0;
        Vector3d pos;
        rc = loaded->getPos(tp, pos);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(pos[0], 7.0e6 + 7.5e3 * 120.0, kTolExact);

        delete loaded;
    }

    // 第二次 open 同一个文件 — 应同样成功
    {
        auto loaded = EphemerisBinary::New();
        rc = loaded->open(tempFile_);
        ASSERT_EQ(rc, eNoError);

        auto epoch = TimePoint::FromUTC(2026, 6, 29, 0, 0, 0);
        TimePoint tp = epoch + 120.0;
        Vector3d pos;
        rc = loaded->getPos(tp, pos);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(pos[0], 7.0e6 + 7.5e3 * 120.0, kTolExact);

        delete loaded;
    }

    delete source;
}

GTEST_MAIN()
