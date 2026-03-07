///
/// @file      testValueView.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-07
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "AstUtil/ValueView.hpp"
#include "AstTest/Test.h"


AST_USING_NAMESPACE

TEST(ValueViewTest, toVector)
{
    {
        ValueView value("");
        auto vec = value.toVector();
        EXPECT_EQ(vec.size(), 1);
    }
    {
        ValueView value(" ");
        auto vec = value.toVector();
        EXPECT_EQ(vec.size(), 1);
        EXPECT_EQ(vec[0].value(), "");
    }
    {
        ValueView value("12345");
        auto vec = value.toVector();
        EXPECT_EQ(vec.size(), 1);
        EXPECT_EQ(vec[0].value(), "12345");
    }
    {
        ValueView value(" 12.34");
        auto vec = value.toVector();
        EXPECT_EQ(vec.size(), 1);
        EXPECT_EQ(vec[0].value(), "12.34");
    }
    {
        ValueView value("a, b, c");
        auto vec = value.toVector();
        EXPECT_EQ(vec.size(), 3);
        EXPECT_EQ(vec[0].value(), "a");
        EXPECT_EQ(vec[1].value(), "b");
        EXPECT_EQ(vec[2].value(), "c");
    }
    {
        ValueView value("a b , c \n d");
        auto vec = value.toVector();
        EXPECT_EQ(vec.size(), 4);
        EXPECT_EQ(vec[0].value(), "a");
        EXPECT_EQ(vec[1].value(), "b");
        EXPECT_EQ(vec[2].value(), "c");
        EXPECT_EQ(vec[3].value(), "d");
    }
    {
        ValueView value(" a b ,, c \n d\n");
        auto vec = value.toVector();
        EXPECT_EQ(vec.size(), 5);
        EXPECT_EQ(vec[0].value(), "a");
        EXPECT_EQ(vec[1].value(), "b");
        EXPECT_EQ(vec[2].value(), "");
        EXPECT_EQ(vec[3].value(), "c");
        EXPECT_EQ(vec[4].value(), "d");
    }
    {
        ValueView value(" ( a b ,12.34D3, c \n d\n) ");
        auto vec = value.toVector();
        EXPECT_EQ(vec.size(), 5);
        EXPECT_EQ(vec[0].value(), "a");
        EXPECT_EQ(vec[1].value(), "b");
        EXPECT_EQ(vec[2].value(), "12.34D3");
        EXPECT_EQ(vec[3].value(), "c");
        EXPECT_EQ(vec[4].value(), "d");
    }
}


GTEST_MAIN();

