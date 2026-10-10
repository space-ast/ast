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
#include "ast/TimelinePrefsLoader.hpp"
#include <string>
#include <vector>

AST_USING_NAMESPACE

const char scenarioDir[] = "./STK/Scenarios/testAccessSTK";
const char scenarioAccessFile[] = "./STK/Scenarios/testAccessSTK/testAccess.sca";

// 访问对象名称：两条对象路径各自去掉 "Scenario/<场景名>/" 前缀后把 '/' 换成 '-'，中间用 "-To-" 连接
const char sensorAccessName[] = "Satellite-Satellite1-Sensor-Sensor1-To-Facility-Facility1";
const char satelliteAccessName[] = "Satellite-Satellite1-To-Facility-Facility1";

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
        aFindChild(scenario, Access::StaticType(), sensorAccessName));
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
        aFindChild(scenario, Access::StaticType(), satelliteAccessName));
    ASSERT_NE(satelliteAccess, nullptr);
    ASSERT_NE(satelliteAccess->baseObject(), nullptr);
    ASSERT_NE(satelliteAccess->targetObject(), nullptr);
    EXPECT_EQ(satelliteAccess->baseObject()->getName(), "Satellite1");
    EXPECT_EQ(satelliteAccess->targetObject()->getName(), "Facility1");

    // 显式接口：直接返回本次加载产生的对象列表
    std::vector<HAccess> loaded;
    ASSERT_EQ(aLoadAccess(aTestDataDirGet() + "/" + scenarioAccessFile, *scenario, loaded), eNoError);
    ASSERT_EQ(loaded.size(), 2u);
    EXPECT_EQ(loaded[0]->getName(), sensorAccessName);
    EXPECT_EQ(loaded[1]->getName(), satelliteAccessName);
    EXPECT_EQ(loaded[0]->baseObject(), sensorAccess->baseObject());
}


TEST(AccessSTKTest, Point_GetPosVel)
{
    std::string scenarioFullPath = aTestDataDirGet() + "/" + scenarioDir;
    SharedPtr<Scenario> scenario = aMakeShared<Scenario>();
    errc_t rc = aLoadScenario(scenarioFullPath, *scenario);
    ASSERT_EQ(rc, eNoError);
    auto satellite = aFindChild<Satellite*>(scenario);
    auto sensor = aFindChild<Sensor*>(satellite);
    ASSERT_NE(satellite, nullptr);
    ASSERT_NE(sensor, nullptr);

    TimeInterval interval1;
    satellite->getInterval(interval1);

    TimeInterval interval2;
    sensor->getInterval(interval2);

    printf("interval1: %s\n", interval1.toString().c_str());
    printf("interval2: %s\n", interval2.toString().c_str());
    EXPECT_EQ(interval1, interval2);

    for(const auto& tp: interval1.discretize(600))
    {
        Vector3d pos1, vel1, pos2, vel2;
        errc_t rc = satellite->getPosVel(tp, pos1, vel1);
        ASSERT_EQ(rc, eNoError);
        rc = sensor->getPosVel(tp, pos2, vel2);
        ASSERT_EQ(rc, eNoError);
        EXPECT_EQ(pos1[0], pos2[0]);
        EXPECT_EQ(pos1[1], pos2[1]);
        EXPECT_EQ(pos1[2], pos2[2]);
        EXPECT_EQ(vel1[0], vel2[0]);
        EXPECT_EQ(vel1[1], vel2[1]);
        EXPECT_EQ(vel1[2], vel2[2]);
    }
}


// 执行 Access 计算，并与 TimelinePrefs 中记录的 AccessIntervals 对比
static void CheckAccessAgainstPrefs(Access* access, const std::string& scenarioDir)
{
    ASSERT_NE(access, nullptr);

    // 执行 Access 计算
    errc_t rc = access->compute();
    ASSERT_EQ(rc, eNoError);
    printf("access name: %s\n", access->getName().c_str());

    // 获取计算结果
    auto& intervalList = access->accessIntervals();
    printf("intervalList.size(): %d\n", intervalList.size());
    printf("intervalList:\n%s\n", intervalList.toString().c_str());

    // 加载 TimelinePrefs
    TimelinePrefs prefs;
    rc = aLoadTimeLinePrefs(scenarioDir, prefs);
    ASSERT_EQ(rc, eNoError);

    auto row = prefs.findRowByDisplayName(access->getName() + " AccessIntervals");
    ASSERT_NE(row, nullptr);

    auto& intervals = row->intervals_;
    ASSERT_EQ(intervals.size(), intervalList.size());

    // 对比计算结果
    int i = 0;
    for(const auto& interval : intervalList)
    {
        EXPECT_NEAR(interval.start() - intervals[i].start(), 0, 1e-2);
        EXPECT_NEAR(interval.stop()  - intervals[i].stop(),  0, 1e-2);
        i++;
    }
}

TEST(AccessSTKTest, Case1)
{
    const char scenarioDir_Case1[] = "./STK/Scenarios/testAccessSTK_Case1";
    const std::string scenarioDir = aTestDataDirGet() + "/" + scenarioDir_Case1;
    SharedPtr<Scenario> scenario = aMakeShared<Scenario>();
    errc_t rc = aLoadScenario(scenarioDir, *scenario);
    ASSERT_EQ(rc, eNoError);

    auto satellite = aFindChild<Satellite*>(scenario);
    auto facility = aFindChild<Facility*>(scenario);
    auto sensorSimpleConic = aFindChild<Sensor*>(satellite, "SimpleConic");
    auto sensorRectangular = aFindChild<Sensor*>(satellite, "Rectangular");
    auto sensorComplexConic = aFindChild<Sensor*>(satellite, "ComplexConic");

    ASSERT_NE(satellite, nullptr);
    ASSERT_NE(facility, nullptr);
    ASSERT_NE(sensorSimpleConic, nullptr);
    ASSERT_NE(sensorRectangular, nullptr);
    ASSERT_NE(sensorComplexConic, nullptr);

    // 获取并验证 Satellite1 <-> Facility1 的 Access 计算工具组件
    CheckAccessAgainstPrefs(aFindAccess(*scenario, *satellite, *facility), scenarioDir);

    // 获取并验证 SensorSimpleConic <-> Facility1 的 Access 计算工具组件
    CheckAccessAgainstPrefs(aFindAccess(*scenario, *sensorSimpleConic, *facility), scenarioDir);

    // 获取并验证 SensorRectangular <-> Facility1 的 Access 计算工具组件
    CheckAccessAgainstPrefs(aFindAccess(*scenario, *sensorRectangular, *facility), scenarioDir);

    // 获取并验证 SensorComplexConic <-> Facility1 的 Access 计算工具组件
    CheckAccessAgainstPrefs(aFindAccess(*scenario, *sensorComplexConic, *facility), scenarioDir);

}

GTEST_MAIN()

