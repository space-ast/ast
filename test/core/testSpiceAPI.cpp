///
/// @file      testSpiceApi.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-22
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

#include "ast/SpiceAPI.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

TEST(SpiceAPI, LoadUnload)
{
    SpiceAPI api;
    const auto libpaths = aTestGetConfigStringVector("CSPICE_LIBS");
    errc_t rc = api.tryload(libpaths);
    EXPECT_FALSE(api.failed());
    EXPECT_EQ(rc, eNoError);
    {
        SpiceAPI api2;
        rc = api2.tryload(libpaths);
        EXPECT_EQ(rc, eNoError);
        EXPECT_FALSE(api2.failed());
    }
    EXPECT_FALSE(api.failed());
}


TEST(SpiceAPI, Instance)
{
    EXPECT_TRUE(SpiceAPI::Instance()->isLoaded());
    bool failed = SpiceAPI::Instance()->failed();
    EXPECT_FALSE(failed);
}


GTEST_MAIN();



