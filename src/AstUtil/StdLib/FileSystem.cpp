/// @file      FileSystem.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      23.11.2025
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
 
#include "FileSystem.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"


#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <limits.h>     // for PATH_MAX
#include <unistd.h>     // for readlink

#ifdef __APPLE__
#include <mach-o/dyld.h>   // for _NSGetExecutablePath
#include <stdlib.h>       // for realpath
#endif

#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#endif
AST_NAMESPACE_BEGIN

// 移除文件名，只保留目录路径
std::string _aFileParentDir(StringView filepath_)
{
    auto filepath = filepath_.to_string();
    #ifdef _WIN32
    size_t lastSlashPos = filepath.find_last_of("/\\");
    #else
    size_t lastSlashPos = filepath.find_last_of('/');
    #endif
    if (lastSlashPos != std::string::npos)
    {
        return filepath.substr(0, lastSlashPos + 1);
    }
    return filepath;
}

std::string aLibDir()
{
    return _aFileParentDir(aLibPath());
}

std::string aExeDir()
{
    return _aFileParentDir(aExePath());
}

std::string aLibPath()
{
#ifdef _WIN32
    // Windows平台实现
    wchar_t buffer[MAX_PATH] = { 0 };
    HMODULE hModule = NULL;

    // 获取当前模块句柄
    if (GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&aLibDir),
        &hModule))
    {
        // 获取模块路径
        if (GetModuleFileNameW(hModule, buffer, MAX_PATH))
        {
            std::string libpath;
            aWideToUtf8(buffer, libpath);
            return libpath;
        }
        else {
            aError("failed to call GetModuleFileNameW");
        }
    }
    else {
        aError("failed to call GetModuleHandleExW");
    }
#else
    // Unix-like平台实现 (Linux, macOS等)
    Dl_info dlInfo;
    if (dladdr(reinterpret_cast<void*>(&aLibDir), &dlInfo))
    {
        if (dlInfo.dli_fname != nullptr)
        {
            return dlInfo.dli_fname;
        }
        else {
            aError("dlInfo.dli_fname is nullptr");
        }
    }
    else {
        aError("failed to call dladdr");
    }
#endif
    return std::string();
}

std::string aExePath()
{

#ifdef _WIN32
    wchar_t buffer[MAX_PATH] = { 0 };
    if (GetModuleFileNameW(NULL, buffer, MAX_PATH)) {
        std::string exepath;
        aWideToUtf8(buffer, exepath);
        return exepath;
    }
    else {
        aError("failed to call GetModuleFileNameW");
    }
#elif defined(__APPLE__)
    // macOS平台实现
    char buffer[PATH_MAX] = { 0 };
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        // 解析符号链接，如果是链接
        char resolvedPath[PATH_MAX] = { 0 };
        if (realpath(buffer, resolvedPath) != nullptr) {
            return resolvedPath;
        }
        return buffer;
    }
    else {
        aError("failed to call _NSGetExecutablePath");
    }
#elif defined(__FreeBSD__)
    // FreeBSD平台实现
    char buffer[PATH_MAX] = { 0 };
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
    size_t size = sizeof(buffer);
    if (sysctl(mib, 4, buffer, &size, NULL, 0) == 0) {
        return buffer;
    }
    else {
        aError("failed to call sysctl");
    }
#else
    // Linux及其他Unix-like系统实现
    char buffer[PATH_MAX] = { 0 };
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return buffer;
    }
    else {
        aError("failed to read /proc/self/exe");
    }
#endif
    return std::string();
}


AST_NAMESPACE_END