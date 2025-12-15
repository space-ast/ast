///
/// @file      testQuantity.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-15
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

#include "AstUtil/Quantity.hpp"
#include "AstUtil/IO.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE
using namespace _AST units;

TEST(Quantity, Add)
{
    auto q1 = 1 * km;
    auto s1 = q1.toString();
    ast_printf("q1 = %s\n", s1.c_str());
    auto q2 = q1 * m;
    auto s2 = q2.toString();
    ast_printf("q2 = %s\n", s2.c_str());
    EXPECT_TRUE(1 * km == 1000 * m);
    nothing();
}

TEST(Quantity, Sub)
{
    auto q1 = 1 * km;
    auto q2 = 1 * m;
    auto q3 = q1 - q2;
    auto s3 = q3.toString();
    ast_printf("q3 = %s\n", s3.c_str());
    nothing();
}


GTEST_MAIN()