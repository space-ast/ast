/// @file      RunTime.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      26.11.2025
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
 
 
#include "RunTime.hpp"
#include "GlobalContext.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include <assert.h>


AST_NAMESPACE_BEGIN



// 线程本地存储的当前全局上下文指针
A_THREAD_LOCAL GlobalContext* t_currentGlobalContext = nullptr;
std::unique_ptr<GlobalContext> g_defaultGlobalContext = nullptr;


err_t aInitialize(GlobalContext* context)
{
    return eNoError;
}


err_t aInitialize()
{
    auto context = aGlobalContext_Ensure();
    return aInitialize(context);
}

err_t aUninitialize()
{
    if (g_defaultGlobalContext) {
        g_defaultGlobalContext = nullptr;
    }
    // 线程局部变量 t_currentGlobalContext 通常不需要在此处显式删除，
    // 因为它指向的是 g_defaultGlobalContext，并且会在线程退出时自动销毁。
    return eNoError;
}

std::string aDataDirGet()
{
    auto context = aGlobalContext_Ensure();
    if (context->dataDir().empty()) {
        aDataDirSet(aDataDirGetDefault());
    }
    return context->dataDir();
}

err_t aDataDirSet(const std::string& dirpath)
{
    if (!fs::is_directory(dirpath)) {
        aError("dirpath is not a directory.");
        return eErrorInvalidParam;
    }
    auto context = aGlobalContext_Ensure();
    context->setDataDir(dirpath);
    return eNoError;
}

std::string aDataDirGetDefault()
{
    // 1. 检查AST_DATA_DIR环境变量
    try {
        const char* datadir = getenv(AST_ENV_DATA_DIR);
        if (datadir && fs::is_directory(datadir))
            return datadir;
    }
    catch (const std::exception& e)
    {
        // 忽略可能的异常, 或者记录下来
        aWarning("Exception while searching for data directory: %s", e.what());
    }
    catch (...)
    {
        aWarning("Exception while searching for data directory");
        // 忽略可能的异常
    }

    // 2. 检查动态库目录的data文件夹
    try {
        fs::path datadir = fs::path(aLibDir()) / AST_DATA_DIR_NAME;
        if (fs::is_directory(datadir))
            return datadir.string();
    }
    catch (const std::exception& e)
    {
        // 忽略可能的异常, 或者记录下来
        aWarning("Exception while searching for data directory: %s", e.what());
    }
    catch (...)
    {
        aWarning("Exception while searching for data directory");
        // 忽略可能的异常
    }
    
    // 3. 检查可执行文件目录的data文件夹
    try {
        fs::path datadir = fs::path(aExeDir()) / AST_DATA_DIR_NAME;
        if (fs::is_directory(datadir))
            return datadir.string();
    }
    catch (const std::exception& e)
    {
        // 忽略可能的异常, 或者记录下来
        aWarning("Exception while searching for data directory: %s", e.what());
    }
    catch (...)
    {
        aWarning("Exception while searching for data directory");
        // 忽略可能的异常
    }
    
    // 4. 检查当前运行目录的data文件夹
    try {
        fs::path currentdir = fs::current_path() / AST_DATA_DIR_NAME;
        if (fs::is_directory(currentdir))
            return currentdir.string();
    }
    catch (const std::exception& e)
    {
        // 忽略可能的异常, 或者记录下来
        aWarning("Exception while searching for data directory: %s", e.what());
    }
    catch (...)
    {
        aWarning("Exception while searching for data directory");
        // 忽略可能的异常
    }

    aError("data dir not found");
    // 如果所有路径都不存在，返回默认的相对路径
    return AST_DATA_DIR_NAME;
}


GlobalContext* aGlobalContext_GetCurrent()
{
    assert(t_currentGlobalContext && "Current GlobalContext is null!");
    return t_currentGlobalContext;
}

GlobalContext* aGlobalContext_GetDefault()
{
    return g_defaultGlobalContext.get();
}

GlobalContext* aGlobalContext_EnsureDefault()
{
    if (!g_defaultGlobalContext) {
        g_defaultGlobalContext.reset(aGlobalContext_New());
    }
    return g_defaultGlobalContext.get();
}

GlobalContext* aGlobalContext_Ensure()
{
    if (!t_currentGlobalContext)
    {
        aGlobalContext_SetCurrent(aGlobalContext_EnsureDefault());
    }
    return t_currentGlobalContext;
}

void aGlobalContext_SetCurrent(GlobalContext* context)
{
    t_currentGlobalContext = context;
}

GlobalContext* aGlobalContext_New()
{
    return new GlobalContext{};
}


 
AST_NAMESPACE_END