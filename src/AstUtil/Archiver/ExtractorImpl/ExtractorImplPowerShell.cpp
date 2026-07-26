#include "ExtractorImplPowerShell.hpp"

#ifdef _WIN32

#include "../ArchiverUtils.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"

#include <string>
#include <cctype>

AST_NAMESPACE_BEGIN

// ============================================================
// 接口实现
// ============================================================

ExtractorImplPowerShell& ExtractorImplPowerShell::Instance()
{
    static ExtractorImplPowerShell instance;
    return instance;
}

bool ExtractorImplPowerShell::isSupported() const
{
    return aIsCommandAvailable("powershell");
}

bool ExtractorImplPowerShell::canExtract(StringView source) const
{
    if (source.empty()) return false;

    // 委托给 aDetectArchiveFormat：仅独立的 .gz 文件返回 eGz
    // .tar.gz 返回 eTarGz，不由此后端处理
    EArchiveFormat fmt = aDetectArchiveFormat(source);
    return fmt == EArchiveFormat::eGz;
}

errc_t ExtractorImplPowerShell::extract(StringView source, StringView target) const
{
    if (source.empty() || target.empty())
    {
        aError("ExtractorImplPowerShell: source or target is empty");
        return eErrorInvalidParam;
    }

    // target 是输出文件路径（非目录）— 确保父目录存在
    fs::path targetPath = std::string(target);
    fs::path parentDir = targetPath.parent_path();
    if (!parentDir.empty() && !fs::exists(parentDir))
    {
        if (!fs::create_directories(parentDir))
        {
            aError("ExtractorImplPowerShell: cannot create parent directory: %s", parentDir.string().c_str());
            return eErrorInvalidFile;
        }
    }

    std::string srcStr(source.data(), source.size());
    std::string outStr = targetPath.string();

    // 4. 转义路径中的单引号（PowerShell 单引号字符串中 '' 表示一个单引号）
    auto escapeForPS = [](const std::string& s) -> std::string {
        std::string result;
        result.reserve(s.size() + 4);
        for (char c : s)
        {
            if (c == '\'')
                result += "''";
            else
                result += c;
        }
        return result;
    };

    std::string escapedSrc = escapeForPS(srcStr);
    std::string escapedOut = escapeForPS(outStr);

    // 5. 构造 PowerShell 命令
    // 兼容 PowerShell 2.0 (.NET 3.5 SP1):
    //   - 不使用 Stream.CopyTo()（.NET 4.0 才引入）
    std::string cmd = "powershell -Command \"";
    cmd += "$s=[System.IO.File]::OpenRead('" + escapedSrc + "');";
    cmd += "$d=[System.IO.File]::Create('" + escapedOut + "');";
    cmd += "$m=[System.IO.Compression.CompressionMode]::Decompress;";
    cmd += "$g=New-Object System.IO.Compression.GZipStream($s,$m);";
    cmd += "$b=New-Object byte[] 65536;";
    cmd += "while(($r=$g.Read($b,0,$b.Length))-gt 0){$d.Write($b,0,$r)};";
    cmd += "$g.Close();$d.Close();$s.Close()";
    cmd += "\"";

    return aRunCommand(cmd);
}

AST_NAMESPACE_END

#endif // _WIN32
