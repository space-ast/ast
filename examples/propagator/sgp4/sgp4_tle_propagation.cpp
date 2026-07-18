///
/// @brief     SGP4 预报示例：从 TLE 两行根数解析并预报轨道
/// @details   演示使用 twoline2rv 解析 TLE 字符串，并使用 sgp4 进行轨道预报。
///            TLE (Two-Line Element) 是北美防空司令部 (NORAD) 发布的卫星轨道
///            根数标准格式，SGP4 结合 TLE 可以对近地卫星进行快速分析预报。
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

/// 打印位置/速度矢量
static void printState(double r[3], double v[3])
{
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  位置 (km):   [" << std::setw(12) << r[0]
              << "  " << std::setw(12) << r[1]
              << "  " << std::setw(12) << r[2] << "]\n";
    std::cout << std::setprecision(8);
    std::cout << "  速度 (km/s): [" << std::setw(14) << v[0]
              << "  " << std::setw(14) << v[1]
              << "  " << std::setw(14) << v[2] << "]\n";
    std::cout << std::setprecision(6);
    std::cout << "  地心距: " << std::setw(12) << SGP4Funcs::mag_SGP4(r) << " km  "
              << "速度大小: " << std::setw(12) << SGP4Funcs::mag_SGP4(v) << " km/s\n";
}

int main()
{
    setlocale(LC_ALL, ".UTF-8");
    std::cout << "========================================\n";
    std::cout << "  SGP4 轨道预报：从 TLE 解析并预报\n";
    std::cout << "  " << SGP4Version << "\n";
    std::cout << "========================================\n\n";

    // ---- 步骤 1: 定义 TLE 数据 ----
    //
    // TLE 格式由两行 69 字符组成：
    //   第1行 (Line 1)
    //     1-1    ：行号 '1'
    //     3-7    ：卫星编号 (5字符，如 25544)
    //     8      ：分类 (U=未分类, C=机密, S=秘密)
    //     10-17  ：国际编号 (如 98067A)
    //     19-20  ：历元年 (后两位，如 24=2024)
    //     21-32  ：历元日 (年内天数 + 小数)
    //     34-43  ：平运动一阶时间导数 (rev/day²)
    //     45-52  ：平运动二阶时间导数 (rev/day³)
    //     54-61  ：B* 大气阻力系数 (1/地球半径)
    //     63      ：星历类型 (0=SGP4)
    //     65-68  ：编号
    //
    //   第2行 (Line 2)
    //     1-1    ：行号 '2'
    //     3-7    ：卫星编号
    //     9-16   ：轨道倾角 (度)
    //     18-25  ：升交点赤经 (度)
    //     27-33  ：偏心率 (前导小数点省略)
    //     35-42  ：近地点幅角 (度)
    //     44-51  ：平近点角 (度)
    //     53-63  ：平运动 (圈/天)
    //     64-68  ：圈次

    // ISS (ZARYA) 典型 TLE
    char line1[130] = "1 25544U 98067A   24185.54861111  .00012345  00000+0  22222-3 0  9991";
    char line2[130] = "2 25544  51.6400 210.1234 0005000 100.0000 260.0000 15.50000000123456";

    std::cout << "输入 TLE:\n";
    std::cout << "  " << line1 << "\n";
    std::cout << "  " << line2 << "\n\n";

    // ---- 步骤 2: 解析 TLE ----
    //
    // twoline2rv 参数：
    //   longstr1/2 : TLE 两行字符串
    //   typerun    : 'm' = manual, 'v' = verification, 'c' = catalog
    //   typeinput  : 'e' = 使用历元时间
    //   opsmode    : 'i' = improved (推荐), 'a' = afspc
    //   whichconst : wgs72old / wgs72 / wgs84
    //   satrec     : 输出的卫星记录，包含所有 SGP4 状态

    elsetrec satrec;
    double startmfe = 0.0, stopmfe = 0.0, deltamin = 0.0;

    SGP4Funcs::twoline2rv(
        line1, line2,
        'c', 'e', 'i',    // typerun='c' = catalog (非交互，自动 -1~+1 天)
        wgs84,
        startmfe, stopmfe, deltamin,
        satrec);

    if (satrec.error != 0)
    {
        std::cout << "错误：TLE 解析失败，错误码 = " << satrec.error << "\n";
        return 1;
    }

    std::cout << "解析成功！卫星: " << satrec.satnumStr << "\n\n";

    // 打印解析出的轨道根数
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "--- 解析的轨道根数 ---\n";
    std::cout << "  倾角:           " << std::setw(12) << satrec.inclo * 180.0 / M_PI << " deg\n";
    std::cout << "  升交点赤经:     " << std::setw(12) << satrec.nodeo * 180.0 / M_PI << " deg\n";
    std::cout << "  偏心率:         " << std::setw(12) << satrec.ecco << "\n";
    std::cout << "  近地点幅角:     " << std::setw(12) << satrec.argpo * 180.0 / M_PI << " deg\n";
    std::cout << "  平近点角:       " << std::setw(12) << satrec.mo * 180.0 / M_PI << " deg\n";
    std::cout << "  平运动:         " << std::setw(12) << satrec.no_kozai * (1440.0 / (2.0 * M_PI)) << " rev/day\n";
    std::cout << std::scientific << std::setprecision(4);
    std::cout << "  B*:             " << std::setw(12) << satrec.bstar << " 1/ER\n";
    std::cout << std::fixed << std::setprecision(8);
    std::cout << "  历元:           " << satrec.epochyr << " + " << satrec.epochdays << " 天\n";
    std::cout << "  预报方法:       " << (satrec.method == 'd' ? "深空 (deep-space)" : "近地 (near-earth)")
              << "\n\n";

    // ---- 步骤 3: SGP4 轨道预报 ----
    //
    // sgp4 参数：
    //   satrec : 已初始化的卫星记录 (会被更新)
    //   tsince : 从历元起的时间差，单位：分钟 (正=未来，负=过去)
    //   r[3]   : 输出 — ECI 位置矢量 (km)
    //   v[3]   : 输出 — ECI 速度矢量 (km/s)
    //   返回值 : true=成功, false=失败 (查看 satrec.error)

    std::cout << "--- SGP4 轨道预报 ---\n";
    std::cout << "历元时刻 t=0 以及未来几个时间点的位置速度:\n\n";

    // 预报历元时刻及之后 90min, 180min, 360min, 720min, 1440min
    double tsinceSteps[] = {0.0, 90.0, 180.0, 360.0, 720.0, 1440.0};
    const char* labels[] = {"历元", "1.5 h", "3 h", "6 h", "12 h", "1 day"};

    for (int i = 0; i < 6; i++)
    {
        double tsince = tsinceSteps[i];
        double r[3], v[3];

        bool ok = SGP4Funcs::sgp4(satrec, tsince, r, v);

        std::cout << "[t+" << labels[i] << " (tsince = " << tsince << " min)]\n";

        if (ok)
            printState(r, v);
        else
            std::cout << "  预报失败！错误码 = " << satrec.error << "\n";
        std::cout << "\n";
    }

    // ---- 步骤 4: 生成一段星历表 ----
    std::cout << "--- 星历表（每 30 分钟，前 3 小时） ---\n";
    std::cout << std::setw(8) << "t(min)" << "  "
              << std::setw(10) << "X(km)" << "  "
              << std::setw(10) << "Y(km)" << "  "
              << std::setw(10) << "Z(km)" << "  "
              << std::setw(10) << "Range(km)" << "\n";
    std::cout << std::string(62, '-') << "\n";

    for (int i = 0; i <= 6; i++)
    {
        double tsince = i * 30.0;
        double r[3], v[3];

        if (SGP4Funcs::sgp4(satrec, tsince, r, v))
        {
            std::cout << std::fixed << std::setprecision(1)
                      << std::setw(8) << tsince << "  "
                      << std::setprecision(3)
                      << std::setw(10) << r[0] << "  "
                      << std::setw(10) << r[1] << "  "
                      << std::setw(10) << r[2] << "  "
                      << std::setw(10) << SGP4Funcs::mag_SGP4(r) << "\n";
        }
    }

    std::cout << "\n示例运行完毕。\n";
    return 0;
}
