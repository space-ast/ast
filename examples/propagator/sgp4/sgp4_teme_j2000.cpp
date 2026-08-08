///
/// @brief     SGP4 预报示例：输出 TEME 和 J2000 系下 4 小时后的位置速度
/// @details   演示使用 SGP4 从 TLE 两行根数预报轨道，并将结果同时输出到
///            TEME（SGP4 原生坐标系）和 J2000（平赤道系）两个坐标系。
/// @author    axel
/// @date      2026-08-02
///

#include "ast/SGP4.hpp"
#include "ast/SGP4.h"
#include "ast/TLE.hpp"
#include "ast/TLELines.hpp"
#include "ast/BuiltinFrame.hpp"
#include "ast/Vector.hpp"
#include "ast/TimePoint.hpp"
#include "ast/DateTime.hpp"
#include "ast/RunTime.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AST_USING_NAMESPACE

int main()
{
    setlocale(LC_ALL, ".UTF-8");
    aInitialize();

    // ---- TLE 数据 ----
    // 1 48274U 21035A   26212.04472080  .00021617  00000-0  27734-3 0  9997
    // 2 48274  41.4687  55.7361 0001081 287.8102  72.2618 15.58621958300012

    char line1[130] = "1 48274U 21035A   26212.04472080  .00021617  00000-0  27734-3 0  9997";
    char line2[130] = "2 48274  41.4687  55.7361 0001081 287.8102  72.2618 15.58621958300012";

    // ---- 初始化 SGP4 ----
    TLE tle = TLE::FromLines(line1, line2);
    SGP4 sgp4(tle);

    if (sgp4.getError() != 0)
    {
        std::cout << "TLE 解析失败，错误码 = " << sgp4.getError() << "\n";
        return 1;
    }

    TimePoint epoch     = tle.epochTime();
    Frame*    frameJ2000 = aFrameEarthJ2000();

    // ---- 预报 4 小时 ----
    const double dtHours = 4.0;
    TimePoint    tp      = epoch + dtHours * 3600.0;

    Vector3d posTEME, velTEME;
    Vector3d posJ2000, velJ2000;

    sgp4.getPosVel(tp, posTEME, velTEME);                // TEME 系（m, m/s）
    sgp4.getPosVelIn(frameJ2000, tp, posJ2000, velJ2000); // J2000 系（m, m/s）

    // 转换为 km, km/s
    constexpr double kInv1000 = 1.0 / 1000.0;
    Vector3d posTEME_km  = posTEME  * kInv1000;
    Vector3d velTEME_kms = velTEME  * kInv1000;
    Vector3d posJ2000_km = posJ2000 * kInv1000;
    Vector3d velJ2000_kms = velJ2000 * kInv1000;

    // ---- 输出 ----
    DateTime epochUTC, tpUTC;
    aTimePointToUTC(epoch, epochUTC);
    aTimePointToUTC(tp, tpUTC);

    std::cout << std::fixed;
    std::cout << "========================================================\n";
    std::cout << "  SGP4 TLE 预报 — 4 小时后位置速度\n";
    std::cout << "========================================================\n";
    std::cout << "TLE:\n";
    std::cout << "  " << line1 << "\n";
    std::cout << "  " << line2 << "\n\n";

    std::cout << "历元 (UTC): " << epochUTC.year() << "-"
              << std::setw(2) << std::setfill('0') << epochUTC.month() << "-"
              << std::setw(2) << epochUTC.day() << " "
              << std::setw(2) << epochUTC.hour() << ":"
              << std::setw(2) << epochUTC.minute() << ":"
              << std::setprecision(3) << epochUTC.second() << std::setfill(' ')
              << "\n";

    std::cout << "预报 (UTC): " << tpUTC.year() << "-"
              << std::setw(2) << std::setfill('0') << tpUTC.month() << "-"
              << std::setw(2) << tpUTC.day() << " "
              << std::setw(2) << tpUTC.hour() << ":"
              << std::setw(2) << tpUTC.minute() << ":"
              << std::setprecision(3) << tpUTC.second() << std::setfill(' ')
              << "\n\n";

    // TEME
    std::cout << "--- TEME (真赤道平春分点系) ---\n";
    std::cout << std::setprecision(4);
    std::cout << "  位置 (km):   ["
              << std::setw(14) << posTEME_km.x()
              << "  " << std::setw(14) << posTEME_km.y()
              << "  " << std::setw(14) << posTEME_km.z() << "]\n";
    std::cout << std::setprecision(8);
    std::cout << "  速度 (km/s): ["
              << std::setw(16) << velTEME_kms.x()
              << "  " << std::setw(16) << velTEME_kms.y()
              << "  " << std::setw(16) << velTEME_kms.z() << "]\n";
    std::cout << std::setprecision(4);
    std::cout << "  地心距: " << posTEME_km.norm() << " km"
              << "  速度: " << velTEME_kms.norm() << " km/s\n\n";

    // J2000
    std::cout << "--- J2000 (地球平赤道系) ---\n";
    std::cout << std::setprecision(4);
    std::cout << "  位置 (km):   ["
              << std::setw(14) << posJ2000_km.x()
              << "  " << std::setw(14) << posJ2000_km.y()
              << "  " << std::setw(14) << posJ2000_km.z() << "]\n";
    std::cout << std::setprecision(8);
    std::cout << "  速度 (km/s): ["
              << std::setw(16) << velJ2000_kms.x()
              << "  " << std::setw(16) << velJ2000_kms.y()
              << "  " << std::setw(16) << velJ2000_kms.z() << "]\n";
    std::cout << std::setprecision(4);
    std::cout << "  地心距: " << posJ2000_km.norm() << " km"
              << "  速度: " << velJ2000_kms.norm() << " km/s\n";

    return 0;
}
