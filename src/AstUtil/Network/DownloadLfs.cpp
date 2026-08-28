///
/// @file      DownloadLfs.cpp
/// @brief     Git LFS 对象下载实现
/// @details   ~
/// @author    axel
/// @date      2026-08-27
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "DownloadLfs.hpp"
#include "Network.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringUtil.hpp"
#include <fstream>
#include <sstream>
#include <iterator>
#include <cstdlib>
#include <string>
#include <system_error>

AST_NAMESPACE_BEGIN

namespace {

/// LFS 标准媒体类型，用于 batch 请求头与请求体序列化
constexpr const char* LfsMediaType = "application/vnd.git-lfs+json";

/// 从 batch 响应 JSON 中提取对象下载地址 href
/// @details 响应形如 {"objects":[{"actions":{"download":{"href":"...",...}}}]}，
///          逐层取用 isObject/isArray/isString 做防卫式判断，缺链时返回错误而非访问空值。
errc_t aLfsExtractHref(const NetworkResponse& response, std::string& href)
{
    JsonValue root = response.toJson();
    if (!root.isObject())
    {
        aError("LFS batch response is not a JSON object");
        return eErrorInvalidFile;
    }

    const JsonValue& objects = root["objects"];
    if (!objects.isArray() || objects.size() == 0)
    {
        aError("LFS batch response missing objects");
        return eErrorNotFound;
    }

    const JsonValue& object = objects[0];
    if (!object.isObject())
    {
        aError("LFS batch response object is not a JSON object");
        return eErrorInvalidFile;
    }

    const JsonValue& actions = object["actions"];
    if (!actions.isObject())
    {
        aError("LFS batch response missing actions");
        return eErrorNotFound;
    }

    const JsonValue& download = actions["download"];
    if (!download.isObject())
    {
        aError("LFS batch response missing download action");
        return eErrorNotFound;
    }

    const JsonValue& entry = download["href"];
    if (entry.isNull() || !entry.isString())
    {
        aError("LFS batch response missing download href");
        return eErrorNotFound;
    }
    href = entry.toString();
    if (href.empty())
    {
        aError("LFS batch response href is empty");
        return eErrorNotFound;
    }
    return eNoError;
}

} // namespace


errc_t aLfsParsePointer(StringView pointerText, LfsPointerInfo& info)
{
    info = LfsPointerInfo();
    if (pointerText.empty())
        return eErrorInvalidParam;

    std::string text(pointerText);
    std::istringstream stream(text);
    std::string line;
    bool hasOid = false;

    while (std::getline(stream, line))
    {
        line = std::string(aStripAsciiWhitespace(line));
        if (line.compare(0, 11, "oid sha256:") == 0)
        {
            info.oid = std::string(aStripAsciiWhitespace(line.substr(11)));
            hasOid = !info.oid.empty();
        }
        else if (line.compare(0, 5, "size ") == 0)
        {
            info.size = static_cast<std::size_t>(
                std::strtoull(line.c_str() + 5, nullptr, 10));
        }
    }

    if (!hasOid)
    {
        aError("invalid LFS pointer: missing oid");
        return eErrorNotFound;
    }
    return eNoError;
}


std::string aLfsEndpoint(StringView remoteUrl)
{
    std::string base(remoteUrl);
    while (!base.empty() && base.back() == '/')
        base.pop_back();
    return base + "/info/lfs";
}


std::string aLfsBatchRequestBody(StringView oid, std::size_t size)
{
    JsonValue root;
    root.setObject();
    root.insert("operation", "download");

    JsonValue transfers;
    transfers.setArray();
    transfers.append("basic");
    root.insert("transfers", transfers);

    JsonValue objects;
    objects.setArray();

    JsonValue item;
    item.setObject();
    item.insert("oid", std::string(oid));
    item.insert("size", static_cast<double>(size));
    objects.append(item);

    root.insert("objects", objects);
    return root.toJsonString();
}


errc_t aDownloadLfs(StringView pointerFile, StringView outputFile, StringView remoteUrl)
{
    std::string pFile(pointerFile);
    std::string oFile(outputFile);
    std::string rUrl(remoteUrl);

    // 读取指针文件文本
    std::ifstream in(pFile.c_str(), std::ios::binary);
    if (!in.is_open())
    {
        aError("cannot open LFS pointer file %s", pFile.c_str());
        return eErrorInvalidFile;
    }
    std::string pointerText((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());

    LfsPointerInfo info;
    errc_t err = aLfsParsePointer(pointerText, info);
    if (err)
        return err;

    std::string batchUrl = aLfsEndpoint(rUrl) + "/objects/batch";

    NetworkRequest request;
    request.setMethod(ENetworkRequestMethod::ePost);
    request.setUrl(batchUrl);
    request.addHeader("Content-Type", LfsMediaType);
    request.addHeader("Accept", LfsMediaType);
    request.setBody(aLfsBatchRequestBody(StringView(info.oid), info.size));

    NetworkResponse response;
    err = aNetworkRequest(request, response);
    if (err)
    {
        aError("LFS batch request failed for %s", batchUrl.c_str());
        return err;
    }
    if (response.statusCode() != 200)
    {
        aError("LFS batch HTTP %d for %s", response.statusCode(), batchUrl.c_str());
        return eErrorInvalidFile;
    }

    std::string href;
    err = aLfsExtractHref(response, href);
    if (err)
        return err;

    // 确保目标目录存在（使用不抛出异常的 std::error_code 版本）
    std::error_code ec;
    fs::path outPath(oFile);
    fs::path dirPath = outPath.parent_path();
    if (!dirPath.empty())
    {
        fs::create_directories(dirPath, ec);
    }

    // 先下载到临时文件，成功后再重命名，失败则清理
    std::string tempFile = oFile + ".part";
    aInfo("downloading lfs file to '%s'", tempFile.c_str());
    err = aDownloadFile(href, tempFile);
    if (err)
    {
        if (fs::exists(tempFile, ec))
            fs::remove(tempFile, ec);
        return err;
    }

    if (!fs::rename(tempFile, oFile))
    {
        aError("rename %s -> %s failed", tempFile.c_str(), oFile.c_str());
        if (fs::exists(tempFile, ec))
            fs::remove(tempFile, ec);
        return eErrorInvalidFile;
    }

    return eNoError;
}

AST_NAMESPACE_END
