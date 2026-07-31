///
/// @file      testSeetElectronFlux.cpp
/// @brief     复现 STK SEET 教程中的电子辐射通量图
/// @details   使用 NASA AE8 模型 (get_flux) 计算轨道上的电子辐射通量，
///            并使用 matplot++ 绘制时间-辐射通量曲线。
/// @author    axel
/// @date      2026-07-31
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// 参考教程: STK SEET Radiation Environment Tutorial
///
/// 场景参数:
///   - 时间: 2016-07-01 16:00:00 UTCG 至 2016-07-02 16:00:00 UTCG (1天)
///   - 轨道: 半长轴 15000 km, 偏心率 0.4, 倾角 30°, 近地点角 50°, RAAN 90°, 真近点角 0°
///   - 模式: NASA AE8, 太阳活动极大年 (Solar Max)
///

#include "ast/AstCore.hpp"
#include "ast/AstWeather.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/TwoBody.hpp"
#include "ast/TimePoint.hpp"
#include "ast/DateTime.hpp"
#include "ast/GeodeticPoint.hpp"
#include "ast/LocalHorizonalFrame.hpp"
#include "ast/SOFA.hpp"
#include "ast/EarthFrame.hpp"
#include "ast/Magnetosphere.hpp"
#include <cstdio>
#include <vector>
#include <string>

#ifdef AST_WITH_MATPLOT
#include <matplot/matplot.h>
#endif

AST_USING_NAMESPACE


int main(int argc, char* argv[])
{
    // ============================
    // 1. STK 教程轨道参数设置
    // ============================
    double sma   = 15000e3;                     // 半长轴 [m]
    double ecc   = 0.4;                         // 偏心率
    double inc   = 30.0 * kDegToRad;            // 倾角 [rad]
    double argp  = 50.0 * kDegToRad;            // 近地点角 [rad]
    double raan  = 90.0 * kDegToRad;            // RAAN [rad]
    double trueA = 0.0;                         // 真近点角 [rad]

    OrbElem elem{sma, ecc, inc, raan, argp, trueA};

    Vector3d pos0, vel0;
    errc_t rc = aOrbElemToCart(elem, kEarthGrav, pos0, vel0);
    if (rc != eNoError)
    {
        fprintf(stderr, "Error: failed to convert orbital elements to Cartesian state.\n");
        return 1;
    }

    double meanMotion = elem.getMeanMotion(kEarthGrav);
    double period = kTwoPI / meanMotion;
    fprintf(stderr, "Orbit period: %.2f min (%.2f hours)\n", period / 60.0, period / 3600.0);

    double rp = sma * (1.0 - ecc);
    double ra = sma * (1.0 + ecc);
    double hp = (rp - kEarthRadius) * 0.001;
    double ha = (ra - kEarthRadius) * 0.001;
    fprintf(stderr, "Perigee: %.2f km, Apogee: %.2f km\n", hp, ha);

    // ============================
    // 2. 时间设置
    // ============================
    TimePoint startTime = TimePoint::FromUTC(2016, 7, 1, 16, 0, 0.0);

    double startDecimalYear = 2016.0 + (182.0 + 57600.0 / 86400.0) / 366.0;
    double secondsPerYear = 366.0 * 86400.0;

    fprintf(stderr, "Start: 2016-07-01 16:00:00 UTCG\n");
    fprintf(stderr, "Decimal year: %.10f\n", startDecimalYear);

    double totalDuration = 86400.0;   // 1 天
    double stepSize = 60.0;           // 60 秒步长

    // ============================
    // 3. 计算并存储数据
    // ============================
    int nSteps = (int)(totalDuration / stepSize) + 1;
    fprintf(stderr, "Computing %d time steps...\n", nSteps);

    // 时间轴 (小时)
    std::vector<double> timeHours;
    // 通量数据
    std::vector<double> flux_e_065;   // >0.65 MeV, Solar Max
    std::vector<double> flux_e_095;   // >0.95 MeV, Solar Max
    std::vector<double> flux_e_160;   // >1.60 MeV, Solar Max

    // CSV 输出
    printf("Time(UTCG),Longitude(deg),Latitude(deg),Altitude(km),L,B/B0,");
    printf("Flux_e_065(m^-2_s^-1),Flux_e_095(m^-2_s^-1),");
    printf("Flux_e_160(m^-2_s^-1)\n");

    for (double t = 0.0; t <= totalDuration; t += stepSize)
    {
        // --- 轨道传播 ---
        Vector3d r = pos0;
        Vector3d v = vel0;
        aTwoBodyProp(t, kEarthGrav, r, v);

        TimePoint currentTime = startTime + t;
        double decimalYear = startDecimalYear + t / secondsPerYear;

        // --- ECI → ECEF ---
        Vector3d rECEF;
        aICRFToECF(currentTime, r, rECEF);

        // --- ECEF → 大地坐标 ---
        GeodeticPoint gpoint;
        aBodyFixedToGeodetic(rECEF, gpoint, kEarthRadius, kEarthFlatFact);

        double lon = gpoint.longitude() * kRadToDeg;   // 弧度→度
        double lat = gpoint.latitude() * kRadToDeg;    // 弧度→度
        double alt_km = gpoint.altitude() * 0.001;

        // --- 磁坐标 ---
        double L, BB0;
        igrf(lon, lat, alt_km, decimalYear, L, BB0);

        // --- 通量计算 (Solar Min, NASA AE8) ---
        double f_065 = get_flux(lon, lat, alt_km, 0.65, decimalYear, 1);
        double f_095 = get_flux(lon, lat, alt_km, 0.95, decimalYear, 1);
        double f_160 = get_flux(lon, lat, alt_km, 1.60, decimalYear, 1);

        // 存储绘图数据 (cm⁻²·s⁻¹ → m⁻²·s⁻¹, ×10000)
        timeHours.push_back(t / 3600.0);
        flux_e_065.push_back(f_065 * 1e4);
        flux_e_095.push_back(f_095 * 1e4);
        flux_e_160.push_back(f_160 * 1e4);

        // CSV 输出
        DateTime dt;
        aTimePointToUTC(currentTime, dt);

        printf("%04d-%02d-%02dT%02d:%02d:%06.3f,", dt.year(), dt.month(), dt.day(),
               dt.hour(), dt.minute(), dt.second());
        printf("%.6f,%.6f,%.3f,", lon, lat, alt_km);
        printf("%.6f,%.6f,", L, BB0);
        printf("%.6e,%.6e,", f_065 * 1e4, f_095 * 1e4);
        printf("%.6e\n", f_160 * 1e4);

        if ((int)(t / stepSize) % 60 == 0)
        {
            fprintf(stderr, "  Progress: %.1f / %.1f hours\r",
                    t / 3600.0, totalDuration / 3600.0);
        }
    }
    fprintf(stderr, "\nComputation done. %zu data points.\n", timeHours.size());

    // ============================
    // 4. 使用 matplot++ 绘图
    // ============================
#ifdef AST_WITH_MATPLOT
    using namespace matplot;

    // 创建图表: 时间-辐射通量 (所有通道)
    auto fig1 = figure(true);
    fig1->name("STK SEET: Electron Radiation Flux (All Channels)");
    fig1->size(1200, 700);

    auto ax1 = fig1->current_axes();

    auto p1 = semilogy(ax1, timeHours, flux_e_065, "-b");
    hold(on);
    auto p2 = semilogy(ax1, timeHours, flux_e_095, "-m");
    auto p3 = semilogy(ax1, timeHours, flux_e_160, "-r");

    p1->line_width(1.5);
    p2->line_width(1.5);
    p3->line_width(2.0);

    xlabel("Time (hours since 2016-07-01 16:00 UTCG)");
    ylabel("Integral Electron Flux (m^{-2} s^{-1})");
    title("STK SEET Electron Radiation Flux\n"
          "Orbit: a=15000km, e=0.4, i=30deg, NASA AE8 Model");

    auto lgd = legend({
        ">0.65 MeV (Solar Min)",
        ">0.95 MeV (Solar Min)",
        ">1.60 MeV (Solar Min)"
    });
    lgd->location(legend::general_alignment::topright);

    grid(on);

    // ============================
    // 5. 保存并显示图表
    // ============================
    fig1->save("seet_electron_flux_time.png");
    fprintf(stderr, "\nPlot saved to: seet_electron_flux_time.png\n");

    show();
#else
    fprintf(stderr, "\n[INFO] matplot++ not available. CSV data has been output to stdout.\n");
    fprintf(stderr, "[INFO] To plot, save CSV and use Python/matplotlib:\n");
    fprintf(stderr, "[INFO]   xmake run testSeetElectronFlux 2>nul > flux.csv\n");
#endif

    return 0;
}
