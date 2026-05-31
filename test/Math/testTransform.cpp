///
/// @file      testTransform.cpp
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

#include "ast/Rotation.hpp"
#include "ast/KinematicRotation.hpp"
#include "ast/Transform.hpp"
#include "ast/KinematicTransform.hpp"
#include "ast/TimePoint.hpp"
#include "ast/EarthFrame.hpp"
#include "ast/Literals.hpp"
#include "ast/Vector.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

TEST(TransformTest, Transform_Identity)
{
    Transform transform = Transform::Identity();
    Vector3d pos{4, -3, 8};
    Vector3d posTrans = transform.transformPosition(pos);
    printf("posTrans: %f, %f, %f\n", posTrans.x(), posTrans.y(), posTrans.z());
    for(int i = 0; i < 3; i++)
    {
        EXPECT_DOUBLE_EQ(posTrans[i], pos[i]);
    }
}

TEST(TransformTest, Transform_Inverse)
{
    Transform transform{Vector3d{-3, 2, 6}, Rotation(30_deg, Vector3d{4, -1, 2})};
    Vector3d pos{4, -3, 8};
    Transform transform_inv = transform.inverse();
    Vector3d posInv = transform.transformPosition(pos);
    Vector3d pos2 = transform_inv.transformPosition(posInv);
    printf("posInv: %f, %f, %f\n", posInv.x(), posInv.y(), posInv.z());
    printf("pos2: %f, %f, %f\n", pos2.x(), pos2.y(), pos2.z());
    for(int i = 0; i < 3; i++)
    {
        EXPECT_DOUBLE_EQ(pos2[i], pos[i]);
    }
    Transform identity = transform.composed(transform_inv);
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++)
        {
            if(i == j) 
                EXPECT_DOUBLE_EQ(identity.getMatrix()(i, i), 1.0);
            else
                EXPECT_NEAR(identity.getMatrix()(i, j), 0.0, 1e-15);
        }
        EXPECT_NEAR(identity.getTranslation()[i], 0.0, 1e-15);
    }
}


TEST(TransformTest, Transform_compose)
{
    Transform transform1{Vector3d{1, -5, 3}, Rotation(30_deg, Vector3d{4, -1, 2})};
    Transform transform2{Vector3d{-3, 2, 6}, Rotation(60_deg, Vector3d{1, -2, 7})};
    Transform transform_compose1 = transform1.composed(transform2);
    Transform transform_compose2(transform1);
    transform_compose2.compose(transform2);
    Vector3d pos{4, 2, 3};
    Vector3d posTrans1 = transform_compose1.transformPosition(pos);
    Vector3d posTrans2 = transform_compose2.transformPosition(pos);
    Vector3d posTrans3 = transform2.transformPosition(transform1.transformPosition(pos));
    printf("posTrans1: %f, %f, %f\n", posTrans1.x(), posTrans1.y(), posTrans1.z());
    printf("posTrans2: %f, %f, %f\n", posTrans2.x(), posTrans2.y(), posTrans2.z());
    printf("posTrans3: %f, %f, %f\n", posTrans3.x(), posTrans3.y(), posTrans3.z());
    for(int i = 0; i < 3; i++)
    {
        EXPECT_DOUBLE_EQ(posTrans1[i], posTrans2[i]);
        EXPECT_DOUBLE_EQ(posTrans1[i], posTrans3[i]);
    }
}

TEST(TransformTest, KinematicTransform_compose)
{
    KinematicTransform transform1{Vector3d{11, -17, 13}, Vector3d{4, -11, 2},  KinematicRotation(Rotation(30_deg, Vector3d{4, 11, 12}), Vector3d{-11, -4, 7})};
    KinematicTransform transform2{Vector3d{-13, 21, 11}, Vector3d{-1, -21, 7}, KinematicRotation(Rotation(50_deg, Vector3d{3, 21, 17}), Vector3d{ 13,  4, 5})};
    KinematicTransform transform_compose1 = transform1.composed(transform2);
    KinematicTransform transform_compose2(transform1);
    transform_compose2.compose(transform2);
    CartState posvel{3, 5, 7, -7, -12, 2};
    auto posvel1 = transform_compose1.transformPositionVelocity(posvel);
    auto posvel2 = transform_compose2.transformPositionVelocity(posvel);
    auto posvel3 = transform2.transformPositionVelocity(transform1.transformPositionVelocity(posvel));
    printf("posTrans1: %f, %f, %f\n", posvel1.pos().x(), posvel1.pos().y(), posvel1.pos().z());
    printf("posTrans2: %f, %f, %f\n", posvel2.pos().x(), posvel2.pos().y(), posvel2.pos().z());
    printf("posTrans3: %f, %f, %f\n", posvel3.pos().x(), posvel3.pos().y(), posvel3.pos().z());
    printf("velTrans1: %f, %f, %f\n", posvel1.vel().x(), posvel1.vel().y(), posvel1.vel().z());
    printf("velTrans2: %f, %f, %f\n", posvel2.vel().x(), posvel2.vel().y(), posvel2.vel().z());
    printf("velTrans3: %f, %f, %f\n", posvel3.vel().x(), posvel3.vel().y(), posvel3.vel().z());
    for(int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(posvel1.pos()[i], posvel2.pos()[i], 1e-13);
        EXPECT_NEAR(posvel1.vel()[i], posvel2.vel()[i], 1e-13);
        EXPECT_NEAR(posvel3.pos()[i], posvel1.pos()[i], 1e-13);
        EXPECT_NEAR(posvel3.vel()[i], posvel1.vel()[i], 1e-13);
    }
}


TEST(TransformTest, KinematicTransform_inverse)
{
    KinematicTransform transform{Vector3d{11, -17, 13}, Vector3d{4, -11, 2},  KinematicRotation(Rotation(30_deg, Vector3d{4, 11, 12}), Vector3d{-11, -4, 7})};
    KinematicTransform transform_inv = transform.inverse();
    CartState posvel1{3, 5, 7, -7, -12, 2};
    auto posvelTrans = transform.transformPositionVelocity(posvel1);
    auto posvel2 = transform_inv.transformPositionVelocity(posvelTrans);
    printf("pos1: %f, %f, %f\n", posvel1.pos().x(), posvel1.pos().y(), posvel1.pos().z());
    printf("pos2: %f, %f, %f\n", posvel2.pos().x(), posvel2.pos().y(), posvel2.pos().z());
    printf("vel1: %f, %f, %f\n", posvel1.vel().x(), posvel1.vel().y(), posvel1.vel().z());
    printf("vel2: %f, %f, %f\n", posvel2.vel().x(), posvel2.vel().y(), posvel2.vel().z());
    for(int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(posvel1.pos()[i], posvel2.pos()[i], 2e-14);
        EXPECT_NEAR(posvel1.vel()[i], posvel2.vel()[i], 2e-14);
    }
}


TEST(TransformTest, Rotation)
{
    Rotation rotation;
    TimePoint tp = TimePoint::FromUTC(2000, 1, 2, 3, 4, 5.0);
    aJ2000ToECFTransform(tp, rotation);
    Vector3d vec1{1, 2, 3};
    Vector3d vecRot = rotation.transformVector(vec1);
    Vector3d vec2 = rotation.inverse().transformVector(vecRot);
    printf("vecRot: %f, %f, %f\n", vecRot.x(), vecRot.y(), vecRot.z());
    printf("vec1: %f, %f, %f\n", vec1.x(), vec1.y(), vec1.z());
    printf("vec2: %f, %f, %f\n", vec2.x(), vec2.y(), vec2.z());
    for(int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(vec2[i], vec1[i], 1e-12);
    }
}

TEST(TransformTest, KinematicRotation_transformVectorVelocity)
{
    KinematicRotation kinematicRotation{Rotation(30_deg, Vector3d{4, -1, 2}), Vector3d{-1, -2, 7}};
    Vector3d vec1{1, -2, 3}, vel1{4, -5, 6};
    Vector3d vecRot, vecvelRot;
    Vector3d vec2, vel2;
    kinematicRotation.transformVectorVelocity(vec1, vel1, vecRot, vecvelRot);
    kinematicRotation.transformVectorVelocityInv(vecRot, vecvelRot, vec2, vel2);
    printf("vecRot: %f, %f, %f\n", vecRot.x(), vecRot.y(), vecRot.z());
    printf("vecvelRot: %f, %f, %f\n", vecvelRot.x(), vecvelRot.y(), vecvelRot.z());
    printf("vec2: %f, %f, %f\n", vec2.x(), vec2.y(), vec2.z());
    printf("vel2: %f, %f, %f\n", vel2.x(), vel2.y(), vel2.z());
    for(int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(vec1[i], vec2[i], 1e-14);
        EXPECT_NEAR(vel1[i], vel2[i], 1e-14);
    }
}

TEST(TransformTest, KinematicRotation_inverse)
{
    KinematicRotation kinematicRotation;
    TimePoint tp = TimePoint::FromUTC(2000, 1, 2, 3, 4, 5.0);
    aJ2000ToECFTransform(tp, kinematicRotation);
    Vector3d vec1{1, 2, -3}, vecvel1{4, -5, 6};
    Vector3d vecRot, vecvelRot;
    Vector3d vec2, vecvel2;
    kinematicRotation.transformVectorVelocity(vec1, vecvel1, vecRot, vecvelRot);
    kinematicRotation.inverse().transformVectorVelocity(vecRot, vecvelRot, vec2, vecvel2);
    printf("vecvel1: %f, %f, %f\n", vecvel1.x(), vecvel1.y(), vecvel1.z());
    printf("vecvel2: %f, %f, %f\n", vecvel2.x(), vecvel2.y(), vecvel2.z());
    printf("vec1: %f, %f, %f\n", vec1.x(), vec1.y(), vec1.z());
    printf("vec2: %f, %f, %f\n", vec2.x(), vec2.y(), vec2.z());
    printf("vecRot: %f, %f, %f\n", vecRot.x(), vecRot.y(), vecRot.z()); 
    printf("vecvelRot: %f, %f, %f\n", vecvelRot.x(), vecvelRot.y(), vecvelRot.z()); 
    for(int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(vecvel2[i], vecvel1[i], 1e-12);
        EXPECT_NEAR(vec2[i], vec1[i], 1e-12);
    }
}

TEST(TransformTest, KinematicRotation_compose)
{
    KinematicRotation kinematicRotation1;
    KinematicRotation kinematicRotation2;
    TimePoint tp1 = TimePoint::FromUTC(2010, 1, 2, 3, 4, 5.0);
    TimePoint tp2 = TimePoint::FromUTC(2020, 1, 2, 3, 4, 5.0);
    aJ2000ToECFTransform(tp1, kinematicRotation1);
    aJ2000ToECFTransform(tp2, kinematicRotation2);

    KinematicRotation kinematicRotationComposed1 = kinematicRotation1.composed(kinematicRotation2);
    KinematicRotation kinematicRotationComposed2 = kinematicRotation1;
    kinematicRotationComposed2.compose(kinematicRotation2);
    Vector3d vec{1, -2, 3}, vel{4, 5, -6};
    Vector3d vec1, vel1;
    Vector3d vec2, vel2;
    Vector3d vec3, vel3;
    Vector3d vecTemp, velTemp;

    kinematicRotationComposed1.transformVectorVelocity(vec, vel, vec1, vel1);
    kinematicRotationComposed2.transformVectorVelocity(vec, vel, vec2, vel2);

    kinematicRotation1.transformVectorVelocity(vec, vel, vecTemp, velTemp);
    kinematicRotation2.transformVectorVelocity(vecTemp, velTemp, vec3, vel3);
    
    printf("vec1: %f, %f, %f\n", vec1.x(), vec1.y(), vec1.z());
    printf("vec2: %f, %f, %f\n", vec2.x(), vec2.y(), vec2.z());
    printf("vec3: %f, %f, %f\n", vec3.x(), vec3.y(), vec3.z());

    printf("vel1: %f, %f, %f\n", vel1.x(), vel1.y(), vel1.z());
    printf("vel2: %f, %f, %f\n", vel2.x(), vel2.y(), vel2.z());
    printf("vel3: %f, %f, %f\n", vel3.x(), vel3.y(), vel3.z());

    EXPECT_NEAR(vec1.x(), vec3.x(), 1e-14);
    EXPECT_NEAR(vec1.y(), vec3.y(), 1e-14);
    EXPECT_NEAR(vec1.z(), vec3.z(), 1e-14);
    EXPECT_NEAR(vel1.x(), vel3.x(), 1e-14);
    EXPECT_NEAR(vel1.y(), vel3.y(), 1e-14);
    EXPECT_NEAR(vel1.z(), vel3.z(), 1e-14);
    EXPECT_NEAR(vec2.x(), vec3.x(), 1e-14);
    EXPECT_NEAR(vec2.y(), vec3.y(), 1e-14);
    EXPECT_NEAR(vec2.z(), vec3.z(), 1e-14);
    EXPECT_NEAR(vel2.x(), vel3.x(), 1e-14);
    EXPECT_NEAR(vel2.y(), vel3.y(), 1e-14);
    EXPECT_NEAR(vel2.z(), vel3.z(), 1e-14);

    EXPECT_EQ(vec1.x(), vec2.x());
    EXPECT_EQ(vec1.y(), vec2.y());
    EXPECT_EQ(vec1.z(), vec2.z());
    EXPECT_EQ(vel1.x(), vel2.x());
    EXPECT_EQ(vel1.y(), vel2.y());
    EXPECT_EQ(vel1.z(), vel2.z());

    {
        auto identity = kinematicRotation1.composed(kinematicRotation1.inverse());
        Vector3d vec1{1, 2, 3}, vel1{4,5,6};
        Vector3d vec2, vel2;
        identity.transformVectorVelocity(vec1, vel1, vec2, vel2);
        
        printf("vec1: %f, %f, %f\n", vec1.x(), vec1.y(), vec1.z());
        printf("vec2: %f, %f, %f\n", vec2.x(), vec2.y(), vec2.z());
        printf("vel1: %f, %f, %f\n", vel1.x(), vel1.y(), vel1.z());
        printf("vel2: %f, %f, %f\n", vel2.x(), vel2.y(), vel2.z());

        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(vec1[i], vec2[i], 1e-14);
            EXPECT_NEAR(vel1[i], vel2[i], 1e-14);
        }
    }
}

GTEST_MAIN()

