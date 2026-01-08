///
/// @file      testBKVPSaxPrint.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-08
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

#include "AstTest/Test.h"
#include "AstUtil/BKVSaxPrint.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Color.hpp"
#include "AstUtil/ColorNamed.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include "AstCore/RunTime.hpp"

AST_USING_NAMESPACE

TEST(BKVSaxPrint, SimplePrint)
{
    BKVSaxPrint printer{};
    printer.begin("root");
    {
        printer.keyValue("key", "value");
        printer.keyValue("key2", 123);
        printer.keyValue("key3", true);
        printer.keyValue("key4", 3.14);
        printer.keyValue("key5", Color(eRed));
        printer.begin("group");
        {
            printer.keyValue("key7", "value7");
        }
        printer.end("group");
        printer.keyValue("key6", "value6");
    }
    printer.end("root");
}

TEST(BKVParser, Parser)
{
    {
        BKVParser parser{};
        BKVSaxPrint printer{};
        fs::path filepath = fs::path(aDataDirGet()) / "Test/TestBKV.txt";
        ast_printf("filepath: %s\n", filepath.string().c_str());
        parser.parseFile(filepath.string(), printer);
    }
    if(1)
    {
        BKVParser parser{};
        BKVSaxPrint printer{};
        fs::path filepath = fs::path(aDataDirGet()) / "Test/Satellite.sa3";
        ast_printf("filepath: %s\n", filepath.string().c_str());
        parser.parseFile(filepath.string(), printer);
    }
    if(1)
    {
        BKVParser parser{};
        BKVSaxPrint printer{};
        fs::path filepath = fs::path(aDataDirGet()) / "Test/Satellite.sa";
        ast_printf("filepath: %s\n", filepath.string().c_str());
        parser.parseFile(filepath.string(), printer);
    }
}


GTEST_MAIN()