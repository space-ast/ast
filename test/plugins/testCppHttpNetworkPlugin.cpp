///
/// @file      testCppHttpNetworkPlugin.cpp
/// @brief     cpp-httplib 网络插件端到端测试
/// @details   验证"扫描插件 → 按名字选择 → 流式收发 → 重定向/HEAD"整条链路。
///            插件由 plugins/AstPluginCppHTTP 构建
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

AST_USING_NAMESPACE

namespace {

const std::string kHttpTestRoot = "https://postman-echo.com";

enum class CppHttpPluginStatus
{
    eAvailable,
    eMissing,
    eMissingInCI,
};

/// 插件缺失时跳过；若配置了 cpp-httplib 却在 CI 上找不到插件则视为失败
CppHttpPluginStatus checkCppHttpPlugin()
{
    aScanPlugins();

    // 元数据枚举只说明插件文件存在，真正可用还要求动态库能加载
    if (aPluginRegistry().find<NetworkInterface>("CppHTTP") != nullptr)
        return CppHttpPluginStatus::eAvailable;

#ifdef AST_WITH_CPP_HTTPLIB
    if (aIsCI())
        return CppHttpPluginStatus::eMissingInCI;
#endif

    return CppHttpPluginStatus::eMissing;
}

#define REQUIRE_CPPHTTP_PLUGIN()                                                 \
    do {                                                                         \
        const CppHttpPluginStatus cpphttp_status_ = checkCppHttpPlugin();        \
        if (cpphttp_status_ == CppHttpPluginStatus::eMissingInCI)                \
        {                                                                        \
            EXPECT_TRUE(false)                                                   \
                << "cpp-httplib 已配置，但未发现 AstPluginCppHTTP 插件";         \
            return;                                                              \
        }                                                                        \
        if (cpphttp_status_ != CppHttpPluginStatus::eAvailable)                  \
        {                                                                        \
            GTEST_SKIP()                                                         \
                << "未发现 AstPluginCppHTTP 插件，跳过 cpp-httplib 网络测试";    \
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

TEST(CppHttpPlugin, UnknownName)
{
    EXPECT_EQ(aNetworkSetImplPlugin(""), eErrorNullInput);
    EXPECT_EQ(aNetworkSetImplPlugin("NoSuchPlugin"), eErrorNotFound);
}

TEST(CppHttpPlugin, SetImpl)
{
    REQUIRE_CPPHTTP_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CppHTTP"), eNoError);

    std::vector<std::string> names;
    aNetworkGetPluginNames(names);
    EXPECT_TRUE(std::find(names.begin(), names.end(), std::string("CppHTTP")) != names.end());
}

TEST(CppHttpPlugin, RequestGet)
{
    REQUIRE_CPPHTTP_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CppHTTP"), eNoError);

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

TEST(CppHttpPlugin, RequestPost)
{
    REQUIRE_CPPHTTP_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CppHTTP"), eNoError);

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

TEST(CppHttpPlugin, StreamingRedirectHeadersOnce)
{
    REQUIRE_CPPHTTP_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CppHTTP"), eNoError);

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

TEST(CppHttpPlugin, HeaderRejection)
{
    REQUIRE_CPPHTTP_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CppHTTP"), eNoError);

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

TEST(CppHttpPlugin, HeadMethod)
{
    REQUIRE_CPPHTTP_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CppHTTP"), eNoError);

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

TEST(CppHttpPlugin, EmptyUrl)
{
    REQUIRE_CPPHTTP_PLUGIN();

    EXPECT_EQ(aNetworkSetImplPlugin("CppHTTP"), eNoError);

    NetworkRequest req;
    RecordingReceiver receiver;
    EXPECT_EQ(aNetworkRequestStream(req, receiver), eErrorInvalidParam);
}

GTEST_MAIN()
