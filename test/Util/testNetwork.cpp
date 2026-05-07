///
/// @file      testNetwork.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-24
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Network.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

// const std::string kHttpTestRoot = "https://postman-echo.com";
const std::string kHttpTestRoot = "https://httpbin.org";

TEST(NetworkTest, RequestGet)
{
    NetworkRequest req;
    req.setMethod(ENetworkRequestMethod::eGet);
    req.setUrl(kHttpTestRoot + "/get?foo1=10&foo2=20");

    // 测试curl命令实现
    {
        aNetworkSetImpl(ENetworkImplType::eCurlCmd);
        NetworkResponse res;
        errc_t rc = aNetworkRequest(req, res);
        EXPECT_EQ(res.statusCode(), 200);
        EXPECT_EQ(rc, eNoError);
        printf("body: %s\n", res.body().c_str());
    }

    #ifdef _WIN32
    // 测试WinHTTP实现
    {
        NetworkResponse res;
        aNetworkSetImpl(ENetworkImplType::eWinHTTP);
        errc_t rc = aNetworkRequest(req, res);
        EXPECT_EQ(res.statusCode(), 200);
        EXPECT_EQ(rc, eNoError);
        printf("body: %s\n", res.body().c_str());
    }
    // 测试WinINet实现
    {
        aNetworkSetImpl(ENetworkImplType::eWinINet);
        NetworkResponse res;
        errc_t rc = aNetworkRequest(req, res);
        EXPECT_EQ(res.statusCode(), 200);
        EXPECT_EQ(rc, eNoError);
        printf("body: %s\n", res.body().c_str());
    }
    #endif

}



TEST(NetworkTest, RequestPost)
{
    NetworkRequest req;
    req.setMethod(ENetworkRequestMethod::ePost);
    req.setUrl(kHttpTestRoot + "/post");
    req.setBody("hello world");
    {
        aNetworkSetImpl(ENetworkImplType::eCurlCmd);
        NetworkResponse res;
        errc_t rc = aNetworkRequest(req, res);
        EXPECT_EQ(res.statusCode(), 200);
        EXPECT_EQ(rc, eNoError);
        printf("body: %s\n", res.body().c_str());
    }
    #ifdef _WIN32
    {
        aNetworkSetImpl(ENetworkImplType::eWinHTTP);
        NetworkResponse res;
        errc_t rc = aNetworkRequest(req, res);
        EXPECT_EQ(res.statusCode(), 200);
        EXPECT_EQ(rc, eNoError);
        printf("body: %s\n", res.body().c_str());
    }
    {
        aNetworkSetImpl(ENetworkImplType::eWinINet);
        NetworkResponse res;
        errc_t rc = aNetworkRequest(req, res);
        EXPECT_EQ(res.statusCode(), 200);
        EXPECT_EQ(rc, eNoError);
        printf("body: %s\n", res.body().c_str());
    }

    #endif
}

GTEST_MAIN()

