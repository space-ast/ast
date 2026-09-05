///
/// @file      save_sgp4_twobody.cpp
/// @brief     将 SGP4 / 二体(TwoBody)星历保存为 STK 星历文件 (.e)
/// @details   演示两种生成星历的途径，并分别用 STKEphemerisFileWriter 写出为 STK 文本星历：
///             1) SGP4  —— 由 TLE（两行根数）构造；
///             2) 二体   —— 由初始状态 (CartState) + 参考系 + 历元 构造 EphemerisTwoBody。
///            注意：这两类星历的时段 getInterval() 都是无穷大（整条时间轴），写入器要求一个
///                 有限写入窗口，因此必须调用 setInterval 指定。
/// @author    axel
/// @date      2026-09-05
///

#include "ast/SGP4.hpp"
#include "ast/TLE.hpp"
#include "ast/TLELines.hpp"
#include "ast/EphemerisTwoBody.hpp"
#include "ast/EphemerisLagrangeVar.hpp"
#include "ast/STKEphemerisFileWriter.hpp"
#include "ast/STKEphemerisFileParser.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/RunTime.hpp"
#include "ast/Constants.hpp"
#include <iostream>
#include <iomanip>
#include <clocale>
#include <cmath>

AST_USING_NAMESPACE

int main()
{
    setlocale(LC_ALL, ".UTF-8");

    // SGP4 / 二体星历的时段均为无穷大，写入器需要有限窗口。
    const TimePoint epoch = TimePoint::FromUTC(2024, 1, 1, 0, 0, 0);
    const TimeInterval window(epoch, 0.0, 3600.0);   // [epoch, epoch + 1h]
    // 采样步长默认 60 秒；如有需要可用 writer.setStep(seconds) 调整。

    // ============ 1) SGP4 星历：从 TLE 构造并保存 ============
    {
        TLELines lines;
        lines.line1() = "1 25544U 98067A   24185.54861111  .00012345  00000+0  22222-3 0  9991";
        lines.line2() = "2 25544  51.6400 210.1234 0005000 100.0000 260.0000 15.50000000123456";
        SGP4 sgp4(TLE::FromLines(lines));   // SGP4 继承自 Ephemeris(=Point)

        STKEphemerisFileWriter writer;
        writer.setInterval(window);
        writer.setCoordinate(STKEphemerisFileWriter::eICRF);
        errc_t rc = writer.write("sgp4_ephemeris.e", sgp4);
        std::cout << "SGP4 星历 -> sgp4_ephemeris.e (rc=" << rc << ")\n";

        STKEphemerisFileParser parser;
        HEphemeris eph;
        parser.parse("sgp4_ephemeris.e", eph);
        if(auto* lag = dynamic_cast<EphemerisLagrangeVar*>(eph.get()))
            std::cout << "  回读点数 = " << lag->size() << "\n";
    }

    // ============ 2) 二体星历：由初始状态构造并保存 ============
    {
        const double radius = kEarthRadius + 35786.0 * 1000.0;   // 地球同步轨道半径 (m)
        const double speed  = std::sqrt(kEarthGrav / radius);    // 圆轨道速度 (m/s)
        CartState initState{Vector3d{radius, 0.0, 0.0}, Vector3d{0.0, speed, 0.0}};

        HFrame j2000 = aGetEarth()->getFrameJ2000();   // 参考系：地球 J2000
        EphemerisTwoBody twoBody(j2000.get(), kEarthGrav, epoch, initState);

        STKEphemerisFileWriter writer;
        writer.setInterval(window);
        writer.setCoordinate(STKEphemerisFileWriter::eJ2000);
        errc_t rc = writer.write("twobody_ephemeris.e", twoBody);
        std::cout << "二体星历 -> twobody_ephemeris.e (rc=" << rc << ")\n";

        STKEphemerisFileParser parser;
        HEphemeris eph;
        parser.parse("twobody_ephemeris.e", eph);
        if(auto* lag = dynamic_cast<EphemerisLagrangeVar*>(eph.get()))
            std::cout << "  回读点数 = " << lag->size() << "\n";
    }

    std::cout << "示例运行完毕。\n";
    return 0;
}
