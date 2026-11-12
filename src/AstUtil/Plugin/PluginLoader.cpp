///
/// @file      PluginLoader.cpp
/// @brief     单个插件动态库的加载与元数据读取实现
/// @author    axel
/// @date      2026-10-14
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

#include "PluginLoader.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/FileStream.hpp"
#include "AstUtil/Logger.hpp"
#include <cstddef>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

AST_NAMESPACE_BEGIN

static_assert(sizeof(PluginMetadataRecord) == 664, "PluginMetadataRecord layout changed; bump AST_PLUGIN_METADATA_VERSION");

namespace{

constexpr size_t kScanChunkSize = 64 * 1024;

/// @brief 校验一条 .astplug 段内元数据记录的完整性
/// @return 记录合法返回 true
bool validateMetadataRecord(const PluginMetadataRecord& record)
{
    if (record.version_ != AST_PLUGIN_METADATA_VERSION)
        return false;
    if (record.size_ != sizeof(PluginMetadataRecord))
        return false;

    // iid：必须在界内 NUL 结尾，且终结符之后全为 0（聚合初始化会清零未用部分）
    const char* iidEnd = static_cast<const char*>(
        std::memchr(record.iid_, '\0', sizeof(record.iid_)));
    if (!iidEnd || iidEnd == record.iid_)
        return false;
    for (const char* p = iidEnd + 1; p != record.iid_ + sizeof(record.iid_); ++p)
    {
        if (*p != '\0')
            return false;
    }

    // keys：同样要求 NUL 结尾且其后全为 0
    const char* keysEnd = static_cast<const char*>(
        std::memchr(record.keys_, '\0', sizeof(record.keys_)));
    if (!keysEnd)
        return false;
    for (const char* p = keysEnd + 1; p != record.keys_ + sizeof(record.keys_); ++p)
    {
        if (*p != '\0')
            return false;
    }
    return true;
}

/// @brief 把已校验的元数据记录解析为 PluginMetadata
/// @return 索引键格式合法返回 true
bool recordToMetadata(const PluginMetadataRecord& record, PluginMetadata& metadata)
{
    const char* iidEnd = static_cast<const char*>(
        std::memchr(record.iid_, '\0', sizeof(record.iid_)));
    const char* keysEnd = static_cast<const char*>(
        std::memchr(record.keys_, '\0', sizeof(record.keys_)));
    if (!iidEnd || !keysEnd) // 调用方应已完成校验，这里只做防御
        return false;

    PluginMetadata parsed;
    parsed.iid.assign(record.iid_, static_cast<size_t>(iidEnd - record.iid_));

    // keys 是以逗号分隔的字符串，拒绝空段（首尾逗号、连续逗号）；空字符串表示没有键
    if (keysEnd != record.keys_)
    {
        const char* keyBegin = record.keys_;
        for (const char* p = record.keys_; p <= keysEnd; ++p)
        {
            if (p != keysEnd && *p != ',')
                continue;
            if (p == keyBegin)
                return false;
            parsed.keys.emplace_back(keyBegin, static_cast<size_t>(p - keyBegin));
            keyBegin = p + 1;
        }
    }

    metadata = std::move(parsed);
    return true;
}

} // namespace

errc_t aReadPluginMetadataRecord(StringView filepath, PluginMetadataRecord& record)
{
    if (filepath.empty())
        return eErrorNullInput;

    const std::string path(filepath.data(), filepath.size());
    cxx::ifstream in(path, std::ios::in | std::ios::binary);
    if (!in.is_open())
        return eErrorInvalidFile;

    // 分块扫描文件字节。保留尾部 sizeof(record)-1 字节，保证跨块的记录下一轮能完整解析
    const size_t carrySize = sizeof(PluginMetadataRecord) - 1;

    std::vector<char> buffer;
    bool sawMagic = false;

    for (;;)
    {
        const size_t offset = buffer.size();
        buffer.resize(offset + kScanChunkSize);
        in.read(buffer.data() + offset, static_cast<std::streamsize>(kScanChunkSize));
        const size_t got = static_cast<size_t>(in.gcount());
        buffer.resize(offset + got);

        if (buffer.empty())
            break;

        size_t searchFrom = 0;
        while (searchFrom + AST_PLUGIN_METADATA_MAGIC_LEN <= buffer.size())
        {
            const void* hit = std::memchr(buffer.data() + searchFrom,
                                          AST_PLUGIN_METADATA_MAGIC[0],
                                          buffer.size() - searchFrom
                                              - AST_PLUGIN_METADATA_MAGIC_LEN + 1);
            if (!hit)
                break;

            const size_t pos = static_cast<size_t>(
                static_cast<const char*>(hit) - buffer.data());
            if (std::memcmp(hit, AST_PLUGIN_METADATA_MAGIC,
                            AST_PLUGIN_METADATA_MAGIC_LEN) == 0)
            {
                sawMagic = true;
                if (pos + sizeof(PluginMetadataRecord) > buffer.size())
                {
                    // 记录跨块：本位置及之后都不会有完整记录，留到下一轮
                    break;
                }

                PluginMetadataRecord candidate;
                std::memcpy(&candidate, buffer.data() + pos, sizeof(candidate));
                if (validateMetadataRecord(candidate))
                {
                    record = candidate;
                    return eNoError;
                }
            }
            searchFrom = pos + 1;
        }

        if (in.eof() || got == 0)
            break;

        const size_t keep = buffer.size() < carrySize ? buffer.size() : carrySize;
        buffer.erase(buffer.begin(), buffer.end() - static_cast<std::ptrdiff_t>(keep));
    }

    // 见过 magic 但记录不完整/损坏 → 解析错误；完全没见过 → 不是插件
    return sawMagic ? eErrorParse : eErrorInvalidFile;
}

errc_t aReadPluginMetadata(StringView filepath, PluginMetadata& metadata)
{
    PluginMetadataRecord record;
    const errc_t rc = aReadPluginMetadataRecord(filepath, record);
    if (rc != eNoError)
        return rc;
    return recordToMetadata(record, metadata) ? eNoError : eErrorParse;
}

PluginLoader::PluginLoader(StringView filepath)
    : filepath_(filepath)
{
}

errc_t PluginLoader::readMetadata()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return readMetadataNoLock();
}

errc_t PluginLoader::readMetadataNoLock()
{
    if (filepath_.empty())
        return eErrorNullInput;

    PluginMetadata metadata;
    const errc_t rc = aReadPluginMetadata(filepath_, metadata);
    if (rc == eErrorParse)
        aWarning(_("插件 '%s' 的元数据格式非法，已忽略"), filepath_.c_str());
    if (rc != eNoError)
        return rc;

    metadata_ = std::move(metadata);
    return eNoError;
}

errc_t PluginLoader::load()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return loadNoLock();
}

errc_t PluginLoader::loadNoLock()
{
    if (handle_)
        return eNoError;
    if (loadAttempted_)
        return loadError_;
    loadAttempted_ = true;

    if (filepath_.empty())
    {
        loadError_ = eErrorNullInput;
        return loadError_;
    }

    // 尚未读取元数据：先读取，失败结果同样缓存
    if (!isPlugin())
    {
        loadError_ = readMetadataNoLock();
        if (loadError_ != eNoError)
            return loadError_;
    }

    handle_ = aLoadLibrary(filepath_.c_str());
    if (!handle_)
    {
        loadError_ = eErrorInvalidFile;
        aWarning(_("插件动态库加载失败：'%s'：%s"), filepath_.c_str(), aGetLoadError());
        return loadError_;
    }
    // 登记为进程退出时卸载：插件库常驻，插件根对象及其创建的对象必须始终有效
    aFreeLibraryOnExit(handle_);

    // 校验运行期导出的 iid 与文件元数据一致，防止扫描后文件被替换
    const char* const* iidAddr = static_cast<const char* const*>(
        aGetProcAddress(handle_, AST_PLUGIN_IID_NAMESTR));
    const char* runtimeIid = iidAddr ? *iidAddr : nullptr;
    if (!runtimeIid || metadata_.iid != runtimeIid)
    {
        aWarning(_("插件元数据与导出的 ast_plugin_iid 不一致：'%s'"), filepath_.c_str());
        loadError_ = eErrorParse;
        return loadError_;
    }

    return eNoError;
}

PluginInstance* PluginLoader::instance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_)
        return instance_;

    const errc_t rc = loadNoLock();
    if (rc != eNoError)
        return nullptr;

    auto fn = reinterpret_cast<FPluginInstance>(
        aGetProcAddress(handle_, AST_PLUGIN_INSTANCE_NAMESTR));
    if (!fn)
    {
        aWarning(_("插件缺少入口符号 ast_plugin_instance：'%s'"), filepath_.c_str());
        return nullptr;
    }

    instance_ = fn();
    return instance_;
}

AST_NAMESPACE_END
