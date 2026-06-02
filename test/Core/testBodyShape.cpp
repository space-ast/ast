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

}

TEST(BodyShapeTest, SpheroidShape)
{
    SharedPtr<BodyShape> bodyShape = SpheroidShape::NewFromMajorAxisFlatFactor(3397000, 1/196.877360);
    {
        GeodeticPoint point{0, 45_deg, 1000};
        Vector3d pos = bodyShape->transform(point);
        printf("pos: %f, %f, %f\n", pos.x(), pos.y(), pos.z());
    }
    {
        GeodeticPoint point{45_deg, 90_deg, 2000};
        Vector3d pos = bodyShape->transform(point);
        printf("pos: %f, %f, %f\n", pos.x(), pos.y(), pos.z());
    }
}

TEST(BodyShapeTest, EllipsoidShape)
{
    
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

