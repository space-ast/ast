///
/// @file      RunTime.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-18
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "RunTime.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/LibraryLoader.hpp"


AST_NAMESPACE_BEGIN


err_t aDataDirGetDefault(std::string &dataDirOut)
{
    // 1. 检查AST_DATA_DIR环境变量
    try {
        // #pragma warning(suppress: 4996)
        const char* datadir = getenv(AST_ENV_DATA_DIR);
        // aDebug("AST_ENV_DATA_DIR: %s\n", datadir?datadir:"(not set)");
        if (datadir && fs::is_directory(datadir)){
            dataDirOut = datadir;
            return eNoError;
        }
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
        // aDebug("datadir: %s\n", datadir.string().c_str());
        if (fs::is_directory(datadir)){
            dataDirOut = datadir.string();
            return eNoError;
        }
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
        // aDebug("datadir: %s\n", datadir.string().c_str());
        if (fs::is_directory(datadir))
        {
            dataDirOut = datadir.string();
            return eNoError;
        }
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
        fs::path datadir = fs::current_path() / AST_DATA_DIR_NAME;
        // aDebug("datadir: %s\n", datadir.string().c_str());
        if (fs::is_directory(datadir))
        {
            dataDirOut = datadir.string();
            return eNoError;
        }
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
    dataDirOut = AST_DATA_DIR_NAME;
    return eErrorNotFound;
}

std::string aDataDirGetDefault()
{
    std::string dataDirOut;
    err_t rc = aDataDirGetDefault(dataDirOut);
    A_UNUSED(rc);
    return dataDirOut;
}

typedef err_t (*DataDirFunc)(std::string&);

DataDirFunc fDataDir{nullptr};

err_t aDataDir(std::string& datadir)
{
    if(A_UNLIKELY(!fDataDir))
    {
        fDataDir = (DataDirFunc)aResolveProcAddress(AST_DEBUG_SUFFIX("AstCore"), "aDataDirGet");
        if(!fDataDir){
            aError("failed to load 'AstCore' and resolve function 'aDataDirGet'");
            fDataDir = &aDataDirGetDefault;
        }
    }
    return fDataDir(datadir);
}

std::string aDataDir()
{
    std::string dataDirOut;
    err_t rc = aDataDir(dataDirOut);
    A_UNUSED(rc);
    return dataDirOut;
}





AST_NAMESPACE_END
