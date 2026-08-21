///
/// @file      testSGP4.cpp
/// @brief     SGP4 星历预报器测试
/// @details   使用已知 TLE 验证 SGP4 传播器的基本功能：
///            1. TLE 解析
///            2. SGP4 初始化与位置/速度计算
///            3. 坐标系标识（TEME）
///            4. TEME ↔ J2000 转换
///            5. 多个时间点的预报一致性
/// @author    axel
/// @date      2026-07-20
/// @copyright 版权所有 (C) 2026-present, ast项目.

#include "ast/AstTestMacro.h"
#include "ast/SGP4.hpp"
#include "ast/SGP4.h"
#include "ast/TLE.hpp"
#include "ast/TLELines.hpp"
#include "ast/Vector.hpp"
#include "ast/Matrix.hpp"
#include "ast/Math.hpp"
#include "ast/BuiltinFrame.hpp"
#include "ast/EarthFrame.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Coordinate.hpp"
#include "ast/RunTime.hpp"
#include "ast/EOP.hpp"
#include "ast/BuiltinFrame.hpp"

// =============================================================================
// 测试 TLE（NORAD 26661, 太阳同步轨道卫星）
// 来源: atkAllTLE.tce
// =============================================================================
static const char* kLine1 = "1 26661U 99057LT  21084.99115140  .00000893  00000-0  18006-3 0  9994";
static const char* kLine2 = "2 26661  98.3488 112.3622 0070611 169.6757 257.7924 14.62494120100130";


/// @brief 从两行字符串创建 TLELines（辅助函数）
static ast::TLELines makeTLELines()
{
    ast::TLELines lines;
    lines.line1() = kLine1;
    lines.line2() = kLine2;
    return lines;
}


TEST(SGP4Test, Propagation)
{

    AST_USING_NAMESPACE
    aDataContext_GetEOP()->unload();

    {
        TLE tle = TLE::FromLines(makeTLELines());
        SGP4 sgp4(tle);
        TimePoint tp = TimePoint::FromUTC(2021, 3, 25, 0, 0, 0);
        Vector3d posTEME, velTEME;
        sgp4.getPosVel(tp, posTEME, velTEME);  // in TEME of date
        printf("posTEME  = %s\n", posTEME.toString().c_str());
        printf("velTEME  = %s\n", velTEME.toString().c_str());

        auto icrf = aFrameEarthICRF();
        Vector3d posICRF, velICRF;
        sgp4.getPosVelIn(icrf, tp, posICRF, velICRF);
        printf("posICRF  = %s\n", posICRF.toString().c_str());
        printf("velICRF  = %s\n", velICRF.toString().c_str());

        auto j2000 = aFrameEarthJ2000();
        Vector3d posJ2000, velJ2000;
        sgp4.getPosVelIn(j2000, tp, posJ2000, velJ2000);
        printf("posJ2000 = %s\n", posJ2000.toString().c_str());
        printf("velJ2000 = %s\n", velJ2000.toString().c_str());

        Vector3d posTEMEExpected{-162063.7772662019, -2279555.1101529584, -6681715.2148036920};
        Vector3d velTEMEExpected{-2913.0173871367, 6553.9673083082, -2221.0934965069};
        EXPECT_NEAR(posTEME.x(), posTEMEExpected.x(), 1e-3);
        EXPECT_NEAR(posTEME.y(), posTEMEExpected.y(), 1e-3);
        EXPECT_NEAR(posTEME.z(), posTEMEExpected.z(), 1e-3);
        EXPECT_NEAR(velTEME.x(), velTEMEExpected.x(), 1e-6);
        EXPECT_NEAR(velTEME.y(), velTEMEExpected.y(), 1e-6);
        EXPECT_NEAR(velTEME.z(), velTEMEExpected.z(), 1e-6);

        Vector3d posICRFExpected{-186453.2458519393, -2278827.9192585987, -6681327.1796391010};
        Vector3d velICRFExpected{-2886.3766668105, 6567.6995864257, -2215.3026827306};
        EXPECT_NEAR(posICRF.x(), posICRFExpected.x(), 1e-3);
        EXPECT_NEAR(posICRF.y(), posICRFExpected.y(), 1e-3);
        EXPECT_NEAR(posICRF.z(), posICRFExpected.z(), 1e-3);
        EXPECT_NEAR(velICRF.x(), velICRFExpected.x(), 1e-6);
        EXPECT_NEAR(velICRF.y(), velICRFExpected.y(), 1e-6);
        EXPECT_NEAR(velICRF.z(), velICRFExpected.z(), 1e-6);
    }
}


/// @brief 测试 TLE 解析
TEST(SGP4Test, TLEParse)
{
    AST_USING_NAMESPACE
    {
        TLE tle = TLE::FromLines(makeTLELines());

        // 验证解析后的轨道根数（与 TLE 字段值对比）
        EXPECT_NEAR(tle.inclination(),        98.3488  * kDegToRad, 1e-10);
        EXPECT_NEAR(tle.rightAscenOfNode(),  112.3622  * kDegToRad, 1e-10);
        EXPECT_NEAR(tle.eccentricity(),        0.0070611,           1e-10);
        EXPECT_NEAR(tle.argOfPerigee(),      169.6757  * kDegToRad, 1e-10);
        EXPECT_NEAR(tle.meanAnomaly(),       257.7924  * kDegToRad, 1e-10);
        EXPECT_NEAR(tle.meanMotion(),         14.62494120 * kTwoPI / 86400.0, 1e-12);
        EXPECT_NEAR(tle.bstar(),              0.00018006,         1e-12);
    }
}


/// @brief 测试 Alpha-5 NORAD 目录编号解析
TEST(SGP4Test, NoradIdAlpha5)
{
    AST_USING_NAMESPACE

    // Alpha-5 官方示例
    EXPECT_EQ(aParseNoradId("A0000"), 100000);
    EXPECT_EQ(aParseNoradId("E8493"), 148493);
    EXPECT_EQ(aParseNoradId("J2931"), 182931);
    EXPECT_EQ(aParseNoradId("P4018"), 234018);
    EXPECT_EQ(aParseNoradId("T1223"), 271223);
    EXPECT_EQ(aParseNoradId("W1928"), 301928);
    EXPECT_EQ(aParseNoradId("Z9999"), 339999);

    // 传统数字 / 右对齐空格 / 前导零
    EXPECT_EQ(aParseNoradId("25544"), 25544);
    EXPECT_EQ(aParseNoradId(" 1234"), 1234);
    EXPECT_EQ(aParseNoradId("01234"), 1234);

    // 端到端：TLE::FromLines 解析 Alpha-5 行（目录编号 A0000 → 100000）
    static const char* alpha5L1 = "1 A0000U 99057LT  21084.99115140  .00000893  00000-0  18006-3 0  9994";
    static const char* alpha5L2 = "2 A0000  98.3488 112.3622 0070611 169.6757 257.7924 14.62494120100130";
    TLE tle = TLE::FromLines(alpha5L1, alpha5L2);
    EXPECT_EQ(tle.noradId(), 100000);
    EXPECT_NEAR(tle.inclination(), 98.3488 * kDegToRad, 1e-10);
}


/// @brief 测试 SGP4 初始化和基本传播
TEST(SGP4Test, BasicPropagation)
{
    AST_USING_NAMESPACE
    {
        TLE tle = TLE::FromLines(makeTLELines());
        SGP4 sgp4(tle);

        // 诊断：检查 sgp4init 是否成功
        elsetrec& satrec = reinterpret_cast<elsetrec&>(sgp4.elementSet());
        printf("\n=== SGP4 Init Diagnostic ===\n");
        printf("satrec.error = %d\n", satrec.error);
        printf("satrec.method = %c\n", satrec.method);
        printf("satrec.operationmode = %c\n", satrec.operationmode);
        printf("satrec.jdsatepoch = %.10f\n", satrec.jdsatepoch);
        printf("satrec.jdsatepochF = %.10f\n", satrec.jdsatepochF);
        printf("satrec.no_kozai = %.10f\n", satrec.no_kozai);
        printf("satrec.ecco = %.10f\n", satrec.ecco);
        printf("satrec.inclo = %.10f\n", satrec.inclo);
        printf("satrec.nodeo = %.10f\n", satrec.nodeo);
        printf("satrec.argpo = %.10f\n", satrec.argpo);
        printf("satrec.mo = %.10f\n", satrec.mo);
        printf("satrec.bstar = %.15e\n", satrec.bstar);
        printf("\n=== SGP4 Init Diagnostic ===\n");
        printf("sgp4 init error = %d\n", sgp4.getError());

        // 获取 TLE 历元时刻
        TimePoint epoch = tle.epochTime();

        // 打印历元信息
        JulianDate jdUTC;
        aTimePointToUTC(epoch, jdUTC);
        printf("TLE epoch JD = %.10f\n", jdUTC.impreciseDay());
        printf("TLE epoch tsince (from satrec epoch) = %.6f min\n",
               (jdUTC.impreciseDay() - (satrec.jdsatepoch + satrec.jdsatepochF)) * 1440.0);

        // ---- 在历元时刻传播 ----
        Vector3d pos0, vel0;
        errc_t rc = sgp4.getPosVel(epoch, pos0, vel0);
        printf("Propagation at epoch: rc = %d, error = %d\n", rc, sgp4.getError());
        printf("Position: [%.3f, %.3f, %.3f]\n", pos0[0], pos0[1], pos0[2]);
        printf("Velocity: [%.6f, %.6f, %.6f]\n", vel0[0], vel0[1], vel0[2]);
        EXPECT_EQ(rc, eNoError);

        // 验证位置/速度量级基本合理（近地太阳同步轨道：高度 ~800km, 速度 ~7.5km/s）
        double r0 = pos0.norm();
        double v0 = vel0.norm();
        EXPECT_GT(r0, 6.5e6);   // > 6500 km
        EXPECT_LT(r0, 7.5e6);   // < 7500 km
        EXPECT_GT(v0, 7.0e3);   // > 7.0 km/s
        EXPECT_LT(v0, 8.0e3);   // < 8.0 km/s

        // ---- 在历元 + 1 天后传播 ----
        TimePoint tp1 = epoch + 86400.0;
        Vector3d pos1, vel1;
        rc = sgp4.getPosVel(tp1, pos1, vel1);
        EXPECT_EQ(rc, eNoError);

        double r1 = pos1.norm();
        double v1 = vel1.norm();
        EXPECT_GT(r1, 6.5e6);
        EXPECT_LT(r1, 7.5e6);
        EXPECT_GT(v1, 7.0e3);
        EXPECT_LT(v1, 8.0e3);

        // 位置不应与历元完全相同
        EXPECT_GT((pos1 - pos0).norm(), 1.0);
    }
}


/// @brief 测试 SGP4 坐标系标识为 TEME
TEST(SGP4Test, FrameIsTEME)
{
    AST_USING_NAMESPACE
    {
        TLE tle = TLE::FromLines(makeTLELines());
        SGP4 sgp4(tle);

        Frame* frame = sgp4.getFrame();
        ASSERT_NE(frame, nullptr);

        // SGP4 输出的坐标系应为 TEME
        Frame* teme = aFrameTEME();
        EXPECT_EQ(frame, teme);
    }
}


/// @brief 测试 TEME → J2000 坐标转换（使用新增的 J2000↔TEME 函数）
TEST(SGP4Test, TEMEtoJ2000)
{
    AST_USING_NAMESPACE
    {
        TLE tle = TLE::FromLines(makeTLELines());
        SGP4 sgp4(tle);

        TimePoint epoch = tle.epochTime();

        Vector3d posTEME, velTEME;
        errc_t rc = sgp4.getPosVel(epoch, posTEME, velTEME);
        ASSERT_EQ(rc, eNoError);

        // 使用新增的 TEME→J2000 直接转换函数
        Vector3d posJ2000;
        aTEMEToJ2000(epoch, posTEME, posJ2000);

        // 验证 J2000→TEME 往返一致性
        Vector3d posBack;
        aJ2000ToTEME(epoch, posJ2000, posBack);

        EXPECT_NEAR(posBack[0], posTEME[0], 1e-6);
        EXPECT_NEAR(posBack[1], posTEME[1], 1e-6);
        EXPECT_NEAR(posBack[2], posTEME[2], 1e-6);

        // 矩阵往返一致性
        Matrix3d temeToJ2000;
        aTEMEToJ2000Matrix(epoch, temeToJ2000);

        Matrix3d j2000ToTEME;
        aJ2000ToTEMEMatrix(epoch, j2000ToTEME);

        // 两矩阵应互为转置（正交矩阵的逆=转置）
        Matrix3d product = temeToJ2000 * j2000ToTEME;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                EXPECT_NEAR(product(i, j), (i == j) ? 1.0 : 0.0, 1e-12);

        // 打印传播结果（供人工核查和参考值对比）
        printf("\n=== SGP4 Propagation at Epoch (TEME) ===\n");
        printf("TLE Epoch: year=2021, day=084, frac=0.99115140\n");
        printf("Position (m):  [%.6f,  %.6f,  %.6f]\n", posTEME[0], posTEME[1], posTEME[2]);
        printf("Velocity (m/s):[%.6f,  %.6f,  %.6f]\n", velTEME[0], velTEME[1], velTEME[2]);
        printf("Position (km): [%.6f,  %.6f,  %.6f]\n",
               posTEME[0]/1000.0, posTEME[1]/1000.0, posTEME[2]/1000.0);
        printf("Velocity (km/s):[%.9f, %.9f, %.9f]\n",
               velTEME[0]/1000.0, velTEME[1]/1000.0, velTEME[2]/1000.0);
        printf("|r| = %.3f km, |v| = %.6f km/s\n", posTEME.norm()/1000.0, velTEME.norm()/1000.0);

        printf("\n=== TEME->J2000 Converted ===\n");
        printf("Position (km): [%.6f,  %.6f,  %.6f]\n",
               posJ2000[0]/1000.0, posJ2000[1]/1000.0, posJ2000[2]/1000.0);
    }
}


/// @brief 测试多个时间点的传播连续性
TEST(SGP4Test, PropagationContinuity)
{
    AST_USING_NAMESPACE
    {
        TLE tle = TLE::FromLines(makeTLELines());
        SGP4 sgp4(tle);

        TimePoint epoch = tle.epochTime();

        // 在历元前后多个时间点传播
        const double offsets[] = {-3600.0, -60.0, 0.0, 60.0, 3600.0, 86400.0};
        Vector3d prevPos, prevVel;

        for (size_t i = 0; i < sizeof(offsets)/sizeof(offsets[0]); ++i)
        {
            double dt = offsets[i];
            TimePoint tp = epoch + dt;

            Vector3d pos, vel;
            errc_t rc = sgp4.getPosVel(tp, pos, vel);
            EXPECT_EQ(rc, eNoError) << "Propagation failed at offset " << dt << "s";

            // 位置量级应在合理范围
            double r = pos.norm();
            EXPECT_GT(r, 6.0e6) << "Position too small at offset " << dt << "s";
            EXPECT_LT(r, 8.0e6) << "Position too large at offset " << dt << "s";

            // 相邻时间点之间的位置变化应连续（非跳跃）
            if (i > 0 && offsets[i] - offsets[i-1] < 200.0)
            {
                double dr = (pos - prevPos).norm();
                double dtStep = offsets[i] - offsets[i-1];
                // 轨道速度约 7.5 km/s，位置变化应在速度×时间附近
                double expectedDr = 7.5e3 * std::abs(dtStep);
                EXPECT_LT(dr, expectedDr * 2.0) << "Position jump at dt=" << dt;
            }

            prevPos = pos;
            prevVel = vel;
        }
    }
}


/// @brief 测试 TimeInterval
TEST(SGP4Test, TimeInterval)
{
    AST_USING_NAMESPACE
    {
        TLE tle = TLE::FromLines(makeTLELines());
        SGP4 sgp4(tle);

        TimeInterval interval;
        errc_t rc = sgp4.getInterval(interval);
        EXPECT_EQ(rc, eNoError);

        // SGP4 返回无限区间
        double dur = interval.duration();
        EXPECT_GT(dur, 1e9);  // > 1e9 秒 ≈ 31 年，即"无限"
    }
}


GTEST_MAIN()
