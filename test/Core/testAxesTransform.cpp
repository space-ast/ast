///
/// @file      testAxesTransform.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-04
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

#include "AstCore/AxesTransform.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/EarthFrame.hpp"
#include "AstCore/RunTime.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstTest/Test.h"


AST_USING_NAMESPACE

class AxesTransformTest : public ::testing::Test
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

TEST_F(AxesTransformTest, Instance)
{
    EXPECT_TRUE(aAxesRoot() != nullptr);
    EXPECT_TRUE(aAxesICRF() != nullptr);
    EXPECT_TRUE(aAxesJ2000() != nullptr);
    EXPECT_TRUE(aAxesECF() != nullptr);
    EXPECT_TRUE(aAxesMOD() != nullptr);
    EXPECT_TRUE(aAxesTOD() != nullptr);
    EXPECT_TRUE(aAxesGTOD() != nullptr);
}

TEST_F(AxesTransformTest, getTransform)
{
    {
        auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
        KinematicRotation rotation1;
        KinematicRotation rotation2;
        {
            HAxes icrf = aAxesICRF();
            HAxes ecf = aAxesECF();
            err_t rc = ecf->getTransformFrom(icrf, tp, rotation1);
            EXPECT_FALSE(rc);
        }
        aICRFToECFTransform(tp, rotation2);
        Matrix3d m1 = rotation1.getMatrix();
        Matrix3d m2 = rotation2.getMatrix();
        Vector3d rotRate1 = rotation1.getRotationRate();
        Vector3d rotRate2 = rotation2.getRotationRate();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                EXPECT_NEAR(m1(i, j), m2(i, j), 1e-14);
            }
            EXPECT_NEAR(rotRate1(i), rotRate2(i), 1e-17);
        }
    }
    {
        auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
        KinematicRotation rotation1;
        KinematicRotation rotation2;
        {
            HAxes j2000 = aAxesJ2000();
            HAxes ecf = aAxesECF();
            err_t rc = ecf->getTransformFrom(j2000, tp, rotation1);
            EXPECT_FALSE(rc);
        }
        aJ2000ToECFTransform(tp, rotation2);
        Matrix3d m1 = rotation1.getMatrix();
        Matrix3d m2 = rotation2.getMatrix();
        Vector3d rotRate1 = rotation1.getRotationRate();
        Vector3d rotRate2 = rotation2.getRotationRate();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                EXPECT_NEAR(m1(i, j), m2(i, j), 1e-14);
            }
            EXPECT_NEAR(rotRate1(i), rotRate2(i), 1e-17);
        }
    }
    {
        auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
        KinematicRotation rotation1;
        KinematicRotation rotation2;
        {
            HAxes tod = aAxesTOD();
            HAxes gtod = aAxesGTOD();
            err_t rc = gtod->getTransformFrom(tod, tp, rotation1);
            EXPECT_FALSE(rc);
        }
        aTODToGTODTransform(tp, rotation2);
        Matrix3d m1 = rotation1.getMatrix();
        Matrix3d m2 = rotation2.getMatrix();
        Vector3d rotRate1 = rotation1.getRotationRate();
        Vector3d rotRate2 = rotation2.getRotationRate();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                EXPECT_NEAR(m1(i, j), m2(i, j), 1e-14);
            }
            EXPECT_NEAR(rotRate1(i), rotRate2(i), 1e-17);
        }
    }
    {
        auto tp = TimePoint::FromUTC(2016, 3, 4, 0, 0, 0);
        Rotation rotation1;
        Rotation rotation2;
        {
            HAxes j2000 = aAxesJ2000();
            HAxes gtod = aAxesGTOD();
            err_t rc = j2000->getTransformTo(gtod, tp, rotation1);
            EXPECT_FALSE(rc);
        }
        {
            Rotation temp1, temp2, temp3;
            aJ2000ToMODTransform(tp, temp1);
            aMODToTODTransform(tp, temp2);
            aTODToGTODTransform(tp, temp3);
            rotation2 = temp1 * temp2 * temp3;
        }
        Matrix3d m1 = rotation1.getMatrix();
        Matrix3d m2 = rotation2.getMatrix();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                EXPECT_NEAR(m1(i, j), m2(i, j), 1e-14);
            }
        }
    }
    {
        auto tp = TimePoint::FromUTC(2011, 3, 4, 0, 0, 0);
        HAxes mod = aAxesMOD();
        HAxes icrf = aAxesICRF();
        Rotation rotation1;
        Rotation rotation2;

        err_t rc;
        rc = aAxesTransform(mod, icrf, tp, rotation1);
        EXPECT_FALSE(rc);
        rc = aAxesTransform(icrf, mod, tp, rotation2);
        EXPECT_FALSE(rc);
        Matrix3d mat1 = rotation1.getMatrix();
        Matrix3d mat2 = rotation2.inverse().getMatrix();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                EXPECT_DOUBLE_EQ(mat1(i, j), mat2(i, j));
            }
        }
    }
    {
        auto tp = TimePoint::FromUTC(2016, 3, 4, 0, 0, 0);
        Rotation rotation;
        HAxes icrf = aAxesICRF();
        HAxes j2000 = aAxesJ2000();
        err_t rc = aAxesTransform(icrf, j2000, tp, rotation);
        EXPECT_FALSE(rc);
        Matrix3d mat = rotation.getMatrix();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                printf("%.16g\t", mat(i, j));
                if(i == j){
                    EXPECT_TRUE(std::abs(mat(i, j)) >= 1-1e-6);
                }else{
                    EXPECT_TRUE(std::abs(mat(i, j)) <= 1e-6);
                }
            }
            printf("\n");
        }
    }
}

GTEST_MAIN();

