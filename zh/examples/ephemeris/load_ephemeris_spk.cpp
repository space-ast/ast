// 本文件为 zh/examples/ephemeris/load_ephemeris_spk.md 中的完整代码

#include "ast/JplSpk.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Vector.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

AST_USING_NAMESPACE

int main()
{
    // 创建 JplSpk 实例并加载 SPK 星历文件（请替换为实际文件路径）
    JplSpk spk;
    errc_t err = spk.open("./data/kernels/spk/de430.bsp");
    if (err != eNoError) {
        std::cerr << "无法加载SPK星历文件" << std::endl;
        return -1;
    }

    // 获取地球（NAIF ID 399）的星历时间区间
    TimeInterval interval;
    err = spk.getInterval(399, interval);
    if (err != eNoError) {
        std::cerr << "获取星历时间区间失败，错误码: " << err << std::endl;
    } else {
        std::cout << "星历时间范围: " << std::endl;
        std::cout << "  开始时间: " << interval.getStart().toString() << std::endl;
        std::cout << "  结束时间: " << interval.getStop().toString() << std::endl;
    }

    // 列出文件中的天体
    std::vector<std::string> names;
    err = spk.getBodyNames(names);
    if (err != eNoError) {
        std::cerr << "获取天体列表失败，错误码: " << err << std::endl;
    } else {
        std::cout << "\n文件中的天体:" << std::endl;
        for (const auto& name : names) {
            std::cout << "  " << name << std::endl;
        }
    }

    // 查询 2000-01-01 12:00:00 UTC 时刻的天体状态
    TimePoint time = TimePoint::FromUTC(2000, 1, 1, 12, 0, 0.0);

    Vector3d pos, vel;

    // 地球相对地月系质心（NAIF ID 3）的位置和速度
    err = JplSpk::getPosVelICRF(time, 399, 3, pos, vel);
    if (err != eNoError) {
        std::cerr << "获取地球位置速度失败，错误码: " << err << std::endl;
    } else {
        std::cout << "\n地球相对地月系质心的状态 (ICRF):" << std::endl;
        std::cout << "  位置 [m]:   " << std::setprecision(6)
                  << pos.x() << "  " << pos.y() << "  " << pos.z() << std::endl;
        std::cout << "  速度 [m/s]: " << std::setprecision(6)
                  << vel.x() << "  " << vel.y() << "  " << vel.z() << std::endl;
    }

    // 月球相对地月系质心的位置，与地球位置作差得到地月相对位置
    Vector3d earthPos, earthVel;
    Vector3d moonPos, moonVel;

    errc_t errEarth = JplSpk::getPosVelICRF(time, 399, 3, earthPos, earthVel);
    errc_t errMoon = JplSpk::getPosVelICRF(time, 301, 3, moonPos, moonVel);
    if (errEarth != eNoError || errMoon != eNoError) {
        std::cerr << "获取地月位置速度失败，错误码: " << errEarth << ", " << errMoon << std::endl;
    } else {
        Vector3d moonRelEarth = moonPos - earthPos;
        std::cout << "\n月球相对地球的位置 [m]:" << std::endl;
        std::cout << "  X: " << std::setprecision(6) << moonRelEarth.x() << std::endl;
        std::cout << "  Y: " << std::setprecision(6) << moonRelEarth.y() << std::endl;
        std::cout << "  Z: " << std::setprecision(6) << moonRelEarth.z() << std::endl;
        std::cout << "  地月距离: " << std::setprecision(6) << moonRelEarth.norm() << " m" << std::endl;
    }

    spk.close();

    return 0;
}
