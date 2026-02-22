///
/// @file      testMoonFrame.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-21
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

#include "AstCore/FrameTransform.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/DateTime.hpp"
#include "AstCore/RunTime.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Rotation.hpp"
#include "AstUtil/Literals.hpp"
#include "AstTest/Test.hpp"


AST_USING_NAMESPACE
using namespace _AST literals;

class MoonFrameTest : public ::testing::Test
{
    void SetUp() override
    {
        aInitialize();
    }
    void TearDown() override
    {
        aUninitialize();
    }
};


TEST_F(MoonFrameTest, ICRFToMoonPrincipalAxesTransform)
{
    {
        TimePoint tp = TimePoint::FromUTC(2026, 2, 22, 0, 0, 0);
        Rotation rotation;
        err_t rc = aICRFToMoonPrincipalAxesTransform(tp, rotation);
        EXPECT_EQ(rc, eNoError);
        Vector3d posICRF{2000_km, 3000_km, 4000_km};
        Vector3d posMoonPA;
        rotation.transformVector(posICRF, posMoonPA);
        printf("posMoonPA: %.15f, %.15f, %.15f\n", posMoonPA[0], posMoonPA[1], posMoonPA[2]);
        Vector3d posMoonPAExpected{-3982.350533525300307_km, -2525.287088059560574_km, 2600.732464328161313_km};
        EXPECT_NEAR(posMoonPA[0], posMoonPAExpected[0], 1e-9);
        EXPECT_NEAR(posMoonPA[1], posMoonPAExpected[1], 1e-9);
        EXPECT_NEAR(posMoonPA[2], posMoonPAExpected[2], 1e-9);
    }
    {
        TimePoint tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        err_t rc = aICRFToMoonPrincipalAxesTransform(tp, rotation);
        EXPECT_EQ(rc, eNoError);
        Vector3d posICRF{2000_km, 3000_km, 4000_km};
        Vector3d posMoonPA;
        rotation.transformVector(posICRF, posMoonPA);
        printf("posMoonPA: %.15f, %.15f, %.15f\n", posMoonPA[0], posMoonPA[1], posMoonPA[2]);
        Vector3d posMoonPAExpected{-4325.631221375571840_km, -1877.545978205365827_km, 2600.718292392648891_km};
        EXPECT_NEAR(posMoonPA[0], posMoonPAExpected[0], 1e-9);
        EXPECT_NEAR(posMoonPA[1], posMoonPAExpected[1], 1e-9);
        EXPECT_NEAR(posMoonPA[2], posMoonPAExpected[2], 1e-9);
    }
}


TEST_F(MoonFrameTest, MoonPAToMeanEarthTransform)
{
    {
        Rotation rotation;
        aMoonPA430ToMeanEarthTransform(rotation);
        Vector3d posMoonPA430{2000_km, 3000_km, 4000_km};
        Vector3d posMeanEarth;
        rotation.transformVector(posMoonPA430, posMeanEarth);
        printf("posMeanEarth: %.15f, %.15f, %.15f\n", posMeanEarth[0], posMeanEarth[1], posMeanEarth[2]);
        Vector3d posMeanEarthExpected{2000.540804508937754_km, 3000.649519473851342_km, 3999.242297082795631_km};
        EXPECT_NEAR(posMeanEarth[0], posMeanEarthExpected[0], 1e-9);
        EXPECT_NEAR(posMeanEarth[1], posMeanEarthExpected[1], 1e-9);
        EXPECT_NEAR(posMeanEarth[2], posMeanEarthExpected[2], 1e-9);
    }
}

TEST_F(MoonFrameTest, ICRFToMoonMeanEarthTransform)
{
    {
        TimePoint tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        err_t rc = aICRFToMoonMeanEarthTransform(tp, rotation);
        EXPECT_EQ(rc, eNoError);
        Vector3d posICRF{2000_km, 3000_km, 4000_km};
        Vector3d posMeanEarth;
        rotation.transformVector(posICRF, posMeanEarth);
        printf("posMeanEarth: %.15f, %.15f, %.15f\n", posMeanEarth[0], posMeanEarth[1], posMeanEarth[2]);
        Vector3d posMeanEarthExpected{-4324.024798676777209_km, -1878.966563572146697_km, 2602.363193984675036_km};
        EXPECT_NEAR(posMeanEarth[0], posMeanEarthExpected[0], 1e-9);
        EXPECT_NEAR(posMeanEarth[1], posMeanEarthExpected[1], 1e-9);
        EXPECT_NEAR(posMeanEarth[2], posMeanEarthExpected[2], 1e-9);
    }
}



GTEST_MAIN()