///
/// @file      PluginRegistry.cpp
/// @brief     插件注册表实现
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

#include "PluginRegistry.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/Logger.hpp"
#include <mutex>
#include <utility>

// 打开后可输出插件扫描耗时（开发调试用）
// #define _AST_DEBUG_PLUGIN_SCAN
#ifdef _AST_DEBUG_PLUGIN_SCAN
#include <chrono>
#endif

AST_NAMESPACE_BEGIN

PluginRegistry* PluginRegistry::Instance()
{
    static PluginRegistry instance;
    return &instance;
}

errc_t PluginRegistry::scanDirectory(StringView dir, StringView namePrefix)
{
    if (namePrefix.empty())
        return scanDirectory(dir, Span<const StringView>());

    return scanDirectory(dir, Span<const StringView>({namePrefix}));
}

errc_t PluginRegistry::scanDirectory(StringView dir, Span<const StringView> namePrefixes)
{
    if (dir.empty())
        return eErrorNullInput;

    const fs::path dirPath(dir);
    if (!fs::is_directory(dirPath))
        return eErrorInvalidFile;

    // 同一个目录只扫描一次
    if (!scannedDirs_.insert(dirPath).second)
        return eNoError;

    for (const fs::directory_entry& entry : fs::directory_iterator(dirPath))
    {
        if (!fs::is_regular_file(entry.status()))
            continue;
        fs::path filename = entry.path().filename();
        if (!aIsLibrary(filename.string()))
            continue;

        #ifdef _WIN32
        {
            // windows 下，release 和 debug 版本不能混用
            // 这里通过文件名后缀进行筛选
            // @todo 元数据暂不记录构建模式，仍以文件名后缀区分 debug/release

            fs::path stem = entry.path().stem();
            #if defined(_DEBUG)
            const bool isDebug = true;
            #else
            const bool isDebug = false;
            #endif

            if(StringView(stem.string()).ends_with(_AST_DEBUG_SUFFIX) != isDebug)
                continue;
        }
        #endif

        if (!namePrefixes.empty())
        {
            const std::string& filenameString = filename.string();
            bool prefixMatched = false;
            for (const StringView& prefix : namePrefixes)
            {
                if (StringView(filenameString).starts_with(prefix))
                {
                    prefixMatched = true;
                    break;
                }
            }
            if (!prefixMatched)
                continue;
        }

        loadPluginMetadata(entry.path().string());
    }

    return eNoError;
}

PluginInstance* PluginRegistry::find(StringView iid, StringView key)
{
    PluginLoader* loader = nullptr;
    auto it = index_.find(std::make_pair(std::string(iid), std::string(key)));
    if (it == index_.end())
        return nullptr;
    loader = it->second;
    return loader->instance();
}

void PluginRegistry::keys(StringView iid, std::vector<std::string>& keys) const
{
    keys.clear();

    for (const auto& kv : index_)
    {
        if (iid == kv.first.first)
            keys.push_back(kv.first.second);
    }
}

std::vector<std::string> PluginRegistry::keys(StringView iid) const
{
    std::vector<std::string> keys;
    this->keys(iid, keys);
    return keys;
}

size_t PluginRegistry::pluginCount() const
{
    return loaders_.size();
}

size_t PluginRegistry::entryCount() const
{
    return index_.size();
}

void PluginRegistry::clear()
{
    // 仅丢弃登记信息；动态库本身仍然驻留，直到进程退出时才卸载
    index_.clear();
    loaders_.clear();
    scannedDirs_.clear();
}

void PluginRegistry::loadPluginMetadata(const std::string& filepath)
{
    std::unique_ptr<PluginLoader> loader(new PluginLoader(filepath));
    if (loader->readMetadata() != eNoError)
        return; // 不是插件，或读取失败，或元数据非法

    size_t indexed = 0;
    const PluginMetadata& metadata = loader->metadata();
    for (const std::string& key : metadata.keys)
    {
        auto id = std::make_pair(metadata.iid, key);
        auto inserted = index_.insert(std::make_pair(id, loader.get()));
        if (inserted.second)
        {
            ++indexed;
        }
        else
        {
            aWarning(_("插件索引键重复：接口 '%s' 的键 '%s'，后注册者被忽略"), metadata.iid.c_str(), key.c_str());
        }
    }

    loaders_.push_back(std::move(loader));

    // aInfo(_("已发现插件 '%s'，新增 %d 条索引"), filepath.c_str(), static_cast<int>(indexed));
}

errc_t aScanPlugins()
{
    PluginRegistry& registry = aPluginRegistry();

#ifdef _AST_DEBUG_PLUGIN_SCAN
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

    bool foundPluginsDir = false;

    const std::string bases[] = { aExeDir(), aLibDir() };
    for (const std::string& base : bases)
    {
        if (base.empty())
            continue;

        const fs::path dir = fs::path(base) / "plugins";
        if (!fs::is_directory(dir))
            continue;

        registry.scanDirectory(dir.string());
        foundPluginsDir = true;
    }

    if (!foundPluginsDir)
    {
        // 回退到库目录，按产物的命名习惯搜索名称以 AstPlugin 开头的库：
        //   MSVC 产物没有 lib 前缀，形如 AstPluginXXX.dll
        //   GCC / Clang / MinGW 的共享库可能带 lib 前缀，
        //   同时接受 AstPluginXxx 与 libAstPluginXxx（.so / .dylib / .dll）
        const std::string libDir = aLibDir();
        if (!libDir.empty())
        {
            #if defined(_MSC_VER)
            const StringView prefixes[] = {"AstPlugin"};
            #else
            const StringView prefixes[] = {"AstPlugin", "libAstPlugin"};
            #endif
            registry.scanDirectory(libDir, prefixes);
        }
    }

#ifdef _AST_DEBUG_PLUGIN_SCAN
    const double elapsedMs = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - begin).count();
    aInfo(_("插件扫描完成：发现 %d 个插件，耗时 %.2f ms"),
          static_cast<int>(registry.pluginCount()), elapsedMs);
#endif

    return eNoError;
}



void aEnsurePluginsScanned()
{
    static std::once_flag flag;
    std::call_once(flag, []() { aScanPlugins(); });
}


AST_NAMESPACE_END
