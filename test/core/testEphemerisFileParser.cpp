///
/// @file      testEphemerisParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-24
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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


#include "ast/EphemerisFileParser.hpp"
#include "ast/STKEphemerisFileParser.hpp"
#include "ast/EphemerisLagrangeVar.hpp"
#include "ast/RunTime.hpp"
#include "ast/Test.h"


AST_USING_NAMESPACE

class EphemerisFileParserTest : public testing::Test
{
public:
    void SetUp() override
    {
        aInitialize();
    }
    void TearDown() override
    {
        aUninitialize();
    }
};


TEST_F(EphemerisFileParserTest, STKEphemerisFileParser)
{
    STKEphemerisFileParser parser;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_EPHEMERIS_FILES");
    printf("num files: %d\n", (int)files.size());
    for (const auto& file : files)
    {
        printf("parse file: %s\n", file.c_str());
        HEphemeris eph;
        errc_t rc = parser.parse(file, eph);
        EXPECT_TRUE(eph != nullptr);
        EXPECT_FALSE(rc);
        EphemerisLagrangeVar* ephLagrange = dynamic_cast<EphemerisLagrangeVar*>(eph.get());
        EXPECT_TRUE(ephLagrange != nullptr);
        EXPECT_GT(ephLagrange->size(), 0);
        printf("parse file: %s done\n", file.c_str());
    }
}


GTEST_MAIN();

