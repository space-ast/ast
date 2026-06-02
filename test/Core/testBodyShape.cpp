///
/// @file      testBodyShape.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-01
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/SphereShape.hpp"
#include "ast/SpheroidShape.hpp"
#include "ast/EllipsoidShape.hpp"
#include "ast/RunTime.hpp"
#include "ast/Literals.hpp"
#include "ast/Test.hpp"
#include "ast/CelestialBody.hpp"


AST_USING_NAMESPACE

TEST(BodyShapeTest, SphereShape)
{
    SharedPtr<BodyShape> bodyShape = new SphereShape(400_km);
    {
        GeodeticPoint point{0, 45_deg, 1000};
        Vector3d pos = bodyShape->transform(point);
        printf("pos: %f, %f, %f\n", pos.x(), pos.y(), pos.z());
        Vector3d posExpected{283.5498192558055734_km, 283.5498192558055734_km, 0_km};
        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(pos[i], posExpected[i], 1e-9);
        }
        GeodeticPoint point2;
        bodyShape->transform(pos, point2);
        printf("point2: %.15lf, %.15lf, %.15lf\n", point2.latitude(), point2.longitude(), point2.altitude());
        EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-9);
        EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-9);
        EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-9);
    }
    {
        GeodeticPoint point{30_deg, 60_deg, 2000};
        Vector3d pos = bodyShape->transform(point);
        printf("pos: %f, %f, %f\n", pos.x(), pos.y(), pos.z());
        Vector3d posExpected{174.0711061606722012_km, 301.5000000000000000_km, 200.9999999999999432_km};
        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(pos[i], posExpected[i], 1e-9);
        }
        GeodeticPoint point2;
        bodyShape->transform(pos, point2);
        printf("point2: %.15lf, %.15lf, %.15lf\n", point2.latitude(), point2.longitude(), point2.altitude());
        EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-9);
        EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-9);
        EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-9);
    }
}

TEST(BodyShapeTest, SpheroidShape)
{
    SharedPtr<BodyShape> bodyShape = SpheroidShape::NewFromMajorMinorAxis(4000_km, 3000_km);
    {
        GeodeticPoint point{0, 45_deg, 0};
        Vector3d pos = bodyShape->transform(point);
        printf("pos: %f, %f, %f\n", pos.x(), pos.y(), pos.z());
        Vector3d posExpected{2828.4271247461902021_km, 2828.4271247461902021_km, 0_km};
        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(pos[i], posExpected[i], 1e-9);
        }
        GeodeticPoint point2;
        bodyShape->transform(pos, point2);
        printf("point2: %.15lf, %.15lf, %.15lf\n", point2.latitude(), point2.longitude(), point2.altitude());
        EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-9);
        EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-9);
        EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-9);
    }
    {
        GeodeticPoint point{31_deg, 43_deg, 2000};
        Vector3d pos = bodyShape->transform(point);
        printf("pos: %f, %f, %f\n", pos.x(), pos.y(), pos.z());
        Vector3d posExpected{2668.3578464093511684_km, 2488.2839469905215992_km, 1233.5920485962060411_km};
        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(pos[i], posExpected[i], 1e-9);
        }
        GeodeticPoint point2;
        bodyShape->transform(pos, point2);
        printf("point2: %.15lf, %.15lf, %.15lf\n", point2.latitude(), point2.longitude(), point2.altitude());
        EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-9);
        EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-9);
        EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-9);
    }
}

TEST(BodyShapeTest, EllipsoidShape)
{
    {
        SharedPtr<BodyShape> bodyShape = new EllipsoidShape(581.1_km, 577.9_km, 567.7_km);
        {
            GeodeticPoint point{45_deg, 45_deg, 0};
            Vector3d pos = bodyShape->transform(point);
            printf("pos     : %.15lf, %.15lf, %.15lf\n", pos.x(), pos.y(), pos.z());
            Vector3d posExpected{294.3328901240764139_km, 291.1001524960552729_km, 397.2737138242756600_km};
            printf("expected: %.15lf, %.15lf, %.15lf\n", posExpected.x(), posExpected.y(), posExpected.z());
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(pos[i], posExpected[i], 1e-10);
            }
            GeodeticPoint point2;
            bodyShape->transform(pos, point2);
            printf("point2: %.15lf, %.15lf, %.15lf\n", point2.latitude(), point2.longitude(), point2.altitude());
            EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-10);
            EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-10);
            EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-10);
        }
        {
            GeodeticPoint point{45_deg, 45_deg, 2000};
            Vector3d pos = bodyShape->transform(point);
            printf("pos     : %.15lf, %.15lf, %.15lf\n", pos.x(), pos.y(), pos.z());
            Vector3d posExpected{295.3328901240764708_km, 292.1001524960553297_km, 398.6879273866486528_km};
            printf("expected: %.15lf, %.15lf, %.15lf\n", posExpected.x(), posExpected.y(), posExpected.z());
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(pos[i], posExpected[i], 1e-10);
            }
            GeodeticPoint point2;   
            bodyShape->transform(pos, point2);
            printf("point2: %.15lf, %.15lf, %.15lf\n", point2.latitude(), point2.longitude(), point2.altitude());
            EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-10);
            EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-10);
            EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-10);
        }
    }
    {
        SharedPtr<BodyShape> bodyShape = new EllipsoidShape(1000_km, 500_km, 200_km);
        {
            GeodeticPoint point{50_deg, 30_deg, 3_km};
            Vector3d pos = bodyShape->transform(point);
            printf("pos     : %.15lf, %.15lf, %.15lf\n", pos.x(), pos.y(), pos.z());
            Vector3d posExpected{930.5145412915294401_km, 135.0313412857787227_km, 53.4261395455051513_km};
            printf("expected: %.15lf, %.15lf, %.15lf\n", posExpected.x(), posExpected.y(), posExpected.z());
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(pos[i], posExpected[i], 1e-9);
            }
            GeodeticPoint point2;
            bodyShape->transform(pos, point2);
            printf("point2: %.15lf, %.15lf, %.15lf\n", point2.latitude(), point2.longitude(), point2.altitude());
            EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-9);
            EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-9);
            EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-9);
        }
        {
            GeodeticPoint point{50_deg, 30_deg, 0};
            Vector3d pos = bodyShape->transform(point);
            printf("pos     : %.15lf, %.15lf, %.15lf\n", pos.x(), pos.y(), pos.z());
            Vector3d posExpected{928.8445300938508353_km, 134.0671598712490322_km, 51.1280062161478028_km};
            printf("expected: %.15lf, %.15lf, %.15lf\n", posExpected.x(), posExpected.y(), posExpected.z());
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(pos[i], posExpected[i], 1e-9);
            }
            GeodeticPoint point2;
            bodyShape->transform(pos, point2);
            printf("point2: %.15lf, %.15lf, %.15lf\n", point2.latitude(), point2.longitude(), point2.altitude());
            EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-9);
            EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-9);
            EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-9);
        }
    }
}

TEST(BodyShapeTest, EarthShape)
{
    aInitialize();
    auto shape = aGetEarth()->getShape();
    auto spheroidShape = aobject_cast<SpheroidShape*>(shape);
    ASSERT_TRUE(spheroidShape);
    printf("shape: %s\n", shape->name().c_str());
    printf("majorAxis: %.15lf\n", spheroidShape->majorAxis());
    printf("flatfactor: %.15lf\n", spheroidShape->flatfactor());
    {
        GeodeticPoint point{45_deg, 90_deg, 2000};
        Vector3d pos = spheroidShape->transform(point);
        printf("pos: %.15lf, %.15lf, %.15lf\n", pos.x(), pos.y(), pos.z());
        EXPECT_NEAR(pos.x(), 0.0, 1e-6);
        EXPECT_NEAR(pos.y(), 4519005.0924131382999, 1e-4);
        EXPECT_NEAR(pos.z(), 4488762.6224228024512, 1e-4);
        GeodeticPoint point2;
        spheroidShape->transform(pos, point2);
        printf("point2: %.15lf, %.15lf, %.15lf\n", point2.latitude(), point2.longitude(), point2.altitude());
        EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-6);
        EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-6);
        EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-6);
    }

}

GTEST_MAIN()

