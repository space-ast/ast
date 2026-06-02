///
/// @file      testSegmentCalculation.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-02
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
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


#include "ast/LandingSiteCalcSunElevation.hpp"
#include "ast/LandingSiteCalcSunAzimuth.hpp"
#include "ast/LandingSite.hpp"
#include "ast/Test.hpp"
#include "ast/RunTime.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Literals.hpp"

AST_USING_NAMESPACE

TEST(SegmentCalculationTest, LandingSiteCalcSunElevation)
{
    using namespace math;
    auto moon = aGetMoon();
    {
        LandingSite landingSite;
        landingSite.setBody(moon);
        landingSite.setPosition({0, 0, 0});
        landingSite.setEpoch(TimePoint::FromUTC(2025, 1, 1, 0, 0, 0.0));
        LandingSiteCalcSunElevation calcElevation;
        LandingSiteCalcSunAzimuth calcAzimuth;

        double elevation;
        double azimuth;
        errc_t rc;
        rc = calcElevation.calculate(landingSite, elevation);
        EXPECT_EQ(rc, 0);
        rc = calcAzimuth.calculate(landingSite, azimuth);
        EXPECT_EQ(rc, 0);
        EXPECT_NEAR(elevation, -72.132035_deg, 1e-6);
        EXPECT_NEAR(azimuth, 94.9608645034_deg, 1e-6);
        printf("elevation: %.10f rad (%.10f deg)\n", elevation, rad2deg(elevation));
        printf("azimuth:   %.10f rad (%.10f deg)\n", azimuth, rad2deg(azimuth));
    }
    {
        LandingSite landingSite;
        landingSite.setBody(moon);
        //landingSite.setPosition({12_deg, 34_deg, 0});
        landingSite.setPosition({12_deg, 34_deg, 0});
        landingSite.setEpoch(TimePoint::FromUTC(2026, 1, 2, 3, 4, 0.0));
        LandingSiteCalcSunElevation calcElevation;
        LandingSiteCalcSunAzimuth calcAzimuth;

        double elevation;
        double azimuth;
        errc_t rc;
        rc = calcElevation.calculate(landingSite, elevation);
        EXPECT_EQ(rc, 0);
        rc = calcAzimuth.calculate(landingSite, azimuth);
        EXPECT_EQ(rc, 0);
        EXPECT_NEAR(elevation, 69.897727_deg, 1e-6);
        EXPECT_NEAR(azimuth, -130.518514_deg, 1e-6);
        printf("elevation: %.10f rad (%.10f deg)\n", elevation, rad2deg(elevation));
        printf("azimuth:   %.10f rad (%.10f deg)\n", azimuth, rad2deg(azimuth));
    }
}

GTEST_MAIN()


