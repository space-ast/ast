///
/// @brief     SGP4 时间工具函数示例
/// @details   演示 SGP4 模块中的时间系统转换工具：公历 ↔ 儒略日、年内天数转换、
///            格林尼治恒星时 (GST) 计算。
/// @author    axel
/// @date      2026-07-18
///

#include "ast/SGP4.h"
#include <iostream>
#include <iomanip>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AST_USING_NAMESPACE

int main()
{
    setlocale(LC_ALL, ".UTF-8");
    std::cout << "========================================\n";
    std::cout << "  SGP4 时间系统转换工具\n";
    std::cout << "  " << SGP4Version << "\n";
    std::cout << "========================================\n\n";

    // ---- 1. 公历日期 → 儒略日 (jday_SGP4) ----
    // 儒略日是轨道力学中最常用的时间系统，
    // jday_SGP4 将公历日期转换为儒略日整部和分秒部。
    {
        int year = 2024, mon = 7, day = 3, hr = 13, minute = 10;
        double sec = 0.0;
        double jd, jdFrac;

        SGP4Funcs::jday_SGP4(year, mon, day, hr, minute, sec, jd, jdFrac);

        std::cout << "1) 公历 → 儒略日 (jday_SGP4)\n";
        std::cout << "   输入: " << year << "-" << mon << "-" << day
                  << " " << hr << ":" << minute << ":" << sec << "\n";
        std::cout << std::fixed << std::setprecision(10);
        std::cout << "   输出: JD = " << jd << ", 小数 = " << jdFrac << "\n";
        std::cout << "   完整 JD = " << jd + jdFrac << "\n\n";
    }

    // ---- 2. 儒略日 → 公历日期 (invjday_SGP4) ----
    // 逆操作：从儒略日反算出公历年月日时分秒。
    {
        double jd = 2460495.0, jdFrac = 0.0486111111111111;

        int year, mon, day, hr, minute;
        double sec;
        SGP4Funcs::invjday_SGP4(jd, jdFrac, year, mon, day, hr, minute, sec);

        std::cout << "2) 儒略日 → 公历 (invjday_SGP4)\n";
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "   输入: JD = " << jd + jdFrac << "\n";
        std::cout << "   输出: " << year << "-" << mon << "-" << day
                  << " " << hr << ":" << minute << ":" << sec << "\n\n";
    }

    // ---- 3. 儒略日 → 格林尼治恒星时 (gstime_SGP4) ----
    // GST 是地面站可见性和坐标转换的基础量。
    {
        double jd = 2460495.0 + 0.0486111111111111;

        double gst = SGP4Funcs::gstime_SGP4(jd);
        double gstDeg = gst * 180.0 / M_PI;
        int gstH = (int)(gstDeg / 15.0);
        int gstM = (int)((gstDeg / 15.0 - gstH) * 60.0);
        double gstS = ((gstDeg / 15.0 - gstH) * 60.0 - gstM) * 60.0;

        std::cout << "3) 儒略日 → 格林尼治恒星时 (gstime_SGP4)\n";
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "   输入: JD = " << jd << "\n";
        std::cout << std::setprecision(8);
        std::cout << "   GST = " << gst << " rad\n";
        std::cout << std::setprecision(6);
        std::cout << "       = " << gstDeg << " deg\n";
        std::cout << "       = " << gstH << "h " << gstM << "m "
                  << std::setprecision(3) << gstS << "s\n\n";
    }

    // ---- 4. 年内天数 → 月日时分秒 (days2mdhms_SGP4) ----
    // TLE 历元中的 ".54861111" 部分就是年内天数，
    // days2mdhms_SGP4 将其转换为常规的月日时分秒。
    {
        int yearD = 2024;
        double doy = 185.54861111;  // 第185天 ≈ 7月3日 13:10

        int mon, day, hr, minute;
        double sec;
        SGP4Funcs::days2mdhms_SGP4(yearD, doy, mon, day, hr, minute, sec);

        std::cout << "4) 年内天数 → 月日时分秒 (days2mdhms_SGP4)\n";
        std::cout << std::fixed << std::setprecision(8);
        std::cout << "   输入: year=" << yearD << ", day-of-year=" << doy << "\n";
        std::cout << "   输出: " << yearD << "-" << mon << "-" << day
                  << " " << hr << ":" << minute << std::setprecision(3)
                  << ":" << sec << "\n\n";
    }

    // ---- 5. 组合使用：TLE 历元 → UTC 日期 ----
    // 典型场景：从解析的 TLE 中取 epochyr + epochdays，得到 UTC 时间。
    {
        // 解析一个 TLE 获取历元信息
        char line1[130] = "1 25544U 98067A   24185.54861111  .00012345  00000+0  22222-3 0  9991";
        char line2[130] = "2 25544  51.6400 210.1234 0005000 100.0000 260.0000 15.50000000123456";

        elsetrec satrec;
        double startmfe = 0.0, stopmfe = 0.0, deltamin = 0.0;

        SGP4Funcs::twoline2rv(
            line1, line2, 'c', 'e', 'i', wgs84,
            startmfe, stopmfe, deltamin, satrec);

        if (satrec.error == 0)
        {
            // TLE 历元: epochyr=24 (2024), epochdays=185.54861111
            int yr = satrec.epochyr;
            // 两位年份转四位: 57-99 → 1957-1999, 00-56 → 2000-2056
            int fullYear = (yr < 57) ? yr + 2000 : yr + 1900;

            int mon, dayD, hr, minute;
            double sec;
            SGP4Funcs::days2mdhms_SGP4(fullYear, satrec.epochdays,
                                        mon, dayD, hr, minute, sec);

            std::cout << "5) TLE 历元 → UTC 日期\n";
            std::cout << "   卫星: " << satrec.satnumStr << "\n";
            std::cout << "   历元: yr=" << yr << ", days="
                      << std::fixed << std::setprecision(8)
                      << satrec.epochdays << "\n";
            std::cout << "   UTC:  " << fullYear << "-" << mon << "-" << dayD
                      << " " << hr << ":" << minute << std::setprecision(3)
                      << ":" << sec << "\n\n";
        }
    }

    std::cout << "示例运行完毕。\n";
    return 0;
}
