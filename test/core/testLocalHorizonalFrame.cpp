///
/// @file      testLocalHorizonalFrame.cpp
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

#include "ast/LocalHorizonalFrame.hpp"
#include "ast/Rotation.hpp"
#include "ast/Coordinate.hpp"
#include "ast/GeodeticPoint.hpp"
#include "ast/Test.hpp"
#include "ast/Literals.hpp"

AST_USING_NAMESPACE
using namespace literals;

TEST(LocalHorizonalFrameTest, ENU_NED)
{
    Rotation rot1;
    Rotation rot2;
    GeodeticPoint point{10_deg, 34_deg, 10_m};
    aGeodeticToENUTransform(point, rot1);
    aGeodeticToNEDTransform(point, rot2);
    Vector3d relPosInInertial{100, 200, 300};
    Vector3d enuPos = rot1.transformVector(relPosInInertial);
    Vector3d nedPos = rot2.transformVector(relPosInInertial);
    printf("enuPos: %f, %f, %f\n", enuPos.x(), enuPos.y(), enuPos.z());
    printf("nedPos: %f, %f, %f\n", nedPos.x(), nedPos.y(), nedPos.z());
    EXPECT_DOUBLE_EQ(enuPos.x(), nedPos.y());
    EXPECT_DOUBLE_EQ(enuPos.y(), nedPos.x());
    EXPECT_DOUBLE_EQ(enuPos.z(), -nedPos.z());
}


TEST(LocalHorizonalFrameTest, lla_ned)
{
    GeodeticPoint point{44.544_deg, -72.814_deg, 1340};
    GeodeticPoint origin{44.532_deg, -72.782_deg, 1699};
    Vector3d ned;
    aGeodeticToNED(point, origin, ned, nullptr);
    printf("ned: %f, %f, %f\n", ned.x(), ned.y(), ned.z());
    EXPECT_NEAR(ned.x(), 1334.3, 0.1);
    EXPECT_NEAR(ned.y(), -2543.6, 0.1);
    EXPECT_NEAR(ned.z(), 359.65, 0.1);
    GeodeticPoint point2;
    aNEDToGeodetic(ned, origin, point2, nullptr);
    printf("point2: %f, %f, %f\n", point2.latitude(), point2.longitude(), point2.altitude());
    EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-6);
    EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-6);
    EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-6);
}


TEST(LocalHorizonalFrameTest, lla_enu)
{
    GeodeticPoint point{45.976_deg, 7.658_deg, 4531};
    GeodeticPoint origin{46.017_deg, 7.750_deg, 1673};
    Vector3d enu;
    aGeodeticToENU(point, origin, enu, nullptr);
    printf("enu: %f, %f, %f\n", enu.x(), enu.y(), enu.z());
    EXPECT_NEAR(enu.x(), -7134.8, 0.1);
    EXPECT_NEAR(enu.y(), -4556.3, 0.1);
    EXPECT_NEAR(enu.z(), 2852.4, 0.1);
    GeodeticPoint point2;
    aENUToGeodetic(enu, origin, point2, nullptr);
    printf("point2: %f, %f, %f\n", point2.latitude(), point2.longitude(), point2.altitude());
    EXPECT_NEAR(point.latitude(), point2.latitude(), 1e-6);
    EXPECT_NEAR(point.longitude(), point2.longitude(), 1e-6);
    EXPECT_NEAR(point.altitude(), point2.altitude(), 1e-6);
}



GTEST_MAIN()

