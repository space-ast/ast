///
/// @file      testSunPosition.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-08
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


#include "ast/AstTestMacro.h"
#include "ast/JplDe.hpp"
#include "ast/TimePoint.hpp"
#include "ast/RunTime.hpp"
#include "ast/Vector.hpp"
#include "ast/Literals.hpp"

AST_USING_NAMESPACE

TEST(SunPosition, Year2000)
{
    aInitialize();
    
    JplDe jplDe;
    errc_t err = jplDe.openDefault();
    ASSERT_FALSE(err) << "Failed to open JPL DE file";
    
    auto time = TimePoint::FromUTC({2000, 1, 1, 0, 0, 0});
    Vector3d pos, vel;
    
    err = jplDe.getPosVelICRF(time, JplDe::eSun, JplDe::eEarth, pos, vel);
    ASSERT_FALSE(err) << "Failed to get sun position";
    
    printf("=== 2000年1月1日 00:00:00 UTC ===\n");
    printf("太阳相对于地球在ICRF下的位置 (km):\n");
    printf("X: %.6f km\n", pos[0] / 1000.0);
    printf("Y: %.6f km\n", pos[1] / 1000.0);
    printf("Z: %.6f km\n", pos[2] / 1000.0);
    printf("\n");
    printf("太阳相对于地球在ICRF下的速度 (km/s):\n");
    printf("VX: %.8f km/s\n", vel[0] / 1000.0);
    printf("VY: %.8f km/s\n", vel[1] / 1000.0);
    printf("VZ: %.8f km/s\n", vel[2] / 1000.0);
    
    EXPECT_TRUE(true);
}

GTEST_MAIN()