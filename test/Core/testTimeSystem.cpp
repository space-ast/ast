/// @file      testTimeSystem.cpp
/// @brief     测试时间类的功能
/// @details   专门测试Time类和相关函数，包括aTimeFromTotalSecond
/// @author    jinke18
/// @date      05.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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


#include "AstTest/AstTestMacro.h"
#include "AstCore/TimeSystem.hpp"
#include "AstCore/JulianDate.hpp"
#include "AstCore/DateTime.hpp"

AST_USING_NAMESPACE 

TEST(TimeSystem, Datetime_JulianDate)
{
    {
        auto jd = JulianDate::FromDateTime({2025, 12, 5, 12, 0, 0});
        EXPECT_EQ(jd.day(), 2461015.000000000);
        EXPECT_EQ(jd.second(), 0);
        nothing();
    }
    {
        auto dttm = DateTime::FromJD({ 2461015, 0});
        EXPECT_EQ(dttm.year(), 2025);
        EXPECT_EQ(dttm.month(), 12);
        EXPECT_EQ(dttm.day(), 5);
        EXPECT_EQ(dttm.hour(), 12);
        EXPECT_EQ(dttm.minute(), 0);
        EXPECT_EQ(dttm.second(), 0);
        nothing();
    }
    {
        auto dttm = DateTime::FromJD({ 2461015, 0 });
        EXPECT_EQ(dttm.year(), 2025);
        EXPECT_EQ(dttm.month(), 12);
        EXPECT_EQ(dttm.day(), 5);
        EXPECT_EQ(dttm.hour(), 12);
        EXPECT_EQ(dttm.minute(), 0);
        EXPECT_EQ(dttm.second(), 0);
        nothing();
    }

}

TEST(TimeSystem, TT_TAI)
{
    {
        JulianDate jdTT{ 2461015, 0 };
        JulianDate jdTAI;
        aTTToTAI(jdTT, jdTAI);
        nothing();
    }
    {
        DateTime dttmTT{ 2025, 12, 5, 12, 0, 0 };
        DateTime dttmTAI;
        aTTToTAI(dttmTT, dttmTAI);
        JulianDate jdTAI = JulianDate::FromDateTime(dttmTAI);
        auto jdTT = aTAIToTT(jdTAI);
        EXPECT_EQ(jdTT.day(), 2461015.00000000);
        EXPECT_EQ(jdTT.second(), 0);
        nothing();
    }

}

GTEST_MAIN()



