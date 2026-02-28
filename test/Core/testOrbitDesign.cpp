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
#include "AstCore/CelestialBody.hpp"
#include "AstUtil/Literals.hpp"
#include "AstTest/Test.hpp"

AST_USING_NAMESPACE
using namespace _AST literals;

class OrbitDesignTest : public ::testing::Test
{
protected:
    void SetUp() override {
        aInitialize();
        auto earth = aGetEarth();
        err_t rc = earth->setGravityModel("WGS84");
        printf("rc: %d\n", rc);
    }

    void TearDown() override {
        aUninitialize();
    }
};


TEST_F(OrbitDesignTest, SimpleOrbitDesigner)  // OK
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


TEST_F(OrbitDesignTest, CriticallyInclinedOrbitDesigner) // OK
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

TEST_F(OrbitDesignTest, CriticallyInclinedSunSyncOrbitDesigner)  // OK
{
    {
        CriticallyInclinedSunSyncOrbitDesigner designer(aGetEarth());
        designer.setPerigeeAltitude(400_km);
        designer.setRAAN(-100_deg);

        OrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getA(), 10640.1451595068047027_km, 2e-2_m);
        EXPECT_NEAR(orbElem.getE(), 0.3629657398100590, 1e-8);
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
        EXPECT_NEAR(orbElem.getA(), 9877.3811004795061308_km, 2e-2_m);
        EXPECT_NEAR(orbElem.getE(), 0.1050120563262616, 1e-8);
        EXPECT_NEAR(orbElem.getI(), 116.5650511767512540_deg, 1e-11);
    }
}


TEST_F(OrbitDesignTest, SunSynchronousOrbitDesigner) // OK
{
    {
        SunSynchronousOrbitDesigner designer(aGetEarth());
        designer.setGeometryDefinition(SunSynchronousOrbitDesigner::eAltitude);
        designer.setAltitude(400_km);
        
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_EQ(orbElem.getA(), designer.getBodyRadius() + 400_km);
        EXPECT_EQ(orbElem.getE(), 0.);
        EXPECT_NEAR(orbElem.getI(), 97.0345971612483993_deg, 1e-4_deg);
    }
    {
        SunSynchronousOrbitDesigner designer(aGetEarth());
        designer.setGeometryDefinition(SunSynchronousOrbitDesigner::eAltitude);
        designer.setAltitude(500_km);
        
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getI(), 97.4064372289805931_deg, 1e-4_deg);
    }
    {
        SunSynchronousOrbitDesigner designer(aGetEarth());
        designer.setGeometryDefinition(SunSynchronousOrbitDesigner::eAltitude);
        designer.setAltitude(4027_km);
        
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getI(), 123.2679770859048318_deg, 1e-7_deg);
    }
    {
        SunSynchronousOrbitDesigner designer(aGetEarth());
        designer.setGeometryDefinition(SunSynchronousOrbitDesigner::eAltitude);
        designer.setAltitude(5974_km);
        
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getI(), 178.0565781247555606_deg, 1e-6_deg);
    }
    
    {
        SunSynchronousOrbitDesigner designer(aGetEarth());
        designer.setGeometryDefinition(SunSynchronousOrbitDesigner::eInclination);
        designer.setInclination(95.6813_deg);
        
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getA(), 6378.1448599859231763_km, 1e-3_m);
    }
    {
        SunSynchronousOrbitDesigner designer(aGetEarth());
        designer.setGeometryDefinition(SunSynchronousOrbitDesigner::eInclination);
        designer.setInclination(97.0346_deg);
        
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getA(), 6778.1370000012748278_km, 1_m);
    }
    {
        SunSynchronousOrbitDesigner designer(aGetEarth());
        designer.setGeometryDefinition(SunSynchronousOrbitDesigner::eInclination);
        designer.setInclination(103_deg);
        
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getA(), 8064.7090264894568463_km, 1e-3_m);
    }
    {
        SunSynchronousOrbitDesigner designer(aGetEarth());
        designer.setGeometryDefinition(SunSynchronousOrbitDesigner::eInclination);
        designer.setInclination(166_deg);
        
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getA(), 12248.0803516784781095_km, 1e-2_m);
    }
    {
        SunSynchronousOrbitDesigner designer(aGetEarth());
        double altitude0 = 600_km;
        printf("altitude0: %.15g\n", altitude0);
        designer.setAltitude(altitude0);
        double inclination = designer.getInclination();
        designer.setInclination(inclination);
        double altitude = designer.getAltitude();
        printf("altitude: %.15g\n", altitude);
        EXPECT_NEAR(altitude, altitude0, 1e-5);
    }
}

TEST_F(OrbitDesignTest, StationaryOrbitDesigner) // OK 
{
    {
        StationaryOrbitDesigner designer(aGetEarth());
        designer.setInclination(0_deg);
        designer.setSubsatellitePoint(-100_deg);
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getA(), 42166.258669178736_km, 0.5_m);
    }
    {
        StationaryOrbitDesigner designer(aGetEarth());
        designer.setInclination(45_deg);
        designer.setSubsatellitePoint(100_deg);
        ModOrbElem orbElem;
        err_t rc = designer.getOrbitState(orbElem);
        EXPECT_EQ(rc, eNoError);
        printf("orbElem: %s\n", orbElem.toString().c_str());
        EXPECT_NEAR(orbElem.getA(), 42166.258669178736_km, 0.5_m);
        EXPECT_EQ(orbElem.getI(), 45_deg);
    }
}


GTEST_MAIN();

