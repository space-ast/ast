///
/// @file      testSolarSystem.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-23
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

#include "AstCore/CelestialBody.hpp"
#include "AstCore/SolarSystem.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstTest/Test.hpp"


AST_USING_NAMESPACE


TEST(SolarSystem, load)
{
    SolarSystem system;
    err_t rc = system.load(aDataDirGet() + "/SolarSystem");
    EXPECT_EQ(rc, 0);
}

TEST(SolarSystem, getBody)
{
    SolarSystem system;
    err_t rc = system.load(aDataDirGet() + "/SolarSystem");
    EXPECT_EQ(rc, 0);
    auto ariel = system.getBody("Ariel");
    EXPECT_NE(ariel, nullptr);
    auto earth = system.getBody("Earth");
    EXPECT_NE(earth, nullptr);
}

TEST(CelestialBody, loadEarth)
{
    CelestialBody earth;
    err_t rc = earth.load(aDataDirGet() + "/SolarSystem/Earth/Earth.cb");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(earth.getName(), "Earth");

}


TEST(CelestialBody, bodyAxes)
{
    aInitialize();
    CelestialBody* earth = aGetEarth();
    {
        auto ecf = earth->getAxesFixed();
        auto j2000 = aAxesJ2000();
        KinematicRotation rot1, rot2;
        auto tp = TimePoint::FromUTC(2022, 3, 8, 3, 11, 3);
        aAxesTransform(j2000, ecf, tp, rot1);
        aJ2000ToECFTransform(tp, rot2);
        auto mtx1 = rot1.getMatrix();
        auto mtx2 = rot2.getMatrix();
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                EXPECT_NEAR(mtx1(i, j), mtx2(i, j), 1e-15);
            }
            EXPECT_NEAR(rot1.getRotationRate()[i], rot2.getRotationRate()[i], 1e-15);
        }
    }
    {
        auto ecf = earth->getAxesFixed();
        auto icrf = aAxesICRF();
        KinematicRotation rot1, rot2;
        auto tp = TimePoint::FromUTC(2022, 3, 8, 3, 11, 3);
        aAxesTransform(icrf, ecf, tp, rot1);
        aICRFToECFTransform(tp, rot2);
        auto mtx1 = rot1.getMatrix();
        auto mtx2 = rot2.getMatrix();
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                EXPECT_NEAR(mtx1(i, j), mtx2(i, j), 1e-15);
            }
            EXPECT_DOUBLE_EQ(rot1.getRotationRate()[i], rot2.getRotationRate()[i]);
        }
    }
    {
        auto mod1 = earth->getAxesMOD();
        auto mod2 = aAxesMOD();
        auto icrf = aAxesICRF();
        KinematicRotation rot1, rot2;
        auto tp = TimePoint::FromUTC(2022, 3, 8, 3, 11, 3);
        aAxesTransform(icrf, mod1, tp, rot1);
        aAxesTransform(icrf, mod2, tp, rot2);
        auto mtx1 = rot1.getMatrix();
        auto mtx2 = rot2.getMatrix();
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                EXPECT_NEAR(mtx1(i, j), mtx2(i, j), 1e-15);
            }
            EXPECT_DOUBLE_EQ(rot1.getRotationRate()[i], rot2.getRotationRate()[i]);
        }
    }
}


TEST(CelestialBody, getJn)
{
    CelestialBody earth;
    err_t rc = earth.load(aDataDirGet() + "/SolarSystem/Earth/Earth.cb");
    EXPECT_EQ(rc, 0);
    // EXPECT_EQ(earth.getJn(0), 0.0);
    double j2 = earth.getJ2();
    printf("J2 = %.15g\n", j2);
    EXPECT_NEAR(j2, 0.0010826261738522, 1e-15);
    double j3 = earth.getJ3();
    printf("J3 = %.15g\n", j3);
    EXPECT_NEAR(j3, -0.0000025324105186, 1e-15);
    double j4 = earth.getJ4();
    printf("J4 = %.15g\n", j4);
    EXPECT_NEAR(j4, -0.0000016198975999, 1e-15);
    double j6 = earth.getJ6();
    printf("J6 = %.15g\n", j6);
    EXPECT_NE(j6, 0);
}

GTEST_MAIN();

