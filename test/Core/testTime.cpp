/// @file      testTime.cpp
/// @brief     测试时间类的功能
/// @details   专门测试Time类和相关函数，包括aTimeFromTotalSecond
/// @author    axel
/// @date      03.12.2025
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


#include "AstCore/Time.hpp"
#include "AstTest/AstTestMacro.h"
#include <cstdio>

TEST(Time, BasicConstructor) 
{
    AST_USING_NAMESPACE
    
    // 测试默认构造
    Time time;
    time.hour() = 10;
    time.minute() = 30;
    time.second() = 45.5;
    EXPECT_EQ(time.hour(), 10);
    EXPECT_EQ(time.minute(), 30);
    EXPECT_DOUBLE_EQ(time.second(), 45.5);
}

TEST(Time, TotalSecond) 
{
    AST_USING_NAMESPACE
    
    // 测试totalSecond方法
    Time time;
    time.hour() = 1;
    time.minute() = 2;
    time.second() = 3.456;
    EXPECT_DOUBLE_EQ(time.totalSecond(), 3723.456); // 1*3600 + 2*60 + 3.456
    
    // 测试零值
    time.hour() = 0;
    time.minute() = 0;
    time.second() = 0;
    EXPECT_DOUBLE_EQ(time.totalSecond(), 0);
    
    // 测试最大值
    time.hour() = 23;
    time.minute() = 59;
    time.second() = 59.999;
    EXPECT_DOUBLE_EQ(time.totalSecond(), 86399.999); // 23*3600 + 59*60 + 59.999
}

TEST(Time, DayFraction) 
{
    AST_USING_NAMESPACE
    
    // 测试dayFraction方法
    Time time;
    time.hour() = 12;
    time.minute() = 0;
    time.second() = 0;
    EXPECT_DOUBLE_EQ(time.dayFraction(), 0.5); // 12/24 = 0.5
    
    // 测试零值
    time.hour() = 0;
    time.minute() = 0;
    time.second() = 0;
    EXPECT_DOUBLE_EQ(time.dayFraction(), 0);
    
    // 测试最大值
    time.hour() = 23;
    time.minute() = 59;
    time.second() = 59.999;
    EXPECT_NEAR(time.dayFraction(), 1.0, 1e-7); // 接近1但不等于1
}

TEST(Time, aTimeFromTotalSecond) 
{
    AST_USING_NAMESPACE
    
    Time time;
    
    // 测试正常情况
    aTimeFromTotalSecond(3723.456, time);
    EXPECT_EQ(time.hour(), 1);
    EXPECT_EQ(time.minute(), 2);
    EXPECT_NEAR(time.second(), 3.456, 1e-8);
    
    // 测试边界情况 - 零值
    aTimeFromTotalSecond(0, time);
    EXPECT_EQ(time.hour(), 0);
    EXPECT_EQ(time.minute(), 0);
    EXPECT_DOUBLE_EQ(time.second(), 0);
    
    // 测试边界情况 - 24小时整（应被限制在23小时）
    aTimeFromTotalSecond(86400.0, time);
    EXPECT_EQ(time.hour(), 23); // 使用min函数限制为23
    EXPECT_EQ(time.minute(), 59); // 86400 - 23*3600 = 3600, 3600/60 = 60, 但会被min限制为59
    EXPECT_DOUBLE_EQ(time.second(), 60.0); // 60 - 59*60 = 60.0
    
    // 测试边界情况 - 超过24小时很多
    aTimeFromTotalSecond(99999.999, time);
    EXPECT_EQ(time.hour(), 23); // 使用min函数限制为23
    EXPECT_EQ(time.minute(), 59); // 99999.999 - 23*3600 = 99999.999 - 82800 = 17199.999
    // 17199.999 / 60 = 286.66665, 取整为286分钟, 但会被min限制为59
    EXPECT_DOUBLE_EQ(time.second(), 17199.999 - 59*60); // 17199.999 - 3540 = 13659.999
    
    // 测试特殊情况 - 带小数的秒数
    aTimeFromTotalSecond(3661.123, time);
    EXPECT_EQ(time.hour(), 1); // 3661.123 / 3600 = 1.016978611...
    EXPECT_EQ(time.minute(), 1); // 3661.123 - 3600 = 61.123, 61.123 / 60 = 1.018716...
    EXPECT_NEAR(time.second(), 1.123, 1e-8); // 61.123 - 60 = 1.123
    
    // 测试特殊情况 - 接近小时边界
    aTimeFromTotalSecond(3599.999, time);
    EXPECT_EQ(time.hour(), 0); // 3599.999 / 3600 < 1
    EXPECT_EQ(time.minute(), 59); // 3599.999 / 60 = 59.999983...
    EXPECT_NEAR(time.second(), 59.999, 1e-8); // 3599.999 - 59*60 = 3599.999 - 3540 = 59.999
    
    // 测试特殊情况 - 接近分钟边界
    aTimeFromTotalSecond(119.999, time);
    EXPECT_EQ(time.hour(), 0);
    EXPECT_EQ(time.minute(), 1); // 119.999 / 60 = 1.999983...
    EXPECT_DOUBLE_EQ(time.second(), 59.999); // 119.999 - 60 = 59.999
    
    // 测试特殊情况 - 负值（函数没有明确处理负值，但测试其行为）
    aTimeFromTotalSecond(-1, time);
    EXPECT_EQ(time.hour(), 0); // floor(-1/3600) = -1, 但会被min限制为0（因为-1 < 23，但可能不是预期行为）
    EXPECT_EQ(time.minute(), 0); // -1 - 0*3600 = -1, floor(-1/60) = -1, 但会被min限制为0
    EXPECT_DOUBLE_EQ(time.second(), -1); // -1 - 0*60 = -1
}

TEST(Time, aTimeNormalize) 
{
    AST_USING_NAMESPACE
    
    Time time;
    
    // 测试基本规范化 - 秒进位
    time.hour() = 0;
    time.minute() = 0;
    time.second() = 61.5;
    aTimeNormalize(time);
    EXPECT_EQ(time.hour(), 0);
    EXPECT_EQ(time.minute(), 1);
    EXPECT_DOUBLE_EQ(time.second(), 1.5);
    
    // 测试基本规范化 - 分钟进位
    time.hour() = 0;
    time.minute() = 61;
    time.second() = 0;
    aTimeNormalize(time);
    EXPECT_EQ(time.hour(), 1);
    EXPECT_EQ(time.minute(), 1);
    EXPECT_DOUBLE_EQ(time.second(), 0);
    
    // 测试基本规范化 - 多级进位
    time.hour() = 23;
    time.minute() = 59;
    time.second() = 61.5;
    aTimeNormalize(time);
    EXPECT_EQ(time.hour(), 24); // 注意：该函数不会进位到日期
    EXPECT_EQ(time.minute(), 0);
    EXPECT_DOUBLE_EQ(time.second(), 1.5);
    
    // 测试基本规范化 - 负秒数
    time.hour() = 1;
    time.minute() = 0;
    time.second() = -1.5;
    aTimeNormalize(time);
    EXPECT_EQ(time.hour(), 0);
    EXPECT_EQ(time.minute(), 59);
    EXPECT_DOUBLE_EQ(time.second(), 58.5);
    
    // 测试normalize方法
    time.hour() = 0;
    time.minute() = 0;
    time.second() = 61.5;
    time.normalize();
    EXPECT_EQ(time.hour(), 0);
    EXPECT_EQ(time.minute(), 1);
    EXPECT_DOUBLE_EQ(time.second(), 1.5);
    
    // 测试normalized方法
    Time time2;
    time2.hour() = 0;
    time2.minute() = 0;
    time2.second() = 61.5;
    Time time3 = time2.normalized();
    EXPECT_EQ(time2.hour(), 0); // 原对象不变
    EXPECT_EQ(time2.minute(), 0);
    EXPECT_DOUBLE_EQ(time2.second(), 61.5);
    EXPECT_EQ(time3.hour(), 0); // 新对象已规范化
    EXPECT_EQ(time3.minute(), 1);
    EXPECT_DOUBLE_EQ(time3.second(), 1.5);
}

GTEST_MAIN()