/// @file      SnoptAPI.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      27.11.2025
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


#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/FileSystem.hpp"
#include "SnoptAPI.hpp"
#include <mutex>

#define AST_SNOPT_LIB_NAME      "csnopt"
#define AST_SNOPT_LIB_NAME_ALT  "snopt"


AST_NAMESPACE_BEGIN

namespace
{
    SnoptCAPI s_snoptAPI{};
    void* s_snoptLibHandle = nullptr;
    std::mutex s_snoptMutex;
}

SnoptCAPI* aSnopt_Load(const char* sharedLibPath)
{
    if (s_snoptLibHandle) {
        return &s_snoptAPI;
    }
    else {
        std::lock_guard<std::mutex> lock(s_snoptMutex); // 使用互斥锁保护加载过程
		// 双重检查
        if (s_snoptLibHandle) {
            return &s_snoptAPI;
        }
        // 尝试加载库
        s_snoptLibHandle = aLoadLibrary(sharedLibPath);
        if (s_snoptLibHandle) {
            s_snoptAPI.snopta = decltype(&snopta_)(aGetProcAddress(s_snoptLibHandle, A_STR(snopta_)));
            s_snoptAPI.snmema = decltype(&snmema_)(aGetProcAddress(s_snoptLibHandle, A_STR(snmema_)));
            s_snoptAPI.snjac = decltype(&snjac_)(aGetProcAddress(s_snoptLibHandle, A_STR(snjac_)));

            for (auto func : s_snoptAPI) {
                if (func == nullptr) {
                    // 如果有任何一个函数加载失败，则认为整个加载失败
                    aSnopt_Unload();
                    return nullptr;
                }
            }
        }
    }
    return aSnopt_Get();
}

void aSnopt_Unload()
{
    s_snoptAPI = {}; // 清空函数指针
    if (s_snoptLibHandle) {
        aFreeLibrary(s_snoptLibHandle);
        s_snoptLibHandle = nullptr;
    }
}

static SnoptCAPI* aSnopt_Load(const fs::path& path)
{
    return aSnopt_Load(path.c_str());
}

SnoptCAPI* aSnopt_Find()
{
    if(s_snoptLibHandle)
        return &s_snoptAPI;
    SnoptCAPI* api;
    if ((api = aSnopt_Load(fs::path(aLibDir()) / AST_SNOPT_LIB_NAME)))
        return api;
    if ((api = aSnopt_Load(fs::current_path() / AST_SNOPT_LIB_NAME)))
        return api;
    if ((api = aSnopt_Load(fs::path(aLibDir()) / AST_SNOPT_LIB_NAME_ALT)))
        return api;
    if ((api = aSnopt_Load(fs::current_path() / AST_SNOPT_LIB_NAME_ALT)))
        return api;
    return nullptr;
}

SnoptCAPI* aSnopt_Get()
{
    if (s_snoptLibHandle)
        return &s_snoptAPI;
    return nullptr;
}




AST_NAMESPACE_END