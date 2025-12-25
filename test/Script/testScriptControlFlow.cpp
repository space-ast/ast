///
/// @file      testScriptControlFlow.cpp
/// @brief     测试控制流
/// @details   测试控制流相关的脚本
/// @author    jinke18
/// @date      2025-12-25
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstScript/ScriptAPI.hpp"
#include "AstScript/Interpreter.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE

class ScriptControlFlowTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试环境
        aScript_SetInterpreter(&interpreter);
    }

    void TearDown() override {
        // 清理测试环境
        aScript_RemoveInterpreter(&interpreter);
    }
protected:
    Interpreter interpreter;
};

void testScriptControlFlowDouble(const char* script, double expect)
{
    printf("script: %s\n", script);
    Value* val = aEval(script);
    ASSERT_TRUE(val != nullptr);
    ASSERT_TRUE(aValueIsDouble(val));
    double result = aValueUnboxDouble(val);
    ASSERT_EQ(result, expect);
}


/// @test      测试if-else条件语句
TEST_F(ScriptControlFlowTest, TestIfElse) {
    // 测试if-else条件语句
    const char* script1 = "if true\n    a = 0\nelse\n    a = 1\nend";
    EXPECT_TRUE(aEval(script1) != nullptr);

    const char* script2 = R"(
if false
    a = 1
elseif false
    a = 0
else
    b = 122
end
a = 12.0
# true ? a : b

)";
    testScriptControlFlowDouble(script2, 12.0);

    const char* script3 = R"(
if false
    a = 1.0
elseif false
    a = 0
else
    b = 12.0
end

true ? b : a

)";
    testScriptControlFlowDouble(script3, 12.0);
}



GTEST_MAIN()