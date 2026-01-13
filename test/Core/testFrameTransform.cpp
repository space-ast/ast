///
/// @file      testFrameTransform.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-07
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
#include "AstMath/Vector.hpp"
#include "AstCore/DateTime.hpp"
#include "AstTest/AstTestMacro.h"
#include "AstUtil/Literals.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/SOFA.hpp"
#include "AstCore/EOP.hpp"

AST_USING_NAMESPACE
using namespace _AST literals;


class FrameTransformTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        aInitialize();
    }
    void TearDown() override
    {
        aUninitialize();
    }
};

TEST_F(FrameTransformTest, J2000ToMOD)
{
    {    
        TimePoint tp = TimePoint::FromUTC(2026, 1, 7, 12, 0, 0);
        Vector3d vecJ2000{1000_km, 2000_km, 3000_km};
        Vector3d vecMOD;
        aJ2000ToMOD(tp, vecJ2000, vecMOD);
        EXPECT_NEAR(vecMOD[0], 980758.8704899253, 1e-9);
        EXPECT_NEAR(vecMOD[1], 2005.762527176539_km, 1e-9);
        EXPECT_NEAR(vecMOD[2], 3002.5037423006997_km, 1e-9);
        printf("vecMOD: %.15f, %.15f, %.15f\n", vecMOD[0], vecMOD[1], vecMOD[2]);
    }
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 7, 12, 0, 0);
        Vector3d vecJ2000{3000_km, 2000_km, 1000_km};
        Vector3d vecMOD;
        aJ2000ToMOD(tp, vecJ2000, vecMOD);
        EXPECT_NEAR(vecMOD[0], 2985774.6999846440, 1e-9);
        EXPECT_NEAR(vecMOD[1], 2017.414131093856_km, 1e-9);
        EXPECT_NEAR(vecMOD[2], 1007.5662085413698_km, 1e-9);
        printf("vecMOD: %.15f, %.15f, %.15f\n", vecMOD[0], vecMOD[1], vecMOD[2]);
    }
}


TEST_F(FrameTransformTest, MODToTOD)
{
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 7, 12, 0, 0);
        Vector3d vecMOD{1000_km, 2000_km, 3000_km};
        Vector3d vecTOD;
        // DE
        aNutationMethodSet(ENutationMethod::eJplDe);

        aMODToTOD(tp, vecMOD, vecTOD);
        EXPECT_NEAR(vecTOD[0], 999.9078961179760654_km, 1e-9);
        EXPECT_NEAR(vecTOD[1], 1999.9073241753194452_km, 1e-9);
        EXPECT_NEAR(vecTOD[2], 3000.0924809063526482_km, 1e-9);
        printf("vecTOD: %.15f, %.15f, %.15f\n", vecTOD[0], vecTOD[1], vecTOD[2]);
    }
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 7, 12, 0, 0);
        Vector3d vecMOD{1000_km, 2000_km, 3000_km};
        Vector3d vecTOD;
        // SOFA
        aNutationMethodSet(ENutationMethod::eIAU1980);

        aMODToTOD(tp, vecMOD, vecTOD);
        EXPECT_NEAR(vecTOD[0], 999.9078963285318196_km, 1e-9);
        EXPECT_NEAR(vecTOD[1], 1999.9073241197208972_km, 1e-9);
        EXPECT_NEAR(vecTOD[2], 3000.0924808732393103_km, 1e-9);
        printf("vecTOD: %.15f, %.15f, %.15f\n", vecTOD[0], vecTOD[1], vecTOD[2]);
    }
}


TEST_F(FrameTransformTest, TODToGTOD)
{
    // aInitialize();
    // GTEST_SKIP() << "TODToGTOD is not implemented";
    aUninitialize(); // 不依赖EOP数据进行计算
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 9, 0, 0, 0);
        Vector3d vecTOD{1000_km, 2000_km, 3000_km};
        Vector3d vecGTOD;
        aNutationMethodSet(ENutationMethod::eIAU1980);
        aTODToGTOD(tp, vecTOD, vecGTOD);
        EXPECT_NEAR(vecGTOD[0], 1578.026663010245_km, 1e-7);
        EXPECT_NEAR(vecGTOD[1], -1584.24488347879_km, 1e-7);
        EXPECT_NEAR(vecGTOD[2], 3000.0_km, 1e-9);
        printf("vecGTOD: %.15f, %.15f, %.15f\n", vecGTOD[0], vecGTOD[1], vecGTOD[2]);
    }
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 9, 12, 0, 0);
        Vector3d vecTOD{3000_km, 2000_km, 1000_km};
        Vector3d vecGTOD;
        aNutationMethodSet(ENutationMethod::eIAU1980);
        aTODToGTOD(tp, vecTOD, vecGTOD);
        EXPECT_NEAR(vecGTOD[0], -911.871459490037_km, 1e-5);
        EXPECT_NEAR(vecGTOD[1], 3488.336342924447_km, 1e-5);
        EXPECT_NEAR(vecGTOD[2], 1000_km, 1e-9);
        printf("vecGTOD: %.15f, %.15f, %.15f\n", vecGTOD[0], vecGTOD[1], vecGTOD[2]);
    }
}


TEST_F(FrameTransformTest, TODToGTODWithVelocity)
{
    aUninitialize(); // 不依赖EOP数据进行计算
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 9, 0, 0, 0);
        Vector3d vecTOD{1000_km, 2000_km, 3000_km};
        Vector3d velTOD{1000, 2000, 3000};
        Vector3d vecGTOD;
        Vector3d velGTOD;
        aNutationMethodSet(ENutationMethod::eIAU1980);
        aTODToGTOD(tp, vecTOD, velTOD, vecGTOD, velGTOD);
        EXPECT_NEAR(vecGTOD[0], 1578.026663010245_km, 1e-7);
        EXPECT_NEAR(vecGTOD[1], -1584.24488347879_km, 1e-7);
        EXPECT_NEAR(vecGTOD[2], 3000.0_km, 1e-9);
        EXPECT_NEAR(velGTOD[0], 1462.5017019144806, 1e-7);
        EXPECT_NEAR(velGTOD[1], -1699.3164047797761, 1e-7);
        EXPECT_NEAR(velGTOD[2], 3000, 1e-9);
        printf("vecGTOD: %.15f, %.15f, %.15f\n", vecGTOD[0], vecGTOD[1], vecGTOD[2]);
        printf("velGTOD: %.15f, %.15f, %.15f\n", velGTOD[0], velGTOD[1], velGTOD[2]);
    }
}

void changeEOPData()
{
    {
        auto eop = aGlobalContext_GetEOP();
        ASSERT_TRUE(eop != nullptr) << "EOP data is not loaded";
        JulianDate jdUTC1 = JulianDate::FromDateTime(2018, 1, 1, 0, 0, 0);
        auto mjd1 = aJDToMJD_Imprecise(jdUTC1);
        auto entry1 = eop->getEntry(mjd1);
        ASSERT_TRUE(entry1 != nullptr) << "EOP entry not found";
        JulianDate jdUTC2 = JulianDate::FromDateTime(2018, 1, 2, 0, 0, 0);
        auto mjd2 = aJDToMJD_Imprecise(jdUTC2);
        auto entry2 = eop->getEntry(mjd2);
        ASSERT_TRUE(entry2 != nullptr) << "EOP entry not found";
        printf("entry1, x = %.20g rad, y = %.20g rad\n", entry1->x, entry1->y);
        printf("entry2, x = %.20g rad, y = %.20g rad\n", entry2->x, entry2->y);
        auto newentry1 = *entry1;
        auto newentry2 = *entry2;
        newentry1.x = 0.059224 * kArcSecToRad;
        newentry1.y = 0.247646 * kArcSecToRad;
        newentry2.x = 0.057406 * kArcSecToRad;
        newentry2.y = 0.248566 * kArcSecToRad;
        eop->setEntry(mjd1, newentry1);
        eop->setEntry(mjd2, newentry2);
    }
}

TEST_F(FrameTransformTest, PoleMotion)
{
    changeEOPData();
    TimePoint tp = TimePoint::FromUTC(2018, 1, 1, 0, 0, 0);
    double x, y;
    aPoleMotion(tp, x, y);
    printf("x = %.20g rad, y = %.20g rad\n", x, y);
    printf("x = %.20g deg, y = %.20g deg\n", x*kRadToDeg, y*kRadToDeg);

}

TEST_F(FrameTransformTest, GTODToECF)
{
    changeEOPData();
    
    {
        TimePoint tp = TimePoint::FromUTC(2018, 1, 1, 0, 0, 0);
        Vector3d vecGTOD{10000_km, 20000_km, 30000_km};
        Vector3d vecECF;
        aGTODToECF(tp, vecGTOD, vecECF);
        EXPECT_NEAR(vecECF[0], 10000.00861378123_km, 1e-3);
        EXPECT_NEAR(vecECF[1], 19999.96398133837_km, 1e-3);
        EXPECT_NEAR(vecECF[2], 30000.02114115036_km, 1e-7);
        printf("vecECF: %.15f, %.15f, %.15f\n", vecECF[0], vecECF[1], vecECF[2]);
    }
    {
        TimePoint tp = TimePoint::FromUTC(2018, 1, 1, 8, 0, 0);
        Vector3d vecGTOD{30000_km, 20000_km, 10000_km};
        Vector3d vecECF;
        aGTODToECF(tp, vecGTOD, vecECF);
        EXPECT_NEAR(vecECF[0], 30000.00284187961_km, 1e-3);
        EXPECT_NEAR(vecECF[1], 19999.98797891128_km, 1e-2);
        EXPECT_NEAR(vecECF[2], 10000.01551651887_km, 1e-8);
        printf("vecECF: %.15f, %.15f, %.15f\n", vecECF[0], vecECF[1], vecECF[2]);
    }
}


TEST_F(FrameTransformTest, J2000ToECF)
{
    aGlobalContext_GetEOP()->unload();
    aNutationMethodSet(ENutationMethod::eJplDe);
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
        Vector3d vecJ2000{1000_km, 2000_km, 3000_km};
        Vector3d vecECF;
        Vector3d vecMOD;
        Vector3d vecTOD;
        Vector3d vecGTOD;

        aJ2000ToMOD(tp, vecJ2000, vecMOD);
        printf("vecMOD: %.15f, %.15f, %.15f\n", vecMOD[0], vecMOD[1], vecMOD[2]);

        aMODToTOD(tp, vecMOD, vecTOD);
        printf("vecTOD: %.15f, %.15f, %.15f\n", vecTOD[0], vecTOD[1], vecTOD[2]);

        aTODToGTOD(tp, vecTOD, vecGTOD);
        printf("vecGTOD: %.15f, %.15f, %.15f\n", vecGTOD[0], vecGTOD[1], vecGTOD[2]);

        aGTODToECF(tp, vecGTOD, vecECF);
        printf("vecECF: %.15f, %.15f, %.15f\n", vecECF[0], vecECF[1], vecECF[2]);

        aJ2000ToECF(tp, vecJ2000, vecECF);
        printf("vecECF: %.15f, %.15f, %.15f\n", vecECF[0], vecECF[1], vecECF[2]);

        EXPECT_NEAR(vecECF[0], 1789.5903328549421_km, 1e-3);
        EXPECT_NEAR(vecECF[1], -1334.846725809680_km, 1e-3);
        EXPECT_NEAR(vecECF[2], 3002.590657939792_km, 1e-8);
    }

}


TEST_F(FrameTransformTest, J2000ToECFWithVelocity)
{
    aGlobalContext_GetEOP()->unload();
    aNutationMethodSet(ENutationMethod::eJplDe);
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
        Vector3d vecJ2000{1000_km, 2000_km, 3000_km};
        Vector3d velJ2000{100_km/s, 200_km/s, 300_km/s};
        Vector3d vecECF;
        Vector3d velECF;
        aJ2000ToECF(tp, vecJ2000, velJ2000, vecECF, velECF);
        printf("vecECF: %.15f, %.15f, %.15f\n", vecECF[0], vecECF[1], vecECF[2]);
        printf("velECF: %.15f, %.15f, %.15f\n", velECF[0], velECF[1], velECF[2]);
        EXPECT_NEAR(vecECF[0], 1789.5903328549421_km, 1e-3);
        EXPECT_NEAR(vecECF[1], -1334.846725809680_km, 1e-3);
        EXPECT_NEAR(vecECF[2], 3002.590657939792_km, 1e-8);
        EXPECT_NEAR(velECF[0], 178.86169472521609_km/s, 1e-4);
        EXPECT_NEAR(velECF[1], -133.61517156869408_km/s, 1e-4);
        EXPECT_NEAR(velECF[2], 300.25906579397923_km/s, 1e-8);
    }
}

TEST_F(FrameTransformTest, ICRFToCIRF)
{
    aGlobalContext_GetEOP()->unload();
    {
        TimePoint tp = TimePoint::FromUTC(2070, 1, 1, 0, 0, 0);
        Vector3d vecICRF{1000_km, 2000_km, 3000_km};
        Vector3d vecCIRF;
        aICRFToCIRF(tp, vecICRF, vecCIRF);
        printf("vecCIRF: %.15f, %.15f, %.15f\n", vecCIRF[0], vecCIRF[1], vecCIRF[2]);
        EXPECT_NEAR(vecCIRF[0], 979521.6677, 1e-4);
        EXPECT_NEAR(vecCIRF[1], 2000283.81514, 1e-4);
        EXPECT_NEAR(vecCIRF[2], 3006559.82166, 1e-4);
    }
}

TEST_F(FrameTransformTest, EarthRotationAngle)
{
    aGlobalContext_GetEOP()->unload();
    {
        TimePoint tp = TimePoint::FromUTC(2070, 1, 1, 0, 0, 0);
        double angle = aEarthRotationAngle_IAU2000(tp);
        printf("angle = %.15f rad\n", angle);
        EXPECT_NEAR(angle, 1.7471233467, 1e-11);
    }
}


TEST_F(FrameTransformTest, CIRFToTIRF)
{
    aGlobalContext_GetEOP()->unload();
    {
        TimePoint tp = TimePoint::FromUTC(2070, 1, 1, 0, 0, 0);
        Vector3d vecCIRF{1000_km, 2000_km, 3000_km};
        Vector3d vecTIRF;
        aCIRFToTIRF(tp, vecCIRF, vecTIRF);
        printf("vecTIRF: %.15f, %.15f, %.15f\n", vecTIRF[0], vecTIRF[1], vecTIRF[2]);
        EXPECT_NEAR(vecTIRF[0],  1793574.5182948, 1e-7);
        EXPECT_NEAR(vecTIRF[1], -1335324.0982337, 1e-7);
        EXPECT_EQ(vecTIRF[2], 3000_km);
    }
}


TEST_F(FrameTransformTest, ICRFToECF)
{
    // 不依赖EOP数据
    aGlobalContext_GetEOP()->unload();
    {
        TimePoint tp = TimePoint::FromUTC(2070, 1, 1, 0, 0, 0);
        Vector3d vecICRF{1000_km, 2000_km, 3000_km};
        Vector3d vecECF;
        aICRFToECF(tp, vecICRF, vecECF);
        printf("vecECF: %.15f, %.15f, %.15f\n", vecECF[0], vecECF[1], vecECF[2]);
        EXPECT_NEAR(vecECF[0],  1797446.134222130, 1e-6);
        EXPECT_NEAR(vecECF[1], -1315213.075245136, 1e-4);
        EXPECT_NEAR(vecECF[2],  3006559.821668970, 1e-4);
    }

}

GTEST_MAIN()
