///
/// @file      testTimelinePrefsLoader.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-08
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/Test.hpp"
#include "ast/TimelinePrefsLoader.hpp"
#include "ast/FileSystem.hpp"
#include <string>

AST_USING_NAMESPACE

const char timelinePrefsPath[] = "./test-data/STK/Scenarios/testAccessSTK/testAccessTimelinePrefs.xml";

TEST(TimelinePrefsLoaderTest, Load)
{
    std::string filePath = aExeDir() + "/" + timelinePrefsPath;
    TimelinePrefs prefs;
    errc_t rc = aLoadTimeLinePrefs(filePath, prefs);
    ASSERT_EQ(rc, eNoError);

    // 内容视图中的两行：场景可用性 + 卫星传感器对设施的访问
    ASSERT_EQ(prefs.rows_.size(), 2u);

    const TimelinePrefs::Row& availability = prefs.rows_[0];
    EXPECT_EQ(availability.componentName_, "AvailabilityIntervals");
    EXPECT_EQ(availability.className_, "Scenario");
    EXPECT_EQ(availability.type_, "IntervalList");
    EXPECT_EQ(availability.intervals_.size(), 1u);

    const TimelinePrefs::Row& access = prefs.rows_[1];
    EXPECT_EQ(access.componentName_, "AccessIntervals");
    EXPECT_EQ(access.className_, "Access");
    EXPECT_EQ(access.intervals_.size(), 17u);
    // Path 是区间归属的唯一标识，必须保留
    EXPECT_FALSE(access.path_.empty());

    // 时间视图的全局分析时段：8 Oct 2026 04:00 ~ 12 Oct 2026 04:00
    EXPECT_FALSE(prefs.globalInterval_.isEmpty());
    EXPECT_DOUBLE_EQ(prefs.globalInterval_.duration(), 4.0 * 86400.0);

    // 可用性行与全局分析时段一致
    ASSERT_EQ(availability.intervals_.size(), 1u);
    EXPECT_DOUBLE_EQ(availability.intervals_[0].duration(), prefs.globalInterval_.duration());
}

GTEST_MAIN()
