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


GTEST_MAIN()