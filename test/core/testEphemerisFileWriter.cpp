///
/// @file      testEphemerisFileWriter.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-05
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


#include "ast/STKEphemerisFileParser.hpp"
#include "ast/STKEphemerisFileWriter.hpp"
#include "ast/EphemerisLagrangeVar.hpp"
#include "ast/RunTime.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/TimePoint.hpp"
#include "ast/SGP4.hpp"
#include "ast/TLE.hpp"
#include "ast/TLELines.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Test.h"

#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


AST_USING_NAMESPACE

class EphemerisFileWriterTest : public testing::Test
{
public:
    void SetUp() override
    {
        aInitialize();
    }
    void TearDown() override
    {
        aUninitialize();
    }
};


TEST_F(EphemerisFileWriterTest, STKEphemerisFileWriterRoundTrip)
{
    STKEphemerisFileParser parser;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_EPHEMERIS_FILES");
    for (const auto& file : files)
    {
        HEphemeris eph;
        errc_t rc = parser.parse(file, eph);
        ASSERT_FALSE(rc);
        ASSERT_TRUE(eph != nullptr);
        EphemerisLagrangeVar* ephLag = dynamic_cast<EphemerisLagrangeVar*>(eph.get());
        ASSERT_TRUE(ephLag != nullptr);
        ASSERT_GT(ephLag->size(), 0u);

        // 写出到临时文件（与源 fixture 相邻，便于定位）
        std::string outFile = file + ".roundtrip.e";
        STKEphemerisFileWriter writer;
        rc = writer.write(outFile, *eph);
        ASSERT_FALSE(rc);

        // 读回并比对
        HEphemeris eph2;
        rc = parser.parse(outFile, eph2);
        ASSERT_FALSE(rc);
        EphemerisLagrangeVar* ephLag2 = dynamic_cast<EphemerisLagrangeVar*>(eph2.get());
        ASSERT_TRUE(ephLag2 != nullptr);
        EXPECT_EQ(ephLag->size(), ephLag2->size());

        // 以原星历的参考系为公共坐标系，避免写入时帧映射（如 Inertial/J2000）引发的微小偏差
        Frame* originalFrame = ephLag->getFrame();
        TimePoint epoch = ephLag->getEpoch();
        const std::vector<double>& times = ephLag->getTimes();

        size_t n = times.size();
        std::vector<size_t> idxs = {0, n / 2, n - 1};
        for (size_t i : idxs)
        {
            TimePoint tp = epoch + times[i];
            Vector3d p1, v1, p2, v2;
            ASSERT_FALSE(ephLag->getPosVel(tp, p1, v1));
            ASSERT_FALSE(ephLag2->getPosVelIn(originalFrame, tp, p2, v2));
            // 位置(米)、速度(米/秒)，用相对容差
            EXPECT_NEAR(p1.x(), p2.x(), 1e-6 * (1.0 + std::abs(p1.x()))) << file << " idx: " << i;
            EXPECT_NEAR(p1.y(), p2.y(), 1e-6 * (1.0 + std::abs(p1.y()))) << file << " idx: " << i;
            EXPECT_NEAR(p1.z(), p2.z(), 1e-6 * (1.0 + std::abs(p1.z()))) << file << " idx: " << i;
            EXPECT_NEAR(v1.x(), v2.x(), 1e-6 * (1.0 + std::abs(v1.x()))) << file << " idx: " << i;
            EXPECT_NEAR(v1.y(), v2.y(), 1e-6 * (1.0 + std::abs(v1.y()))) << file << " idx: " << i;
            EXPECT_NEAR(v1.z(), v2.z(), 1e-6 * (1.0 + std::abs(v1.z()))) << file << " idx: " << i;
        }

        printf("roundtrip ok: %s\n", outFile.c_str());
    }
}


namespace {

// 构造三维向量
Vector3d makeVec(double x, double y, double z)
{
    Vector3d v;
    v.x() = x;
    v.y() = y;
    v.z() = z;
    return v;
}

// 读取输出文件中 CoordinateSystem 字段的值
std::string readCoordinateSystem(const std::string& path)
{
    std::ifstream in(path);
    std::string key, val;
    while (in >> key)
    {
        if (key == "CoordinateSystem")
        {
            in >> val;
            return val;
        }
    }
    return "";
}

// 以重采样后星历自身的确定时间点为基准，比对 reparse 与原始星历在同一绝对时间的状态
void expectSameTrajectory(const Ephemeris &original, const EphemerisLagrangeVar *reparse, const char *what)
{
    Frame* rf = reparse->getFrame();
    TimePoint epoch = reparse->getEpoch();
    const std::vector<double>& ts = reparse->getTimes();
    for (size_t i = 0; i < ts.size(); ++i)
    {
        TimePoint tp = epoch + ts[i];
        Vector3d p1, v1, p2, v2;
        ASSERT_FALSE(original.getPosVelIn(rf, tp, p1, v1));
        ASSERT_FALSE(reparse->getPosVelIn(rf, tp, p2, v2));
        // 位置(米)、速度(米/秒)，相对容差
        EXPECT_NEAR(p1.x(), p2.x(), 1e-6 * (1.0 + std::abs(p1.x()))) << what << " idx:" << i;
        EXPECT_NEAR(p1.y(), p2.y(), 1e-6 * (1.0 + std::abs(p1.y()))) << what << " idx:" << i;
        EXPECT_NEAR(p1.z(), p2.z(), 1e-6 * (1.0 + std::abs(p1.z()))) << what << " idx:" << i;
        EXPECT_NEAR(v1.x(), v2.x(), 1e-6 * (1.0 + std::abs(v1.x()))) << what << " idx:" << i;
        EXPECT_NEAR(v1.y(), v2.y(), 1e-6 * (1.0 + std::abs(v1.y()))) << what << " idx:" << i;
        EXPECT_NEAR(v1.z(), v2.z(), 1e-6 * (1.0 + std::abs(v1.z()))) << what << " idx:" << i;
    }
}

} // namespace


// 手工构造 J2000 星历
TEST_F(EphemerisFileWriterTest, SelfConstructedJ2000)
{
    EphemerisLagrangeVar lag;
    lag.setEpoch(TimePoint::FromUTC(2024, 1, 1, 0, 0, 0.0));
    lag.setFrame(aGetEarth()->makeFrameJ2000());
    lag.setTimes({0.0, 100.0, 200.0});
    lag.setPositions({makeVec(7.0e6, 0, 0), makeVec(7.05e6, 0, 0), makeVec(7.1e6, 0, 0)});
    lag.setVelocities({makeVec(0, 7.5e3, 0), makeVec(0, 7.5e3, 0), makeVec(0, 7.5e3, 0)});
    lag.setInterpolateOrder(1);

    std::string outFile = "testWriter_self.e";
    STKEphemerisFileWriter writer;
    ASSERT_FALSE(writer.write(outFile, lag));

    // 默认坐标系统应推断为 J2000
    EXPECT_EQ(readCoordinateSystem(outFile), std::string("J2000"));

    STKEphemerisFileParser parser;
    HEphemeris r;
    ASSERT_FALSE(parser.parse(outFile, r));
    EphemerisLagrangeVar* rl = dynamic_cast<EphemerisLagrangeVar*>(r.get());
    ASSERT_TRUE(rl != nullptr);
    EXPECT_EQ(rl->size(), lag.size());

    expectSameTrajectory(lag, rl, "self");
}


// 显式指定 ICRF 坐标系统：原生 J2000 星历跨参考系输出
TEST_F(EphemerisFileWriterTest, ExplicitICRFCoordinate)
{
    EphemerisLagrangeVar lag;
    lag.setEpoch(TimePoint::FromUTC(2024, 1, 1, 0, 0, 0.0));
    lag.setFrame(aGetEarth()->makeFrameJ2000());
    lag.setTimes({0.0, 60.0, 120.0, 180.0});
    lag.setPositions({makeVec(7.0e6, 0, 0), makeVec(7.02e6, 0, 0), makeVec(7.04e6, 0, 0), makeVec(7.06e6, 0, 0)});
    lag.setVelocities({makeVec(0, 7.5e3, 0), makeVec(0, 7.5e3, 0), makeVec(0, 7.5e3, 0), makeVec(0, 7.5e3, 0)});
    lag.setInterpolateOrder(1);

    std::string outFile = "testWriter_icrf.e";
    STKEphemerisFileWriter writer;
    writer.setCoordinate(STKEphemerisFileWriter::eICRF);
    ASSERT_FALSE(writer.write(outFile, lag));

    EXPECT_EQ(readCoordinateSystem(outFile), std::string("ICRF"));

    STKEphemerisFileParser parser;
    HEphemeris r;
    ASSERT_FALSE(parser.parse(outFile, r));
    EphemerisLagrangeVar* rl = dynamic_cast<EphemerisLagrangeVar*>(r.get());
    ASSERT_TRUE(rl != nullptr);
    expectSameTrajectory(lag, rl, "icrf");
}


// 按步长重采样
TEST_F(EphemerisFileWriterTest, Resample)
{
    EphemerisLagrangeVar lag;
    lag.setEpoch(TimePoint::FromUTC(2024, 1, 1, 0, 0, 0.0));
    lag.setFrame(aGetEarth()->makeFrameJ2000());

    std::vector<double> times;
    std::vector<Vector3d> poss, vels;
    for (int i = 0; i < 5; ++i)
    {
        double t = i * 600.0;                     // 原始离散点间距 600s
        times.push_back(t);
        poss.push_back(makeVec(7.0e6 + i * 1e3, 0, 0));
        vels.push_back(makeVec(0, 7.5e3, 0));
    }
    lag.setTimes(times);
    lag.setPositions(poss);
    lag.setVelocities(vels);
    lag.setInterpolateOrder(1);

    std::string outFile = "testWriter_step.e";
    const double stepSec = 60;

    STKEphemerisFileWriter writer;
    writer.setStep(stepSec);
    ASSERT_FALSE(writer.write(outFile, lag));

    STKEphemerisFileParser parser;
    HEphemeris r;
    ASSERT_FALSE(parser.parse(outFile, r));
    EphemerisLagrangeVar* rl = dynamic_cast<EphemerisLagrangeVar*>(r.get());
    ASSERT_TRUE(rl != nullptr);
    ASSERT_GT(rl->size(), 1u);
    EXPECT_NEAR(rl->getTimes()[1] - rl->getTimes()[0], stepSec, 1e-6 * stepSec);

    expectSameTrajectory(lag, rl, "step");
}


// 生成 SGP4 星历并保存：SGP4 本身即是一条 Ephemeris(=Point)，可直接交给写入器
TEST_F(EphemerisFileWriterTest, SGP4EphemerisRoundTrip)
{
    TLELines lines;
    lines.line1() = "1 26661U 99057LT  21084.99115140  .00000893  00000-0  18006-3 0  9994";
    lines.line2() = "2 26661  98.3488 112.3622 0070611 169.6757 257.7924 14.62494120100130";
    TLE tle = TLE::FromLines(lines);
    SGP4 sgp4(tle);
    TimePoint epoch = tle.epochTime();

    // SGP4 的时段为无穷大，须给它一个有限的写入窗口，否则写入器会拒绝
    std::string outFile = "testWriter_sgp4.e";
    STKEphemerisFileWriter writer;
    writer.setInterval(TimeInterval(epoch, 0.0, 3600.0));   // [epoch, epoch+3600s]
    ASSERT_FALSE(writer.write(outFile, sgp4));

    EXPECT_EQ(readCoordinateSystem(outFile), std::string("Inertial"));

    STKEphemerisFileParser parser;
    HEphemeris r;
    ASSERT_FALSE(parser.parse(outFile, r));
    EphemerisLagrangeVar* rl = dynamic_cast<EphemerisLagrangeVar*>(r.get());
    ASSERT_TRUE(rl != nullptr);
    ASSERT_GT(rl->size(), 1u);

    // 比对 SGP4 与写回的结果
    expectSameTrajectory(sgp4, rl, "sgp4");
}


GTEST_MAIN();
