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
    GTEST_SKIP() << "TODToGTOD is not implemented";
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 7, 12, 0, 0);
        Vector3d vecTOD{1000_km, 2000_km, 3000_km};
        Vector3d vecGTOD;
        aNutationMethodSet(ENutationMethod::eIAU1980);
        aTODToGTOD(tp, vecTOD, vecGTOD);
        EXPECT_NEAR(vecGTOD[0], 1000.0924809063526482_km, 1e-9);
        EXPECT_NEAR(vecGTOD[1], 2000.0924808732393103_km, 1e-9);
        EXPECT_NEAR(vecGTOD[2], 3000.0924808732393103_km, 1e-9);
        printf("vecGTOD: %.15f, %.15f, %.15f\n", vecGTOD[0], vecGTOD[1], vecGTOD[2]);
    }
}

GTEST_MAIN()
