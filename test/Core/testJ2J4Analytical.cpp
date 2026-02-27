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
#include "AstCore/J4Analytical.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstUtil/Literals.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE
using namespace _AST literals;

const double gm = 398600441500000;
const double re = 6378136.3;
const double j2 = 0.00108262617385222271;
const double j4 = -0.0000016198976;
const ModOrbElem initModOrbElem{6678137, 0.0, 28.5_deg, 0.0, 0.0, 0.0};
const double duration = 86400;

TEST(J2Analytical, J2Prop)
{
    ModOrbElem modOrbElem = initModOrbElem;
    aJ2AnalyticalProp(duration, gm, j2, re, modOrbElem);
    EXPECT_NEAR(modOrbElem.raan() * kRadToDeg, 352.53739173, 1e-2);
    double u = modOrbElem.trueA() + modOrbElem.argper();
    printf("u: %f\n", u * kRadToDeg);
    printf("raan: %f\n", modOrbElem.raan() * kRadToDeg);
    EXPECT_NEAR(u * kRadToDeg, 344.6712, 2e-2);
    printf("modOrbElem: %s\n", modOrbElem.toString().c_str());
}

TEST(J4Analytical, J4Prop)
{
    ModOrbElem modOrbElem = initModOrbElem;
    aJ4AnalyticalProp(duration, gm, j2, j4, re, modOrbElem);
    EXPECT_NEAR(modOrbElem.raan() * kRadToDeg, 352.554857667, 4e-2);
    double u = modOrbElem.trueA() + modOrbElem.argper();
    printf("u: %f\n", u * kRadToDeg);
    printf("raan: %f\n", modOrbElem.raan() * kRadToDeg);
    EXPECT_NEAR(u * kRadToDeg, 344.701, 1e-3);
    
    printf("modOrbElem: %s\n", modOrbElem.toString().c_str());
}



GTEST_MAIN()


