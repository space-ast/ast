///
/// @file      NetworkImplWinINet.cpp
/// @brief     基于 WinINet 的网络实现
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
#include "NetworkImplWinINet.hxx"

AST_NAMESPACE_BEGIN

class NetworkImplWinINet::Impl {
public:
    bool isLoaded() const { return false; }
};

NetworkImplWinINet* NetworkImplWinINet::Instance()
{
    return nullptr;
}

NetworkImplWinINet::NetworkImplWinINet()
    : impl_(new Impl())
{
}

NetworkImplWinINet::~NetworkImplWinINet()
{
    delete impl_;
}

errc_t NetworkImplWinINet::request(const NetworkRequest& /*request*/, NetworkResponse& /*response*/)
{
    return eErrorNotInit;
}

bool NetworkImplWinINet::isSupported() const
{
    return false;
}

AST_NAMESPACE_END

#else  // ============ Windows 实现 ============

#include "NetworkImplWinINet.hxx"
#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/Encode.hpp"      // for aWideToUtf8
#include "AstUtil/StringUtil.hpp"

#include <windows.h>
#include <wininet.h>

#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <vector>

AST_NAMESPACE_BEGIN

namespace wininetproto {
    // 宽字符版本函数指针（正确支持 Unicode）
    typedef HINTERNET (WINAPI *InternetOpenW_func)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
    typedef HINTERNET (WINAPI *InternetConnectW_func)(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUsername, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
    typedef HINTERNET (WINAPI *HttpOpenRequestW_func)(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferer, LPCWSTR *lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
    typedef BOOL (WINAPI *HttpSendRequestW_func)(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength);
    typedef BOOL (WINAPI *HttpQueryInfoW_func)(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
    typedef BOOL (WINAPI *InternetReadFile_func)(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
    typedef BOOL (WINAPI *InternetCloseHandle_func)(HINTERNET hInternet);
    typedef BOOL (WINAPI *InternetSetOptionW_func)(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);
}




/* ---- Impl 类实现 ---- */

class NetworkImplWinINet::Impl {
public:
    Impl() : library_(nullptr) {
        loadLibrary();
    }
    
    ~Impl() {
        unloadLibrary();
    }
    
    errc_t loadLibrary() {
        library_ = aLoadLibrary("wininet.dll");
        if (!library_) {
            return eErrorNotInit;
        }
        
        // 加载宽字符版本函数
        internetOpenW_ = reinterpret_cast<wininetproto::InternetOpenW_func>(aGetProcAddress(library_, "InternetOpenW"));
        internetConnectW_ = reinterpret_cast<wininetproto::InternetConnectW_func>(aGetProcAddress(library_, "InternetConnectW"));
        httpOpenRequestW_ = reinterpret_cast<wininetproto::HttpOpenRequestW_func>(aGetProcAddress(library_, "HttpOpenRequestW"));
        httpSendRequestW_ = reinterpret_cast<wininetproto::HttpSendRequestW_func>(aGetProcAddress(library_, "HttpSendRequestW"));
        httpQueryInfoW_ = reinterpret_cast<wininetproto::HttpQueryInfoW_func>(aGetProcAddress(library_, "HttpQueryInfoW"));
        internetReadFile_ = reinterpret_cast<wininetproto::InternetReadFile_func>(aGetProcAddress(library_, "InternetReadFile"));
        internetCloseHandle_ = reinterpret_cast<wininetproto::InternetCloseHandle_func>(aGetProcAddress(library_, "InternetCloseHandle"));
        internetSetOptionW_ = reinterpret_cast<wininetproto::InternetSetOptionW_func>(aGetProcAddress(library_, "InternetSetOptionW"));
        
        // 检查是否所有函数都加载成功
        if (!internetOpenW_ || !internetConnectW_ || !httpOpenRequestW_ || !httpSendRequestW_ || 
            !httpQueryInfoW_ || !internetReadFile_ || !internetCloseHandle_ || !internetSetOptionW_) {
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
        
        internetOpenW_ = nullptr;
        internetConnectW_ = nullptr;
        httpOpenRequestW_ = nullptr;
        httpSendRequestW_ = nullptr;
        httpQueryInfoW_ = nullptr;
        internetReadFile_ = nullptr;
        internetCloseHandle_ = nullptr;
        internetSetOptionW_ = nullptr;
    }
    
    bool isLoaded() const {
        return library_ != nullptr;
    }
    
    // 设置超时（单位：毫秒）
    void setTimeout(HINTERNET hInternet, DWORD connectTimeoutMs, DWORD sendTimeoutMs, DWORD receiveTimeoutMs) {
        if (internetSetOptionW_) {
            internetSetOptionW_(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &connectTimeoutMs, sizeof(connectTimeoutMs));
            internetSetOptionW_(hInternet, INTERNET_OPTION_SEND_TIMEOUT,    &sendTimeoutMs,    sizeof(sendTimeoutMs));
            internetSetOptionW_(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &receiveTimeoutMs, sizeof(receiveTimeoutMs));
        }
    }
    
    wininetproto::InternetOpenW_func internetOpenW_ = nullptr;
    wininetproto::InternetConnectW_func internetConnectW_ = nullptr;
    wininetproto::HttpOpenRequestW_func httpOpenRequestW_ = nullptr;
    wininetproto::HttpSendRequestW_func httpSendRequestW_ = nullptr;
    wininetproto::HttpQueryInfoW_func httpQueryInfoW_ = nullptr;
    wininetproto::InternetReadFile_func internetReadFile_ = nullptr;
    wininetproto::InternetCloseHandle_func internetCloseHandle_ = nullptr;
    wininetproto::InternetSetOptionW_func internetSetOptionW_ = nullptr;
    
private:
    void* library_ = nullptr;
};


/* ---- 单例与构造/析构 ---- */

NetworkImplWinINet* NetworkImplWinINet::Instance()
{
    static NetworkImplWinINet instance;
    return &instance;
}

NetworkImplWinINet::NetworkImplWinINet() 
    : impl_(new Impl()) 
{
}

NetworkImplWinINet::~NetworkImplWinINet() {
    delete impl_;
}


/* ---- 核心请求方法 ---- */

errc_t NetworkImplWinINet::request(const NetworkRequest& request, NetworkResponse& response)
{
    if (!impl_->isLoaded()) {
        return eErrorNotInit;
    }

    // 解析 URL
    std::string url = request.url();
    if (url.empty()) {
        return eErrorInvalidParam;
    }

    std::string protocol = "http";
    std::string host;
    std::string path = "/";

    size_t protocolPos = url.find("://");
    if (protocolPos != std::string::npos) {
        protocol = aAsciiStrToLower(url.substr(0, protocolPos));   // 统一转为小写
        url = url.substr(protocolPos + 3);
    }else {
        // 若无 scheme，应该默认补充 http://（保持行为一致）
        // 但这里为了安全，仍当作错误处理
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
    std::string portStr;
    if (portPos != std::string::npos) {
        portStr = host.substr(portPos + 1);
        host = host.substr(0, portPos);
    }

    INTERNET_PORT defaultPort = (protocol == "https") ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
    INTERNET_PORT port = aParsePort(portStr, defaultPort);

    // 转换为宽字符（Unicode），正确支持各种语言
    std::wstring hostW; 
    std::wstring pathW; 
    aUtf8ToWide(host.c_str(), hostW);
    aUtf8ToWide(path.c_str(), pathW);

    // 初始化 WinINet（宽字符版本）
    HINTERNET hInternet = impl_->internetOpenW_(L"SpaceAST", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) {
        return eErrorNotInit;
    }

    // 设置超时（30秒连接，60秒发送，300秒接收）
    impl_->setTimeout(hInternet, 30000, 60000, 300000);

    // 连接服务器
    HINTERNET hConnect = impl_->internetConnectW_(
        hInternet,
        hostW.c_str(),
        port,
        NULL,
        NULL,
        INTERNET_SERVICE_HTTP,
        0,
        0
    );
    if (!hConnect) {
        impl_->internetCloseHandle_(hInternet);
        return eErrorNotInit;
    }

    // 确定请求方法（宽字符）
    LPCWSTR method = L"GET";
    std::string methodStr = "GET";  // 用于判断 body
    switch (request.method()) {
    case ENetworkRequestMethod::eGet:    method = L"GET";    methodStr = "GET";    break;
    case ENetworkRequestMethod::ePost:   method = L"POST";   methodStr = "POST";   break;
    case ENetworkRequestMethod::ePut:    method = L"PUT";    methodStr = "PUT";    break;
    case ENetworkRequestMethod::eDelete: method = L"DELETE"; methodStr = "DELETE"; break;
    case ENetworkRequestMethod::eOptions:method = L"OPTIONS";methodStr = "OPTIONS";break;
    case ENetworkRequestMethod::eTrace:  method = L"TRACE";  methodStr = "TRACE";  break;
    case ENetworkRequestMethod::eConnect:method = L"CONNECT";methodStr = "CONNECT";break;
    case ENetworkRequestMethod::ePatch:  method = L"PATCH";  methodStr = "PATCH";  break;
    default:                             method = L"GET";    methodStr = "GET";    break;
    }

    // 创建请求句柄（宽字符）
    DWORD dwFlags = (protocol == "https") ? INTERNET_FLAG_SECURE : 0;
    HINTERNET hRequest = impl_->httpOpenRequestW_(
        hConnect,
        method,
        pathW.c_str(),
        NULL,
        NULL,
        NULL,
        dwFlags,
        0
    );
    if (!hRequest) {
        impl_->internetCloseHandle_(hConnect);
        impl_->internetCloseHandle_(hInternet);
        return eErrorNotInit;
    }

    // 添加请求头（转为宽字符）
    std::wstring headersW;
    for (const auto& header : request.headers()) {
        std::wstring keyW;
        std::wstring valueW;
        aUtf8ToWide(header.first.c_str(), keyW);
        aUtf8ToWide(header.second.c_str(), valueW);
        headersW += keyW + L": " + valueW + L"\r\n";
    }

    // 只有允许携带 body 的方法且 body 非空时才发送
    LPVOID bodyPtr = NULL;
    DWORD bodyLen = 0;
    if (aMethodAllowsBody(methodStr)) {
        const std::string& body = request.body();
        if (!body.empty()) {
            bodyPtr = (LPVOID)body.c_str();
            bodyLen = static_cast<DWORD>(body.length());
        }
    }

    BOOL sendResult = impl_->httpSendRequestW_(
        hRequest,
        headersW.empty() ? NULL : headersW.c_str(),
        headersW.empty() ? 0 : static_cast<DWORD>(headersW.length()),
        bodyPtr,
        bodyLen
    );
    if (!sendResult) {
        impl_->internetCloseHandle_(hRequest);
        impl_->internetCloseHandle_(hConnect);
        impl_->internetCloseHandle_(hInternet);
        return eErrorNotInit;
    }

    // 读取响应状态码
    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    impl_->httpQueryInfoW_(
        hRequest,
        HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
        &statusCode,
        &statusCodeSize,
        NULL
    );
    response.setStatusCode(static_cast<int>(statusCode));

    // 正确解析响应头：一次获取全部原始头（宽字符），然后按行解析
    DWORD headerSize = 0;
    impl_->httpQueryInfoW_(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, NULL, &headerSize, NULL);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && headerSize > 0) {
        std::vector<wchar_t> rawBuf(headerSize / sizeof(wchar_t) + 1);
        if (impl_->httpQueryInfoW_(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, &rawBuf[0], &headerSize, NULL)) {
            std::wstring rawHeadersW(&rawBuf[0], headerSize / sizeof(wchar_t));
            // 将宽字符头转换为 UTF-8 以便统一处理
            std::string rawHeaders;
            if (aWideToUtf8(rawHeadersW.c_str(), rawHeaders) == eNoError) {
                size_t start = 0;
                while (start < rawHeaders.size()) {
                    size_t end = rawHeaders.find("\r\n", start);
                    if (end == std::string::npos) end = rawHeaders.size();
                    std::string line = rawHeaders.substr(start, end - start);
                    start = end + 2;  // 跳过 \r\n

                    if (line.empty()) continue;
                    // 跳过状态行（如 "HTTP/1.1 200 OK"）
                    if (line.find("HTTP/") == 0) continue;

                    size_t colonPos = line.find(":");
                    if (colonPos != std::string::npos) {
                        std::string key = line.substr(0, colonPos);
                        std::string value = line.substr(colonPos + 1);
                        // 去掉前导空格
                        size_t valStart = value.find_first_not_of(" \t");
                        if (valStart != std::string::npos) {
                            value = value.substr(valStart);
                        } else {
                            value.clear();
                        }
                        response.addHeader(key, value);
                    }
                }
            }
        }
    }

    // 读取响应体（二进制数据，无需宽字符处理）
    std::string responseBody;
    char buffer[1024];
    DWORD bytesRead;
    while (impl_->internetReadFile_(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        responseBody.append(buffer, bytesRead);
    }
    response.setBody(responseBody);

    // 清理资源
    impl_->internetCloseHandle_(hRequest);
    impl_->internetCloseHandle_(hConnect);
    impl_->internetCloseHandle_(hInternet);

    return eNoError;
}


bool NetworkImplWinINet::isSupported() const {
    return impl_->isLoaded();
}

AST_NAMESPACE_END

#endif // _WIN32