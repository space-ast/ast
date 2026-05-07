///
/// @file      testMissionCommandLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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

#include "AstLoader/MissionCommandLoader.hpp"
#include "AstCore/MissionCommand.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/Segment.hpp"
#include "AstCore/HPOP.hpp"
#include "AstLoader/ValXMLLoader.hpp"
#include "AstLoader/PropagatorLoader.hpp"
#include "AstScript/ValDict.hpp"
#include "AstUtil/ObjectLinker.hpp"
#include "AstTest/Test.h"


AST_USING_NAMESPACE;



TEST(MissionLoaderTest, LoadValue)
{
    SharedPtr<Value> value;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_VALUE_FILES");
    for(auto& file: files){
        printf("loading file: %s\n", file.c_str());
        errc_t rc = aLoadValue(file, value);
        EXPECT_TRUE(value != nullptr);
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
        if(value)
        {
            auto dict = value->toValDict();
            if(dict)
            {
                std::string dictStr = dict->toJsonString(2);
                printf("%s\n", dictStr.c_str());
            }
        }
    }
}

TEST(MissionLoaderTest, LoadInitialState)
{
    InitialState initialState;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_INITIALSTATE_FILES");
    for(auto& file: files){
        printf("loading file: %s\n", file.c_str());
        errc_t rc = aLoadInitialState(file, initialState);
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
    }
}


TEST(MissionLoaderTest, LoadMissionCommand)
{
    SharedPtr<MissionCommand> missionCommand;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_SEQUENCE_FILES");
    for(auto& file: files){
        printf("loading file: %s\n", file.c_str());
        errc_t rc = aLoadMissionCommand(file, missionCommand);
        EXPECT_TRUE(missionCommand != nullptr);
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
    }
}

TEST(MissionLoaderTest, LoadPropagate)
{
    SharedPtr<MissionCommand> missionCommand;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_PROPAGATE_FILES");
    for(auto& file: files){
        printf("loading file: %s\n", file.c_str());
        errc_t rc = aLoadMissionCommand(file, missionCommand);
        EXPECT_TRUE(missionCommand != nullptr);
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
    }
}


TEST(MissionLoaderTest, LoadPropagator)
{
    std::vector<std::string> files = aTestGetConfigStringVector("STK_PROPAGATOR_FILES");
    for(auto& file: files){
        printf("loading file: %s\n", file.c_str());
        HPOP propagator;
        errc_t rc = aLoadPropagator(file, propagator);
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
    }
}


TEST(MissionLoaderTest, LoadHohmannTransfer)
{
    std::string file = aTestGetConfigValue("STK_HOHMANNTRANSFER_FILE").toString();
    {
        errc_t rc;
        printf("loading file: %s\n", file.c_str());
        SharedPtr<MissionCommand> missionCommand;
        rc = aLoadMissionCommand(file, missionCommand);
        EXPECT_EQ(rc, eNoError);
        rc = aResolveAllLinks();
        EXPECT_EQ(rc, eNoError);
        SharedPtr<SpacecraftState> initialState = SpacecraftState::NewDefault();
        initialState->setStateEpoch(TimePoint::FromUTC(2026, 4, 7, 4, 0, 0));
        if(auto segment = aobject_cast<Segment*>(missionCommand.get()))
        {
            segment->setInputState(initialState.get());
        }
        rc = missionCommand->execute();
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
    }
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