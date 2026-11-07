///
/// @file      testCurlPlugin.cpp
/// @brief     libcurl 网络插件端到端测试
/// @details   验证"扫描插件 → 按名字选择 → 流式收发 → 重定向/HEAD"整条链路。
///            插件由 plugins/AstPluginCURL 构建
/// @author    axel
/// @date      2026-11-04
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "ast/Test.hpp"
#include "ast/Network.hpp"
#include "ast/PluginRegistry.hpp"
#include "ast/Environment.hpp"

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <locale>

AST_USING_NAMESPACE

namespace {

const std::string kHttpTestRoot = "https://postman-echo.com";

enum class CurlPluginStatus
{
    eAvailable,
    eMissing,
    eMissingInCI,
};

/// 插件缺失时跳过；若配置了 libcurl 却在 CI 上找不到插件则视为失败
CurlPluginStatus checkCurlPlugin()
{
    aScanPlugins();

    std::vector<std::string> names;
    aNetworkGetPluginNames(names);
    if (std::find(names.begin(), names.end(), std::string("CURL")) != names.end())
        return CurlPluginStatus::eAvailable;

#ifdef AST_WITH_LIBCURL
    if (aIsCI())
        return CurlPluginStatus::eMissingInCI;
#endif

    return CurlPluginStatus::eMissing;
}

#define REQUIRE_CURL_PLUGIN()                                                    \
    do {                                                                         \
        const CurlPluginStatus curl_status_ = checkCurlPlugin();                 \
        if (curl_status_ == CurlPluginStatus::eMissingInCI)                      \
        {                                                                        \
            EXPECT_TRUE(false) << "libcurl 已配置，但未发现 AstPluginCURL 插件"; \
            return;                                                              \
        }                                                                        \
        if (curl_status_ != CurlPluginStatus::eAvailable)                        \
        {                                                                        \
            GTEST_SKIP() << "未发现 AstPluginCURL 插件，跳过 libcurl 网络测试";  \
        }                                                                        \
    } while (0)

/// 记录回调顺序与内容的接收器
class RecordingReceiver : public NetworkStreamReceiver
{
public:
    errc_t onHeaders(int statusCode,
                     const std::map<std::string, std::string>& headers) override
    {
        ++headerCount;
        lastStatus = statusCode;
        lastHeaders = headers;
        return rejectHeaders ? eErrorCancelled : eNoError;
    }

    errc_t onData(const char* /*data*/, size_t size) override
    {
        if (headerCount == 0)
            dataBeforeHeaders = true;
        bytes += size;
        return eNoError;
    }

    void onError(errc_t error) override
    {
        errorCalled = true;
        lastError = error;
    }

    void onComplete() override { ++completeCount; }

    int headerCount = 0;
    int completeCount = 0;
    int lastStatus = 0;
    size_t bytes = 0;
    bool dataBeforeHeaders = false;
    bool rejectHeaders = false;
    bool errorCalled = false;
    errc_t lastError = eNoError;
    std::map<std::string, std::string> lastHeaders;
};

} // namespace

TEST(CurlPlugin, UnknownName)
{
    EXPECT_EQ(aNetworkSetImplPlugin(""), eErrorNullInput);
    EXPECT_EQ(aNetworkSetImplPlugin("NoSuchPlugin"), eErrorNotFound);
}

TEST(CurlPlugin, SetImpl)
{
    REQUIRE_CURL_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CURL"), eNoError);

    std::vector<std::string> names;
    aNetworkGetPluginNames(names);
    EXPECT_TRUE(std::find(names.begin(), names.end(), std::string("CURL")) != names.end());
}

TEST(CurlPlugin, RequestGet)
{
    REQUIRE_CURL_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CURL"), eNoError);

    NetworkRequest req;
    req.setMethod(ENetworkRequestMethod::eGet);
    req.setUrl(kHttpTestRoot + "/get?foo1=10&foo2=20");

    NetworkResponse res;
    const errc_t rc = aNetworkRequest(req, res);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(res.statusCode(), 200);
    EXPECT_FALSE(res.body().empty());
    EXPECT_TRUE(res.body().find("foo1") != std::string::npos);
}

TEST(CurlPlugin, RequestPost)
{
    REQUIRE_CURL_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CURL"), eNoError);

    NetworkRequest req;
    req.setMethod(ENetworkRequestMethod::ePost);
    req.setUrl(kHttpTestRoot + "/post");
    req.setBody("hello world");

    NetworkResponse res;
    const errc_t rc = aNetworkRequest(req, res);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(res.statusCode(), 200);
    EXPECT_TRUE(res.body().find("hello world") != std::string::npos);
}

TEST(CurlPlugin, StreamingRedirectHeadersOnce)
{
    REQUIRE_CURL_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CURL"), eNoError);

    NetworkRequest req;
    req.setMethod(ENetworkRequestMethod::eGet);
    req.setUrl(kHttpTestRoot + "/redirect-to?url=" + kHttpTestRoot
               + "%2Fget&status_code=302");

    RecordingReceiver receiver;
    const errc_t rc = aNetworkRequestStream(req, receiver);

    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(receiver.headerCount, 1);
    EXPECT_EQ(receiver.lastStatus, 200);
    EXPECT_FALSE(receiver.dataBeforeHeaders);
    EXPECT_GT(receiver.bytes, 0u);
    EXPECT_EQ(receiver.completeCount, 1);
    EXPECT_FALSE(receiver.errorCalled);
}

TEST(CurlPlugin, HeaderRejection)
{
    REQUIRE_CURL_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CURL"), eNoError);

    NetworkRequest req;
    req.setMethod(ENetworkRequestMethod::eGet);
    req.setUrl(kHttpTestRoot + "/get");

    RecordingReceiver receiver;
    receiver.rejectHeaders = true;
    const errc_t rc = aNetworkRequestStream(req, receiver);

    EXPECT_EQ(rc, eErrorCancelled);
    EXPECT_EQ(receiver.headerCount, 1);
    EXPECT_EQ(receiver.bytes, 0u);
    EXPECT_EQ(receiver.completeCount, 0);
}

TEST(CurlPlugin, HeadMethod)
{
    REQUIRE_CURL_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CURL"), eNoError);

    NetworkRequest req;
    req.setMethod(ENetworkRequestMethod::eHead);
    req.setUrl(kHttpTestRoot + "/get");

    RecordingReceiver receiver;
    const errc_t rc = aNetworkRequestStream(req, receiver);

    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(receiver.headerCount, 1);
    EXPECT_EQ(receiver.lastStatus, 200);
    EXPECT_EQ(receiver.bytes, 0u);
    EXPECT_EQ(receiver.completeCount, 1);
}

TEST(CurlPlugin, EmptyUrl)
{
    REQUIRE_CURL_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CURL"), eNoError);

    NetworkRequest req;
    RecordingReceiver receiver;
    EXPECT_EQ(aNetworkRequestStream(req, receiver), eErrorInvalidParam);
}

GTEST_MAIN()
