///
/// @file      testCmdParse.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-11
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
#include "ast/Test.h"


AST_USING_NAMESPACE

TEST(CmdParseTest, Test1)
{
    EXPECT_EQ(aExecuteCommand(R"(Test1 123 "456")"), eNoError);
    EXPECT_EQ(aExecuteCommand(R"(Test1 123 "")"), eNoError);
    EXPECT_EQ(aExecuteCommand(R"( Test1   ""  ""  )"), eNoError);
    // 参数过多
    EXPECT_NE(aExecuteCommand(R"(Test1 123 "456" 789)"), eNoError);
    // 参数不足
    EXPECT_NE(aExecuteCommand(R"(Test1 123)"), eNoError);
}


TEST(CmdParseTest, SetState)
{
    CommandResult result;

    // 参数解析失败
    EXPECT_NE(
        aExecuteCommand(
            R"(SetState */Satellite/ERS1 Cartesian J4Perturbation "1 Nov 2000 00:00:00.00" "1 Nov 2000 08:00:00.00" 60 
            J2000 "1 Nov 2000 00:00:00.00" aaaa 4630000.194365 0.0 712.713627 841.292034 7377.687805)", 
            result
        ), 
        eNoError
    );
    for(const auto& msg : result)
        printf("%s\n", msg.c_str());
}

// GTEST_MAIN()

int main(int argc, char **argv) {
    printf("Running main() from %s\n", __FILE__); 
    testing::GTEST_FLAG(catch_exceptions) = false;
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}

