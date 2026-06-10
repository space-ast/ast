///
/// @file      NetworkImplCurlCmd.cpp
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


#include "NetworkImplCurlCmd.hpp"
#include "AstUtil/NetworkRequest.hpp"
#include "AstUtil/NetworkResponse.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#define popen _popen
#define pclose _pclose
#define NULL_DEVICE "nul"
#else
#include <unistd.h>
#define NULL_DEVICE "/dev/null"
#endif

AST_NAMESPACE_BEGIN

namespace
{
    // 将请求方法转换为 curl 所需的字符串
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
        default:
            return "GET";
        }
    }

    // 转义单引号，以便在单引号字符串中安全使用
    std::string escapeForShell(const std::string& s)
    {
        /*!
        @todo 
        escapeForShell 函数的实现存在安全风险。它仅转义了双引号，但在 Shell 环境下，
        双引号内的 $, `, \ 等字符仍具有特殊含义。
        如果 request.url() 或请求头包含恶意构造的字符串（例如 $(命令)），
        可能会导致命令注入攻击。考虑使用更完备的转义逻辑，
        或者考虑使用不经过 Shell 的进程启动方式（如 Windows 的 CreateProcess 或 Unix 的 execvp）
        */

        std::string escaped;
        escaped.reserve(s.size() + 2);
        escaped = "\"";
        for (char c : s)
        {
            if (c == '"')
                escaped += "\\\"";
            else
                escaped += c;
        }
        escaped += "\"";
        return escaped;
    }

    // 去除字符串两端的空白（用于解析头部）
    std::string trim(const std::string& s)
    {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
}


NetworkImplCurlCmd* NetworkImplCurlCmd::Instance()
{
    static NetworkImplCurlCmd instance;
    return &instance;
}

errc_t NetworkImplCurlCmd::request(const NetworkRequest& request, NetworkResponse& response)
{
    // 如果没有 URL，直接返回错误
    if (request.url().empty())
        return -1;

    // 构建 curl 命令参数
    std::string command = "curl -s -S -i -L";  // -L 跟随重定向

    // 请求方法
    command += " -X " + std::string(methodToString(request.method()));

    // 请求头
    for (const auto& header : request.headers())
    {
        command += " -H " + escapeForShell(header.first + ": " + header.second);
    }

    // 请求体处理（使用临时文件避免 shell 二进制问题）
    std::string tmpFilePath;
    if (!request.body().empty())
    {
#ifdef _WIN32
        char tmpPath[MAX_PATH];
        char tmpFile[MAX_PATH];
        if (GetTempPathA(MAX_PATH, tmpPath) == 0)
            return -2;
        if (GetTempFileNameA(tmpPath, "ast", 0, tmpFile) == 0)
            return -2;
        tmpFilePath = tmpFile;
#else
        char tmpTemplate[] = "/tmp/ast_network_XXXXXX";
        int fd = mkstemp(tmpTemplate);
        if (fd == -1)
            return -2;
        close(fd);
        tmpFilePath = tmpTemplate;
#endif
        // 写入请求体
        {
            std::ofstream tmpFile(tmpFilePath, std::ios::binary);
            if (!tmpFile)
                return -2;
            tmpFile.write(request.body().data(), request.body().size());
            tmpFile.close();
        }
        command += " --data-binary @" + escapeForShell(tmpFilePath);
    }

    // 目标 URL
    command += " " + escapeForShell(request.url());

    // 重定向 stderr 到 null
    command += " 2>" NULL_DEVICE;

    // 执行命令并读取输出
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        if (!tmpFilePath.empty())
            std::remove(tmpFilePath.c_str());
        return -3;
    }

    // 读取全部输出
    std::vector<char> buffer(4096);
    std::string rawResponse;
    while (std::fread(buffer.data(), sizeof(char), buffer.size(), pipe) > 0)
    {
        rawResponse.append(buffer.data());
    }

    int status = pclose(pipe);
    if (!tmpFilePath.empty())
        std::remove(tmpFilePath.c_str());

    // 检查 curl 自身是否成功执行
    if (status != 0)
        return -4;

    // ---------- 解析 HTTP 响应 ----------
    std::istringstream stream(rawResponse);
    std::string line;

    // 1. 状态行
    if (!std::getline(stream, line))
        return -5;
    // 示例: "HTTP/1.1 200 OK"
    line = trim(line);
    if (line.empty())
        return -5;
    // 提取状态码（第二个字段）
    std::istringstream statusLine(line);
    std::string httpVersion;
    int statusCode = 0;
    statusLine >> httpVersion >> statusCode;
    if (statusCode == 0)
        return -6;
    response.setStatusCode(statusCode);

    // 2. 头部
    std::map<std::string, std::string> respHeaders;
    while (std::getline(stream, line))
    {
        line = trim(line);
        if (line.empty())   // 空行表示头部结束
            break;
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;       // 忽略不合法行
        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));
        respHeaders[key] = value;
    }
    response.setHeaders(respHeaders);

    // 3. 主体（剩余所有内容）
    std::string body;
    while (std::getline(stream, line))
    {
        body += line + "\n";
    }
    // 注意：如果原始响应没有最后的换行，上面会多一个 '\n'，但无伤大雅
    if (!body.empty() && body.back() == '\n')
        body.pop_back();   // 去掉最后多加的换行符
    response.setBody(body);

    return 0;
}

bool NetworkImplCurlCmd::isSupported() const
{
#ifdef _WIN32
        // Windows: 将错误和输出都丢弃到 NUL
        int ret = std::system("curl --version > NUL 2>&1");
#else
        // Unix-like: 重定向到 /dev/null
        int ret = std::system("curl --version > /dev/null 2>&1");
#endif
    return ret == 0;
}

AST_NAMESPACE_END
