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
#include "AstUtil/Logger.hpp"
#include <clocale>              // for _create_locale, _free_locale
#include <cstdarg>              // for va_list, va_start, va_end
#include <memory>               // for std::unique_ptr
#include <type_traits>          // for std::remove_pointer
#ifdef _WIN32
#include <windows.h>            // for Windows API
#include <io.h>                 // for _get_osfhandle
#else
#include <fcntl.h>              // for fcntl, F_GETPATH
#include <unistd.h>             // for fileno
#endif


AST_NAMESPACE_BEGIN
 
#ifdef _WIN32

struct LocaleDeleter
{
    void operator()(_locale_t locale) const
    {
        if (locale) {
            _free_locale(locale);
        }
    }
};

A_THREAD_LOCAL std::unique_ptr<std::remove_pointer<_locale_t>::type, LocaleDeleter> t_locale;  // utf-8 locale

_locale_t _ast_locale_ensure()
{
    static const char* locale_strs[]{
            ".UTF-8",
            "zh_CN.UTF-8",
            "en_US.UTF-8",
            "C.UTF-8",
            "C",
            ""
    };

    if (A_UNLIKELY(!t_locale)) {
        for (const char* locale_str : locale_strs) {
            _locale_t locale = _create_locale(LC_ALL, locale_str);
            if (locale) {
                t_locale.reset(locale);
                break;
            }
        }
        if (!t_locale) {
            aError("failed to create locale");
        }
    }
    return t_locale.get();
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

int posix::fprintf(FILE * stream, const char * format, ...)
{
    va_list args;
    int result;
    va_start(args, format);
    result = _vfprintf_l(stream, format, _ast_locale_ensure(), args);
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


int ast_printf(const char* format, ...)
{
    va_list args;
    int result;
    va_start(args, format);
    result = posix::vprintf(format, args);
    va_end(args);
    return result;
}


int aCurrentLineNumber(std::FILE *file)
{
    if (file == NULL) {
        return -1;  // 错误：文件指针为空
    }
    
    long currentPos = ftell(file);  // 保存当前位置
    if (currentPos == -1L) {
        aError("failed to get file position");
        return -1;  // 错误：无法获取文件位置
    }
    
    // 移动到文件开头
    if (fseek(file, 0, SEEK_SET) != 0) {
        aError("failed to move file pointer to the beginning");
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
        aError("failed to move file pointer to the original position");
        return -1;  // 错误：无法移动文件指针到原始位置
    }
    
    return lineCount;
}

err_t aGetFilePath(std::FILE *file, std::string &filepath)
{
    if (file == NULL) {
        return eErrorNullInput;
    }

#ifdef _WIN32
    // Windows平台实现
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(file));
    if (hFile == INVALID_HANDLE_VALUE) {
        aError("failed to get file handle");
        return eErrorInvalidFile;
    }

    // 首先尝试获取路径大小
    DWORD pathSize = GetFinalPathNameByHandleW(hFile, NULL, 0, FILE_NAME_NORMALIZED);
    if (pathSize == 0) {
        aError("failed to get file path size");
        return eErrorInvalidParam;
    }

    // 分配缓冲区并获取路径
    std::wstring wpath(pathSize, L'\0');
    pathSize = GetFinalPathNameByHandleW(hFile, &wpath[0], pathSize, FILE_NAME_NORMALIZED);
    if (pathSize == 0) {
        aError("failed to get file path");
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
        aError("failed to get file descriptor");
        return eErrorInvalidParam;
    }

    // 使用fcntl获取路径
    char path[PATH_MAX];
    if (fcntl(fd, F_GETPATH, path) != -1) {
        filepath = path;
        return eNoError;
    }

    char proc_path[256];
    snprintf(proc_path, sizeof(proc_path), "/proc/self/fd/%d", fd);
    ssize_t len = readlink(proc_path, path, sizeof(path) - 1);
    if (len == -1) 
        return eErrorInvalidParam;
    path[len] = '\0';
    filepath = path;
    return eNoError;
#endif
}

AST_NAMESPACE_END