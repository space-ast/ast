/// @file      IO.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      30.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#include "IO.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/FileLock.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Network.hpp"
#include "AstUtil/RunTime.hpp"
#include "AstUtil/StringUtil.hpp"
#include <cerrno>               // for EINVAL, ENOENT, EIO
#include <clocale>              // for _create_locale, _free_locale
#include <cstdarg>              // for va_list, va_start, va_end
#include <cstdint>              // for uint64_t
#include <cstring>              // for strcmp
#include <memory>               // for std::unique_ptr
#include <type_traits>          // for std::remove_pointer

#ifdef _WIN32
#include <windows.h>            // for Windows API
#include <io.h>                 // for _get_osfhandle
#include <wchar.h>              // for _wfopen, _wfreopen, _vwprintf_l, _vwfprintf_l
#else
#include <limits.h>             // for PATH_MAX
#include <fcntl.h>              // for fcntl, F_GETPATH
#include <unistd.h>             // for fileno
#endif


AST_NAMESPACE_BEGIN
 
#ifdef _WIN32


static _locale_t _ast_locale_ensure()
{
    return aUTF8Locale();
}

std::FILE* posix::fopen(const char* filepath, const char* mode)
{
    // support utf8
    std::wstring wpath;
    std::wstring wmode;
    aUtf8ToWide(filepath, wpath);
    aUtf8ToWide(mode, wmode);
    #ifndef AST_USE_CRT_SAFE
    return ::_wfopen(wpath.c_str(), wmode.c_str());
    #else
    FILE* stream = nullptr;
    errno_t err = ::_wfopen_s(&stream, wpath.c_str(), wmode.c_str());
    if (err != 0) {
        return nullptr;
    }
    return stream;
    #endif
}


FILE *posix::freopen(const char *filepath, const char *mode, FILE *stream)
{
    std::wstring wpath;
    std::wstring wmode;
    aUtf8ToWide(filepath, wpath);
    aUtf8ToWide(mode, wmode);
    #ifndef AST_USE_CRT_SAFE
    return ::_wfreopen(wpath.c_str(), wmode.c_str(), stream);
    #else
    errno_t err = ::_wfreopen_s(&stream, wpath.c_str(), wmode.c_str(), stream);
    if (err != 0) {
        return nullptr;
    }
    return stream;
    #endif
}


int posix::vprintf(const char* format, va_list args)
{
    _locale_t locale = _ast_locale_ensure();
    return _vprintf_l(format, locale, args);
}

int posix::printf(const char* format, ...)
{
    va_list args;
    int result;
    va_start(args, format);
    result = _AST posix::vprintf(format, args);
    va_end(args);
    return result;
}

int posix::vfprintf(FILE * stream, const char * format, va_list args)
{
    return _vfprintf_l(stream, format, _ast_locale_ensure(), args);
}

int posix::fprintf(FILE * stream, const char * format, ...)
{
    va_list args;
    int result;
    va_start(args, format);
    result = posix::vfprintf(stream, format, args);
    va_end(args);
    return result;
}

int posix::wprintf(const wchar_t * format, ...)
{
    va_list args;
    int result;
    va_start(args, format);
    result = _vwprintf_l(format, _ast_locale_ensure(), args);
    va_end(args);
    return result;
}

int posix::fwprintf(FILE * stream, const wchar_t * format, ...)
{
    va_list args;
    int result;
    va_start(args, format);
    result = _vfwprintf_l(stream, format, _ast_locale_ensure(), args);
    va_end(args);
    return result;
}

#else



#endif


namespace{

/// @brief 提取URI的协议名（scheme）
/// @note 只有 "xxx://" 前缀或恰好是 "file:" 才被当作协议，
///       这样 "C:\Users\x"、"E:/tmp/a.txt" 这类普通路径不会被误判成协议
StringView uriScheme(StringView uri)
{
    size_t pos = uri.find(StringView("://"));
    if (pos != StringView::npos && pos > 0)
        return uri.substr(0, pos);
    if (uri.size() >= 5 && aEqualsIgnoreCase(uri.substr(0, 5), StringView("file:")))
        return StringView("file");
    return StringView();
}

/// @brief 是否是网络文件（http、https）
bool uriIsRemote(StringView uri)
{
    const StringView scheme = uriScheme(aStripAsciiWhitespace(uri));
    return aEqualsIgnoreCase(scheme, StringView("http")) ||
           aEqualsIgnoreCase(scheme, StringView("https"));
}

/// @brief 获取十六进制字符对应的数值，非十六进制字符返回-1
int hexDigit(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

/// @brief percent解码（%XX）
std::string percentDecode(StringView str)
{
    std::string result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '%' && i + 2 < str.size())
        {
            const int hi = hexDigit(str[i + 1]);
            const int lo = hexDigit(str[i + 2]);
            if (hi >= 0 && lo >= 0)
            {
                result.push_back(static_cast<char>((hi << 4) | lo));
                i += 2;
                continue;
            }
        }
        result.push_back(str[i]);
    }
    return result;
}

/// @brief file: URI 转换为本地文件路径
errc_t fileUriToPath(StringView uri, std::string& path)
{
    StringView rest = uri.substr(5);                        // 去掉 "file:"
    if (rest.starts_with(StringView("//")))
    {
        rest.remove_prefix(2);
        const size_t slash = rest.find('/');
        if (slash == StringView::npos)
        {
            return eErrorInvalidParam;                      // 如 "file://"、"file://host"
        }
        StringView host = rest.substr(0, slash);
        if (!host.empty() && !aEqualsIgnoreCase(host, StringView("localhost")))
        {
            return eErrorUnsupported;                       // 不支持其它主机上的文件，如 "file://host/path"
        }
        rest = rest.substr(slash);                          // 保留前导 '/'
    }
    if (rest.empty())
        return eErrorInvalidParam;

    path = percentDecode(rest);
#ifdef _WIN32
    // "file:///C:/path" → "C:/path"：去掉盘符前的 '/'
    if (path.size() >= 3 && path[0] == '/' && path[2] == ':' &&
        ((path[1] >= 'a' && path[1] <= 'z') || (path[1] >= 'A' && path[1] <= 'Z')))
        path.erase(0, 1);
#endif
    return eNoError;
}

/// @brief 取URI末尾一段作为缓存文件的可读名字（唯一性由哈希保证）
std::string uriCacheName(StringView uri)
{
    const size_t slash = uri.rfind('/');
    StringView name = (slash == StringView::npos) ? uri : uri.substr(slash + 1);

    std::string result = percentDecode(name);
    const size_t query = result.find('?');
    if (query != std::string::npos)
        result.resize(query);

    for (char& c : result)
    {
        const unsigned char u = static_cast<unsigned char>(c);
        if (u < 0x20 || u == '<' || u == '>' || u == ':' || u == '"' ||
            u == '/' || u == '\\' || u == '|' || u == '?' || u == '*')
            c = '_';
    }
    if (result.size() > 64)
        result.resize(64);
    if (result.empty())
        result = "download";
    return result;
}

/// @brief FNV-1a 64位哈希，转成16位小写十六进制
std::string uriCacheHash(StringView uri)
{
    uint64_t hash = 14695981039346656037ULL;                // FNV 偏移基数
    for (size_t i = 0; i < uri.size(); ++i)
    {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(uri[i]));
        hash *= 1099511628211ULL;                           // FNV 质数
    }
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%016llx", static_cast<unsigned long long>(hash));
    return std::string(buffer);
}

/// @brief 缓存文件是否可用（存在且非空）
bool isUsableCacheFile(const std::string& filepath)
{
    std::error_code ec;
    const bool empty = fs::is_empty(filepath, ec);
    return !ec && !empty;
}

/// @brief 构造缓存文件路径与锁文件路径，并确保缓存文件夹已存在
errc_t uriCachePath(StringView requestUri, std::string& cacheFile, std::string& lockFile)
{
    std::string cacheDir;
    errc_t rc = aCacheDir(cacheDir);
    if (rc != eNoError || cacheDir.empty())
    {
        aError(_("无法确定缓存文件夹"));
        return eErrorNotFound;
    }

    fs::path uriDir = fs::path(cacheDir) / "uri";
    std::error_code ec;
    // 文件锁不会创建父目录，必须先建好；已存在时返回 true
    if (!fs::create_directories(uriDir, ec) || ec)
    {
        aError(_("无法创建缓存文件夹 '%s'"), uriDir.string().c_str());
        return eErrorInvalidFile;
    }

    const std::string hash = uriCacheHash(requestUri);
    cacheFile = (uriDir / (hash + "_" + uriCacheName(requestUri))).string();
    lockFile = (uriDir / (hash + ".lock")).string();
    return eNoError;
}

/// @brief 持锁读取缓存，缓存不存在时下载
errc_t fetchRemote(const std::string& uri, const std::string& cacheFile, FileLock& lock)
{
    if (lock.lock() != eNoError)
        aWarning(_("无法锁定缓存锁文件 '%s'，跳过多进程协调"), cacheFile.c_str());

    // 命中检查必须在持锁之后：另一个进程可能刚下载完
    if (isUsableCacheFile(cacheFile))
        return eNoError;

    // 三参版本 + 空回调：静默下载，不打印进度条
    return aDownloadFile(uri, cacheFile, DownloadProgressCallback{});
}

}   // namespace


errc_t aUriFetch(StringView uri, std::string& filepath)
{
    filepath.clear();
    uri = aStripAsciiWhitespace(uri);
    if (uri.empty())
        return eErrorInvalidParam;

    const StringView scheme = uriScheme(uri);

    // 普通文件路径：原样返回
    if (scheme.empty())
    {
        filepath.assign(uri.data(), uri.size());
        return eNoError;
    }

    // 本地文件URI
    if (aEqualsIgnoreCase(scheme, StringView("file")))
        return fileUriToPath(uri, filepath);

    // 远程资源
    if (!aEqualsIgnoreCase(scheme, StringView("http")) &&
        !aEqualsIgnoreCase(scheme, StringView("https")))
    {
        aError(_("不支持的URI协议: %.*s"), static_cast<int>(scheme.size()), scheme.data());
        return eErrorUnsupported;
    }

    // fragment 不参与请求，也不参与缓存键
    StringView request = uri;
    const size_t fragmentPos = request.find('#');
    if (fragmentPos != StringView::npos)
        request = request.substr(0, fragmentPos);
    const std::string requestUri(request.data(), request.size());

    std::string cacheFile;
    std::string lockFile;
    errc_t rc = uriCachePath(request, cacheFile, lockFile);
    if (rc != eNoError)
        return rc;

    FileLock lock(lockFile);
    rc = fetchRemote(requestUri, cacheFile, lock);
    if (rc != eNoError)
        return rc;

    filepath = cacheFile;
    return eNoError;
}


std::FILE* uriopen(const char* uri, const char* mode)
{
    if (uri == nullptr || mode == nullptr)
    {
        errno = EINVAL;
        return nullptr;
    }
    // 网络文件只支持只读模式：远程资源只能下载缓存副本，无法回写
    if (uriIsRemote(uri) && strcmp(mode, "r") != 0 && strcmp(mode, "rb") != 0)
    {
        aError(_("网络文件只支持 'r'、'rb' 模式，不支持 '%s'"), mode);
        errno = EINVAL;
        return nullptr;
    }

    std::string filepath;
    const errc_t rc = aUriFetch(StringView(uri), filepath);
    if (rc != eNoError)
    {
        // 统一成"打不开"的语义，便于调用方使用 perror
        switch (rc)
        {
        case eErrorNotFound:
        case eErrorInvalidFile: errno = ENOENT; break;
        case eErrorInvalidParam:
        case eErrorUnsupported: errno = EINVAL; break;
        default:                errno = EIO;    break;
        }
        return nullptr;
    }

    // mode 原样透传，本地文件与 fopen 完全一致
    return posix::fopen(filepath.c_str(), mode);
}


int ast_printf(const char* format, ...)
{
    va_list args;
    int result;
    va_start(args, format);
    result = posix::vprintf(format, args);
    va_end(args);
    return result;
}


int aPrintLink(StringView text, StringView link)
{
    return posix::printf(
        "\033]8;;%.*s\033\\%.*s\033]8;;\033\\",
        static_cast<int>(link.size()), link.data(),
        static_cast<int>(text.size()), text.data()
    );
}


int aCurrentLineNumber(std::FILE *file)
{
    if (file == NULL) {
        return -1;  // 错误：文件指针为空
    }
    
    long currentPos = ftell(file);  // 保存当前位置
    if (currentPos == -1L) {
        aError(_("获取文件位置失败"));
        return -1;  // 错误：无法获取文件位置
    }
    
    // 移动到文件开头
    if (fseek(file, 0, SEEK_SET) != 0) {
        aError(_("移动文件指针到开头失败"));
        return -1;  // 错误：无法移动文件指针到开头
    }
    
    int lineCount = 1;  // 行号从1开始
    int ch;
    
    // 从文件开头读取到当前位置，统计换行符
    while ((ch = fgetc(file)) != EOF && ftell(file) <= currentPos) {
        if (ch == '\n') {
            lineCount++;
        }
    }
    
    // 恢复原始位置
    if (fseek(file, currentPos, SEEK_SET) != 0) {
        aError(_("移动文件指针到原始位置失败"));
        return -1;  // 错误：无法移动文件指针到原始位置
    }
    
    return lineCount;
}

errc_t aGetFilePath(std::FILE *file, std::string &filepath)
{
    if (file == NULL) {
        return eErrorNullInput;
    }

#ifdef _WIN32
    // Windows平台实现
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(file));
    if (hFile == INVALID_HANDLE_VALUE) {
        aError(_("获取文件句柄失败"));
        return eErrorInvalidFile;
    }

    // 首先尝试获取路径大小
    DWORD pathSize = GetFinalPathNameByHandleW(hFile, NULL, 0, FILE_NAME_NORMALIZED);
    if (pathSize == 0) {
        aError(_("获取文件路径大小失败"));
        return eErrorInvalidParam;
    }

    // 分配缓冲区并获取路径
    std::wstring wpath(pathSize, L'\0');
    pathSize = GetFinalPathNameByHandleW(hFile, &wpath[0], pathSize, FILE_NAME_NORMALIZED);
    if (pathSize == 0) {
        aError(_("获取文件路径失败"));
        return eErrorInvalidParam;
    }

    // 移除"\\?\"前缀（如果存在）
    if (wpath.size() >= 4 && wpath.substr(0, 4) == L"\\\\?\\") {
        wpath = wpath.substr(4);
    }

    // 转换为UTF-8
    aWideToUtf8(wpath.c_str(), filepath);
    return eNoError;
#else
    // POSIX平台实现
    int fd = fileno(file);
    if (fd == -1) {
        aError(_("获取文件描述符失败"));
        return eErrorInvalidParam;
    }

    char path[PATH_MAX]{'\0'};

    // 通过readlink获取路径
    char proc_path[256];
    snprintf(proc_path, sizeof(proc_path), "/proc/self/fd/%d", fd);
    ssize_t len = readlink(proc_path, path, sizeof(path) - 1);
    if (len != -1){
        filepath = std::string(path, len);
        return eNoError;
    } 

    // 使用fcntl获取路径
    #ifdef F_GETPATH
    if (fcntl(fd, F_GETPATH, path) != -1) {
        filepath = path;
        return eNoError;
    }
    #endif

    return eErrorInvalidParam;
#endif
}

AST_NAMESPACE_END