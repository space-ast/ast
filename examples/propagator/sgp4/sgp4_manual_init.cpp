///
/// @brief     SGP4 预报示例：使用 sgp4init 手动初始化并预报轨道
/// @details   演示不通过 TLE 字符串，而是直接调用 sgp4init 设置轨道根数，
///            然后使用 sgp4 进行轨道预报。
///            这种用法适合需要程序化修改轨道根数的场景。
/// @author    axel
/// @date      2026-07-18
///

#include "ast/SGP4.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AST_USING_NAMESPACE

/// 打印位置/速度矢量
static void printState(const char* label, double tsince, double r[3], double v[3])
{
    std::cout << std::fixed;
    std::cout << "  [" << label << "] tsince = " << std::setw(8) << std::setprecision(1) << tsince << " min\n";
    std::cout << std::setprecision(4);
    std::cout << "    位置 (km):   " << std::setw(12) << r[0]
              << "  " << std::setw(12) << r[1]
              << "  " << std::setw(12) << r[2] << "\n";
    std::cout << "    速度 (km/s): " << std::setprecision(6)
              << std::setw(14) << v[0]
              << "  " << std::setw(14) << v[1]
              << "  " << std::setw(14) << v[2] << "\n";
    std::cout << std::setprecision(4);
    std::cout << "    地心距: " << std::setw(10) << SGP4Funcs::mag_SGP4(r) << " km\n\n";
}

int main()
{
    setlocale(LC_ALL, ".UTF-8");
    std::cout << "========================================\n";
    std::cout << "  SGP4 轨道预报：手动 sgp4init 初始化\n";
    std::cout << "  " << SGP4Version << "\n";
    std::cout << "========================================\n\n";

    // ---- 步骤 1: 准备轨道根数 ----
    //
    // 以 ISS 为例。TLE 中各参数的含义和单位：
    //   TLE epoch 格式: YYDDD.DDDDDDDD (年份后两位 + 年内天数及小数)
    //   但 sgp4init 的 epoch 参数要求的格式不同，见下方说明。

    // TLE 原始数据（与 twoline2rv 示例中的 TLE 完全一致）
    // 1 25544U 98067A   24185.54861111  .00012345  00000+0  22222-3 0  9991
    // 2 25544  51.6400 210.1234 0005000 100.0000 260.0000 15.50000000123456

    // TLE 原始值 (deg, rev/day 等单位)
    const double tle_inclo  = 51.64;
    const double tle_nodeo  = 210.1234;
    const double tle_argpo  = 100.0;
    const double tle_mo     = 260.0;
    const double tle_no     = 15.5;         // rev/day
    const double tle_ndot   = 0.00012345;   // rev/day²
    const double tle_nddot  = 0.0;          // rev/day³

    // 单位转换：TLE 单位 → sgp4init 期望的单位
    //   角度: deg → rad
    //   平运动: rev/day → rad/min (除以 xpdotp)
    //   ndot:  rev/day² → rad/min² (除以 xpdotp*1440)
    //   nddot: rev/day³ → rad/min³ (除以 xpdotp*1440²)
    const double xpdotp    = 1440.0 / (2.0 * M_PI);   // ≈ 229.183
    const double deg2rad   = M_PI / 180.0;

    // 以下为 sgp4init 的参数，均已从 TLE 单位转换：
    //   satn[6] : 卫星编号字符串 (5字符，如 "25544")
    //   bstar   : B* 大气阻力系数 (1/ER)，TLE 第1行 54-61 列
    //   ndot    : 平运动一阶时间导数 (rad/min²)，TLE 第1行 34-43 列
    //   nddot   : 平运动二阶时间导数 (rad/min³)，TLE 第1行 45-52 列
    //   ecco    : 偏心率（无量纲），TLE 第2行 27-33 列
    //   argpo   : 近地点幅角 (rad)，TLE 第2行 35-42 列
    //   inclo   : 轨道倾角 (rad)，TLE 第2行 9-16 列
    //   mo      : 平近点角 (rad)，TLE 第2行 44-51 列
    //   no_kozai: Kozai 平运动 (rad/min)，TLE 第2行 53-63 列
    //             SGP4 内部会由 no_kozai 计算出 Brouwer 平运动 no_unkozai
    //   nodeo   : 升交点赤经 (rad)，TLE 第2行 18-25 列
    //   epoch   : 历元，1950-01-01 以来的天数 (JD - 2433281.5)

    const char satn[6]    = "25544";
    const double bstar    = 2.2222e-4;                         // B* (1/ER)
    const double ndot     = tle_ndot / (xpdotp * 1440.0);      // → rad/min²
    const double nddot    = tle_nddot / (xpdotp * 1440.0 * 1440.0); // → rad/min³
    const double ecco     = 0.0005000;                          // 偏心率 (无量纲)
    const double argpo    = tle_argpo * deg2rad;                // → rad
    const double inclo    = tle_inclo * deg2rad;                // → rad
    const double mo       = tle_mo * deg2rad;                   // → rad
    const double no_kozai = tle_no / xpdotp;                    // → rad/min
    const double nodeo    = tle_nodeo * deg2rad;                // → rad

    // ---- 步骤 2: 准备历元（关键步骤！） ----
    //
    // TLE 的历元 "24185.54861111" 表示 2024 年第 185.54861111 天，
    // 但 sgp4init 的 epoch 参数要求的是 **1950-01-01 以来的天数**
    // （即 JD - 2433281.5，其中 2433281.5 是 1950-01-01 00:00:00 的儒略日）。
    //
    // 正确做法：先将 TLE 历元转为公历日期 → 算儒略日 → 减 2433281.5

    int epochYr = 24;                               // TLE 历元年（后两位）
    double epochDays = 185.54861111;                 // TLE 历元日（年内天数）
    int fullYear = (epochYr < 57) ? epochYr + 2000 : epochYr + 1900;

    int mon, day, hr, minute;
    double sec;
    SGP4Funcs::days2mdhms_SGP4(fullYear, epochDays, mon, day, hr, minute, sec);

    double jd, jdFrac;
    SGP4Funcs::jday_SGP4(fullYear, mon, day, hr, minute, sec, jd, jdFrac);

    double epochForSgp4 = jd + jdFrac - 2433281.5;   // 1950-01-01 以来的天数

    std::cout << "TLE 历元转换:\n";
    std::cout << "  TLE 格式: " << epochYr << " + " << std::fixed << std::setprecision(8)
              << epochDays << " 天\n";
    std::cout << "  公历日期: " << fullYear << "-" << mon << "-" << day
              << " " << hr << ":" << minute << ":" << sec << "\n";
    std::cout << std::setprecision(6);
    std::cout << "  儒略日:   " << jd + jdFrac << "\n";
    std::cout << "  sgp4init epoch: " << std::setprecision(8) << epochForSgp4 << " (1950-01-01 以来的天数)\n\n";

    std::cout << "输入轨道根数:\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  卫星编号:       " << satn << "\n";
    std::cout << "  倾角:           " << inclo * 180.0 / M_PI << " deg\n";
    std::cout << "  升交点赤经:     " << nodeo * 180.0 / M_PI << " deg\n";
    std::cout << "  偏心率:         " << ecco << "\n";
    std::cout << "  近地点幅角:     " << argpo * 180.0 / M_PI << " deg\n";
    std::cout << "  平近点角:       " << mo * 180.0 / M_PI << " deg\n";
    std::cout << "  平运动:         " << no_kozai * 1440.0 / (2.0 * M_PI) << " rev/day\n";
    std::cout << std::scientific << std::setprecision(4);
    std::cout << "  B*:             " << bstar << " 1/ER\n\n";

    // ---- 步骤 3: 调用 sgp4init 初始化 ----
    //
    // 重力常数选项:
    //   wgs72old — WGS-72 (旧版)
    //   wgs72    — WGS-72
    //   wgs84    — WGS-84 (推荐，与 GPS 一致)
    //
    // opsmode:
    //   'i' — improved 模式 (推荐，修正了一些 SGP4 的已知问题)
    //   'a' — afspc 模式 (与 AFSPC 官方实现一致)

    elsetrec satrec;
    memset(&satrec, 0, sizeof(satrec));

    bool initOk = SGP4Funcs::sgp4init(
        wgs84,           // whichconst
        'i',             // opsmode
        satn,            // 卫星编号
        epochForSgp4,    // 历元 (1950-01-01 以来的天数)
        bstar, ndot, nddot,
        ecco, argpo, inclo, mo, no_kozai,
        nodeo,
        satrec);

    if (!initOk)
    {
        std::cout << "错误：sgp4init 初始化失败！\n";
        return 1;
    }

    std::cout << "sgp4init 初始化成功。\n";
    std::cout << "  方法: " << (satrec.method == 'd' ? "'d' (深空, period >= 225 min)" : "'n' (近地, period < 225 min)") << "\n";
    std::cout << "  Kozai 平运动:        " << satrec.no_kozai * 1440.0 / (2.0 * M_PI) << " rev/day\n";
    std::cout << "  Brouwer 平运动:      " << satrec.no_unkozai * 1440.0 / (2.0 * M_PI) << " rev/day\n\n";

    // ---- 步骤 4: 进行预报 ----
    //
    // sgp4 函数每次调用都会更新 satrec 内部状态，
    // 对于同一个 satrec，建议按时间顺序调用 sgp4 以获得最佳精度。

    std::cout << "--- SGP4 预报结果 ---\n\n";

    // 预报未来 24 小时，每 3 小时取一个点
    for (int hour = 0; hour <= 24; hour += 3)
    {
        double tsince = hour * 60.0;  // 小时 → 分钟
        double r[3], v[3];

        char label[32];
        snprintf(label, sizeof(label), "T+%2dh", hour);

        bool ok = SGP4Funcs::sgp4(satrec, tsince, r, v);

        if (ok)
            printState(label, tsince, r, v);
        else
            std::cout << "  预报失败！错误码 = " << satrec.error << "\n";
    }

    // ---- 步骤 5: 对比不同引力常数模型 ----
    std::cout << "--- 不同引力常数模型对比 (tsince = 360 min) ---\n\n";

    gravconsttype models[] = {wgs72old, wgs72, wgs84};
    const char* modelNames[] = {"WGS-72 (old)", "WGS-72", "WGS-84"};

    for (int m = 0; m < 3; m++)
    {
        elsetrec sr;
        memset(&sr, 0, sizeof(sr));

        SGP4Funcs::sgp4init(
            models[m], 'i', satn, epochForSgp4,
            bstar, ndot, nddot,
            ecco, argpo, inclo, mo, no_kozai,
            nodeo, sr);

        double r[3], v[3];
        if (SGP4Funcs::sgp4(sr, 360.0, r, v))
        {
            std::cout << "  " << modelNames[m] << ":\n";
            std::cout << std::fixed << std::setprecision(4);
            std::cout << "    位置: [" << r[0] << ", " << r[1] << ", " << r[2] << "] km\n";
            std::cout << "    地心距: " << SGP4Funcs::mag_SGP4(r) << " km\n";
        }
    }

    std::cout << "\n示例运行完毕。\n";
    return 0;
}
