///
/// @file      testObjectComponentLoader.cpp
/// @brief     测试对象扩展组件（AccessConstraints）加载
/// @author    axel
/// @date      2026-10-09
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

#include "ast/Facility.hpp"
#include "ast/ObjectAccessConstraints.hpp"
#include "ast/ObjectComponent.hpp"
#include "ast/FacilityLoader.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/RunTime.hpp"
#include "ast/Test.h"
#include "ast/TestConfig.hpp"
#include "ast/StringUtil.hpp"
#include <string>


AST_USING_NAMESPACE

TEST(ObjectComponentLoaderTest, LoadFacilityAccessConstraints)
{
    aInitialize();

    std::string file = aTestDataDirGet() + "/STK/Scenarios/testAccessSTK/Facility1.f";
    Facility facility;
    errc_t rc = aLoadFacility(file, facility);
    EXPECT_EQ(rc, eNoError);

    auto* constraints = aFindChild<ObjectAccessConstraints*>(&facility);
    ASSERT_NE(constraints, nullptr);
    const auto& list = constraints->constraints();
    ASSERT_EQ(list.size(), 3u);

    // LineOfSight IncludeIntervals
    EXPECT_TRUE(list[0]->type() == EAccessConstraint::eLineOfSight);
    EXPECT_TRUE(list[0]->enabled());
    EXPECT_FALSE(list[0]->exclude());
    EXPECT_FALSE(list[0]->useMin());
    EXPECT_FALSE(list[0]->useMax());

    // ElevationAngle Min 0.0 Max 50.0 IncludeIntervals
    EXPECT_TRUE(list[1]->type() == EAccessConstraint::eElevationAngle);
    EXPECT_TRUE(list[1]->enabled());
    EXPECT_FALSE(list[1]->exclude());
    EXPECT_TRUE(list[1]->useMin());
    EXPECT_TRUE(list[1]->useMax());
    EXPECT_NEAR(list[1]->min(), 0.0, 1e-12);
    EXPECT_NEAR(list[1]->max(), 50.0, 1e-12);

    // Range Min 4.0e+04 ExcludeIntervals
    EXPECT_TRUE(list[2]->type() == EAccessConstraint::eRange);
    EXPECT_TRUE(list[2]->enabled());
    EXPECT_FALSE(list[2]->exclude());
    EXPECT_TRUE(list[2]->useMin());
    EXPECT_FALSE(list[2]->useMax());
    EXPECT_NEAR(list[2]->min(), 4.0e4, 1e-6);
}


GTEST_MAIN();

