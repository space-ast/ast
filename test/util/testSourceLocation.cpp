///
/// @file      testSourceLocation.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-09
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


#include "ast/SourceLocation.hpp"
#include "ast/Test.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

TEST(SourceLocation, ExtractAfterLastDotDot) {
    constexpr const char* path = extract_after_last_dotdot("abc/../../a/b/c");
    printf("%s\n", path);
    EXPECT_EQ(path, std::string("a/b/c"));
}


GTEST_MAIN()