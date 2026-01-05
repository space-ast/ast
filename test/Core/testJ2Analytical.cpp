///
/// @file      testJ2Analytical.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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

#include "AstCore/J2Analytical.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstUtil/Literals.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE
using namespace _AST literals;

TEST(J2Analytical, Prop)
{
    double gm = 398600441500000;
    double re = 6378135.3;
    double j2 = 0.00108263;
    ModOrbElem modOrbElem{6678137, 0.0, 28.5_deg, 0.0, 0.0, 0.0};
    double duration = 86400;
    aJ2AnalyticalProp(duration, gm, j2, re, modOrbElem);
    EXPECT_NEAR(modOrbElem.raan(), 352.53739173_deg, 1e-3);

    printf("modOrbElem: %s\n", modOrbElem.toString().c_str());
}



GTEST_MAIN()


