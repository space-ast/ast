#include "ExtractorImplSystem.hpp"
#include "../ArchiverUtils.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

AST_NAMESPACE_BEGIN

// ============================================================
// 接口实现
// ============================================================

ExtractorImplSystem& ExtractorImplSystem::Instance()
{
    static ExtractorImplSystem instance;
    return instance;
}

bool ExtractorImplSystem::isSupported() const
{
    // 至少有一个可用的解压工具
    return aIsCommandAvailable("tar")
        || aIsCommandAvailable("unzip")
        || aIsCommandAvailable("7z");
}

bool ExtractorImplSystem::canExtract(StringView source) const
{
    EArchiveFormat fmt = aDetectArchiveFormat(source);
    switch (fmt)
    {
    case EArchiveFormat::eTar:
    case EArchiveFormat::eTarGz:
    case EArchiveFormat::eTarBz2:
    case EArchiveFormat::eTarXz:
    case EArchiveFormat::eZip:
    case EArchiveFormat::e7z:
    case EArchiveFormat::eRar:
    case EArchiveFormat::eGz:
        return true;
    default:
        return false;
    }
}

errc_t ExtractorImplSystem::extract(StringView source, StringView target) const
{
    if (source.empty() || target.empty())
    {
        aError("ExtractorImplSystem: source or target is empty");
        return eErrorInvalidParam;
    }

    EArchiveFormat fmt = aDetectArchiveFormat(source);

    // .gz 是单文件压缩，target 是输出文件路径（非目录）
    // 需要在 switch 之前单独处理，避免下面把 target 当目录创建
    if (fmt == EArchiveFormat::eGz)
    {
        std::string srcStr(source);
        std::string tgtStr(target);

        // 确保目标文件的父目录存在
        fs::path tgtPath(tgtStr);
        fs::path parentDir = tgtPath.parent_path();
        if (!parentDir.empty() && !fs::exists(parentDir))
        {
            if (!fs::create_directories(parentDir))
            {
                aError("ExtractorImplSystem: cannot create parent directory for .gz output: %s",
                       parentDir.string().c_str());
                return eErrorInvalidFile;
            }
        }

        // 优先使用 gzip（Linux 标配，最轻量）
        if (aIsCommandAvailable("gzip"))
        {
            std::string cmd = "gzip -d -c \"";
            cmd += srcStr;
            cmd += "\" > \"";
            cmd += tgtStr;
            cmd += "\"";
            return aRunCommand(cmd);
        }

        // 回退到 7z
        if (aIsCommandAvailable("7z"))
        {
            std::string cmd = "7z e \"" + srcStr + "\" -so -y > \"" + tgtStr + "\"";
            return aRunCommand(cmd);
        }

        aError("ExtractorImplSystem: no tool available for .gz (need gzip or 7z)");
        return eErrorNotImplemented;
    }

    // 确保目标目录存在（tar/zip/7z 等格式解压到目录）
    fs::path targetPath = std::string(target);
    if (!fs::exists(targetPath))
    {
        if (!fs::create_directories(targetPath))
        {
            aError("cannot create target directory: %s", targetPath.string().c_str());
            return eErrorInvalidFile;
        }
    }

    std::string srcStr(source);
    std::string tgtStr(target);

    // 根据格式选择命令，按优先级探测
    switch (fmt)
    {
    case EArchiveFormat::eTar:
    case EArchiveFormat::eTarGz:
    case EArchiveFormat::eTarBz2:
    case EArchiveFormat::eTarXz:
    {
        if (!aIsCommandAvailable("tar"))
        {
            // tar 不可用，尝试 7z
            if (aIsCommandAvailable("7z"))
            {
                std::string cmd = "7z x \"" + srcStr + "\" -o\"" + tgtStr + "\" -y";
                return aRunCommand(cmd);
            }
            aError("ExtractorImplSystem: tar is not available");
            return eErrorNotImplemented;
        }

        std::string cmd = "tar -x";
        const char* flag = aTarCompressFlag(fmt);
        if (flag) { cmd += flag; }
        cmd += "f \"";
        cmd += srcStr;
        cmd += "\" -C \"";
        cmd += tgtStr;
        cmd += "\"";
        return aRunCommand(cmd);
    }

    case EArchiveFormat::eZip:
    {
        if (aIsCommandAvailable("unzip"))
        {
            std::string cmd = "unzip -o \"";
            cmd += srcStr;
            cmd += "\" -d \"";
            cmd += tgtStr;
            cmd += "\"";
            return aRunCommand(cmd);
        }
        // unzip 不可用，尝试 7z
        if (aIsCommandAvailable("7z"))
        {
            std::string cmd = "7z x \"" + srcStr + "\" -o\"" + tgtStr + "\" -y";
            return aRunCommand(cmd);
        }
        // 最后尝试 tar（某些 tar 版本支持 zip）
        if (aIsCommandAvailable("tar"))
        {
            std::string cmd = "tar -xf \"";
            cmd += srcStr;
            cmd += "\" -C \"";
            cmd += tgtStr;
            cmd += "\"";
            return aRunCommand(cmd);
        }
        aError("ExtractorImplSystem: no tool available for .zip");
        return eErrorNotImplemented;
    }

    case EArchiveFormat::e7z:
    case EArchiveFormat::eRar:
    {
        if (aIsCommandAvailable("7z"))
        {
            std::string cmd = "7z x \"" + srcStr + "\" -o\"" + tgtStr + "\" -y";
            return aRunCommand(cmd);
        }
        aError("ExtractorImplSystem: 7z is not available");
        return eErrorNotImplemented;
    }

    default:
        aError("ExtractorImplSystem: unsupported format");
        return eErrorUnsupported;
    }
}

AST_NAMESPACE_END
