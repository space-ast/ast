///
/// @file      CppHttpNetworkInterface.cpp
/// @brief     基于 cpp-httplib 的网络实现
/// @details   使用 cpp-httplib 的 Request 回调逐块转发响应体，并保证 onHeaders 只交付一次。
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

// httplib.h 在 Windows 下会引入 winsock2.h，先于其它头文件包含以避免冲突
#include "httplib.h"

#include "CppHttpNetworkInterface.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Network.hpp"

#include <cctype>
#include <map>
#include <string>

AST_NAMESPACE_BEGIN

namespace {

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

/// @brief 把 URL 拆成 origin（scheme://authority）与 path（含 query，去掉 fragment）
/// @details httplib::Client 只接受 scheme://host[:port]，路径需要单独放到 Request::path。
///          authority 中的 userinfo 会被丢弃（httplib 的 Client 不接受 URL 里的用户信息）。
/// @return 解析成功返回 true；失败时 error 为对应错误码
bool parseUrl(const std::string& url, std::string& origin, std::string& path,
              errc_t& error)
{
    const std::string::size_type schemeEnd = url.find("://");
    if (schemeEnd == std::string::npos)
    {
        error = eErrorInvalidParam;
        return false;
    }

    std::string scheme = url.substr(0, schemeEnd);
    for (std::string::size_type i = 0; i < scheme.size(); ++i)
        scheme[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(scheme[i])));

    if (scheme != "http" && scheme != "https")
    {
        error = eErrorUnsupported;
        return false;
    }

    const std::string::size_type authorityBegin = schemeEnd + 3;
    const std::string::size_type pathBegin =
        url.find_first_of("/?#", authorityBegin);
    std::string authority = url.substr(
        authorityBegin,
        pathBegin == std::string::npos ? std::string::npos : pathBegin - authorityBegin);

    // 去掉 userinfo（user:pass@host），保留 IPv6 字面量的方括号
    const std::string::size_type at = authority.rfind('@');
    if (at != std::string::npos)
        authority.erase(0, at + 1);

    if (authority.empty())
    {
        error = eErrorInvalidParam;
        return false;
    }

    if (pathBegin == std::string::npos)
    {
        path = "/";
    }
    else
    {
        std::string rest = url.substr(pathBegin);
        const std::string::size_type fragment = rest.find('#');
        if (fragment != std::string::npos)
            rest.erase(fragment);

        if (rest.empty())
            path = "/";
        else if (rest[0] == '/')
            path = rest;
        else
            path = "/" + rest;  // "?query" 紧跟在 authority 后面
    }

    origin = scheme + "://" + authority;
    return true;
}

std::map<std::string, std::string> toHeaderMap(const httplib::Headers& headers)
{
    std::map<std::string, std::string> result;
    for (httplib::Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
        result[it->first] = it->second;
    return result;
}

errc_t mapHttpError(httplib::Error error)
{
    switch (error)
    {
    case httplib::Error::Success:
        return eNoError;
    case httplib::Error::Canceled:
        return eErrorCancelled;
    case httplib::Error::UnsupportedMultipartBoundaryChars:
    case httplib::Error::Compression:
        return eErrorUnsupported;
    case httplib::Error::ExceedRedirectCount:
        return eErrorOutOfRange;
    default:
        return eError;
    }
}

} // namespace

bool CppHttpNetworkInterface::isSupported() const
{
    return true;
}

errc_t CppHttpNetworkInterface::requestStream(const NetworkRequest& request,
                                              NetworkStreamReceiver& receiver)
{
    if (request.url().empty())
        return eErrorInvalidParam;

    std::string origin;
    std::string path;
    errc_t urlError = eNoError;
    if (!parseUrl(request.url(), origin, path, urlError))
        return urlError;

#if !defined(CPPHTTPLIB_SSL_ENABLED)
    if (origin.compare(0, 8, "https://") == 0)
    {
        aError(_("cpp-httplib 未启用 SSL，无法访问 https 地址"));
        return eErrorUnsupported;
    }
#endif

    httplib::Client client(origin);
    if (!client.is_valid())
        return eErrorInvalidParam;

#if defined(CPPHTTPLIB_SSL_ENABLED) && defined(__linux__)
    // OpenSSL 从源码构建时编译期默认 CA 路径在部分发行版上并不存在；
    // 由 AstUtil 统一解析环境变量（可能为 ':' 分隔列表）并回退到发行版常见位置。
    if (origin.compare(0, 8, "https://") == 0)
    {
        const std::string caFile = aGetCaCertFile();
        const std::string caDir = aGetCaCertDir();
        if (!caFile.empty() || !caDir.empty())
            client.set_ca_cert_path(caFile, caDir);
        else
            aWarning(_("https 请求找不到可用的 CA"));
    }
#endif

    client.set_connection_timeout(30, 0);
    client.set_read_timeout(300, 0);
    client.set_write_timeout(300, 0);
    client.set_follow_location(true);

    httplib::Request httpRequest;
    httpRequest.method = methodToString(request.method());
    httpRequest.path = path;
    if (aMethodAllowsBody(httpRequest.method))
        httpRequest.body = request.body();

    const std::map<std::string, std::string>& requestHeaders = request.headers();
    for (std::map<std::string, std::string>::const_iterator it = requestHeaders.begin();
         it != requestHeaders.end(); ++it)
    {
        const std::string key = sanitizeHeaderPart(it->first);
        if (key.empty())
            continue;
        httpRequest.headers.emplace(key, sanitizeHeaderPart(it->second));
    }

    errc_t receiverError = eNoError;
    bool headersDelivered = false;

    httpRequest.response_handler = [&](const httplib::Response& response) -> bool
    {
        headersDelivered = true;
        receiverError = receiver.onHeaders(response.status, toHeaderMap(response.headers));
        return receiverError == eNoError;
    };

    httpRequest.content_receiver = [&](const char* data, size_t size,
                                       size_t /*offset*/, size_t /*total*/) -> bool
    {
        receiverError = receiver.onData(data, size);
        return receiverError == eNoError;
    };

    httplib::Result result = client.send(httpRequest);

    // 接收器主动中止（onHeaders/onData 返回非 0）优先于 httplib 的错误码
    if (receiverError != eNoError)
        return receiverError;

    if (!result)
    {
        const httplib::Error httpError = result.error();
        aError(_("网络请求失败: %s (httplib=%d)"),
               httplib::to_string(httpError).c_str(), static_cast<int>(httpError));
        const errc_t mapped = mapHttpError(httpError);
        receiver.onError(mapped);
        return mapped;
    }

    // HEAD、204、CONNECT 不触发 response_handler，这里补交响应头
    if (!headersDelivered)
    {
        receiverError = receiver.onHeaders(result->status, toHeaderMap(result->headers));
        if (receiverError != eNoError)
            return receiverError;
    }

    receiver.onComplete();
    return eNoError;
}

AST_NAMESPACE_END
