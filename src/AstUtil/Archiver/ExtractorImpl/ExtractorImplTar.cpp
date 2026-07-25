#include "ExtractorImplTar.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/LocalBuffer.hpp"
#include "AstUtil/StringView.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>

AST_NAMESPACE_BEGIN

// ============================================================
// 辅助函数
// ============================================================

errc_t ExtractorImplTar::createDirectory(FILE* /*src*/, const TarHeader& header, const std::string& targetDir)
{
    // 构建完整路径：targetDir / name
    char nameBuf[101] = {};
    std::memcpy(nameBuf, header.name, 100);
    nameBuf[100] = '\0';

    // 去掉末尾的 '/'
    size_t nameLen = std::strlen(nameBuf);
    while (nameLen > 0 && nameBuf[nameLen - 1] == '/')
        nameBuf[--nameLen] = '\0';

    fs::path dirPath = fs::path(targetDir) / fs::path(nameBuf);
    if (!fs::exists(dirPath))
    {
        if (!fs::create_directories(dirPath))
        {
            aError("ExtractorImplTar: cannot create directory: %s", dirPath.string().c_str());
            return eError;
        }
    }
    return eNoError;
}

errc_t ExtractorImplTar::createFile(FILE* src, const TarHeader& header, const std::string& targetDir)
{
    char nameBuf[101] = {};
    std::memcpy(nameBuf, header.name, 100);
    nameBuf[100] = '\0';

    size_t fileSize = aTarParseOctal(header.size, sizeof(header.size));
    fs::path filePath = fs::path(targetDir) / fs::path(nameBuf);

    // 确保父目录存在
    fs::path parentPath = filePath.parent_path();
    if (!parentPath.empty() && !fs::exists(parentPath))
    {
        if (!fs::create_directories(parentPath))
        {
            aError("ExtractorImplTar: cannot create parent directory: %s", parentPath.string().c_str());
            return eError;
        }
    }

    FILE* dst = posix::fopen(filePath.string().c_str(), "wb");
    if (!dst)
    {
        aError("ExtractorImplTar: cannot create file: %s", filePath.string().c_str());
        // 跳过数据
        if (fileSize > 0)
        {
            // 使用 fseek 跳过数据（如果可能）
            long pos = ftell(src);
            if (pos >= 0)
            {
                fseek(src, static_cast<long>(fileSize), SEEK_CUR);
            }
        }
        return eErrorInvalidFile;
    }

    errc_t ret = eNoError;
    constexpr size_t kBufSize = 65536; // 64KB
    A_LOCAL_BUFFER(char, buf, kBufSize);

    size_t remaining = fileSize;
    while (remaining > 0)
    {
        size_t toRead = remaining < kBufSize ? remaining : kBufSize;
        size_t nread = fread(buf, 1, toRead, src);
        if (nread == 0)
        {
            if (ferror(src))
            {
                aError("ExtractorImplTar: read error for file: %s", nameBuf);
                ret = eError;
            }
            break;
        }
        size_t nwritten = fwrite(buf, 1, nread, dst);
        if (nwritten != nread)
        {
            aError("ExtractorImplTar: write error for file: %s", nameBuf);
            ret = eError;
            break;
        }
        remaining -= nread;
    }

    fclose(dst);

    if (ret != eNoError)
    {
        fs::remove(filePath);
    }
    return ret;
}

// ============================================================
// 接口实现
// ============================================================

ExtractorImplTar& ExtractorImplTar::Instance()
{
    static ExtractorImplTar instance;
    return instance;
}

bool ExtractorImplTar::canExtract(StringView source) const
{
    if (source.empty()) return false;

    // 检查 .tar 扩展名
    size_t len = source.size();
    if (len >= 4)
    {
        // 大小写不敏感的 .tar
        StringView ext(source.data() + len - 4, 4);
        if ((ext[0] == '.')
            && (ext[1] == 't' || ext[1] == 'T')
            && (ext[2] == 'a' || ext[2] == 'A')
            && (ext[3] == 'r' || ext[3] == 'R'))
        {
            return true;
        }
    }

    // 读取完整头部并通过魔数 + 校验和双重验证
    FILE* fp = posix::fopen(source.data(), "rb");
    if (!fp) return false;

    TarHeader header;
    bool isTar = false;
    size_t nr = fread(&header, 1, sizeof(TarHeader), fp);
    if (nr == sizeof(TarHeader))
    {
        // 0. 排除已知的其他归档格式：ZIP 文件以 "PK\x03\x04" 开头
        const unsigned char* raw = reinterpret_cast<const unsigned char*>(&header);
        if (raw[0] == 0x50 && raw[1] == 0x4B) // 'P', 'K'
        {
            fclose(fp);
            return false;
        }
        // 排除 7z 文件以 "7z\xBC\xAF\x27\x1C" 开头
        if (raw[0] == 0x37 && raw[1] == 0x7A && raw[2] == 0xBC && raw[3] == 0xAF)
        {
            fclose(fp);
            return false;
        }

        // 1. USTAR 魔数在偏移 257 处: "ustar\0" 或 "ustar "
        if (std::memcmp(header.magic, "ustar", 5) == 0
            && (header.version[0] == '0' && header.version[1] == '0'))
        {
            // 2. 校验和验证：将原始校验和字段保存后替换为空格，重新计算并比对
            char savedChksum[8];
            std::memcpy(savedChksum, header.chksum, 8);

            std::memset(header.chksum, ' ', 8);

            unsigned int sum = 0;
            const unsigned char* p = reinterpret_cast<const unsigned char*>(&header);
            for (size_t i = 0; i < sizeof(TarHeader); ++i)
                sum += p[i];

            // 非零和：全零块是归档结束标记，不是有效文件头
            if (sum == 0) { fclose(fp); return false; }

            // 解析原始校验和（八进制）
            size_t expected = aTarParseOctal(savedChksum, sizeof(savedChksum));

            if (sum == expected)
                isTar = true;
        }
    }
    fclose(fp);
    return isTar;
}

errc_t ExtractorImplTar::extract(StringView source, StringView target) const
{
    if (source.empty() || target.empty())
    {
        aError("ExtractorImplTar: source or target is empty");
        return eErrorInvalidParam;
    }

    FILE* fp = posix::fopen(source.data(), "rb");
    if (!fp)
    {
        aError("ExtractorImplTar: cannot open source file: %s", source.data());
        return eErrorInvalidFile;
    }

    // 确保目标目录存在
    fs::path targetDir(target.data());
    if (!fs::exists(targetDir))
    {
        if (!fs::create_directories(targetDir))
        {
            aError("ExtractorImplTar: cannot create target directory: %s", target.data());
            fclose(fp);
            return eErrorInvalidFile;
        }
    }

    errc_t ret = eNoError;
    TarHeader header;
    std::string longName;  // GNU 长文件名缓冲区
    bool prevWasZero = false;

    while (true)
    {
        size_t nread = fread(&header, 1, sizeof(TarHeader), fp);
        if (nread < sizeof(TarHeader))
        {
            if (ferror(fp))
            {
                aError("ExtractorImplTar: read error at header");
                ret = eError;
            }
            break;
        }

        // 检查归档结束标记
        if (aTarIsZeroBlock(header))
        {
            if (prevWasZero) break; // 连续两个零块 = 结束
            prevWasZero = true;

            // 跳过后面的填充
            long pos = ftell(fp);
            if (pos >= 0)
            {
                // 跳到下一个 512 字节边界
                long next = ((pos + 511) / 512) * 512;
                if (next > pos) fseek(fp, next, SEEK_SET);
            }
            continue;
        }
        prevWasZero = false;

        // 读取实体大小
        size_t entrySize = aTarParseOctal(header.size, sizeof(header.size));
        long dataStart = ftell(fp);
        if (dataStart < 0) { ret = eError; break; }

        // 处理条目类型
        char typeflag = header.typeflag;

        // 构建文件名（考虑 prefix 字段）
        char nameBuf[256] = {};
        if (header.prefix[0] != '\0')
        {
            std::memcpy(nameBuf, header.prefix, 155);
            size_t prefixLen = std::strlen(nameBuf);
            if (prefixLen > 0 && nameBuf[prefixLen - 1] != '/')
                nameBuf[prefixLen++] = '/';
            std::memcpy(nameBuf + prefixLen, header.name, 100);
        }
        else
        {
            std::memcpy(nameBuf, header.name, 100);
        }

        std::string entryName(nameBuf);

        // 去除末尾斜杠
        while (!entryName.empty() && entryName.back() == '/')
            entryName.pop_back();

        // 处理 GNU 长文件名（typeflag == 'L'）
        if (typeflag == 'L' || typeflag == 'K')
        {
            longName.resize(entrySize);
            if (entrySize > 0)
            {
                size_t nr = fread(&longName[0], 1, entrySize, fp);
                if (nr < entrySize && ferror(fp))
                {
                    aError("ExtractorImplTar: read error at long name");
                    ret = eError;
                    break;
                }
                // 去掉尾部空字符
                while (!longName.empty() && longName.back() == '\0')
                    longName.pop_back();
            }
            // 跳到下一个 512 字节边界
            long pos = ftell(fp);
            if (pos >= 0 && pos > dataStart)
            {
                long next = ((static_cast<long>(entrySize) + 511) / 512) * 512;
                long targetPos = dataStart + next;
                if (targetPos > pos) fseek(fp, targetPos, SEEK_SET);
            }
            continue;
        }

        // 如果之前读取了 GNU 长文件名，使用它
        if (!longName.empty())
        {
            entryName = longName;
            longName.clear();
        }

        // 跳过空文件名和 '.' 和 '..'
        if (entryName.empty() || entryName == "." || entryName == "..")
        {
            // 跳过数据
            if (entrySize > 0)
            {
                long next = ((static_cast<long>(entrySize) + 511) / 512) * 512;
                fseek(fp, dataStart + next, SEEK_SET);
            }
            continue;
        }

        switch (typeflag)
        {
        case '\0':
        case '0':  // 普通文件
        case '7':  // 连续文件（GNU 扩展）
        {
            // 将文件名临时写回头部，供 createFile 使用
            TarHeader tmpHeader = header;
            std::memset(tmpHeader.name, 0, 100);
            std::memcpy(tmpHeader.name, entryName.c_str(),
                        entryName.size() < 100 ? entryName.size() : 99);
            ret = createFile(fp, tmpHeader, targetDir);
            break;
        }
        case '5':  // 目录
        {
            TarHeader tmpHeader = header;
            std::memset(tmpHeader.name, 0, 100);
            std::memcpy(tmpHeader.name, entryName.c_str(),
                        entryName.size() < 100 ? entryName.size() : 99);
            ret = createDirectory(fp, tmpHeader, targetDir);
            // 目录没有数据体，直接跳到下一个
            break;
        }
        case '1':  // 硬链接
        case '2':  // 符号链接
        case '3':  // 字符设备
        case '4':  // 块设备
        case '6':  // FIFO
        default:
            // 不支持的类型，跳过
            break;
        }

        if (ret != eNoError) break;

        // 跳到下一个 512 字节边界
        if (entrySize > 0)
        {
            long next = ((static_cast<long>(entrySize) + 511) / 512) * 512;
            fseek(fp, dataStart + next, SEEK_SET);
        }
    }

    fclose(fp);
    return ret;
}

AST_NAMESPACE_END
