///
/// @file      testRPO.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-08
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

#include "ast/MissionCommandLoader.hpp"
#include "ast/Segment.hpp"
#include "ast/Sequence.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/RunTime.hpp"
#include "ast/EOP.hpp"
#include "ast/Scenario.hpp"
#include "ast/Satellite.hpp"
#include "ast/ObjectLinker.hpp"
#include "ast/Literals.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/MathOperator.hpp"
#include "ast/Test.h"


AST_USING_NAMESPACE
using namespace math;

TEST(RPOTest, Set_Reference_Vehicle)
{
    // GTEST_SKIP();
    SharedPtr<Scenario> scenario = aNewObject<Scenario>();
    SharedPtr<Satellite> rpo = aNewObject<Satellite>(scenario.get());
    rpo->setName("RPO");
    SharedPtr<Satellite> target = aNewObject<Satellite>(scenario.get());
    target->setName("Target");

    errc_t rc;
    std::string filepath = aTestGetConfigValue("STK_RPO_SET_REFERENCE_VEHICLE_FILE").toString();
    printf("loading file: %s\n", filepath.c_str());
    SharedPtr<MissionCommand> missionCommand;
    rc = aLoadMissionCommand(filepath, missionCommand);
    EXPECT_EQ(rc, eNoError);
    rc = aResolveAllLinks();
    EXPECT_EQ(rc, eNoError);
    rc = missionCommand->execute();
    EXPECT_EQ(rc, eNoError);
}


TEST(RPOTest, Set_Initial_State)
{
    SharedPtr<Scenario> scenario = aNewObject<Scenario>();
    SharedPtr<Satellite> rpo = aNewObject<Satellite>(scenario.get());
    rpo->setName("RPO");
    SharedPtr<Satellite> target = aNewObject<Satellite>(scenario.get());
    target->setName("Target");

    errc_t rc;
    std::string filepath = aTestGetConfigValue("STK_RPO_SET_INITIAL_STATE_FILE").toString();
    printf("loading file: %s\n", filepath.c_str());
    SharedPtr<MissionCommand> missionCommand;
    rc = aLoadMissionCommand(filepath, missionCommand);
    EXPECT_EQ(rc, eNoError);
    rc = aResolveAllLinks();
    EXPECT_EQ(rc, eNoError);
    rc = missionCommand->execute();
    EXPECT_EQ(rc, eNoError);
}


TEST(RPOTest, Coast)
{
    errc_t rc;
    std::string filepath = aTestGetConfigValue("STK_RPO_COAST_FILE").toString();
    printf("loading file: %s\n", filepath.c_str());
    SharedPtr<MissionCommand> missionCommand;
    rc = aLoadMissionCommand(filepath, missionCommand);
    EXPECT_EQ(rc, eNoError);
    rc = aResolveAllLinks();
    EXPECT_EQ(rc, eNoError);
    rc = missionCommand->execute();
    EXPECT_EQ(rc, eNoError);
}


// GTEST_MAIN()

int main(int argc, char **argv) {
    if(aIsCI())
        return 0;
    printf("Running main() from %s\n", __FILE__); 
    testing::GTEST_FLAG(catch_exceptions) = false;
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}
