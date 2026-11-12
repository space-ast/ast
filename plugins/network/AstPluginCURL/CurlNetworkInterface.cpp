///
/// @file      CurlNetworkInterface.cpp
/// @brief     基于 libcurl 的网络实现
/// @details   使用 libcurl easy 接口逐块转发响应体，并保证 onHeaders 只交付一次。
/// @author    axel
/// @date      2026-11-04
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

#include <curl/curl.h>

#include "CurlNetworkInterface.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Network.hpp"
#include "AstUtil/StringUtil.hpp"

#include <cctype>
#include <cstdlib>
#include <map>
#include <mutex>
#include <string>

// 旧版 curl.h 没有这个宏；那些版本也不会对 deprecated 选项告警。
#ifndef CURL_IGNORE_DEPRECATION
#   define CURL_IGNORE_DEPRECATION(statements) statements
#endif

AST_NAMESPACE_BEGIN

namespace {

/// @brief libcurl 全局初始化的守卫
/// @details 故意不在析构时调用 curl_global_cleanup()：插件库在进程退出阶段才被
///          卸载，此时 Winsock/CRT 可能已经开始 teardown，在 DLL detach 中清理
///          libcurl 全局状态会导致访问违例。libcurl 允许省略 cleanup。
class CurlRuntime
{
public:
    CurlRuntime()
        : initialized_(curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK)
    {}

    CurlRuntime(const CurlRuntime&) = delete;
    CurlRuntime& operator=(const CurlRuntime&) = delete;

    bool initialized() const { return initialized_; }

private:
    bool initialized_;
};

/// 进程内只初始化一次
CurlRuntime& curlRuntime()
{
    static CurlRuntime runtime;
    return runtime;
}

/// @brief CURL easy handle 的 RAII 包装
class CurlHandle
{
public:
    CurlHandle()
        : handle_(curl_easy_init())
    {}

    ~CurlHandle()
    {
        if (handle_ != nullptr)
            curl_easy_cleanup(handle_);
    }

    CurlHandle(const CurlHandle&) = delete;
    CurlHandle& operator=(const CurlHandle&) = delete;

    CURL* get() const { return handle_; }

private:
    CURL* handle_;
};

/// @brief 请求头链表的 RAII 包装
class CurlHeaderList
{
public:
    CurlHeaderList() = default;

    ~CurlHeaderList()
    {
        if (list_ != nullptr)
            curl_slist_free_all(list_);
    }

    CurlHeaderList(const CurlHeaderList&) = delete;
    CurlHeaderList& operator=(const CurlHeaderList&) = delete;

    bool append(const std::string& line)
    {
        curl_slist* next = curl_slist_append(list_, line.c_str());
        if (next == nullptr)
            return false;
        list_ = next;
        return true;
    }

    curl_slist* get() const { return list_; }

private:
    curl_slist* list_ = nullptr;
};

/// @brief 单次请求的接收上下文
struct CurlRequestContext
{
    explicit CurlRequestContext(NetworkStreamReceiver& receiver)
        : receiver(&receiver),
          statusCode(0),
          headersDelivered(false),
          receiverError(eNoError),
          headers()
    {}

    CurlRequestContext(const CurlRequestContext&) = delete;
    CurlRequestContext& operator=(const CurlRequestContext&) = delete;

    NetworkStreamReceiver* receiver;
    int statusCode;
    bool headersDelivered;
    errc_t receiverError;
    std::map<std::string, std::string> headers;
};

const char* methodToString(ENetworkRequestMethod method)
{
    switch (method)
    {
    case ENetworkRequestMethod::eGet:     return "GET";
    case ENetworkRequestMethod::eHead:    return "HEAD";
    case ENetworkRequestMethod::ePost:    return "POST";
    case ENetworkRequestMethod::ePut:     return "PUT";
    case ENetworkRequestMethod::eDelete:  return "DELETE";
    case ENetworkRequestMethod::eOptions: return "OPTIONS";
    case ENetworkRequestMethod::eTrace:   return "TRACE";
    case ENetworkRequestMethod::eConnect: return "CONNECT";
    case ENetworkRequestMethod::ePatch:   return "PATCH";
    default:                              return "GET";
    }
}

#if defined(__linux__)
/// URL 是否以 https:// 开头（大小写不敏感）
bool isHttpsUrl(const std::string& url)
{
    static const char prefix[] = "https://";
    const std::string::size_type len = sizeof(prefix) - 1;
    return aEqualsIgnoreCase(StringView(url).substr(0, len), prefix);
}
#endif

std::string trim(const std::string& value)
{
    std::string::size_type begin = 0;
    while (begin < value.size() &&
           std::isspace(static_cast<unsigned char>(value[begin])) != 0)
    {
        ++begin;
    }

    std::string::size_type end = value.size();
    while (end > begin &&
           std::isspace(static_cast<unsigned char>(value[end - 1])) != 0)
    {
        --end;
    }

    return value.substr(begin, end - begin);
}

/// 去掉 CR/LF，防止请求头注入
std::string sanitizeHeaderPart(const std::string& value)
{
    std::string result;
    result.reserve(value.size());
    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
    {
        if (*it != '\r' && *it != '\n')
            result.push_back(*it);
    }
    return result;
}

/// @brief 解析响应头；重定向链中的后一块头会覆盖前一块
size_t headerCallback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    const size_t total = size * nitems;
    CurlRequestContext* context = static_cast<CurlRequestContext*>(userdata);

    std::string line(buffer, total);
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
        line.pop_back();

    if (line.compare(0, 5, "HTTP/") == 0)
    {
        // 新的状态行：重置当前响应的状态码与响应头
        context->headers.clear();
        context->statusCode = 0;
        const std::string::size_type pos = line.find(' ');
        if (pos != std::string::npos)
            context->statusCode = std::atoi(line.c_str() + pos + 1);
    }
    else if (!line.empty())
    {
        const std::string::size_type colon = line.find(':');
        if (colon != std::string::npos)
        {
            const std::string key = trim(line.substr(0, colon));
            if (!key.empty())
                context->headers[key] = trim(line.substr(colon + 1));
        }
    }

    return total;
}

/// @brief 收到第一块响应体时交付响应头，然后逐块转发数据
size_t writeCallback(char* ptr, size_t size, size_t nitems, void* userdata)
{
    const size_t total = size * nitems;
    CurlRequestContext* context = static_cast<CurlRequestContext*>(userdata);

    if (!context->headersDelivered)
    {
        context->headersDelivered = true;
        const errc_t rc = context->receiver->onHeaders(context->statusCode,
                                                       context->headers);
        if (rc != eNoError)
        {
            context->receiverError = rc;
            return 0;  // 中止传输
        }
    }

    const errc_t rc = context->receiver->onData(ptr, total);
    if (rc != eNoError)
    {
        context->receiverError = rc;
        return 0;  // 中止传输
    }

    return total;
}

errc_t mapCurlError(CURLcode code)
{
    switch (code)
    {
    case CURLE_OK:
        return eNoError;
    case CURLE_UNSUPPORTED_PROTOCOL:
        return eErrorUnsupported;
    case CURLE_URL_MALFORMAT:
    case CURLE_BAD_FUNCTION_ARGUMENT:
        return eErrorInvalidParam;
    case CURLE_ABORTED_BY_CALLBACK:
        return eErrorCancelled;
    default:
        return eError;
    }
}

} // namespace

bool CurlNetworkInterface::isSupported() const
{
    return curlRuntime().initialized() &&
           curl_version_info(CURLVERSION_NOW) != nullptr;
}

errc_t CurlNetworkInterface::requestStream(const NetworkRequest& request,
                                           NetworkStreamReceiver& receiver)
{
    if (request.url().empty())
        return eErrorInvalidParam;

    if (!curlRuntime().initialized())
    {
        aError(_("libcurl 初始化失败"));
        return eErrorNotInit;
    }

    CurlHandle handle;
    CURL* curl = handle.get();
    if (curl == nullptr)
        return eErrorNotInit;

    char errorBuffer[CURL_ERROR_SIZE] = {};
    CurlRequestContext context(receiver);

    curl_easy_setopt(curl, CURLOPT_URL, request.url().c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 20L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
#if defined(__linux__)
    // 空串表示没找到，保留 libcurl 默认 CA 配置。
    // 变量必须活到 perform 之后，不要直接传 aGetCaCertFile().c_str()。
    std::string caCertFile;
    std::string caCertDir;
    if (isHttpsUrl(request.url()))
    {
        caCertFile = aGetCaCertFile();
        if (!caCertFile.empty())
            curl_easy_setopt(curl, CURLOPT_CAINFO, caCertFile.c_str());
        caCertDir = aGetCaCertDir();
        if (!caCertDir.empty())
            curl_easy_setopt(curl, CURLOPT_CAPATH, caCertDir.c_str());
    }
#endif
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &context);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &context);

    // 只允许 http/https（含重定向）。
    // 插件运行时加载，实际链接的 libcurl 可能比编译头文件老，因此按运行期版本选择接口；
    // 位掩码选项在新版中只是 deprecated，仍然可用。
#if LIBCURL_VERSION_NUM >= 0x075500
    const curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);
    if (info != nullptr && info->version_num >= 0x075500)
    {
        curl_easy_setopt(curl, CURLOPT_PROTOCOLS_STR, "http,https");
        curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS_STR, "http,https");
    }
    else
#endif
    {
        const long protocols = static_cast<long>(CURLPROTO_HTTP | CURLPROTO_HTTPS);
        CURL_IGNORE_DEPRECATION(
            curl_easy_setopt(curl, CURLOPT_PROTOCOLS, protocols);
            curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, protocols);
        )
    }

    const ENetworkRequestMethod method = request.method();
    switch (method)
    {
    case ENetworkRequestMethod::eGet:
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        break;
    case ENetworkRequestMethod::eHead:
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        break;
    case ENetworkRequestMethod::ePost:
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        // 显式声明零长度请求体：否则 libcurl 认为长度未知，会改用 chunked
        // 编码并从默认读回调（stdin）读取正文。有正文时下面设置的
        // CURLOPT_POSTFIELDSIZE_LARGE 会覆盖此值。
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
        break;
    default:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, methodToString(method));
        break;
    }

    const std::string& body = request.body();
    if (aMethodAllowsBody(methodToString(method)) && !body.empty())
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE,
                         static_cast<curl_off_t>(body.size()));
    }

    CurlHeaderList headerList;
    const std::map<std::string, std::string>& requestHeaders = request.headers();
    for (std::map<std::string, std::string>::const_iterator it = requestHeaders.begin();
         it != requestHeaders.end(); ++it)
    {
        const std::string key = sanitizeHeaderPart(it->first);
        if (key.empty())
            continue;
        const std::string line = key + ": " + sanitizeHeaderPart(it->second);
        if (!headerList.append(line))
            return eErrorOutOfRange;
    }
    if (headerList.get() != nullptr)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList.get());

    const CURLcode code = curl_easy_perform(curl);

    // 接收器主动中止（onHeaders/onData 返回非 0）优先于 curl 的错误码
    if (context.receiverError != eNoError)
        return context.receiverError;

    if (code != CURLE_OK)
    {
        const char* detail = errorBuffer[0] != '\0'
            ? errorBuffer
            : curl_easy_strerror(code);
        aError(_("网络请求失败: %s (curl=%d)"), detail, static_cast<int>(code));
        const errc_t mapped = mapCurlError(code);
        receiver.onError(mapped);
        return mapped;
    }

    // HEAD、204 等无响应体的情况不会触发 writeCallback，这里补交响应头
    if (!context.headersDelivered)
    {
        long statusCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
        const errc_t rc = receiver.onHeaders(static_cast<int>(statusCode),
                                             context.headers);
        if (rc != eNoError)
            return rc;
    }

    receiver.onComplete();
    return eNoError;
}

AST_NAMESPACE_END
