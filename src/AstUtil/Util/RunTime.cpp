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


errc_t aDataDirGetDefault(std::string &dataDirOut)
{
    // 1. 检查AST_DATA_DIR环境变量
    {
        // #pragma warning(suppress: 4996)
        const char* datadir = getenv(AST_ENV_DATA_DIR);
        std::error_code ec;
        // aDebug("AST_ENV_DATA_DIR: %s\n", datadir?datadir:"(not set)");
        if (datadir && fs::is_directory(datadir, ec)){
            dataDirOut = datadir;
            return eNoError;
        }
    }

    // 2. 检查动态库目录的data文件夹
    {
        fs::path datadir = fs::path(aLibDir()) / AST_DATA_DIR_NAME;
        std::error_code ec;
        // aDebug("datadir: %s\n", datadir.string().c_str());
        if (fs::is_directory(datadir, ec)){
            dataDirOut = datadir.string();
            return eNoError;
        }
    }

    // 3. 检查可执行文件目录的data文件夹
    {
        fs::path datadir = fs::path(aExeDir()) / AST_DATA_DIR_NAME;
        std::error_code ec;
        // aDebug("datadir: %s\n", datadir.string().c_str());
        if (fs::is_directory(datadir, ec))
        {
            dataDirOut = datadir.string();
            return eNoError;
        }
    }

    // 4. 检查当前运行目录的data文件夹
    {
        std::error_code ec;
        fs::path datadir = fs::current_path(ec) / AST_DATA_DIR_NAME;
        // aDebug("datadir: %s\n", datadir.string().c_str());
        if (!ec && fs::is_directory(datadir, ec))
        {
            dataDirOut = datadir.string();
            return eNoError;
        }
    }

    aError("data dir not found");
    // 如果所有路径都不存在，返回默认的相对路径
    dataDirOut = AST_DATA_DIR_NAME;
    return eErrorNotFound;
}

std::string aDataDirGetDefault()
{
    std::string dataDirOut;
    errc_t rc = aDataDirGetDefault(dataDirOut);
    A_UNUSED(rc);
    return dataDirOut;
}

typedef errc_t (*DataDirFunc)(std::string&);

DataDirFunc fDataDir{nullptr};

errc_t aDataDir(std::string& datadir)
{
    if(A_UNLIKELY(!fDataDir))
    {
        fDataDir = (DataDirFunc)aResolveProcAddress(AST_LIB_LINKNAME("AstCore"), "aDataDirGet");
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
    errc_t rc = aDataDir(dataDirOut);
    A_UNUSED(rc);
    return dataDirOut;
}





AST_NAMESPACE_END
