///
/// @file      testEncode.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-13
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

#include "ast/Encode.hpp"
#include "ast/IO.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

TEST(EncodeTest, CRTToWide)
{
#ifdef _WIN32
    // setlocale(LC_ALL, ".utf-8");
    wchar_t wstr[] = L"\u4E2D\u6587";
    std::string ansi;
    std::string utf8;
    aWideToAnsi(wstr, ansi);
    aWideToUtf8(wstr, utf8);
    ast_printf("utf8: %s\n", utf8.c_str());
    printf("ansi: %s\n", ansi.c_str());

    EXPECT_EQ(utf8, u8"\u4E2D\u6587");
    std::wstring wide;
    EXPECT_EQ(aCRTToWide(ansi.c_str(), wide), eNoError);
    EXPECT_EQ(wide, L"\u4E2D\u6587");
    std::string crt;
    EXPECT_EQ(aWideToCRT(wstr, crt), eNoError);
    EXPECT_EQ(crt, ansi);
#endif
}


GTEST_MAIN()

