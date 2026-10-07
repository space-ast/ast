///
/// @file      testFrameWithEpoch.cpp
/// @brief     带轴系历元的坐标系测试
/// @details   测试FrameWithEpoch类的创建和基本功能
/// @author    axel
/// @date      2026-04-11
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见： 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/FrameWithEpoch.hpp"
#include "ast/PointRoot.hpp"
#include "ast/AxesRoot.hpp"
#include "ast/TimePoint.hpp"
#include "ast/AstTestMacro.h"

AST_USING_NAMESPACE

class FrameWithEpochTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
    
    void TearDown() override
    {
    }
};

// 测试基本构造
TEST_F(FrameWithEpochTest, BasicConstruction)
{
    // 使用PointRoot和AxesRoot的Instance()方法获取单例
    Point* origin = PointRoot::Instance();
    Axes* axes = AxesRoot::Instance();
    
    // 创建FrameWithEpoch
    auto frame = FrameWithEpoch::New(origin, axes);
    ASSERT_NE(frame, nullptr);
    
    // 验证基本属性
    EXPECT_EQ(frame->getOrigin(), origin);
    EXPECT_EQ(frame->getAxes(), axes);
    EXPECT_FALSE(frame->getUseEpoch());
    
    delete frame;
}

// 测试带历元的构造
TEST_F(FrameWithEpochTest, ConstructionWithEpoch)
{
    Point* origin = PointRoot::Instance();
    Axes* axes = AxesRoot::Instance();
    
    // 创建TimePoint
    TimePoint tp = TimePoint::FromUTC(2026, 4, 11, 12, 0, 0.0);
    
    // 创建FrameWithEpoch
    auto frame = FrameWithEpoch::New(origin, axes);
    ASSERT_NE(frame, nullptr);
    
    EXPECT_EQ(frame->getOrigin(), origin);
    EXPECT_EQ(frame->getAxes(), axes);
    
    delete frame;
}

// 测试MakeShared
TEST_F(FrameWithEpochTest, MakeSharedConstruction)
{
    Point* origin = PointRoot::Instance();
    Axes* axes = AxesRoot::Instance();
    
    auto frame = FrameWithEpoch::MakeShared(origin, axes);
    ASSERT_NE(frame, nullptr);
    
    EXPECT_EQ(frame->getOrigin(), origin);
    EXPECT_EQ(frame->getAxes(), axes);
}

// 测试UseEpoch设置
TEST_F(FrameWithEpochTest, SetUseEpoch)
{
    Point* origin = PointRoot::Instance();
    Axes* axes = AxesRoot::Instance();
    
    auto frame = FrameWithEpoch::New(origin, axes);
    ASSERT_NE(frame, nullptr);
    
    // 默认不使用历元
    EXPECT_FALSE(frame->getUseEpoch());
    
    // 设置使用历元
    frame->setUseEpoch(true);
    EXPECT_TRUE(frame->getUseEpoch());
    
    // 设置不使用历元
    frame->setUseEpoch(false);
    EXPECT_FALSE(frame->getUseEpoch());
    
    delete frame;
}

// 测试SourceAxes设置
TEST_F(FrameWithEpochTest, SetSourceAxes)
{
    Point* origin = PointRoot::Instance();
    Axes* axes = AxesRoot::Instance();
    
    auto frame = FrameWithEpoch::New(origin, axes);
    ASSERT_NE(frame, nullptr);
    
    // 获取并设置SourceAxes
    Axes* sourceAxes = frame->getSourceAxes();
    EXPECT_NE(sourceAxes, nullptr);
    
    frame->setSourceAxes(axes);
    EXPECT_EQ(frame->getSourceAxes(), axes);
    
    delete frame;
}

// 测试ReferenceAxes
TEST_F(FrameWithEpochTest, ReferenceAxes)
{
    Point* origin = PointRoot::Instance();
    Axes* axes = AxesRoot::Instance();
    
    auto frame = FrameWithEpoch::New(origin, axes);
    ASSERT_NE(frame, nullptr);
    
    // 设置ReferenceAxes
    frame->setReferenceAxes(axes);
    EXPECT_EQ(frame->getReferenceAxes(), axes);
    
    delete frame;
}

GTEST_MAIN()
