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
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/IO.hpp"

#include <cstddef>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

AST_NAMESPACE_BEGIN


namespace{

constexpr size_t kScanChunkSize = 64 * 1024;

/// @brief 校验一条 .astplug 段内元数据记录的定长头
/// @details 数据区的格式由 recordToMetadata() 进一步校验
/// @return 记录头合法返回 true
bool validateMetadataRecord(const PluginMetadataRecord& record)
{
    if (record.version_ != AST_PLUGIN_METADATA_VERSION)
        return false;
    if (record.dataType_ != AST_PLUGIN_DATA_TEXT && record.dataType_ != AST_PLUGIN_DATA_JSON)
        return false;
    if (record.size_ > AST_PLUGIN_DATA_MAX_LEN)
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
    return true;
}

/// @brief 把一条记录字节解析为 PluginMetadata
/// @param record 整条记录的字节，定长头在前、数据区紧随其后
/// @return 记录格式合法返回 true
bool recordToMetadata(const std::vector<char>& record, PluginMetadata& metadata)
{
    if (record.size() < sizeof(PluginMetadataRecord))
        return false;

    // 记录字节的前 sizeof(PluginMetadataRecord) 字节就是定长头
    PluginMetadataRecord header;
    std::memcpy(&header, record.data(), sizeof(PluginMetadataRecord));
    if (header.size_ > record.size() - sizeof(PluginMetadataRecord))
        return false;
    const StringView data(record.data() + sizeof(PluginMetadataRecord), header.size_);

    const char* iidEnd = static_cast<const char*>(
        std::memchr(header.iid_, '\0', sizeof(header.iid_)));
    if (!iidEnd) // 调用方应已完成校验，这里只做防御
        return false;

    PluginMetadata parsed;
    parsed.iid_.assign(header.iid_, static_cast<size_t>(iidEnd - header.iid_));

    if (header.dataType_ == AST_PLUGIN_DATA_JSON)
    {
        if (parsed.json_.parseFromString(data) != eNoError)
            return false;

        // @TODO 解析 JSON 元数据中的索引键
    }
    else
    {
        // 纯文本：逗号分隔的索引键，拒绝空段（首尾逗号、连续逗号）；空字符串表示没有键
        const char* begin = data.data();
        const char* end = begin + data.size();
        if (begin != end)
        {
            const char* keyBegin = begin;
            for (const char* p = begin; p < end; ++p)
            {
                if (*p != ',')
                    continue;
                if (p == keyBegin) // 首字符即逗号，或连续逗号
                    return false;
                parsed.keys_.emplace_back(keyBegin, static_cast<size_t>(p - keyBegin));
                keyBegin = p + 1; // p < end，p + 1 最多到 end，不会越过 one-past-end
            }
            // 循环结束时 keyBegin == end 说明尾随逗号，拒绝
            if (keyBegin == end)
                return false;
            parsed.keys_.emplace_back(keyBegin, static_cast<size_t>(end - keyBegin));
        }
    }

    metadata = std::move(parsed);
    return true;
}

const char* _find_sub(const char* hay, std::size_t n,
                      const char* pat, std::size_t m)
{
    if (m == 0) return hay;          // 空模式：返回起点
    if (m > n)  return nullptr;      // 模式比文本还长，必然找不到

    const char first = pat[0];
    const char* p   = hay;
    const char* end = hay + (n - m + 1);   // 最后一个可能的匹配起点 +1
    while (p < end) {
        const void* r = std::memchr(p, first, static_cast<std::size_t>(end - p));
        if (!r) 
            return nullptr;
        p = static_cast<const char*>(r);
        if (std::memcmp(p, pat, m) == 0) 
            return p;
        ++p;
    }
    return nullptr;
}

/// @brief 在缓冲区中定位一条完整的元数据记录
/// @details 记录头或数据区没读全时视为本轮未找到，调用方保留足够的尾部字节后重试
errc_t _find_metadata(const char* buffer, size_t size, std::vector<char>& record)
{
    size_t searchFrom = 0;
    for(;;)
    {
        const char* hit = _find_sub(buffer + searchFrom,
                                    size - searchFrom,
                                    AST_PLUGIN_METADATA_MAGIC,
                                    AST_PLUGIN_METADATA_MAGIC_LEN);
        if(!hit)
            break;
        const size_t pos = static_cast<size_t>(hit - buffer);
        const size_t remaining = size - pos;
        if (remaining < sizeof(PluginMetadataRecord))
        {
            // 头都没读全，剩余字节也容纳不下另一条记录
            return eErrorNotFound;
        }
        PluginMetadataRecord candidate;
        std::memcpy(&candidate, hit, sizeof(PluginMetadataRecord));
        if (!validateMetadataRecord(candidate))
        {
            // 魔数无自重叠（首字符只出现一次），未通过校验时整段跳过
            searchFrom = pos + AST_PLUGIN_METADATA_MAGIC_LEN;
            continue;
        }
        if (remaining - sizeof(PluginMetadataRecord) < candidate.size_)
        {
            // 数据区没读全，本轮跳过，等读到更多字节后再解析
            searchFrom = pos + AST_PLUGIN_METADATA_MAGIC_LEN;
            continue;
        }
        // 定长头与数据区在文件里连续，一次拷完整条记录
        record.assign(hit, hit + sizeof(PluginMetadataRecord) + candidate.size_);
        return eNoError;
    }
    return eErrorNotFound;
}

// 分块扫描时保留的尾部长度：记录头 + 最大数据区，足以覆盖任何跨界记录
constexpr size_t kCarrySize = sizeof(PluginMetadataRecord) + AST_PLUGIN_DATA_MAX_LEN;

} // namespace

errc_t aReadPluginMetadataRecord(StringView filepath, std::vector<char>& record)
{
    if (filepath.empty())
        return eErrorNullInput;

    const std::string path(filepath);
    ScopedPtr<std::FILE> file = ast_fopen(path.c_str(), "rb");
    if (file == nullptr)
        return eErrorInvalidFile;

#ifdef _MSC_VER
    /*
    MSVC 工具链的链接器一般把 .astplug 段放在文件末尾，优先只读末尾一块；
    未命中或文件不足一块时回退到从头完整扫描
    */
    if (fseek(file, -static_cast<long>(kScanChunkSize), SEEK_END) == 0)
    {
        std::vector<char> tail(kScanChunkSize);
        const size_t got = fread(tail.data(), 1, kScanChunkSize, file);
        tail.resize(got);
        if (_find_metadata(tail.data(), tail.size(), record) == eNoError)
            return eNoError;
    }
    fseek(file, 0, SEEK_SET);
#endif

    // 分块扫描文件字节。保留尾部 kCarrySize 字节，保证跨块的记录下一轮能完整解析
    std::vector<char> buffer;

    for (;;)
    {
        const size_t offset = buffer.size();
        buffer.resize(offset + kScanChunkSize);
        const size_t got = fread(buffer.data() + offset, 1, kScanChunkSize, file);
        buffer.resize(offset + got);

        if (_find_metadata(buffer.data(), buffer.size(), record) == eNoError)
            return eNoError;

        if (got < kScanChunkSize)
            break;

        const size_t keep = buffer.size() < kCarrySize ? buffer.size() : kCarrySize;
        buffer.erase(buffer.begin(), buffer.end() - static_cast<std::ptrdiff_t>(keep));
    }

    return eErrorInvalidFile;
}

errc_t aReadPluginMetadata(StringView filepath, PluginMetadata& metadata)
{
    std::vector<char> record;
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

    // 先加载到临时句柄：校验通过前不能写 handle_，否则失败后 load()/isLoaded() 会误报已加载
    void* handle = aLoadLibrary(filepath_.c_str());
    if (!handle)
    {
        loadError_ = eErrorInvalidFile;
        aWarning(_("插件动态库加载失败：'%s'：%s"), filepath_.c_str(), aGetLoadError());
        return loadError_;
    }

    // 校验运行期导出的 iid 与文件元数据一致，防止扫描后文件被替换
    const char* const* iidAddr = static_cast<const char* const*>(
        aGetProcAddress(handle, AST_PLUGIN_IID_NAMESTR));
    const char* runtimeIid = iidAddr ? *iidAddr : nullptr;
    if (!runtimeIid || metadata_.iid_ != runtimeIid)
    {
        aWarning(_("插件元数据与导出的 ast_plugin_iid 不一致：'%s'"), filepath_.c_str());
        loadError_ = eErrorParse;
        aFreeLibrary(handle);
        return loadError_;
    }

    // 登记为进程退出时卸载：插件库常驻，插件根对象及其创建的对象必须始终有效
    aFreeLibraryOnExit(handle);
    handle_ = handle;

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
