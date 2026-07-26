///
/// @file      VDFLoader.cpp
/// @brief     VDF 文件加载器实现
/// @details   解压 gzip → 解析 Header 获取文件列表 → 按元数据切分提取 → 加载场景
/// @author    axel
/// @date      2026-07-26
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "VDFLoader.hpp"

#include "AstUtil/Archiver.hpp"
#include "AstUtil/ArchiverUtils.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/BKVItemView.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/ScopedPtr.hpp"

#include "AstSim/Scenario.hpp"
#include "AstLoader/ScenarioLoader.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

namespace
{
    struct TempDirGuard
    {
        std::string path;

        TempDirGuard() : path() {}

        ~TempDirGuard()
        {
            if (!path.empty() && fs::exists(path))
                fs::remove_all(path);
        }
    };

    /// @brief 解析 VDF Header 的 VDFFiles 段，获取文件名列表（有序）
    bool parseVDFFiles(BKVParser& parser, std::vector<std::string>& outFileNames)
    {
        BKVItemView item;
        BKVParser::EToken token;
        bool inHeader = false;
        bool inVDFFiles = false;

        while (true)
        {
            token = parser.getNext(item);
            if (token == BKVParser::eEOF || token == BKVParser::eError)
                break;

            if (token == BKVParser::eBlockBegin)
            {
                if (aEqualsIgnoreCase(item.value(), "Header"))
                    inHeader = true;
                else if (inHeader && aEqualsIgnoreCase(item.value(), "VDFFiles"))
                    inVDFFiles = true;
                else if (inVDFFiles && aEqualsIgnoreCase(item.value(), "File"))
                {
                    // 提取 Name
                    std::string name;
                    while (true)
                    {
                        token = parser.getNext(item);
                        if (token == BKVParser::eKeyValue && aEqualsIgnoreCase(item.key(), "Name"))
                        {
                            name.assign(item.value().data(), item.value().size());
                        }
                        else if (token == BKVParser::eBlockEnd)
                            break;
                        else if (token == BKVParser::eEOF || token == BKVParser::eError)
                            break;
                    }
                    if (!name.empty())
                        outFileNames.push_back(name);
                }
            }
            else if (token == BKVParser::eBlockEnd)
            {
                if (inVDFFiles && aEqualsIgnoreCase(item.value(), "VDFFiles"))
                    inVDFFiles = false;
                else if (inHeader && aEqualsIgnoreCase(item.value(), "Header"))
                    break; // Header 解析完成
            }
        }

        return !outFileNames.empty();
    }


} // anonymous namespace

// ============================================================
// 公开 API
// ============================================================

errc_t aExtractVDF(StringView filepath, StringView outputDir)
{
    if (filepath.empty() || outputDir.empty())
    {
        aError("empty filepath or outputDir");
        return eErrorInvalidParam;
    }

    std::string srcPath(filepath.data(), filepath.size());
    std::string outDir(outputDir.data(), outputDir.size());

    // 1. 确保输出目录
    if (!fs::create_directories(outDir))
    {
        aError("cannot create output dir: %s", outDir.c_str());
        return eErrorInvalidFile;
    }

    // 2. 解压 gzip（.gz 解压到单个文件，不是目录）
    std::string workPath = srcPath;
    TempDirGuard tempGuard;

    if (aIsArchiveFile(srcPath))
    {
        // gzip 解压到单个文件
        fs::path srcFsPath(srcPath);
        tempGuard.path = outDir + "/" + srcFsPath.stem().string() + "_decompressed";
        workPath = tempGuard.path;

        errc_t rc = aExtract(srcPath, workPath);
        if (rc != eNoError)
        {
            aError("aExtract failed: %s", srcPath.c_str());
            return rc;
        }
    }

    // 3. 解析 Header 获取文件列表
    BKVParser headerParser(workPath);
    std::vector<std::string> fileNames;

    if (!parseVDFFiles(headerParser, fileNames))
    {
        aError("no files found in VDF header");
        return eErrorInvalidFile;
    }

    aInfo("found %zu files in VDF header", fileNames.size());

    // 4. 关闭 BKVParser，用二进制模式重新打开
    //    注意：不能依赖 BKVParser::tell() 获取 Body 偏移量，
    //    因为 BKVParser 内部有行缓冲区 (lineBuffer_) 且 fscanf 的 stdio 缓冲
    //    可能已提前读取了 Body 数据，导致 ftell 位置不准确。
    //    改用 fgets 逐行读取直到 "End Header" 来定位 Body 起始位置。
    headerParser.close();

    ScopedPtr<std::FILE> fp = ast_fopen(workPath.c_str(), "rb");
    if (!fp)
    {
        aError("cannot reopen file in binary mode: %s", workPath.c_str());
        return eErrorInvalidFile;
    }

    // 4a. 逐行读取直到 "End Header"，下一行即为 Body 起始
    {
        char lineBuf[4096];
        bool foundEndHeader = false;
        while (fgets(lineBuf, sizeof(lineBuf), fp))
        {
            StringView line(lineBuf);

            if (line.starts_with("End Header"))
            {
                foundEndHeader = true;
                break;
            }
        }

        if (!foundEndHeader)
        {
            aError("cannot find 'End Header' marker in VDF file");
            return eErrorInvalidFile;
        }
        // fgets 已将文件指针定位到 "End Header" 行之后，
        // 即 Body 的第一个字节处，无需额外 seek
    }

    // 5. 按 VDFFiles 顺序提取文件
    for (size_t i = 0; i < fileNames.size(); ++i)
    {
        const std::string& fileName = fileNames[i];

        // 5a. 读取 ASCII 十进制字节数（逐字节读取直到 \n）
        std::string sizeStr;
        int ch;
        while ((ch = fgetc(fp)) != EOF && ch != '\n' && ch != '\r')
        {
            if (ch >= '0' && ch <= '9')
                sizeStr += static_cast<char>(ch);
            else
            {
                aError("invalid size char '%c' (0x%02X) at file [%zu/%zu]: %s",
                       ch, static_cast<unsigned>(ch), i + 1, fileNames.size(), fileName.c_str());
                return eErrorInvalidFile;
            }
        }

        // 跳过可能剩余的 \r 或 \n（处理 Windows \r\n）
        if (ch == '\r')
        {
            ch = fgetc(fp);
            if (ch != EOF && ch != '\n')
                ungetc(ch, fp);
        }

        if (sizeStr.empty())
        {
            aError("empty size at file [%zu/%zu]: %s",
                   i + 1, fileNames.size(), fileName.c_str());
            return eErrorInvalidFile;
        }

        // 使用 strtoull 而非 aParseInt，因为 aParseInt 返回 int（32 位有符号），
        // 无法表示 >2.1GB 的文件大小（溢出导致 UB 或静默截断）
        size_t fileSize = static_cast<size_t>(strtoull(sizeStr.c_str(), nullptr, 10));

        // 5b. 构建输出路径，必要时创建父目录
        fs::path outPath(outDir);
        outPath /= fileName;

        fs::path parentPath = outPath.parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath))
        {
            if (!fs::create_directories(parentPath))
            {
                aError("cannot create parent directory: %s", parentPath.string().c_str());
                return eErrorInvalidFile;
            }
        }

        // 5c. 分块读取并写入文件内容
        ScopedPtr<std::FILE> outFp = ast_fopen(outPath.string().c_str(), "wb");
        if (!outFp)
        {
            aError("cannot create output file: %s", outPath.string().c_str());
            return eErrorInvalidFile;
        }

        errc_t rc = aCopyFileRange(fp, outFp, fileSize);
        if (rc != eNoError)
            return rc;
    }

    aInfo("extracted %zu files to %s", fileNames.size(), outDir.c_str());

    return eNoError;
}

errc_t aLoadVDF(StringView filepath, Scenario& scenario)
{
    if (filepath.empty())
    {
        aError("aLoadVDF: empty filepath");
        return eErrorInvalidParam;
    }

    // 创建临时输出目录
    std::string srcPath(filepath.data(), filepath.size());
    fs::path srcFsPath(srcPath);
    std::string tempDir = srcFsPath.parent_path().string() + "/_vdf_load_" + srcFsPath.stem().string();

    TempDirGuard tempGuard;
    tempGuard.path = tempDir;

    // 解包
    errc_t rc = aExtractVDF(filepath, tempDir);
    if (rc != eNoError)
    {
        aError("aLoadVDF: extraction failed");
        return rc;
    }

    // 找 .sc 文件并加载
    std::string scPath;
    for (auto& entry : fs::directory_iterator(tempDir))
    {
        if (!fs::is_regular_file(entry.status())) continue;
        std::string name = entry.path().filename().string();
        // 匹配 *.sc
        if (name.size() > 3)
        {
            StringView nameView = name;
            if (nameView.ends_with(".sc"))
            {
                scPath = entry.path().string();
                break;
            }
        }
    }

    if (scPath.empty())
    {
        aError("aLoadVDF: no .sc file found");
        return eErrorInvalidFile;
    }

    return aLoadScenario(scPath, scenario);
}

AST_NAMESPACE_END
