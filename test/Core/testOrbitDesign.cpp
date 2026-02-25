///
/// @file      testOrbitDesigner.cpp
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

#include "AstCore/OrbitDesigner.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/Literals.hpp"
#include "AstTest/Test.hpp"

AST_USING_NAMESPACE
using namespace _AST literals;

class OrbitDesignTest : public ::testing::Test
{
protected:
    void SetUp() override {
        aInitialize();
    }

    void TearDown() override {
        aUninitialize();
    }
};


TEST_F(OrbitDesignTest, SimpleOrbitDesigner)
{
    {
        SimpleOrbitDesigner designer(aGetEarth());
        designer.setSemimajorAxis(6678137);
        designer.setInclination(28.5_deg);
        designer.setRightAscensionOfAscendingNode(12_deg);
        designer.setArgumentOfPeriapsis(30_deg);
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);

        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_EQ(orbElem.getA(), 6678137);
        EXPECT_EQ(orbElem.getE(), 0.);
        EXPECT_EQ(orbElem.getI(), 28.5_deg);
        EXPECT_EQ(orbElem.getRAAN(), 12.0_deg);
        EXPECT_EQ(orbElem.getArgPer(), 30.0_deg);
        EXPECT_EQ(orbElem.getTrueA(), 0.0);
    }
    {
        SimpleOrbitDesigner designer(aGetMoon());
        designer.setSemimajorAxis(3474.800000_km);
        designer.setInclination(45.0_deg);
        designer.setRightAscensionOfAscendingNode(12.0_deg);
        designer.setArgumentOfPeriapsis(30.0_deg);
        OrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_EQ(orbElem.getA(), 3474.800000_km);
        EXPECT_EQ(orbElem.getE(), 0.0);
        EXPECT_EQ(orbElem.getI(), 45.0_deg);
        EXPECT_EQ(orbElem.getRAAN(), 12.0_deg);
        EXPECT_EQ(orbElem.getArgPer(), 30.0_deg);
        EXPECT_EQ(orbElem.getTrueA(), 0.0);
    }
}


TEST_F(OrbitDesignTest, CriticallyInclinedOrbitDesigner)
{
    {
        CriticallyInclinedOrbitDesigner designer(aGetEarth());
        designer.setDirection(CriticallyInclinedOrbitDesigner::ePosigrade);
        designer.setApogeeAltitude(12000_km);
        designer.setPerigeeAltitude(400_km);
        designer.setRAAN(-100_deg);

        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_DOUBLE_EQ(orbElem.getA(), 12578.137_km);
        EXPECT_NEAR(orbElem.getE(), 0.46111757249901197, 1e-12);
        EXPECT_NEAR(orbElem.getI(), 63.4349488226876375_deg, 1e-11);
        EXPECT_EQ(orbElem.getRAAN(), -100.0_deg);
        EXPECT_EQ(orbElem.getArgPer(), 0.0_deg);
        EXPECT_EQ(orbElem.getTrueA(), 0.0);
    }
    {
        CriticallyInclinedOrbitDesigner designer(aGetEarth());
        designer.setDirection(CriticallyInclinedOrbitDesigner::eRetrograde);
        designer.setApogeeAltitude(12000_km);
        designer.setPerigeeAltitude(400_km);
        designer.setRAAN(-100_deg);

        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_DOUBLE_EQ(orbElem.getA(), 12578.137_km);
        EXPECT_NEAR(orbElem.getE(), 0.46111757249901197, 1e-12);
        EXPECT_NEAR(orbElem.getI(), 116.5650511767512540_deg, 1e-11);
        EXPECT_EQ(orbElem.getRAAN(), -100.0_deg);
        EXPECT_EQ(orbElem.getArgPer(), 0.0_deg);
        EXPECT_EQ(orbElem.getTrueA(), 0.0);
    }
}

TEST_F(OrbitDesignTest, CriticallyInclinedSunSyncOrbitDesigner)
{
    {
        CriticallyInclinedSunSyncOrbitDesigner designer(aGetEarth());
        designer.setPerigeeAltitude(400_km);
        designer.setRAAN(-100_deg);

        OrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getA(), 10640.1309854711416847_km, 600);
        EXPECT_NEAR(orbElem.getE(), 0.362965, 1e-4);
        EXPECT_NEAR(orbElem.getI(), 116.5650511767512540_deg, 1e-11);
    }
    {
        CriticallyInclinedSunSyncOrbitDesigner designer(aGetEarth());
        designer.setPerigeeAltitude(2462_km);
        designer.setRAAN(-100_deg);
        OrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getA(), 9877.3708317850232561_km, 500);
        EXPECT_NEAR(orbElem.getE(), 0.1050111258805068, 1e-4);
        EXPECT_NEAR(orbElem.getI(), 116.5650511767512540_deg, 1e-11);
    }
}


GTEST_MAIN();

