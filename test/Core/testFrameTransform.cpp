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

GTEST_MAIN()
