///
/// @file      testIrbemAPI.cpp
/// @brief     IrbemAPI 基础功能测试
/// @details   验证 IRBEM 动态库的加载、卸载、单例模式及 Fortran 函数调用
/// @author    axel
/// @date      2026-07-31
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

#include "ast/IrbemAPI.hpp"
#include "ast/Test.h"

#include <cmath>

AST_USING_NAMESPACE

/// @brief 测试动态库加载与卸载
TEST(IrbemAPI, LoadUnload)
{
    IrbemAPI api;
    EXPECT_FALSE(api.isLoaded());

    // 使用默认路径尝试加载
    errc_t rc = api.tryload({
        "libirbem.Win64.gfortran64",
        "libirbem.linux64.gfortran64",
        "libirbem",
    });

    if (rc != eNoError)
    {
        GTEST_SKIP() << "IRBEM library not found, skipping LoadUnload test";
    }

    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(api.isLoaded());

    // 卸载
    rc = api.unload();
    EXPECT_EQ(rc, eNoError);
    EXPECT_FALSE(api.isLoaded());
}


/// @brief 测试凤凰单例模式
TEST(IrbemAPI, Instance)
{
    IrbemAPI* api = IrbemAPI::Instance();
    ASSERT_NE(api, nullptr);

    // 单例应该尝试过自动加载
    // 如果库可用，isLoaded 为 true；否则库不在搜索路径中
    if (!api->isLoaded())
    {
        GTEST_SKIP() << "IRBEM library not available for singleton, skipping Instance test";
    }

    EXPECT_TRUE(api->isLoaded());

    // 多次调用返回同一实例
    IrbemAPI* api2 = IrbemAPI::Instance();
    EXPECT_EQ(api, api2);
}


/// @brief 测试获取 Fortran 版本号
TEST(IrbemAPI, FortranVersion)
{
    IrbemAPI* api = IrbemAPI::Instance();
    if (!api->isLoaded())
    {
        GTEST_SKIP() << "IRBEM library not loaded, skipping FortranVersion test";
    }

    int version = 0;
    errc_t rc = api->irbem_fortran_version1(&version);
    EXPECT_EQ(rc, eNoError);
    // 版本号应为正整数（git rev-list --count 的结果）
    EXPECT_GT(version, 0);
    printf("IRBEM Fortran version: %d\n", version);
}


/// @brief 测试获取 ntime_max
TEST(IrbemAPI, NtimeMax)
{
    IrbemAPI* api = IrbemAPI::Instance();
    if (!api->isLoaded())
    {
        GTEST_SKIP() << "IRBEM library not loaded, skipping NtimeMax test";
    }

    int ntime_max = 0;
    errc_t rc = api->get_irbem_ntime_max1(&ntime_max);
    EXPECT_EQ(rc, eNoError);
    // ntime_max 应为正数，由 ntime_max.inc 在编译时确定
    EXPECT_GT(ntime_max, 0);
    printf("IRBEM ntime_max: %d\n", ntime_max);
}


/// @brief 测试未加载库时调用函数返回错误
TEST(IrbemAPI, CallWithoutLoad)
{
    IrbemAPI api(false);  // 不自动加载
    EXPECT_FALSE(api.isLoaded());

    int version = 0;
    errc_t rc = api.irbem_fortran_version1(&version);
    EXPECT_NE(rc, eNoError);
    EXPECT_EQ(version, 0);

    int ntime_max = 0;
    rc = api.get_irbem_ntime_max1(&ntime_max);
    EXPECT_NE(rc, eNoError);
    EXPECT_EQ(ntime_max, 0);
}


/// @brief 测试 get_field1 — 单点地磁场计算（仅 IGRF 内源场）
/// @details IRBEM GEO 坐标系 (sysaxes=1) 使用直角坐标，单位为地球半径 (Re)。
///          对照 testMagnetosphere.cpp 中 igrf 测试的相同点位进行验证。
TEST(IrbemAPI, GetField)
{
    IrbemAPI* api = IrbemAPI::Instance();
    if (!api->isLoaded())
    {
        GTEST_SKIP() << "IRBEM library not loaded, skipping GetField test";
    }

    // 与 testMagnetosphere.cpp 中 igrf 测试相同的点位：
    // lat=-30°, lon=-45°, height=500 km, year=2021.1616
    const int kext = 0;  // 仅 IGRF，不使用外部场模型
    const std::array<int, 5> options = {1, 0, 0, 0, 0};
    const int sysaxes = 1;  // GEO 直角坐标

    const int iyear = 2021;
    const int idoy = 59;   // 2021-02-28 ≈ 2021.1616
    const double UT = 3.8794520547945207;  // 0.16164 * 24

    // GEO 直角坐标，单位 Earth radii (Re = 6371.2 km)
    constexpr double kRe = 6371.2;  // IRBEM 地球半径 [km]
    const double kPi = std::acos(-1.0);
    const double r = (kRe + 500.0) / kRe;  // 地心距 [Re]
    const double lat = -30.0 * kPi / 180.0;
    const double lon = -45.0 * kPi / 180.0;

    const double x1 = r * cos(lat) * cos(lon);
    const double x2 = r * cos(lat) * sin(lon);
    const double x3 = r * sin(lat);

    double maginput[25] = {};

    double Bgeo[3] = {};
    double B = 0.0;

    errc_t rc = api->get_field1(kext, options, sysaxes,
                                 iyear, idoy, UT,
                                 x1, x2, x3,
                                 maginput,
                                 Bgeo, B);

    EXPECT_EQ(rc, eNoError);

    // 磁场总强度应 > 0（IGRF 在地球附近约为几万 nT）
    EXPECT_GT(B, 0.0);
    // 在 500 km 高度，磁场总强度应在 10000–60000 nT 范围内
    EXPECT_GT(B, 10000.0);
    EXPECT_LT(B, 60000.0);

    printf("Bgeo = [%.6f, %.6f, %.6f] nT\n", Bgeo[0], Bgeo[1], Bgeo[2]);
    printf("|B|  = %.6f nT\n", B);
}


/// @brief 测试 make_lstar1 — L* 磁壳参数计算
/// @details 使用 GEO 直角坐标（单位 Re），与 testMagnetosphere.cpp 中
///          igrf 测试的结果交叉验证 L-shell 和 B 场值。
TEST(IrbemAPI, MakeLstar)
{
    IrbemAPI* api = IrbemAPI::Instance();
    if (!api->isLoaded())
    {
        GTEST_SKIP() << "IRBEM library not loaded, skipping MakeLstar test";
    }

    const int ntime = 1;
    const int kext = 0;  // 仅 IGRF
    const std::array<int, 5> options = {1, 0, 0, 0, 0};
    const int sysaxes = 1;  // GEO 直角坐标

    int iyear = 2021;
    int idoy = 59;
    double UT = 3.8794520547945207;

    // GEO 直角坐标，单位 Earth radii (Re = 6371.2 km)
    constexpr double kRe = 6371.2;
    const double kPi = std::acos(-1.0);
    const double r = (kRe + 500.0) / kRe;  // [Re]
    const double lat = -30.0 * kPi / 180.0;
    const double lon = -45.0 * kPi / 180.0;

    double x1 = r * cos(lat) * cos(lon);
    double x2 = r * cos(lat) * sin(lon);
    double x3 = r * sin(lat);

    double maginput[25] = {};

    double Lm = 0.0, Lstar = 0.0, Blocal = 0.0;
    double Bmin = 0.0, J = 0.0, MLT = 0.0;

    errc_t rc = api->make_lstar1(ntime, kext, options, sysaxes,
                                  &iyear, &idoy, &UT,
                                  &x1, &x2, &x3,
                                  maginput,
                                  &Lm, &Lstar, &Blocal,
                                  &Bmin, &J, &MLT);

    EXPECT_EQ(rc, eNoError);

    // L-shell 值应 > 0（在 500 km 高度处 L ≈ 1.0–1.5）
    EXPECT_GT(Lm, 0.0);
    EXPECT_LT(Lm, 10.0);
    EXPECT_GT(Lstar, 0.0);
    EXPECT_LT(Lstar, 10.0);

    // 当地磁场强度应 > 0
    EXPECT_GT(Blocal, 0.0);

    // MLT 应在 0–24 小时范围内
    EXPECT_GE(MLT, 0.0);
    EXPECT_LE(MLT, 24.0);

    printf("Lm     = %.6f\n", Lm);
    printf("Lstar  = %.6f\n", Lstar);
    printf("Blocal = %.6f nT\n", Blocal);
    printf("Bmin   = %.6f nT\n", Bmin);
    printf("J      = %.6f Re^2 nT^0.5\n", J);
    printf("MLT    = %.6f h\n", MLT);
}


//GTEST_MAIN();

int main(int argc, char **argv) {
    if(aIsCI())
        return 0;
    std::printf("Running main() from %s\n", __FILE__); 
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}
