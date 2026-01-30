///
/// @file      testTimePoint.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-13
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

#include "AstCore/TimePoint.hpp"
#include "AstCore/DateTime.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(TimePoint, BitIntCase)
{
    {
        DateTime dt{2068, 1, 1, 0, 0, 0};
        TimePoint tp = TimePoint::FromUTC(dt);
        DateTime dt2;
        aTimePointToUTC(tp, dt2);

        EXPECT_EQ(dt.year(), dt2.year());
        EXPECT_EQ(dt.month(), dt2.month());
        EXPECT_EQ(dt.day(), dt2.day());
        EXPECT_EQ(dt.hour(), dt2.hour());
        EXPECT_EQ(dt.minute(), dt2.minute());
        EXPECT_EQ(dt.second(), dt2.second());
    }
    {
        DateTime dt{2069, 1, 1, 0, 0, 0};
        TimePoint tp = TimePoint::FromUTC(dt);
        DateTime dt2;
        aTimePointToUTC(tp, dt2);

        EXPECT_EQ(dt.year(), dt2.year());
        EXPECT_EQ(dt.month(), dt2.month());
        EXPECT_EQ(dt.day(), dt2.day());
        EXPECT_EQ(dt.hour(), dt2.hour());
        EXPECT_EQ(dt.minute(), dt2.minute());
        EXPECT_EQ(dt.second(), dt2.second());
    }
    {
        DateTime dt{3070, 3, 1, 0, 0, 0};
        TimePoint tp = TimePoint::FromUTC(dt);
        DateTime dt2;
        aTimePointToUTC(tp, dt2);

        EXPECT_EQ(dt.year(), dt2.year());
        EXPECT_EQ(dt.month(), dt2.month());
        EXPECT_EQ(dt.day(), dt2.day());
        EXPECT_EQ(dt.hour(), dt2.hour());
        EXPECT_EQ(dt.minute(), dt2.minute());
        EXPECT_EQ(dt.second(), dt2.second());
    }
    {
        DateTime dt{99071, 1, 1, 0, 0, 0};
        TimePoint tp = TimePoint::FromUTC(dt);
        DateTime dt2;
        aTimePointToUTC(tp, dt2);

        EXPECT_EQ(dt.year(), dt2.year());
        EXPECT_EQ(dt.month(), dt2.month());
        EXPECT_EQ(dt.day(), dt2.day());
        EXPECT_EQ(dt.hour(), dt2.hour());
        EXPECT_EQ(dt.minute(), dt2.minute());
        EXPECT_EQ(dt.second(), dt2.second());
    }
    {
        DateTime dt{-9072, 1, 1, 0, 0, 0};
        TimePoint tp = TimePoint::FromUTC(dt);
        DateTime dt2;
        aTimePointToUTC(tp, dt2);
        TimePoint tp2 = TimePoint::FromUTC(dt2);

        
        EXPECT_EQ(dt.year(), dt2.year());
        EXPECT_EQ(dt.month(), dt2.month());
        EXPECT_EQ(dt.day(), dt2.day());
        EXPECT_EQ(dt.hour(), dt2.hour());
        EXPECT_EQ(dt.minute(), dt2.minute());
        EXPECT_EQ(dt.second(), dt2.second());

        EXPECT_EQ(tp.daysFromJ2000TT(), tp2.daysFromJ2000TT());
        EXPECT_EQ(tp.integerPart(), tp2.integerPart());
        EXPECT_EQ(tp.fractionalPart(), tp2.fractionalPart());
        
        printf("jd2k1: %lf\njd2k2: %lf\n", tp.daysFromJ2000TT(), tp2.daysFromJ2000TT());
        printf("dt:  %s\ndt2: %s\n", dt.toString().c_str(), dt2.toString().c_str());
    }
}



GTEST_MAIN();