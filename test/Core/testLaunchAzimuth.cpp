///
/// @file      testLaunchAzimuth.cpp
/// @brief     发射方位角（射向）计算测试
/// @details   校验 aInclinationToLaunchAzimuth() 等接口满足 cos(i) = cos(lat) * sin(az)。
///
/// @author    axel
/// @date      2026-09-17
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/Launch.hpp"
#include "ast/Literals.hpp"
#include "ast/Test.hpp"
#include "ast/Posix.hpp"
#include <cmath>

AST_USING_NAMESPACE

namespace {

/// 30° 纬度发射 60° 倾角轨道的发射方位角，asin(cos60° / cos30°) = asin(1/√3) [rad]
const double kAzNorth = 35.264389682754654_deg;
/// 与 kAzNorth 对称的南向发射方位角 [rad]
const double kAzSouth = 144.73561031724535_deg;

}  // namespace

TEST(LaunchAzimuth, NorthAndSouthSolutions)
{
    double azNorth{};
    double azSouth{};
    errc_t err = aInclinationToLaunchAzimuths(60.0_deg, 30.0_deg, azNorth, azSouth);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(azNorth, kAzNorth, 1e-12);
    EXPECT_NEAR(azSouth, kAzSouth, 1e-12);

    // 向北发射的解位于正北两侧，向南发射的解为其关于正东方向的镜像
    EXPECT_NEAR(azNorth + azSouth, 180.0_deg, 1e-12);
}

TEST(LaunchAzimuth, RetrogradeInclination)
{
    // 120° 倾角轨道：cos(120°) 为负，两个解都偏向西侧
    double azNorth{};
    double azSouth{};
    errc_t err = aInclinationToLaunchAzimuths(120.0_deg, 30.0_deg, azNorth, azSouth);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(azNorth, 360.0_deg - kAzNorth, 1e-12);
    EXPECT_NEAR(azSouth, 180.0_deg + kAzNorth, 1e-12);
}

TEST(LaunchAzimuth, BoundaryInclination)
{
    // 入轨倾角等于发射点纬度时只能沿正东发射（地球自转带来的最小倾角）
    double azNorth{};
    double azSouth{};
    errc_t err = aInclinationToLaunchAzimuths(19.6_deg, 19.6_deg, azNorth, azSouth);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(azNorth, 90.0_deg, 1e-12);
    EXPECT_NEAR(azSouth, 90.0_deg, 1e-12);   // 两个解重合

    // 任意纬度发射极轨：正北或正南发射
    err = aInclinationToLaunchAzimuths(90.0_deg, 45.0_deg, azNorth, azSouth);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(azNorth, 0.0_deg, 1e-12);
    EXPECT_NEAR(azSouth, 180.0_deg, 1e-12);

    // 赤道发射场：0° 倾角正东发射，180° 倾角正西发射
    err = aInclinationToLaunchAzimuths(0.0_deg, 0.0_deg, azNorth, azSouth);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(azNorth, 90.0_deg, 1e-12);
    EXPECT_NEAR(azSouth, 90.0_deg, 1e-12);

    err = aInclinationToLaunchAzimuths(180.0_deg, 0.0_deg, azNorth, azSouth);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(azNorth, 270.0_deg, 1e-12);
    EXPECT_NEAR(azSouth, 270.0_deg, 1e-12);
}

TEST(LaunchAzimuth, UnreachableInclination)
{
    // 目标倾角低于发射点纬度，无法直接入轨
    double azNorth{};
    double azSouth{};
    EXPECT_EQ(aInclinationToLaunchAzimuths(10.0_deg, 19.6_deg, azNorth, azSouth), eErrorOutOfRange);

    // 参数非法
    EXPECT_EQ(aInclinationToLaunchAzimuths(-1.0_deg, 30.0_deg, azNorth, azSouth), eErrorInvalidParam);
    EXPECT_EQ(aInclinationToLaunchAzimuths(60.0_deg, 100.0_deg, azNorth, azSouth), eErrorInvalidParam);
}

TEST(LaunchAzimuth, AzimuthToInclination)
{
    // 逆运算：由方位角反解倾角
    EXPECT_NEAR(aLaunchAzimuthToInclination(kAzNorth, 30.0_deg), 60.0_deg, 1e-12);
    EXPECT_NEAR(aLaunchAzimuthToInclination(kAzSouth, 30.0_deg), 60.0_deg, 1e-12);
    EXPECT_NEAR(aLaunchAzimuthToInclination(90.0_deg, 19.6_deg), 19.6_deg, 1e-12);
    EXPECT_NEAR(aLaunchAzimuthToInclination(0.0_deg, 45.0_deg), 90.0_deg, 1e-12);

    // 纬度非法
    EXPECT_TRUE(std::isnan(aLaunchAzimuthToInclination(90.0_deg, 100.0_deg)));
}

TEST(LaunchAzimuth, RoundTrip)
{
    const double azimuths[] = {0.0, 35.0, 89.0, 120.0, 179.0, -170.0, -90.0};
    const double latitudes[] = {-80.0, -45.0, -10.0, 0.0, 19.6, 30.0, 41.0, 80.0};

    for (double azimuthDeg : azimuths) {
        for (double latitudeDeg : latitudes) {
            const double azimuth = azimuthDeg * kDegToRad;
            const double latitude = latitudeDeg * kDegToRad;
            const double inclination = aLaunchAzimuthToInclination(azimuth, latitude);
            // 由该倾角反解出的两个射向应与输入的方位角同属一个解（正弦值相同）
            double azNorth{};
            double azSouth{};
            const errc_t err = aInclinationToLaunchAzimuths(inclination, latitude, azNorth, azSouth);
            EXPECT_EQ(err, eNoError) << "azimuth " << azimuthDeg << "°, latitude " << latitudeDeg << "°";
            EXPECT_NEAR(std::sin(azNorth), std::sin(azimuth), 1e-12)
                << "azimuth " << azimuthDeg << "°, latitude " << latitudeDeg << "°";
            EXPECT_NEAR(std::sin(azSouth), std::sin(azimuth), 1e-12)
                << "azimuth " << azimuthDeg << "°, latitude " << latitudeDeg << "°";
        }
    }
}

TEST(LaunchAzimuth, Hainan)
{
    // 海南文昌发射场
    {
        double inclination = 20_deg;
        double latitude = 19.6_deg;

        double azNorth{};
        double azSouth{};
        errc_t err = aInclinationToLaunchAzimuths(inclination, latitude, azNorth, azSouth);
        EXPECT_EQ(err, eNoError);
        posix::printf("azNorth: %f°, azSouth: %f°\n", azNorth * kRadToDeg, azSouth * kRadToDeg);
        EXPECT_NEAR(azSouth, 94_deg, 0.1_deg);
        EXPECT_NEAR(azNorth, 86_deg, 0.1_deg);
    }
}

GTEST_MAIN()
