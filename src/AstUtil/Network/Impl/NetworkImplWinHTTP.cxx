///
/// @file      NetworkImplWinHTTP.cpp
/// @brief     基于 WinHTTP 的网络实现
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

#ifndef _WIN32
// 非 Windows 平台：提供所有接口的空实现，避免链接错误
#include "NetworkImplWinHTTP.hpp"

AST_NAMESPACE_BEGIN

// 简化版 Impl，只提供必要的 isLoaded 返回 false
class NetworkImplWinHTTP::Impl {
public:
    bool isLoaded() const { return false; }
};

NetworkImplWinHTTP* NetworkImplWinHTTP::Instance()
{
    return nullptr;
}

NetworkImplWinHTTP::NetworkImplWinHTTP()
    : impl_(new Impl())
{
}

NetworkImplWinHTTP::~NetworkImplWinHTTP()
{
    delete impl_;
}

errc_t NetworkImplWinHTTP::request(const NetworkRequest& /*request*/, NetworkResponse& /*response*/)
{
    return eErrorNotInit;
}

bool NetworkImplWinHTTP::isSupported() const
{
    return false;
}

AST_NAMESPACE_END

#else  // ============ Windows 实现 ============

#include "NetworkImplWinHTTP.hpp"
#include "AstUtil/NetworkInterface.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/ParseFormat.hpp"

#include <windows.h>
#include <winhttp.h>

#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <vector>

AST_NAMESPACE_BEGIN

namespace winhttpproto {
    // WinHTTP API 函数原型
    typedef HINTERNET (WINAPI *WinHttpOpen_func)(LPCWSTR pszAgentW, DWORD dwAccessType, LPCWSTR pszProxyW, LPCWSTR pszProxyBypassW, DWORD dwFlags);
    typedef HINTERNET (WINAPI *WinHttpConnect_func)(HINTERNET hSession, LPCWSTR pswzServerName, INTERNET_PORT nServerPort, DWORD dwReserved);
    typedef HINTERNET (WINAPI *WinHttpOpenRequest_func)(HINTERNET hConnect, LPCWSTR pwszVerb, LPCWSTR pwszObjectName, LPCWSTR pwszVersion, LPCWSTR pwszReferer, LPCWSTR *ppwszAcceptTypes, DWORD dwFlags);
    typedef BOOL (WINAPI *WinHttpSendRequest_func)(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength, DWORD dwTotalLength, DWORD_PTR dwContext);
    typedef BOOL (WINAPI *WinHttpReceiveResponse_func)(HINTERNET hRequest, LPVOID lpReserved);
    typedef BOOL (WINAPI *WinHttpQueryHeaders_func)(HINTERNET hRequest, DWORD dwInfoLevel, LPCWSTR lpName, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
    typedef BOOL (WINAPI *WinHttpReadData_func)(HINTERNET hRequest, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
    typedef BOOL (WINAPI *WinHttpCloseHandle_func)(HINTERNET hInternet);
}





/* ---- Impl 类实现 ---- */

class NetworkImplWinHTTP::Impl {
public:
    Impl() : library_(nullptr) {
        loadLibrary();
    }
    
    ~Impl() {
        unloadLibrary();
    }
    
    errc_t loadLibrary() {
        // 加载 winhttp.dll
        library_ = aLoadLibrary("winhttp.dll");
        if (!library_) {
            return eErrorNotInit;
        }
        
        // 获取函数指针
        winHttpOpen_ = reinterpret_cast<winhttpproto::WinHttpOpen_func>(aGetProcAddress(library_, "WinHttpOpen"));
        winHttpConnect_ = reinterpret_cast<winhttpproto::WinHttpConnect_func>(aGetProcAddress(library_, "WinHttpConnect"));
        winHttpOpenRequest_ = reinterpret_cast<winhttpproto::WinHttpOpenRequest_func>(aGetProcAddress(library_, "WinHttpOpenRequest"));
        winHttpSendRequest_ = reinterpret_cast<winhttpproto::WinHttpSendRequest_func>(aGetProcAddress(library_, "WinHttpSendRequest"));
        winHttpReceiveResponse_ = reinterpret_cast<winhttpproto::WinHttpReceiveResponse_func>(aGetProcAddress(library_, "WinHttpReceiveResponse"));
        winHttpQueryHeaders_ = reinterpret_cast<winhttpproto::WinHttpQueryHeaders_func>(aGetProcAddress(library_, "WinHttpQueryHeaders"));
        winHttpReadData_ = reinterpret_cast<winhttpproto::WinHttpReadData_func>(aGetProcAddress(library_, "WinHttpReadData"));
        winHttpCloseHandle_ = reinterpret_cast<winhttpproto::WinHttpCloseHandle_func>(aGetProcAddress(library_, "WinHttpCloseHandle"));
        
        // 检查是否所有函数都加载成功
        if (!winHttpOpen_ || !winHttpConnect_ || !winHttpOpenRequest_ || !winHttpSendRequest_ || 
            !winHttpReceiveResponse_ || !winHttpQueryHeaders_ || !winHttpReadData_ || !winHttpCloseHandle_) {
            unloadLibrary();
            return eErrorNotInit;
        }
        
        return eNoError;
    }
    
    void unloadLibrary() {
        if (library_) {
            aFreeLibrary(library_);
            library_ = nullptr;
        }
        
        winHttpOpen_ = nullptr;
        winHttpConnect_ = nullptr;
        winHttpOpenRequest_ = nullptr;
        winHttpSendRequest_ = nullptr;
        winHttpReceiveResponse_ = nullptr;
        winHttpQueryHeaders_ = nullptr;
        winHttpReadData_ = nullptr;
        winHttpCloseHandle_ = nullptr;
    }
    
    bool isLoaded() const {
        return library_ != nullptr;
    }
    
    // 函数指针
    winhttpproto::WinHttpOpen_func winHttpOpen_ = nullptr;
    winhttpproto::WinHttpConnect_func winHttpConnect_ = nullptr;
    winhttpproto::WinHttpOpenRequest_func winHttpOpenRequest_ = nullptr;
    winhttpproto::WinHttpSendRequest_func winHttpSendRequest_ = nullptr;
    winhttpproto::WinHttpReceiveResponse_func winHttpReceiveResponse_ = nullptr;
    winhttpproto::WinHttpQueryHeaders_func winHttpQueryHeaders_ = nullptr;
    winhttpproto::WinHttpReadData_func winHttpReadData_ = nullptr;
    winhttpproto::WinHttpCloseHandle_func winHttpCloseHandle_ = nullptr;
    
private:
    void* library_ = nullptr; ///< 库句柄
};


/* ---- 单例与构造/析构 ---- */

NetworkImplWinHTTP* NetworkImplWinHTTP::Instance() {
    static NetworkImplWinHTTP instance;
    return &instance;
}

NetworkImplWinHTTP::NetworkImplWinHTTP() 
    : impl_(new Impl()) 
{
}

NetworkImplWinHTTP::~NetworkImplWinHTTP() {
    delete impl_;
}


/* ---- 核心请求方法 ---- */

errc_t NetworkImplWinHTTP::request(const NetworkRequest& request, NetworkResponse& response) {
    if (!impl_->isLoaded()) {
        return eErrorNotInit;
    }

    // 获取 URL 并初步解析
    std::string url = request.url();
    if (url.empty()) {
        return eErrorInvalidParam;   // 明确为参数错误
    }

    std::string protocol = "http";
    std::string host;
    std::string path = "/";

    size_t protocolPos = url.find("://");
    if (protocolPos != std::string::npos) {
        protocol = aAsciiStrToLower(url.substr(0, protocolPos));   // 统一小写
        url = url.substr(protocolPos + 3);
    }

    size_t pathPos = url.find("/");
    if (pathPos != std::string::npos) {
        host = url.substr(0, pathPos);
        path = url.substr(pathPos);
    } else {
        host = url;
    }

    // 提取端口
    size_t portPos = host.find(":");
    std::string portStr = (protocol == "https") ? "443" : "80";
    if (portPos != std::string::npos) {
        portStr = host.substr(portPos + 1);
        host = host.substr(0, portPos);
    }
    // 简单验证端口
    int port = aParseInt(portStr.c_str());
    if (port <= 0 || port > 65535) {
        port = (protocol == "https") ? 443 : 80;
    }

    // 正确的编码转换：UTF-8 → UTF-16
    std::wstring hostW;
    std::wstring pathW;
    aUtf8ToWide(host.c_str(), hostW);
    aUtf8ToWide(path.c_str(), pathW);

    // 初始化 WinHTTP
    HINTERNET hSession = impl_->winHttpOpen_(L"SpaceAST", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        return eErrorNotInit;  // 可考虑扩展为更详细的网络错误码
    }
    // TODO: 可在此通过 WinHttpSetOption 设置超时等选项以提高健壮性

    // 连接
    HINTERNET hConnect = impl_->winHttpConnect_(hSession, hostW.c_str(), static_cast<INTERNET_PORT>(port), 0);
    if (!hConnect) {
        impl_->winHttpCloseHandle_(hSession);
        return eErrorNotInit;
    }

    // 确定请求方法（宽字符）
    LPCWSTR methodW = L"GET";
    std::string methodStr = "GET";  // 用于后续判断 body
    switch (request.method()) {
    case ENetworkRequestMethod::eGet:    methodW = L"GET";    methodStr = "GET";    break;
    case ENetworkRequestMethod::ePost:   methodW = L"POST";   methodStr = "POST";   break;
    case ENetworkRequestMethod::ePut:    methodW = L"PUT";    methodStr = "PUT";    break;
    case ENetworkRequestMethod::eDelete: methodW = L"DELETE"; methodStr = "DELETE"; break;
    case ENetworkRequestMethod::eOptions:methodW = L"OPTIONS";methodStr = "OPTIONS";break;
    case ENetworkRequestMethod::eTrace:  methodW = L"TRACE";  methodStr = "TRACE";  break;
    case ENetworkRequestMethod::eConnect:methodW = L"CONNECT";methodStr = "CONNECT";break;
    case ENetworkRequestMethod::ePatch:  methodW = L"PATCH";  methodStr = "PATCH";  break;
    default:                             methodW = L"GET";    methodStr = "GET";    break;
    }

    // 创建请求句柄
    DWORD dwFlags = 0;
    if (protocol == "https") {
        dwFlags |= WINHTTP_FLAG_SECURE;
    }

    HINTERNET hRequest = impl_->winHttpOpenRequest_(hConnect, methodW, pathW.c_str(), L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
    if (!hRequest) {
        impl_->winHttpCloseHandle_(hConnect);
        impl_->winHttpCloseHandle_(hSession);
        return eErrorNotInit;
    }

    // 添加请求头
    std::wstring headersW;
    for (const auto& header : request.headers()) {
        std::wstring keyW;
        std::wstring valueW;
        aUtf8ToWide(header.first.c_str(), keyW);
        aUtf8ToWide(header.second.c_str(), valueW);
        headersW += keyW + L": " + valueW + L"\r\n";
    }

    // 只有允许携带 body 的方法且 body 非空时才发送 body
    LPVOID bodyPtr = nullptr;
    DWORD bodyLen = 0;
    if (aMethodAllowsBody(methodStr)) {
        const std::string& body = request.body();
        if (!body.empty()) {
            bodyPtr = (LPVOID)body.c_str();
            bodyLen = static_cast<DWORD>(body.length());
        }
    }

    BOOL sendResult = impl_->winHttpSendRequest_(hRequest, 
        headersW.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : headersW.c_str(),
        headersW.empty() ? 0 : static_cast<DWORD>(headersW.length()),
        bodyPtr,
        bodyLen,
        bodyLen,
        0
    );

    if (!sendResult) {
        impl_->winHttpCloseHandle_(hRequest);
        impl_->winHttpCloseHandle_(hConnect);
        impl_->winHttpCloseHandle_(hSession);
        return eErrorNotInit;
    }

    // 接收响应
    if (!impl_->winHttpReceiveResponse_(hRequest, NULL)) {
        impl_->winHttpCloseHandle_(hRequest);
        impl_->winHttpCloseHandle_(hConnect);
        impl_->winHttpCloseHandle_(hSession);
        return eErrorNotInit;
    }

    // 读取响应状态码
    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    impl_->winHttpQueryHeaders_(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize, NULL);
    response.setStatusCode(static_cast<int>(statusCode));

    // ---- 正确解析响应头（一次获取全部原始头，手动拆分） ----
    DWORD headerSize = 0;
    impl_->winHttpQueryHeaders_(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &headerSize, NULL);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && headerSize > 0) {
        std::wstring rawHeadersWide(headerSize / sizeof(wchar_t), L'\0');
        if (impl_->winHttpQueryHeaders_(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, &rawHeadersWide[0], &headerSize, NULL)) {
            // 转换为 UTF-8 以便处理
            std::string rawHeaders;
            errc_t rc = aWideToUtf8(rawHeadersWide.c_str(), rawHeaders);
            if (rc != eNoError) {
                aWarning("failed to convert response headers to utf8");
            } else {
                // 按行分割
                size_t start = 0;
                while (start < rawHeaders.size()) {
                    size_t end = rawHeaders.find("\r\n", start);
                    if (end == std::string::npos) end = rawHeaders.size();
                    std::string line = rawHeaders.substr(start, end - start);
                    start = end + 2;  // 跳过 \r\n

                    if (line.empty()) continue;
                    // 跳过状态行（HTTP/1.1 200 OK）
                    if (line.find("HTTP/") == 0) continue;
                    // 解析键值对
                    size_t colonPos = line.find(":");
                    if (colonPos != std::string::npos) {
                        std::string key = line.substr(0, colonPos);
                        std::string value = line.substr(colonPos + 1);
                        // 去掉前导空格（常见写法）
                        size_t valStart = value.find_first_not_of(" \t");
                        if (valStart != std::string::npos) {
                            value = value.substr(valStart);
                        } else {
                            value.clear();
                        }
                        response.addHeader(key, value);
                    }
                    // 续行（obsolete）暂不处理，因为现代服务器几乎不使用
                }
            }
        }
    }

    // 读取响应体
    std::string responseBody;
    char buffer[1024];
    DWORD bytesRead;
    while (impl_->winHttpReadData_(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        responseBody.append(buffer, bytesRead);
    }
    response.setBody(responseBody);

    // 清理资源
    impl_->winHttpCloseHandle_(hRequest);
    impl_->winHttpCloseHandle_(hConnect);
    impl_->winHttpCloseHandle_(hSession);

    return eNoError;
}


bool NetworkImplWinHTTP::isSupported() const {
    return impl_->isLoaded();
}

AST_NAMESPACE_END

#endif // _WIN32