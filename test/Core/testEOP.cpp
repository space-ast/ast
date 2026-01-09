///
/// @file      testEOP.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-08
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

#include "AstCore/EOP.hpp"
#include "AstTest/Test.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/StringView.hpp"
#include "AstCore/JulianDate.hpp"

AST_USING_NAMESPACE

TEST(EOP, Load)
{
    EOP eop;
    err_t err = eop.load(aDataDirGet() + "/SolarSystem/Earth/EOP-All.txt");
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(eop.size() > 0);
    printf("eop size: %zu\n", eop.size());
    EXPECT_TRUE(eop.size() >=  23382 + 181);
}

TEST(EOP, UT1MinusUTC_UTC)
{
    EOP eop;
    err_t err = eop.load(aDataDirGet() + "/SolarSystem/Earth/EOP-All.txt");
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(eop.size() > 0);
    {
        JulianDate jdUTC = JulianDate::FromDateTime(2026, 1, 1, 0, 0, 0.0);
        double ut1_minus_utc = eop.getUT1MinusUTC_UTC(jdUTC);
        printf("ut1_minus_utc: %.15lf\n", ut1_minus_utc);
        EXPECT_TRUE(ut1_minus_utc != 0.0);
        EXPECT_EQ(ut1_minus_utc, 0.0740852);
    }
    {
        JulianDate jdUTC = JulianDate::FromDateTime(2026, 1, 1, 8, 0, 0.0);
        double ut1_minus_utc = eop.getUT1MinusUTC_UTC(jdUTC);
        double expect = 0.07411696666666667;
        printf("ut1_minus_utc: %.18lf\n", ut1_minus_utc);
        printf("expect       : %.18lf\n", expect);
        EXPECT_TRUE(ut1_minus_utc != 0.0);

        EXPECT_NEAR(ut1_minus_utc, expect, 1e-15);
    }
}


GTEST_MAIN();