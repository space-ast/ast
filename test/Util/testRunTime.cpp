///
/// @file      testRunTime.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-18
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

#include "AstCore/RunTime.hpp"
#include "AstUtil/StringView.hpp"
#include "AstTest/Test.h"
#include <cstdio>

AST_USING_NAMESPACE

TEST(RunTime, DataDir)
{
    std::string datadir = aDataDir();
    EXPECT_TRUE(datadir.size() > 0);
    const char newdatadir[] = "./data/SolarSystem";
    aDataDirSet(newdatadir);
    datadir = aDataDir();
    printf("datadir: %s\n", datadir.c_str());
    EXPECT_TRUE(datadir == newdatadir);
    datadir = aDataDirGet();
    printf("datadir: %s\n", datadir.c_str());
    EXPECT_TRUE(datadir == newdatadir);
}

GTEST_MAIN()