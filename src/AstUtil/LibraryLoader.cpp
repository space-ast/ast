/// @file      LibraryLoader.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      16.11.2025
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
 
 
#include "LibraryLoader.hpp"
#include "Encode.hpp"
#include <stdio.h>
#include <cstring>

// 平台特定的头文件包含
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif
 
AST_NAMESPACE_BEGIN



void* aLoadLibrary(const char* filepath)
{
    if (!filepath) {
        return nullptr;
    }

#if defined(_WIN32) || defined(_WIN64)
    // Windows平台
	std::wstring widePath;
	aUtf8ToWide(filepath, widePath);
    return LoadLibraryW(widePath.c_str());
#else
    static char* prefixes[]{ "", "lib" };
    static char* suffixes[]{ "", ".so" };
    
    // Linux/Unix平台
    // RTLD_LAZY: 延迟绑定，只在需要时解析符号
    // RTLD_LOCAL: 符号不与其他模块共享
	int flag = RTLD_LAZY | RTLD_LOCAL;

	bool has_dot = strrchr(filepath, '.') != nullptr;
	bool has_dir_sep = strchr(filepath, '/') != nullptr;
    
    if (has_dot || has_dir_sep) {
        // 如果路径中已经包含扩展名或目录分隔符，则直接尝试加载
        return dlopen(filepath, flag);
	}

	// 尝试不同的前缀和后缀组合加载库
    for (const char* prefix : prefixes) {
        for (const char* suffix : suffixes) {
            std::string fullPath = std::string(prefix) + filepath + std::string(suffix);
            void* handle = dlopen(fullPath.c_str(), flag);
            if (handle) {
                return handle;
            }
        }
	}

    return nullptr;
#endif
}


void* aGetProcAddress(void* lib, const char* funcName)
{
    if (!lib || !funcName) {
        return nullptr;
    }

#if defined(_WIN32) || defined(_WIN64)
    // Windows平台
    return (void*)GetProcAddress((HMODULE)lib, funcName);
#else
    // Linux/Unix平台
    return dlsym(lib, funcName);
#endif
}


void* aResolveProcAddress(const char* filepath, const char* funcName)
{
    if (!filepath || !funcName) {
        return nullptr;
    }

    // 加载动态库
    void* library = aLoadLibrary(filepath);
    if (!library) {
        return nullptr;
    }

    // 获取函数指针
    void* procAddress = aGetProcAddress(library, funcName);

    // 注意：这里不卸载库，因为调用者需要使用这个库的函数
    // @fixme 应该在适当的时候调用相应的卸载函数
    return procAddress;
}


err_t aFreeLibrary(void* lib)
{
    if (!lib) {
        return eErrorNullInput;
    }

#if defined(_WIN32) || defined(_WIN64)
    // Windows平台
    return FreeLibrary((HMODULE)lib) != 0;
#else
    // Linux/Unix平台
    return dlclose(lib) == 0;
#endif
}


const char* aGetLoadError()
{
#if defined(_WIN32) || defined(_WIN64)
    // Windows平台 
    // @todo：这里返回的是简单的错误码，实际使用时可能需要格式化错误消息
    static char buffer[32];
    DWORD errorCode = GetLastError();
    snprintf(buffer, sizeof(buffer), "Error code: %lu", errorCode);
    return buffer;
#else
    // Linux/Unix平台
    return dlerror();
#endif
}

AST_NAMESPACE_END