///
/// @file      testVDFSTK.cpp
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
#include "ast/ScenarioLoader.hpp"
#include "ast/FileSystem.hpp"
#include "ast/Scenario.hpp"
#include "ast/Facility.hpp"
#include "ast/Satellite.hpp"
#include "ast/Sensor.hpp"
#include "ast/RTTIAPI.hpp"
#include <string>

AST_USING_NAMESPACE

const char scenarioPath[] = "./STK/Scenarios/testVDFSTK/testVDFSTK.vdf";

TEST(VDFSTKTest, LoadScenario)
{
    std::string scenarioFullPath = aTestDataDirGet() + "/" + scenarioPath;
    SharedPtr<Scenario> scenario = aMakeShared<Scenario>();
    errc_t rc = aLoadScenario(scenarioFullPath, *scenario);
    ASSERT_EQ(rc, eNoError);
    auto satellites = aFindChildren(scenario, Satellite::StaticType());
    auto facilities = aFindChildren(scenario, Facility::StaticType());
    ASSERT_EQ(satellites.size(), 1);
    ASSERT_EQ(facilities.size(), 1);
    auto sensors = aFindChildren(satellites[0], Sensor::StaticType());
    ASSERT_EQ(sensors.size(), 1);
}

GTEST_MAIN()

