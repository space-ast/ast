///
/// @file      testScriptComment.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-24
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
#include "AstTest/AstTestMacro.h"
#include "AstUtil/StringView.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstScript/Value.hpp"

AST_USING_NAMESPACE




// 辅助函数：测试双精度浮点数值表达式
void testScriptComment(StringView str, double expectedValue)
{
    printf("testScriptComment: %s\n", str.data());
    SharedPtr<Value> value = aEval(str);
    EXPECT_TRUE(value.get() != nullptr);
    if(aValueIsDouble(value.get())){
        EXPECT_DOUBLE_EQ(aValueUnboxDouble(value.get()), expectedValue);
    }else if(aValueIsBool(value.get())){
        EXPECT_TRUE(aValueUnboxBool(value.get()) == expectedValue);
    }
    // else if(aValueIsString(value.get())){
        //@todo 字符串比较
        //EXPECT_TRUE(aValueUnboxString(value.get()) == expectedValue);
    // }
    else if(aValueIsInt(value.get())){
        EXPECT_EQ(aValueUnboxInt(value.get()), expectedValue);
    }
}

TEST(ScriptComment, Basic)
{
    // 测试#注释
    testScriptComment("1 + 2 # 这是一个注释", 3.0);
    testScriptComment("# 这是一个整行注释\n42", 42.0);
    testScriptComment("10 + # 中间的注释\n20", 30.0);
    
}

GTEST_MAIN()