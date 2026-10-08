///
/// @file      testAccessSTK.cpp
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
#include "ast/Access.hpp"
#include "ast/AccessLoader.hpp"
#include "ast/RTTIAPI.hpp"
#include <string>
#include <vector>

AST_USING_NAMESPACE

const char scenarioDir[] = "./STK/Scenarios/testAccessSTK";
const char scenarioAccessFile[] = "./STK/Scenarios/testAccessSTK/testAccess.sca";

TEST(AccessSTKTest, LoadScenario)
{
    std::string scenarioFullPath = aTestDataDirGet() + "/" + scenarioDir;
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

TEST(AccessSTKTest, LoadAccess)
{
    std::string scenarioFullPath = aTestDataDirGet() + "/" + scenarioDir;
    SharedPtr<Scenario> scenario = aMakeShared<Scenario>();
    ASSERT_EQ(aLoadScenario(scenarioFullPath, *scenario), eNoError);

    // .sca 中的两个 BEGIN Access 块
    auto accesses = aFindChildren(scenario, Access::StaticType());
    ASSERT_EQ(accesses.size(), 2);

    // 第一条：Sensor1 <-> Facility1
    auto* sensorAccess = aobject_cast<Access*>(
        aFindChild(scenario, Access::StaticType(), "Sensor1-Facility1"));
    ASSERT_NE(sensorAccess, nullptr);
    ASSERT_NE(sensorAccess->baseObject(), nullptr);
    ASSERT_NE(sensorAccess->targetObject(), nullptr);
    EXPECT_EQ(sensorAccess->baseObject()->getName(), "Sensor1");
    EXPECT_EQ(sensorAccess->targetObject()->getName(), "Facility1");
    EXPECT_TRUE(sensorAccess->config().useLightTimeDelay_);
    EXPECT_EQ(sensorAccess->config().clockHost_, EClockHost::eFirstObject);
    EXPECT_EQ(sensorAccess->config().timeSense_, ETimeSense::eTransmit);
    EXPECT_EQ(sensorAccess->config().aberrationType_, EAberrationType::eAnnual);
    EXPECT_DOUBLE_EQ(sensorAccess->config().maxTimeStep_, 360.0);
    EXPECT_DOUBLE_EQ(sensorAccess->config().minTimeStep_, 0.01);
    EXPECT_DOUBLE_EQ(sensorAccess->config().timeConvergence_, 0.005);
    EXPECT_DOUBLE_EQ(sensorAccess->config().absValueConvergence_, 1e-14);
    EXPECT_DOUBLE_EQ(sensorAccess->config().relValueConvergence_, 1e-08);

    // 第二条：Satellite1 <-> Facility1
    auto* satelliteAccess = aobject_cast<Access*>(
        aFindChild(scenario, Access::StaticType(), "Satellite1-Facility1"));
    ASSERT_NE(satelliteAccess, nullptr);
    ASSERT_NE(satelliteAccess->baseObject(), nullptr);
    ASSERT_NE(satelliteAccess->targetObject(), nullptr);
    EXPECT_EQ(satelliteAccess->baseObject()->getName(), "Satellite1");
    EXPECT_EQ(satelliteAccess->targetObject()->getName(), "Facility1");

    // 显式接口：直接返回本次加载产生的对象列表
    std::vector<HAccess> loaded;
    ASSERT_EQ(aLoadAccess(aTestDataDirGet() + "/" + scenarioAccessFile, *scenario, loaded), eNoError);
    ASSERT_EQ(loaded.size(), 2u);
    EXPECT_EQ(loaded[0]->getName(), "Sensor1-Facility1");
    EXPECT_EQ(loaded[1]->getName(), "Satellite1-Facility1");
    EXPECT_EQ(loaded[0]->baseObject(), sensorAccess->baseObject());
}

GTEST_MAIN()

