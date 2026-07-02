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

#include "ast/FrameTransform.hpp"
#include "ast/TimePoint.hpp"
#include "ast/DateTime.hpp"
#include "ast/RunTime.hpp"
#include "ast/Vector.hpp"
#include "ast/Rotation.hpp"
#include "ast/Literals.hpp"
#include "ast/Test.hpp"
#include "ast/CelestialBody.hpp"


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
        errc_t rc = aICRFToMoonPrincipalAxesTransform(tp, rotation);
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
        errc_t rc = aICRFToMoonPrincipalAxesTransform(tp, rotation);
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
        errc_t rc = aICRFToMoonMeanEarthTransform(tp, rotation);
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


TEST_F(MoonFrameTest, MoonICRFToFixed)
{
    auto moon = aGetMoon();
    ASSERT_TRUE(moon != nullptr);
    auto icrf = aAxesICRF();
    auto inertial = moon->getAxesInertial();
    auto fixed = moon->getAxesFixed();
    // test ICRF to Inertial
    {
        TimePoint tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        icrf->getTransformTo(inertial, tp, rotation);
        Vector3d posICRF{2000_km, 3000_km, 4000_km};
        Vector3d posInertial;
        rotation.transformVector(posICRF, posInertial);
        printf("posInertial: %.15f, %.15f, %.15f\n", posInertial[0], posInertial[1], posInertial[2]);
        Vector3d posInertialExpected{1832.5467826059721119_km, 4478.5059101219530930_km, 2363.2090687375125526_km};
        EXPECT_NEAR(posInertial[0], posInertialExpected[0], 1e-8);
        EXPECT_NEAR(posInertial[1], posInertialExpected[1], 1e-8);
        EXPECT_NEAR(posInertial[2], posInertialExpected[2], 1e-9);
    }
    // test ICRF to Fixed
    {
        TimePoint tp = TimePoint::FromUTC(2026, 6, 9, 0, 0, 0);
        KinematicRotation rotation;
        icrf->getTransformTo(fixed, tp, rotation);
        Vector3d posICRF{2000_km, 3000_km, 4000_km};
        Vector3d velICRF{100_m/s, 200_m/s, 300_m/s};
        Vector3d posFixed;
        Vector3d velFixed;
        rotation.transformVectorVelocity(posICRF, velICRF, posFixed, velFixed);
        printf("posFixed: %.15f, %.15f, %.15f\n", posFixed[0], posFixed[1], posFixed[2]);
        printf("velFixed: %.15f, %.15f, %.15f\n", velFixed[0], velFixed[1], velFixed[2]);
        
        Vector3d posFixedExpected{-2168.4231068207191129_km, -4185.4503133240195893_km, 2603.8331176367364606_km};
        Vector3d velFixedExpected{-122.6377326825566172_m/s, -287.3392504434122543_m/s, 204.0934756090281894_m/s};
        
        EXPECT_NEAR(posFixed[0], posFixedExpected[0], 1e-8);
        EXPECT_NEAR(posFixed[1], posFixedExpected[1], 1e-8);
        EXPECT_NEAR(posFixed[2], posFixedExpected[2], 1e-9);

        EXPECT_NEAR(velFixed[0], velFixedExpected[0], 1e-8);
        EXPECT_NEAR(velFixed[1], velFixedExpected[1], 1e-8);
        EXPECT_NEAR(velFixed[2], velFixedExpected[2], 1e-9);
    }
}


TEST_F(MoonFrameTest, MoonPrincipalAxes)
{
    auto moon = aGetMoon();
    ASSERT_TRUE(moon != nullptr);
    auto inertial = moon->getAxesInertial();
    auto pa403 = moon->getAxes("PrincipalAxes_403");
    auto pa418 = moon->getAxes("PrincipalAxes_418");
    auto pa = moon->getAxes("PrincipalAxes");
    ASSERT_TRUE(inertial != nullptr);
    ASSERT_TRUE(pa403 != nullptr);
    ASSERT_TRUE(pa418 != nullptr);
    ASSERT_TRUE(pa != nullptr);
    // test Inertial to PA403
    {
        auto tp = "2026-06-09 00:00:00"_utc;
        Vector3d posMoonInertial{2000_km, 3000_km, 4000_km};
        Vector3d velMoonInertial{100_m/s, 200_m/s, 300_m/s};
        Vector3d posMoonPA403;
        Vector3d velMoonPA403;
        KinematicRotation rotation;
        inertial->getTransformTo(pa403, tp, rotation);
        rotation.transformVectorVelocity(posMoonInertial, velMoonInertial, posMoonPA403, velMoonPA403);
        Vector3d posExpected{-2136.6079165018709318_km, -2635.7972644377359757_km, 4181.8033660042037809_km};
        Vector3d velExpected{-115.1143331527156448_m/s, -172.0341467514254816_m/s, 311.0138275890610657_m/s};
        printf("posMoonPA403: %.15f, %.15f, %.15f\n", posMoonPA403[0], posMoonPA403[1], posMoonPA403[2]);
        printf("velMoonPA403: %.15f, %.15f, %.15f\n", velMoonPA403[0], velMoonPA403[1], velMoonPA403[2]);

        EXPECT_NEAR(posMoonPA403[0], posExpected[0], 1e-8);
        EXPECT_NEAR(posMoonPA403[1], posExpected[1], 1e-8);
        EXPECT_NEAR(posMoonPA403[2], posExpected[2], 1e-9);

        EXPECT_NEAR(velMoonPA403[0], velExpected[0], 1e-8);
        EXPECT_NEAR(velMoonPA403[1], velExpected[1], 1e-8);
        EXPECT_NEAR(velMoonPA403[2], velExpected[2], 1e-9);
    }
    // test Inertial to PA
    {
        auto tp = "2026-06-09 00:00:00"_utc;
        Vector3d posMoonInertial{2000_km, 3000_km, 4000_km};
        Vector3d velMoonInertial{100_m/s, 200_m/s, 300_m/s};
        Vector3d posMoonPA;
        Vector3d velMoonPA;
        KinematicRotation rotation;
        inertial->getTransformTo(pa, tp, rotation);
        rotation.transformVectorVelocity(posMoonInertial, velMoonInertial, posMoonPA, velMoonPA);
        Vector3d posExpected{-2136.6447978866440280_km, -2635.7563912223972693_km, 4181.8102842900734686_km};
        Vector3d velExpected{-115.1166486312233417_m/s, -172.0318870059388132_m/s, 311.0142205082832447_m/s};
        printf("posMoonPA: %.15f, %.15f, %.15f\n", posMoonPA[0], posMoonPA[1], posMoonPA[2]);
        printf("velMoonPA: %.15f, %.15f, %.15f\n", velMoonPA[0], velMoonPA[1], velMoonPA[2]);

        EXPECT_NEAR(posMoonPA[0], posExpected[0], 1e-8);
        EXPECT_NEAR(posMoonPA[1], posExpected[1], 1e-8);
        EXPECT_NEAR(posMoonPA[2], posExpected[2], 1e-9);

        EXPECT_NEAR(velMoonPA[0], velExpected[0], 1e-8);
        EXPECT_NEAR(velMoonPA[1], velExpected[1], 1e-8);
        EXPECT_NEAR(velMoonPA[2], velExpected[2], 1e-9);
    }
}

GTEST_MAIN()
