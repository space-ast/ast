///
/// @file      testGmatStartupFile.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-20
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

#include "ast/StartupConfig.hpp"
#include "ast/RunTime.hpp"
#include "ast/Test.hpp"

AST_USING_NAMESPACE

TEST(StartupConfig, gmat_startup_file)
{
    if(!aIsGithubCI()) GTEST_SKIP();
    const char filepath[] = "data/Test/GMAT/gmat_startup_file.txt";
    StartupConfig config;
    errc_t rc = config.load(filepath);
    EXPECT_EQ(rc, eNoError);
    config.printConfig();
    {
        ValueView value = config.getConfig("EOP_FILE");
        EXPECT_EQ(value.toStringView(), "../data/planetary_coeff/eopc04_08.62-now");
    }
    {
        ValueView value = config.getConfig("LSK_FILE");
        EXPECT_EQ(value.toStringView(), "../data/time/SPICELeapSecondKernel.tls");
    }
    
}

TEST(StartupConfig, ast_startup_file)
{
    const char filepath[] = "data/Test/ast_startup_file.txt";
    errc_t rc = aInitializeByConfig(filepath);
    // EXPECT_EQ(rc, eNoError);
}

GTEST_MAIN()