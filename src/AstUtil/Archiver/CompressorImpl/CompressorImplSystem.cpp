#include "CompressorImplSystem.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

AST_NAMESPACE_BEGIN

// ============================================================
// 辅助函数
// ============================================================

namespace
{
    // 检查路径中是否包含 shell 元字符（防止命令注入）
    bool hasShellMetacharacters(const std::string& path)
    {
        for (char c : path)
        {
            switch (c)
            {
            case '`': case '$': case '!':
            case '|': case ';': case '<': case '>':
            case '(': case ')': case '\n': case '\r':
                return true;
            default:
                break;
            }
        }
        return false;
    }
}

// ============================================================
// 接口实现
// ============================================================

CompressorImplSystem& CompressorImplSystem::Instance()
{
    static CompressorImplSystem instance;
    return instance;
}

bool CompressorImplSystem::isSupported() const
{
    // 至少有一个可用的压缩工具
    return aIsCommandAvailable("tar")
        || aIsCommandAvailable("zip")
        || aIsCommandAvailable("7z");
}

bool CompressorImplSystem::canCompress(StringView source, StringView target) const
{
    // 检查目标格式并确认有对应的工具可用
    EArchiveFormat fmt = aDetectArchiveFormat(target);
    switch (fmt)
    {
    case EArchiveFormat::eTar:
    case EArchiveFormat::eTarGz:
    case EArchiveFormat::eTarBz2:
    case EArchiveFormat::eTarXz:
        return aIsCommandAvailable("tar") || aIsCommandAvailable("7z");

    case EArchiveFormat::eZip:
        return aIsCommandAvailable("zip") || aIsCommandAvailable("7z");

    case EArchiveFormat::e7z:
        return aIsCommandAvailable("7z");

    default:
        return false;
    }
}

errc_t CompressorImplSystem::compress(StringView source, StringView target, StringView curdir) const
{
    if (source.empty() || target.empty())
    {
        aError("CompressorImplSystem: source or target is empty");
        return eErrorInvalidParam;
    }

    EArchiveFormat fmt = aDetectArchiveFormat(target);

    std::string srcStr(source.data(), source.size());
    std::string tgtStr(target.data(), target.size());

    // 安全检查：拒绝包含 shell 元字符的路径，防止命令注入
    if (hasShellMetacharacters(srcStr) || hasShellMetacharacters(tgtStr))
    {
        aError("CompressorImplSystem: path contains invalid shell metacharacters");
        return eErrorInvalidParam;
    }
    {
        std::string curStr(curdir.data(), curdir.size());
        if (!curStr.empty() && hasShellMetacharacters(curStr))
        {
            aError("CompressorImplSystem: curdir path contains invalid shell metacharacters");
            return eErrorInvalidParam;
        }
    }

    // 统一行为：curdir 为空 = source.parent()，计算工作目录和相对路径
    std::string curAbs, relativePath;
    aResolveArchivePath(source, curdir, curAbs, relativePath);

#ifdef _WIN32
    const char* kCdPrefix = "cd /d \"";
#else
    const char* kCdPrefix = "cd \"";
#endif

    // 根据目标格式选择命令
    switch (fmt)
    {
    case EArchiveFormat::eTar:
    case EArchiveFormat::eTarGz:
    case EArchiveFormat::eTarBz2:
    case EArchiveFormat::eTarXz:
    {
        if (!aIsCommandAvailable("tar"))
        {
            if (aIsCommandAvailable("7z"))
            {
                std::string cmd;
                cmd += kCdPrefix;
                cmd += curAbs;
                cmd += "\" && 7z a \"";
                cmd += tgtStr;
                cmd += "\" \"";
                cmd += relativePath;
                cmd += "\"";
                return aRunCommand(cmd);
            }
            aError("CompressorImplSystem: tar is not available");
            return eErrorNotImplemented;
        }

        std::string cmd = "tar -c";
        const char* flag = aTarCompressFlag(fmt);
        if (flag) { cmd += flag; }
        cmd += "f \"";
        cmd += tgtStr;
        cmd += "\" -C \"";
        cmd += curAbs;
        cmd += "\" \"";
        cmd += relativePath;
        cmd += "\"";
        return aRunCommand(cmd);
    }

    case EArchiveFormat::eZip:
    {
        if (aIsCommandAvailable("zip"))
        {
            std::string cmd;
            cmd += kCdPrefix;
            cmd += curAbs;
            cmd += "\" && zip -r \"";
            cmd += tgtStr;
            cmd += "\" \"";
            cmd += relativePath;
            cmd += "\"";
            return aRunCommand(cmd);
        }
        if (aIsCommandAvailable("7z"))
        {
            std::string cmd;
            cmd += kCdPrefix;
            cmd += curAbs;
            cmd += "\" && 7z a \"";
            cmd += tgtStr;
            cmd += "\" \"";
            cmd += relativePath;
            cmd += "\"";
            return aRunCommand(cmd);
        }
        aError("CompressorImplSystem: no tool available for .zip");
        return eErrorNotImplemented;
    }

    case EArchiveFormat::e7z:
    {
        if (aIsCommandAvailable("7z"))
        {
            std::string cmd;
            cmd += kCdPrefix;
            cmd += curAbs;
            cmd += "\" && 7z a \"";
            cmd += tgtStr;
            cmd += "\" \"";
            cmd += relativePath;
            cmd += "\"";
            return aRunCommand(cmd);
        }
        aError("CompressorImplSystem: 7z is not available");
        return eErrorNotImplemented;
    }

    default:
        aError("CompressorImplSystem: unsupported target format");
        return eErrorUnsupported;
    }
}

AST_NAMESPACE_END
