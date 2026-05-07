///
/// @file      testScenarioLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-07
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

#include "AstLoader/ScenarioLoader.hpp"
#include "AstSim/Scenario.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/ObjectManager.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(ScenarioLoader, LoadScenario)
{
    if(aIsCI())
        GTEST_SKIP();
    
    // 测试加载场景文件
    auto testLoadScenario = []()
    {
        Scenario scenario;
        StringView scenarioFile = aTestGetConfigValue("STK_SCENARIO_FILE");
        errc_t rc = aLoadScenario(scenarioFile, scenario);
        EXPECT_EQ(rc, eNoError);
        auto objectCount = ObjectManager::CurrentInstance().getObjectCount();
        EXPECT_GT(objectCount, 0);
        aPrintObjectTree(&scenario);
    };
    testLoadScenario();

    // 检查对象是否已经销毁
    {
        auto objectCount1 = ObjectManager::CurrentInstance().getObjectCount();
        testLoadScenario();
        auto objectCount2 = ObjectManager::CurrentInstance().getObjectCount();
        EXPECT_EQ(objectCount2, objectCount1);
        auto objects = ObjectManager::CurrentInstance().getAllObjects();
        EXPECT_EQ(objects.size(), objectCount2);
    }
}

GTEST_MAIN()
