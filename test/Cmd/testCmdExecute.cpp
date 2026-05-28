///
/// @file      testCmdExecute.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-12
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

#include "ast/CommandAPI.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/Scenario.hpp"
#include "ast/Sensor.hpp"
#include "ast/Satellite.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE


TEST(CmdExecuteTest, New)
{
    aRemoveAllObjects();
    CommandResult result;

    errc_t rc;
    Scenario* sce = nullptr;
    Satellite* sat = nullptr;
    {
        rc = aExecuteCommand(R"(New / Scenario See_DC)", result);
        EXPECT_EQ(rc, eNoError);
        sce = aFindObject<Scenario*>("See_DC");
        result.debugPrint();
        ASSERT_TRUE(sce != nullptr);
        EXPECT_EQ(sce->getName(), "See_DC");
    }
    {
        rc = aExecuteCommand(R"(New / */Satellite ERS1)", result);
        EXPECT_EQ(rc, eNoError);
        sat = aFindChild<Satellite*>(sce, "ERS1");
        result.debugPrint();
        ASSERT_TRUE(sat != nullptr);
        EXPECT_EQ(sat->getName(), "ERS1");
    }
    {
        rc = aExecuteCommand(R"(New / */Satellite/ERS1/Sensor sens1)", result);
        EXPECT_EQ(rc, eNoError);
        Sensor* sensor = aFindChild<Sensor*>(sat, "sens1");
        result.debugPrint();
        ASSERT_TRUE(sensor != nullptr);
        EXPECT_EQ(sensor->getName(), "sens1");
    }
}

TEST(CmdExecuteTest, DoesObjExist)
{
    aRemoveAllObjects();
    CommandResult result;
    errc_t rc;
    {
        rc = aExecuteCommand(R"(DoesObjExist / */Satellite/ers1)", result);
        EXPECT_EQ(rc, eNoError);
        EXPECT_EQ(result[0], "0");
        result.debugPrint();
    }
    SharedPtr<Scenario> scenario = aNewObject<Scenario>();
    {

        SharedPtr<Satellite> sat = aNewObject<Satellite>(scenario);
        sat->setName("ers1");
        rc = aExecuteCommand(R"(DoesObjExist / */Satellite/ers1)", result);
        EXPECT_EQ(rc, eNoError);
        EXPECT_EQ(result[0], "1");
        result.debugPrint();
    }
}


TEST(CmdExecuteTest, SetStateClassical)
{
    CommandResult result;
    errc_t rc;
    SharedPtr<Scenario> scenario = aNewObject<Scenario>();
    {
        SharedPtr<Satellite> sat = aNewObject<Satellite>(scenario);
        sat->setName("ERS1");
        rc = aExecuteCommand(R"(SetState */Satellite/ERS1 Classical J2Perturbation "1 Nov 2000 00:00:00.00" "1 Nov 2000 04:00:00.00" 60 J2000 "1 Nov 2000 00:00:00.00" 7163000.137079 0.0 98.5 0.0 139.7299 360.0)", result);
        EXPECT_EQ(rc, eNoError);
        for(auto& line : result)
            printf("%s\n", line.c_str());
    }
}


// GTEST_MAIN()

int main(int argc, char **argv) {
    printf("Running main() from %s\n", __FILE__); 
    testing::GTEST_FLAG(catch_exceptions) = false;
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}
