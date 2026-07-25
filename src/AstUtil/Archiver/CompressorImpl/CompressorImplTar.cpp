#include "CompressorImplTar.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/LocalBuffer.hpp"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdint>

// 跨平台大文件支持
#ifdef _WIN32
#define A_FTELL _ftelli64
#else
#define A_FTELL ftello
#endif

AST_NAMESPACE_BEGIN

// ============================================================
// 辅助函数
// ============================================================

errc_t CompressorImplTar::writeFileEntry(FILE* dst, const std::string& name,
                                          const std::string& filePath)
{
    // 打开源文件
    FILE* src = posix::fopen(filePath.c_str(), "rb");
    if (!src)
    {
        aError("CompressorImplTar: cannot open source file: %s", filePath.c_str());
        return eErrorInvalidFile;
    }

    // 获取文件大小（使用 64 位 API 支持大文件）
    fseek(src, 0, SEEK_END);
    int64_t fileSize = A_FTELL(src);
    fseek(src, 0, SEEK_SET);
    if (fileSize < 0) fileSize = 0;

    // 构建头部
    TarHeader header = {};
    std::memset(&header, 0, sizeof(TarHeader));

    // 文件名（如果超过 100 字节，需要 GNU 长文件名扩展或 prefix）
    size_t nameLen = name.size();
    if (nameLen < 100)
    {
        std::memcpy(header.name, name.c_str(), nameLen);
    }
    else
    {
        // 使用 prefix 字段：拆分出最后一个 '/' 之前的路径部分
        // 简单处理：如果名字超过 100 字节，把超出部分放到 prefix
        // 注意：prefix 最多 155 字节，name 最多 100 字节
        size_t splitPos = nameLen;
        if (nameLen > 100)
        {
            // 找到可行的拆分点
            for (size_t i = 100; i > 0; --i)
            {
                if (i < nameLen && name[i - 1] == '/')
                {
                    splitPos = i;
                    break;
                }
            }
        }
        if (splitPos <= 155 && (nameLen - splitPos) <= 100)
        {
            std::memcpy(header.prefix, name.c_str(), splitPos);
            std::memcpy(header.name, name.c_str() + splitPos, nameLen - splitPos);
        }
        else
        {
            // 文件名太长，截断
            std::memcpy(header.name, name.c_str(), 99);
            aError("CompressorImplTar: file name too long, truncating: %s", name.c_str());
        }
    }

    // 文件模式（八进制）：0644
    std::memcpy(header.mode, "000644 ", 7);
    header.mode[7] = '\0';

    // uid/gid
    std::memcpy(header.uid, "000000 ", 7);
    header.uid[7] = '\0';
    std::memcpy(header.gid, "000000 ", 7);
    header.gid[7] = '\0';

    // 文件大小（八进制）
    char sizeBuf[13] = {};
    snprintf(sizeBuf, sizeof(sizeBuf), "%011llo", static_cast<unsigned long long>(fileSize));
    std::memcpy(header.size, sizeBuf, 11);

    // 修改时间（八进制）
    char mtimeBuf[13] = {};
    snprintf(mtimeBuf, sizeof(mtimeBuf), "%011lo", static_cast<unsigned long>(std::time(nullptr)));
    std::memcpy(header.mtime, mtimeBuf, 11);

    // 类型标志：普通文件
    header.typeflag = '0';

    // USTAR 魔数
    std::memcpy(header.magic, "ustar ", 6);
    std::memcpy(header.version, "00", 2);

    // 计算校验和
    aTarCalculateChecksum(header);

    // 写入头部
    if (fwrite(&header, 1, sizeof(TarHeader), dst) != sizeof(TarHeader))
    {
        aError("CompressorImplTar: write error at header for: %s", name.c_str());
        fclose(src);
        return eError;
    }

    // 写入文件数据
    errc_t ret = eNoError;
    constexpr size_t kBufSize = 65536; // 64KB
    A_LOCAL_BUFFER(char, buf, kBufSize);

    size_t remaining = static_cast<size_t>(fileSize);
    while (remaining > 0)
    {
        size_t toRead = remaining < kBufSize ? remaining : kBufSize;
        size_t nread = fread(buf, 1, toRead, src);
        if (nread == 0)
        {
            if (ferror(src))
            {
                aError("CompressorImplTar: read error for file: %s", filePath.c_str());
                ret = eError;
            }
            break;
        }
        size_t nwritten = fwrite(buf, 1, nread, dst);
        if (nwritten != nread)
        {
            aError("CompressorImplTar: write error for file: %s", filePath.c_str());
            ret = eError;
            break;
        }
        remaining -= nread;
    }

    fclose(src);

    // 填充到 512 字节边界
    size_t pad = (512 - (static_cast<size_t>(fileSize) % 512)) % 512;
    if (pad > 0)
    {
        char zeroPad[512] = {};
        if (fwrite(zeroPad, 1, pad, dst) != pad)
        {
            aError("CompressorImplTar: write error at padding for: %s", name.c_str());
            ret = eError;
        }
    }

    return ret;
}

errc_t CompressorImplTar::writeDirectoryEntry(FILE* dst, const std::string& name)
{
    TarHeader header = {};
    std::memset(&header, 0, sizeof(TarHeader));

    // 目录名（确保以 '/' 结尾）
    std::string dirName = name;
    if (dirName.empty() || dirName.back() != '/')
        dirName += '/';

    size_t nameLen = dirName.size();
    if (nameLen < 100)
    {
        std::memcpy(header.name, dirName.c_str(), nameLen);
    }
    else
    {
        std::memcpy(header.name, dirName.c_str(), 99);
    }

    // 文件模式（八进制）：0755
    std::memcpy(header.mode, "000755 ", 7);
    header.mode[7] = '\0';

    // uid/gid
    std::memcpy(header.uid, "000000 ", 7);
    header.uid[7] = '\0';
    std::memcpy(header.gid, "000000 ", 7);
    header.gid[7] = '\0';

    // 目录大小为 0
    std::memcpy(header.size, "00000000000", 11);

    // 修改时间
    char mtimeBuf[13] = {};
    snprintf(mtimeBuf, sizeof(mtimeBuf), "%011lo", static_cast<unsigned long>(std::time(nullptr)));
    std::memcpy(header.mtime, mtimeBuf, 11);

    // 类型标志：目录
    header.typeflag = '5';

    // USTAR 魔数
    std::memcpy(header.magic, "ustar ", 6);
    std::memcpy(header.version, "00", 2);

    // 计算校验和
    aTarCalculateChecksum(header);

    // 写入头部
    if (fwrite(&header, 1, sizeof(TarHeader), dst) != sizeof(TarHeader))
    {
        aError("CompressorImplTar: write error at directory header for: %s", name.c_str());
        return eError;
    }

    return eNoError;
}

errc_t CompressorImplTar::archiveDirectory(FILE* dst, const fs::path& dirPath,
                                           const std::string& basePath)
{
    for (auto it = fs::directory_iterator(dirPath); it != fs::directory_iterator(); ++it)
    {
        const auto& entry = *it;
        auto entryPath = entry.path();
        fs::path relativePath = fs::path(basePath) / entryPath.filename();

        if (fs::is_directory(entry.status()))
        {
            // 写入目录条目
            errc_t err = writeDirectoryEntry(dst, relativePath.string());
            if (err) return err;

            // 递归处理子目录
            err = archiveDirectory(dst, entryPath, relativePath.string() + "/");
            if (err) return err;
        }
        else
        {
            // 写入文件条目
            errc_t err = writeFileEntry(dst, relativePath.string(), entryPath.string());
            if (err) return err;
        }
    }

    return eNoError;
}

// ============================================================
// 接口实现
// ============================================================

CompressorImplTar& CompressorImplTar::Instance()
{
    static CompressorImplTar instance;
    return instance;
}

bool CompressorImplTar::canCompress(StringView source, StringView target) const
{
    (void)source;
    // 检查目标扩展名是否为 .tar
    size_t len = target.size();
    if (len >= 4)
    {
        StringView ext(target.data() + len - 4, 4);
        if ((ext[0] == '.')
            && (ext[1] == 't' || ext[1] == 'T')
            && (ext[2] == 'a' || ext[2] == 'A')
            && (ext[3] == 'r' || ext[3] == 'R'))
        {
            return true;
        }
    }
    return false;
}

errc_t CompressorImplTar::compress(StringView source, StringView target, StringView curdir) const
{
    if (source.empty() || target.empty())
    {
        aError("CompressorImplTar: source or target is empty");
        return eErrorInvalidParam;
    }

    fs::path srcPath(source.data());
    if (!fs::exists(srcPath))
    {
        aError("CompressorImplTar: source does not exist: %s", source.data());
        return eErrorInvalidFile;
    }

    // 计算归档内路径（工作目录和相对路径由 aResolveArchivePath 计算）
    std::string workDir, basePath;
    aResolveArchivePath(source, curdir, workDir, basePath);
    if (basePath == ".") basePath = "";

    FILE* dst = posix::fopen(target.data(), "wb");
    if (!dst)
    {
        aError("CompressorImplTar: cannot create target file: %s", target.data());
        return eErrorInvalidFile;
    }

    errc_t ret = eNoError;

    if (fs::is_directory(srcPath))
    {
        ret = archiveDirectory(dst, srcPath, basePath);
    }
    else
    {
        // basePath 已经是 source 相对于 curdir 的完整相对路径（含文件名）
        std::string filename = basePath.empty() ? srcPath.filename().string() : basePath;
        ret = writeFileEntry(dst, filename, std::string(source.data(), source.size()));
    }

    if (ret == eNoError)
    {
        // 写入归档结束标记：两个全零的 512 字节块
        char zeroBlock[512] = {};
        if (fwrite(zeroBlock, 1, 512, dst) != 512 ||
            fwrite(zeroBlock, 1, 512, dst) != 512)
        {
            aError("CompressorImplTar: write error at end markers");
            ret = eError;
        }
    }

    fclose(dst);

    if (ret != eNoError)
    {
        // 删除失败时产生的不完整文件
        fs::remove(fs::path(std::string(target.data(), target.size())));
    }

    return ret;
}

AST_NAMESPACE_END
