///
/// @file      testIdentifier.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-18
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

#include "AstUtil/IdentifierAPI.hpp"
#include "AstUtil/Identifier.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(IdentifierTest, BasicOperations) {
    auto id1 = aIdentifier("testIdentifier");
    auto id2 = aIdentifier(std::string("testIdentifier"));
    auto id3 = aIdentifier("testIdentifier");
    printf("id1: %s, id2: %s, id3: %s\n", id1->c_str(), id2->c_str(), id3->c_str());
    EXPECT_EQ(id1, id2);
    EXPECT_EQ(id2, id3);
    EXPECT_EQ(id3, id1);
}


GTEST_MAIN()