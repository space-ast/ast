///
/// @file      testSpaceWeather.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-17
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


#include "AstCore/SpaceWeather.hpp"
#include "AstCore/Date.hpp"
#include "AstTest/Test.hpp"


AST_USING_NAMESPACE

TEST(SpaceWeather, Load)
{
    SpaceWeather sw;
    err_t rc = sw.loadDefault();
    EXPECT_TRUE(rc == eNoError);
    {
        int mjd = aDateToMJD(2026, 2, 12);
        auto entry = sw.getEntry(mjd);
        ASSERT_TRUE(entry != nullptr);
        EXPECT_TRUE(entry->mjd == mjd);
        EXPECT_EQ(entry->F10p7Adj, 125.7);
        EXPECT_EQ(entry->F10p7Obs, 129.0);
    }
    {
        int mjd = aDateToMJD(2026, 2, 18);
        auto entry = sw.getEntry(mjd);
        ASSERT_TRUE(entry != nullptr);
        EXPECT_TRUE(entry->mjd == mjd);
        EXPECT_EQ(entry->F10p7Adj, 180.0);
        EXPECT_EQ(entry->F10p7Obs, 184.3);
    }
    {
        int mjd = aDateToMJD(2041, 9, 1);
        auto entry = sw.getEntry(mjd);
        ASSERT_TRUE(entry != nullptr);
        EXPECT_TRUE(entry->mjd == mjd);
        EXPECT_EQ(entry->F10p7Adj, 70.2);
        EXPECT_EQ(entry->F10p7Obs, 68.9);
    }
    {
        int mjd = aDateToMJD(2041, 10, 1);
        auto entry = sw.getEntry(mjd);
        ASSERT_TRUE(entry != nullptr);
        EXPECT_TRUE(entry->mjd == mjd);
        EXPECT_EQ(entry->F10p7Adj, 70.0);
        EXPECT_EQ(entry->F10p7Obs, 69.8);
    }
}


GTEST_MAIN();


