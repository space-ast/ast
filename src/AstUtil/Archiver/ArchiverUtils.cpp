#include "ArchiverUtils.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/LocalBuffer.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#define A_POPEN  _popen
#define A_PCLOSE _pclose
#define A_NULL_DEVICE "NUL"
#else
#define A_POPEN  popen
#define A_PCLOSE pclose
#define A_NULL_DEVICE "/dev/null"
#endif

AST_NAMESPACE_BEGIN

// ============================================================
// 归档路径解析（CompressorImplTar / CompressorImplSystem 共享）
// ============================================================

void aResolveArchivePath(StringView source, StringView curdir,
                          std::string& outWorkDir, std::string& outRelativePath)
{
    fs::path srcPath(source.data());
    fs::path curPath;

    if (curdir.empty())
        curPath = srcPath.parent_path();
    else
        curPath = fs::path(curdir.data());

    // 使用 fs::relative 计算相对路径（对标 C++17）
    std::error_code ec;
    fs::path rel = fs::relative(srcPath, curPath, ec);
    if (ec || rel.empty())
        outRelativePath = ".";
    else
        outRelativePath = rel.string();

    outWorkDir = curPath.string();
}

// ============================================================
// TAR 格式工具函数（CompressorImplTar 和 ExtractorImplTar 共享）
// ============================================================

void aTarCalculateChecksum(TarHeader& header)
{
    // 先将 chksum 字段填充为空格
    std::memset(header.chksum, ' ', sizeof(header.chksum));

    unsigned int sum = 0;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(&header);
    for (size_t i = 0; i < sizeof(TarHeader); ++i)
        sum += p[i];

    // 将校验和格式化为 6 位八进制数 + 空字符 + 空格
    char chk[9] = {};
    snprintf(chk, sizeof(chk), "%06o", sum);
    std::memcpy(header.chksum, chk, 7);
    header.chksum[7] = ' ';
}

size_t aTarParseOctal(const char* field, size_t len)
{
    const char* end = field + len;
    // 跳过前导空格或空字符
    while (field < end && (*field == ' ' || *field == '\0'))
        ++field;

    size_t value = 0;
    while (field < end && *field >= '0' && *field <= '7')
    {
        value = (value << 3) + static_cast<size_t>(*field - '0');
        ++field;
    }
    return value;
}

bool aTarIsZeroBlock(const TarHeader& header)
{
    const char* p = reinterpret_cast<const char*>(&header);
    for (size_t i = 0; i < sizeof(TarHeader); ++i)
    {
        if (p[i] != '\0') return false;
    }
    return true;
}

namespace
{
    // 读取文件开头的魔数字节
    bool readMagicBytes(StringView path, unsigned char* buf, size_t count)
    {
        FILE* fp = posix::fopen(path.data(), "rb");
        if (!fp) return false;
        size_t nread = fread(buf, 1, count, fp);
        fclose(fp);
        return nread == count;
    }

    // 获取文件扩展名（小写）
    std::string getExtensionLower(StringView path)
    {
        std::string s(path.data(), path.size());
        auto pos = s.rfind('.');
        if (pos == std::string::npos) return {};
        std::string ext = s.substr(pos);
        for (auto& c : ext) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
        return ext;
    }

    // 检查路径是否以指定后缀结尾
    bool endsWith(StringView str, const char* suffix)
    {
        size_t slen = str.size();
        size_t suffixLen = std::strlen(suffix);
        if (slen < suffixLen) return false;
        for (size_t i = 0; i < suffixLen; ++i)
        {
            char c1 = static_cast<char>(::tolower(static_cast<unsigned char>(str.data()[slen - suffixLen + i])));
            char c2 = suffix[i];
            if (c2 >= 'A' && c2 <= 'Z') c2 = static_cast<char>(c2 - 'A' + 'a');
            if (c1 != c2) return false;
        }
        return true;
    }
}

EArchiveFormat aDetectArchiveFormat(StringView path)
{
    if (path.empty()) return EArchiveFormat::eUnknown;

    std::string ext = getExtensionLower(path);

    // 先通过扩展名检测
    if (ext == ".zip")
        return EArchiveFormat::eZip;

    if (ext == ".7z")
        return EArchiveFormat::e7z;

    if (ext == ".rar")
        return EArchiveFormat::eRar;

    if (ext == ".tar")
        return EArchiveFormat::eTar;

    if (ext == ".tgz" || endsWith(path, ".tar.gz"))
        return EArchiveFormat::eTarGz;

    if (ext == ".tbz2" || ext == ".tbz" || endsWith(path, ".tar.bz2"))
        return EArchiveFormat::eTarBz2;

    if (ext == ".txz" || endsWith(path, ".tar.xz"))
        return EArchiveFormat::eTarXz;

    if (ext == ".gz")
    {
        // .gz 可能是单独的 gzip 文件，也可能是 .tar.gz 的一部分
        // 如果已经被上面的 .tar.gz 处理了就不会到这里
        return EArchiveFormat::eGz;
    }

    // 扩展名无法判断时，尝试魔数检测
    unsigned char magic[8] = {};
    if (!readMagicBytes(path, magic, sizeof(magic)))
        return EArchiveFormat::eUnknown;

    // ZIP: PK\x03\x04
    if (magic[0] == 0x50 && magic[1] == 0x4B && magic[2] == 0x03 && magic[3] == 0x04)
        return EArchiveFormat::eZip;

    // 7z: 7z\xBC\xAF\x27\x1C
    if (magic[0] == 0x37 && magic[1] == 0x7A && magic[2] == 0xBC && magic[3] == 0xAF
        && magic[4] == 0x27 && magic[5] == 0x1C)
        return EArchiveFormat::e7z;

    // RAR: Rar!\x1A\x07\x00 (v4) or Rar!\x1A\x07\x01\x00 (v5)
    if (magic[0] == 0x52 && magic[1] == 0x61 && magic[2] == 0x72 && magic[3] == 0x21
        && magic[4] == 0x1A && magic[5] == 0x07)
        return EArchiveFormat::eRar;

    // GZ: \x1F\x8B
    if (magic[0] == 0x1F && magic[1] == 0x8B)
        return EArchiveFormat::eGz;

    // TAR: ustar 魔数在偏移 257 处
    {
        FILE* fp = posix::fopen(path.data(), "rb");
        if (fp)
        {
            unsigned char ustar[6] = {};
            if (fseek(fp, 257, SEEK_SET) == 0)
            {
                size_t nr = fread(ustar, 1, 5, fp);
                fclose(fp);
                if (nr == 5 && std::memcmp(ustar, "ustar", 5) == 0)
                    return EArchiveFormat::eTar;
            }
            else
            {
                fclose(fp);
            }
        }
    }

    return EArchiveFormat::eUnknown;
}

bool aIsArchiveFile(StringView path)
{
    EArchiveFormat fmt = aDetectArchiveFormat(path);
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

// ============================================================
// System 后端共享工具函数（CompressorImplSystem 和 ExtractorImplSystem 共享）
// ============================================================

bool aIsCommandAvailable(const char* cmd)
{
    // 通过执行测试标志验证命令实际可用
    const char* testFlags[] = { " --version", " --help", " -h", " /?" };
    for (const char* flag : testFlags)
    {
        std::string testCmd = std::string(cmd) + flag + " >" A_NULL_DEVICE " 2>&1";
        if (::system(testCmd.c_str()) == 0)
            return true;
    }
    return false;
}

errc_t aRunCommand(const std::string& cmd)
{
    FILE* pipe = A_POPEN(cmd.c_str(), "r");
    if (!pipe)
    {
        aError("aRunCommand: popen failed for: %s", cmd.c_str());
        return eError;
    }

    char buffer[4096];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe))
        output += buffer;

    int exitCode = A_PCLOSE(pipe);
    if (exitCode != 0)
    {
        if (!output.empty())
            aError("aRunCommand: command failed (exit=%d): %s\nOutput: %s",
                   exitCode, cmd.c_str(), output.c_str());
        else
            aError("aRunCommand: command failed (exit=%d): %s",
                   exitCode, cmd.c_str());
        return eError;
    }

    return eNoError;
}

const char* aTarCompressFlag(EArchiveFormat fmt)
{
    switch (fmt)
    {
    case EArchiveFormat::eTarGz:  return "z";
    case EArchiveFormat::eTarBz2: return "j";
    case EArchiveFormat::eTarXz:  return "J";
    default:                      return nullptr;
    }
}

// ============================================================
// Raw 后端共享工具函数（CompressorImplRaw 和 ExtractorImplRaw 共享）
// ============================================================

errc_t aCopyFile(StringView from, StringView to, const char* logPrefix)
{
    constexpr size_t kBufSize = 65536; // 64KB 缓冲区

    FILE* src = posix::fopen(from.data(), "rb");
    if (!src)
    {
        aError("%s: cannot open source file: %s", logPrefix, from.data());
        return eErrorInvalidFile;
    }

    FILE* dst = posix::fopen(to.data(), "wb");
    if (!dst)
    {
        aError("%s: cannot create target file: %s", logPrefix, to.data());
        fclose(src);
        return eErrorInvalidFile;
    }

    A_LOCAL_BUFFER(char, buf, kBufSize);
    errc_t ret = eNoError;

    while (true)
    {
        size_t nread = fread(buf, 1, kBufSize, src);
        if (nread == 0)
        {
            if (ferror(src))
            {
                aError("%s: read error", logPrefix);
                ret = eError;
            }
            break;
        }
        size_t nwritten = fwrite(buf, 1, nread, dst);
        if (nwritten != nread)
        {
            aError("%s: write error", logPrefix);
            ret = eError;
            break;
        }
    }

    fclose(src);
    fclose(dst);

    if (ret != eNoError)
    {
        fs::remove(fs::path(std::string(to.data(), to.size())));
    }
    return ret;
}

errc_t aCopyFileRange(FILE* src, FILE* dst, size_t size)
{
    constexpr size_t kBufSize = 65536; // 64KB 缓冲区
    A_LOCAL_BUFFER(char, buf, kBufSize);

    size_t remaining = size;
    while (remaining > 0)
    {
        size_t toRead = remaining < kBufSize ? remaining : kBufSize;
        size_t nread = fread(buf, 1, toRead, src);
        if (nread == 0)
        {
            if (ferror(src))
                aError("read error");
            else
                aError("unexpected EOF (expected %zu more bytes)", toRead);
            return eErrorInvalidFile;
        }

        size_t nwritten = fwrite(buf, 1, nread, dst);
        if (nwritten != nread)
        {
            aError("write error (wrote %zu of %zu bytes)", nwritten, nread);
            return eErrorInvalidFile;
        }
        remaining -= nread;
    }

    return eNoError;
}

errc_t aCopyDirectoryRecursive(const fs::path& srcDir, const fs::path& dstDir, const char* logPrefix)
{
    if (!fs::exists(dstDir))
    {
        if (!fs::create_directories(dstDir))
        {
            aError("%s: cannot create directory: %s", logPrefix, dstDir.string().c_str());
            return eErrorInvalidFile;
        }
    }

    for (auto it = fs::directory_iterator(srcDir); it != fs::directory_iterator(); ++it)
    {
        const auto& entry = *it;
        auto srcPath = entry.path();
        auto dstPath = dstDir / srcPath.filename();

        if (fs::is_directory(entry.status()))
        {
            errc_t err = aCopyDirectoryRecursive(srcPath, dstPath, logPrefix);
            if (err) return err;
        }
        else
        {
            std::string srcStr = srcPath.string();
            std::string dstStr = dstPath.string();
            errc_t err = aCopyFile(srcStr.c_str(), dstStr.c_str(), logPrefix);
            if (err) return err;
        }
    }

    return eNoError;
}

AST_NAMESPACE_END
