///
/// @file      DataDownloader.cpp
/// @brief     数据文件夹下载器实现
/// @details   ~
/// @author    axel
/// @date      2026-08-26
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "DataDownloader.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Network.hpp"
#include "AstUtil/Extract.hpp"
#include "AstUtil/RunTime.hpp"
#include <ctime>
#include <cstdio>

AST_NAMESPACE_BEGIN

namespace{

/// @brief 生成下载临时文件/目录用的时间标签
std::string makeTimeTag()
{
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    char buf[64];
    snprintf(buf, sizeof(buf), "%04d%02d%02d_%02d%02d%02d",
             local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
             local->tm_hour, local->tm_min, local->tm_sec);
    return buf;
}


class TempZipGuard
{
public:
    TempZipGuard() = default;
    TempZipGuard(const fs::path& libDir)
    {
        // 临时下载文件和临时解压目录（带时间标签避免相互覆盖）
        std::string dirName = "data_" + makeTimeTag();
        std::string fileName = dirName + ".zip";
        extractDir_ = libDir / dirName;
        zipFile_    = libDir / fileName;
    }
    ~TempZipGuard()
    {
        fs::remove_all(extractDir_);
        fs::remove(zipFile_);
    }
    fs::path& zipFile() { return zipFile_; }
    fs::path& extractDir() { return extractDir_; }
private:
    fs::path zipFile_{};
    fs::path extractDir_{};
};

}

errc_t aDownloadData(StringView dataDir)
{
    // 数据仓库 master 分支压缩包地址，优先尝试 gitcode，失败时回退到 github
    static const char* kDataUrls[] = {
        "https://raw.gitcode.com/space-ast/ast-data/archive/refs/heads/master.zip",
        "https://github.com/space-ast/ast-data/archive/refs/heads/master.zip",
    };

    // 默认目标 data 目录：动态库所在文件夹下的 data/
    fs::path parentDir, targetDir;
    if(dataDir.empty())
    {
        parentDir = aLibDir();
        targetDir = parentDir / AST_DATA_DIR_NAME;
    }
    else
    {
        targetDir = std::string(dataDir);
        parentDir = targetDir.parent_path();
        if(parentDir.empty())
            parentDir = ".";
        if(!fs::is_directory(parentDir))
        {
            aError("dir '%s' has no parent directory", targetDir.string().c_str());
            return eErrorNotFound;
        }
    }

    TempZipGuard zipGuard(parentDir);
    fs::path& tmpZip = zipGuard.zipFile();
    fs::path& tmpExDir = zipGuard.extractDir();

    // ---- 1. 下载压缩包 ----
    errc_t err = eError;
    for (const char* url : kDataUrls)
    {
        aInfo("downloading %s", url);
        err = aDownloadFile(url, tmpZip.string());
        if (err == eNoError) break;
        if (err == eErrorCancelled) break;   // 用户取消，不再尝试备用源
        aWarning("download from %s failed (err=%d)", url, err);
    }
    if (err != eNoError)
    {
        aError("all download sources failed (err=%d)", err);
        return err;
    }
    aInfo("downloaded to %s", tmpZip.string().c_str());

    // ---- 2. 解压到临时目录 ----
    err = aExtract(tmpZip.string(), tmpExDir.string());
    if (err != eNoError)
    {
        aError("extract failed (err=%d)", err);

        return err;
    }

    // ---- 3. 定位源码根目录 ----
    // GitHub/GitCode 压缩包会额外多出一层顶层打包文件夹
    // 若临时目录内只有一个子目录则使用该子目录，否则直接使用临时目录。
    fs::path dataRoot = tmpExDir;
    {
        int count = 0;
        fs::path onlyDir;
        for (auto& entry : fs::directory_iterator(tmpExDir))
        {
            ++count;
            if (fs::is_directory(entry.path()))
            {
                onlyDir = entry.path();
            }
        }
        if (count == 1)
            dataRoot = onlyDir;
    }
    if (!fs::is_directory(dataRoot))
    {
        aError("no data content under '%s'", dataRoot.string().c_str());
        return eErrorNotFound;
    }

    // ---- 4. 将下载的数据安装到目标 data 目录 ----
    // 若已有 data 目录，先改名为 .bak，避免同名目录导致 rename 失败；安装失败时还原。
    std::string backupPath;
    std::error_code ec;
    if (fs::exists(targetDir, ec)) {
        backupPath = targetDir.string() + ".bak";
        fs::remove_all(backupPath);
        if (!fs::rename(targetDir, backupPath)) {
            aError("failed to backup existing data dir '%s'", targetDir.string().c_str());
            return eError;
        }
    }
    // 移动解压出的数据目录到目标
    if (!fs::rename(dataRoot, targetDir))
    {
        aError("failed to install data into '%s'", targetDir.string().c_str());
        // 还原备份，避免数据目录状态损坏
        if (!backupPath.empty())
            fs::rename(backupPath, targetDir);
        return eError;
    }

    aInfo("data installed to %s", targetDir.string().c_str());
    return eNoError;
}

AST_NAMESPACE_END
