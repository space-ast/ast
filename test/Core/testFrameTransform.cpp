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

AST_USING_NAMESPACE
using namespace _AST literals;

TEST(FrameTransform, J2000ToMOD)
{
    {    
        TimePoint tp = TimePoint::FromUTC(2026, 1, 7, 12, 0, 0);
        Vector3d vecJ2000{1000_km, 2000_km, 3000_km};
        Vector3d vecMOD;
        aJ2000ToMOD(tp, vecJ2000, vecMOD);
        EXPECT_NEAR(vecMOD[0], 980758.8704899253, 1e-9);
        EXPECT_NEAR(vecMOD[1], 2005.762527176539_km, 1e-9);
        EXPECT_NEAR(vecMOD[2], 3002.503742300700_km, 1e-9);
        printf("vecMOD: %.15f, %.15f, %.15f\n", vecMOD[0], vecMOD[1], vecMOD[2]);
    }
    {
        TimePoint tp = TimePoint::FromUTC(2026, 1, 7, 12, 0, 0);
        Vector3d vecJ2000{3000_km, 2000_km, 1000_km};
        Vector3d vecMOD;
        aJ2000ToMOD(tp, vecJ2000, vecMOD);
        EXPECT_NEAR(vecMOD[0], 2985774.6999846440, 1e-9);
        EXPECT_NEAR(vecMOD[1], 2017.414131093856_km, 1e-9);
        EXPECT_NEAR(vecMOD[2], 1007.566208541370_km, 1e-9);
        printf("vecMOD: %.15f, %.15f, %.15f\n", vecMOD[0], vecMOD[1], vecMOD[2]);
    }
}


GTEST_MAIN()