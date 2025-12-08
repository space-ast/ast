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
#include "AstCore/RunTime.hpp"

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



TEST(TimeSystem, TAIToUTC)
{
    aInitialize();
    // 使用已知的闰秒值进行测试
    // 注意：闰秒值可能会更新，需要根据实际情况调整
    
    // 测试JulianDate版本
    JulianDate jdTAI{ 2459000, 0.5 }; // 2020-01-01 12:00:00 TAI
    JulianDate jdUTC;
    aTAIToUTC(jdTAI, jdUTC);
    
    // 验证转换后的时间应该比TAI早37秒（当前闰秒值）
    double diff = (jdTAI - jdUTC).second();
    EXPECT_DOUBLE_EQ(diff, 37.0); // 假设当前闰秒是37秒
    
    // 测试DateTime版本
    DateTime dttmTAI{ 2020, 1, 1, 12, 0, 0 };
    DateTime dttmUTC;
    aTAIToUTC(dttmTAI, dttmUTC);
    
    // 转换为JulianDate进行比较
    JulianDate jdTAI2 = JulianDate::FromDateTime(dttmTAI);
    JulianDate jdUTC2 = JulianDate::FromDateTime(dttmUTC);
    diff = (jdTAI2 - jdUTC2).second();
    EXPECT_DOUBLE_EQ(diff, 37.0); // 假设当前闰秒是37秒
}

TEST(TimeSystem, UTCToTAI)
{
    // 使用已知的闰秒值进行测试
    // 注意：闰秒值可能会更新，需要根据实际情况调整
    
    // 测试JulianDate版本
    JulianDate jdUTC{ 2459000, 0.5 }; // 2020-01-01 12:00:00 UTC
    JulianDate jdTAI;
    aUTCToTAI(jdUTC, jdTAI);
    
    // 验证转换后的时间应该比UTC晚37秒（当前闰秒值）
    double diff = (jdTAI - jdUTC).second();
    EXPECT_DOUBLE_EQ(diff, 37.0); // 假设当前闰秒是37秒
    
    // 测试DateTime版本
    DateTime dttmUTC{ 2020, 1, 1, 12, 0, 0 };
    DateTime dttmTAI;
    aUTCToTAI(dttmUTC, dttmTAI);
    
    // 转换为JulianDate进行比较
    JulianDate jdUTC2 = JulianDate::FromDateTime(dttmUTC);
    JulianDate jdTAI2 = JulianDate::FromDateTime(dttmTAI);
    diff = (jdTAI2 - jdUTC2).second();
    EXPECT_DOUBLE_EQ(diff, 37.0); // 假设当前闰秒是37秒
}

TEST(TimeSystem, UTCToTT)
{
    // UTC -> TAI -> TT
    // UTC到TT的转换应该等于UTC到TAI加上TAI到TT
    
    // 测试JulianDate版本
    JulianDate jdUTC{ 2459000, 0.5 }; // 2020-01-01 12:00:00 UTC
    JulianDate jdTT;
    aUTCToTT(jdUTC, jdTT);
    
    // 手动计算：UTC -> TAI -> TT
    JulianDate jdTAI;
    aUTCToTAI(jdUTC, jdTAI);
    JulianDate jdTTManual;
    aTAIToTT(jdTAI, jdTTManual);
    
    EXPECT_EQ(jdTT.day(), jdTTManual.day());
    EXPECT_NEAR(jdTT.second(), jdTTManual.second(), 1e-12);
    
    // 测试DateTime版本
    DateTime dttmUTC{ 2020, 1, 1, 12, 0, 0 };
    DateTime dttmTT;
    aUTCToTT(dttmUTC, dttmTT);
    
    // 手动计算：UTC -> TAI -> TT
    DateTime dttmTAI;
    aUTCToTAI(dttmUTC, dttmTAI);
    DateTime dttmTTManual;
    aTAIToTT(dttmTAI, dttmTTManual);
    
    // 转换为JulianDate进行比较
    JulianDate jdTT2 = JulianDate::FromDateTime(dttmTT);
    JulianDate jdTTManual2 = JulianDate::FromDateTime(dttmTTManual);
    EXPECT_EQ(jdTT2.day(), jdTTManual2.day());
    EXPECT_NEAR(jdTT2.second(), jdTTManual2.second(), 1e-12);
}

TEST(TimeSystem, TTToUTC)
{
    // TT -> TAI -> UTC
    // TT到UTC的转换应该等于TT到TAI加上TAI到UTC
    
    // 测试JulianDate版本
    JulianDate jdTT{ 2459000, 0.5 }; // 2020-01-01 12:00:00 TT
    JulianDate jdUTC;
    aTTToUTC(jdTT, jdUTC);
    
    // 手动计算：TT -> TAI -> UTC
    JulianDate jdTAI;
    aTTToTAI(jdTT, jdTAI);
    JulianDate jdUTCManual;
    aTAIToUTC(jdTAI, jdUTCManual);
    
    EXPECT_EQ(jdUTC.day(), jdUTCManual.day());
    EXPECT_NEAR(jdUTC.second(), jdUTCManual.second(), 1e-12);
    
    // 测试DateTime版本
    DateTime dttmTT{ 2020, 1, 1, 12, 0, 0 };
    DateTime dttmUTC;
    aTTToUTC(dttmTT, dttmUTC);
    
    // 手动计算：TT -> TAI -> UTC
    DateTime dttmTAI;
    aTTToTAI(dttmTT, dttmTAI);
    DateTime dttmUTCManual;
    aTAIToUTC(dttmTAI, dttmUTCManual);
    
    // 转换为JulianDate进行比较
    JulianDate jdUTC2 = JulianDate::FromDateTime(dttmUTC);
    JulianDate jdUTCManual2 = JulianDate::FromDateTime(dttmUTCManual);
    EXPECT_EQ(jdUTC2.day(), jdUTCManual2.day());
    EXPECT_NEAR(jdUTC2.second(), jdUTCManual2.second(), 1e-12);
}

TEST(TimeSystem, TTToTDB)
{
    // 测试TT到TDB的转换
    // TDB与TT的差值非常小（通常在毫秒级别）
    
    // 测试JulianDate版本
    JulianDate jdTT{ 2459000, 43200 }; // 2020-01-01 12:00:00 TT
    JulianDate jdTDB;
    aTTToTDB(jdTT, jdTDB);
    
    // 计算差值（TDB - TT）
    double diff = aTDBMinusTT(jdTT);
    double diff2 = (jdTDB - jdTT).second();
    
    // 验证两种方式计算的差值是否一致
    EXPECT_NEAR(diff, diff2, 1e-12);
    
    // TDB与TT的差值应该很小
    EXPECT_NEAR(diff, 0.0, 0.01); // 差值应该在10毫秒以内
    
    // 测试DateTime版本
    DateTime dttmTT = DateTime::FromJulianDate(jdTT);
    DateTime dttmTDB;
    aTTToTDB(dttmTT, dttmTDB);
    
    // 转换为JulianDate进行比较
    JulianDate jdTT2 = JulianDate::FromDateTime(dttmTT);
    JulianDate jdTDB2 = JulianDate::FromDateTime(dttmTDB);
    double diff3 = (jdTDB2 - jdTT2).totalSecond();
    
    // 验证与aTDBMinusTT的结果一致
    EXPECT_NEAR(diff3, diff, 1e-11);
}

TEST(TimeSystem, TDBMinusTT)
{
    // 测试TDB与TT的差值计算
    
    // J2000.0时刻的差值
    JulianDate jdJ2000{ (int)kJ2000Epoch, 0 };
    double diff = aTDBMinusTT(jdJ2000);
    
    // 验证差值应该很小
    EXPECT_NEAR(diff, 0.0, 0.01); // 差值应该在10毫秒以内
    
    // 测试不同日期的差值
    JulianDate jd1{ 2459000, 0 }; // 2020-01-01 00:00:00 TT
    JulianDate jd2{ 2460000, 0 }; // 2022-09-22 00:00:00 TT
    
    double diff1 = aTDBMinusTT(jd1);
    double diff2 = aTDBMinusTT(jd2);
    
    // 验证差值随时间变化
    EXPECT_NE(diff1, diff2);
    
    // 验证差值的符号
    // 注意：TDB与TT的差值符号可能随时间变化，这里只是验证计算是否有效
    EXPECT_TRUE(std::abs(diff1) < 0.01);
    EXPECT_TRUE(std::abs(diff2) < 0.01);
}

GTEST_MAIN()



