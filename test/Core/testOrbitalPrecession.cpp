///
/// @file      testOrbitalPrecession.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-27
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

#include "AstCore/OrbitalPrecession.hpp"
#include "AstUtil/Constants.h"
#include "AstTest/Test.hpp"
#include <vector>
#include <tuple>


AST_USING_NAMESPACE

const double EARTH_J2 = 1.0826261e-3;


TEST(OrbitalPrecession, MeanAnomalyRate)
{
    std::vector<std::tuple<double, double, double, double, double, double, double>> testCases = {
        {kEarthGrav, EARTH_J2, kEarthRadius, 4000e3, 0.1, 0.5, 0.00250247672293061},
        {kEarthGrav, EARTH_J2, kEarthRadius, 5000e3, 0.4, 0.2, 0.0017914885718043}
    };
    for(const auto& param : testCases)
    {
        double gm, j2, rb, a, e, i, expect;
        std::tie(gm, j2, rb, a, e, i, expect) = param;
        double meanMotn = aMeanAnomalyRate(gm, j2, rb, a, e, i);
        printf("meanMotn=%.15g\n", meanMotn);
        EXPECT_NEAR(meanMotn, expect, 1e-15);
    }
}


TEST(OrbitalPrecession, RAANRate)
{
    std::vector<std::tuple<double, double, double, double, double, double, double>> testCases = {
        {kEarthGrav, EARTH_J2, kEarthRadius, 5000e3, 0.4, 0.2, -6.57549541625983e-06},
        {kEarthGrav, EARTH_J2, kEarthRadius, 5000e3, 0.1, 0.5, -4.2326427773044e-06},
    };
    for(const auto& param : testCases)
    {
        double gm, j2, rb, a, e, i, expect;
        std::tie(gm, j2, rb, a, e, i, expect) = param;
        double raanRate = aRAANRate(gm, j2, rb, a, e, i);
        printf("raanRate=%.15g\n", raanRate);
        EXPECT_NEAR(raanRate, expect, 1e-15);
    }
}


TEST(OrbitalPrecession, ArgPerRate)
{
    std::vector<std::tuple<double, double, double, double, double, double, double>> testCases = {
        {kEarthGrav, EARTH_J2, kEarthRadius, 5000e3, 0.4, 0.2, 1.275644152659e-05},
        {kEarthGrav, EARTH_J2, kEarthRadius, 5000e3, 0.1, 0.5, 6.87469835954771e-06},
    };
    for(const auto& param : testCases)
    {
        double gm, j2, rb, a, e, i, expect;
        std::tie(gm, j2, rb, a, e, i, expect) = param;
        double argPerRate = aArgPerRate(gm, j2, rb, a, e, i);
        printf("argPerRate=%.15g\n", argPerRate);
        EXPECT_NEAR(argPerRate, expect, 1e-15);
    }
}



GTEST_MAIN()




