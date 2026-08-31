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

#include "ast/J2Analytical.hpp"
#include "ast/J4Analytical.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/RunTime.hpp"
#include "ast/Literals.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/AstTestMacro.h"

AST_USING_NAMESPACE
using namespace _AST literals;

const double gm = 398600441500000;
const double re = 6378136.3;
const double j2 = 0.00108262617385222271;
const double j4 = -0.0000016198976;
const ModOrbElem initModOrbElem{6678137, 0.0, 28.5_deg, 0.0, 0.0, 0.0};
const double duration = 86400;

TEST(J2Analytical, J2PropEarth)
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

TEST(J2Analytical, J2PropMars)
{
    auto mars = aGetMars();
    ModOrbElem modOrbElem;
    CartState stateMarsInertial{3694_km, 0, 0, 0, 3_km/s, 1.6_km/s};
    aCartToModOrbElem(stateMarsInertial.pos(), stateMarsInertial.vel(), mars->getGM(), modOrbElem);
    printf("modOrbElem: %s\n", modOrbElem.toString().c_str());
    double gm = mars->getGM();
    double j2 = mars->getJ2();
    double rb = mars->getGravityRefDistance(); // 采用引力场参考半径来计算
    printf("gm: %.15g j2: %.15g rb: %.15g\n", gm, j2, rb);
    double duration = 1_day;
    aJ2AnalyticalProp(duration, gm, j2, rb, modOrbElem);
    printf("modOrbElem: %s\n", modOrbElem.toString().c_str());
    EXPECT_NEAR(modOrbElem.raan(),   349.892991934_deg, 1e-5);
    EXPECT_NEAR(modOrbElem.argper(), 196.567566162_deg, 1e-5);
    EXPECT_NEAR(modOrbElem.trueA(),  91.1492978709_deg, 1e-5);
}

TEST(J4Analytical, J4PropEarth)
{
    ModOrbElem modOrbElem = initModOrbElem;
    aJ4AnalyticalProp(duration, gm, j2, j4, re, modOrbElem);
    EXPECT_NEAR(modOrbElem.raan() * kRadToDeg, 352.554857667, 4e-2);
    double u = modOrbElem.trueA() + modOrbElem.argper();
    printf("u: %f\n", u * kRadToDeg);
    printf("raan: %f\n", modOrbElem.raan() * kRadToDeg);
    EXPECT_NEAR(u * kRadToDeg, 344.701, 2e-2);
    
    printf("modOrbElem: %s\n", modOrbElem.toString().c_str());
}



GTEST_MAIN()


