///
/// @file      UiToolPluginManager.cpp
/// @brief     界面工具插件管理器实现
/// @author    axel
/// @date      2026-11-22
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

#include "UiToolPluginManager.hpp"
#include "UiLanguage.hpp"
#include "UiToolProvider.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/PluginLoader.hpp"
#include "AstUtil/PluginRegistry.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <utility>

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 工具 id 是否合法
/// @details 只允许字母、数字与 _ . -，避免出现不便查找或写进配置后难以转义的键
bool isValidToolId(const std::string& id)
{
    if (id.empty() || id.size() > 64)
        return false;

    for (char c : id)
    {
        const unsigned char u = static_cast<unsigned char>(c);
        if (!std::isalnum(u) && c != '_' && c != '.' && c != '-')
            return false;
    }
    return true;
}

/// @brief 从 JSON 对象中取字段，不存在返回 nullptr
const JsonValue* findMember(const std::map<std::string, JsonValue>& object, const char* key)
{
    auto it = object.find(key);
    return it == object.end() ? nullptr : &it->second;
}

/// @brief 把元数据里的图标字段解析成绝对路径
/// @details 相对路径按插件动态库所在目录解析；文件不存在时返回空串
std::string resolveIconPath(const std::string& pluginPath, const std::string& icon)
{
    if (icon.empty())
        return std::string();

    const fs::path iconPath(icon);
    if (iconPath.is_absolute())
        return fs::exists(iconPath) ? iconPath.string() : std::string();

    const fs::path full = fs::path(pluginPath).parent_path() / iconPath;
    return fs::exists(full) ? full.string() : std::string();
}

} // namespace


void UiToolPluginManager::ParseTools(const std::string& pluginPath,
                                     const PluginMetadata& metadata,
                                     std::vector<UiToolPluginEntry>& out)
{
    if (!metadata.json_.isObject())
    {
        aWarning(_("插件 '%s' 的元数据不是 JSON 对象，未声明任何界面工具"), pluginPath.c_str());
        return;
    }

    const std::map<std::string, JsonValue>& root = metadata.json_.getObject();

    const JsonValue* toolsNode = findMember(root, "tools");
    if (!toolsNode || !toolsNode->isObject())
    {
        aWarning(_("插件 '%s' 的元数据缺少 tools 对象，未声明任何界面工具"), pluginPath.c_str());
        return;
    }

    int api = 1;
    if (const JsonValue* apiNode = findMember(root, "api"))
        api = apiNode->toInt(1);

    for (const auto& kv : toolsNode->getObject())
    {
        const std::string& id = kv.first;

        if (!isValidToolId(id))
        {
            aWarning(_("插件 '%s' 的工具 id '%s' 非法（只允许字母、数字与 _ . -，长度不超过 64）"),
                     pluginPath.c_str(), id.c_str());
            continue;
        }

        if (!kv.second.isObject())
        {
            aWarning(_("插件 '%s' 的工具 '%s' 描述不是对象，已跳过"),
                     pluginPath.c_str(), id.c_str());
            continue;
        }

        const std::map<std::string, JsonValue>& object = kv.second.getObject();

        // title 保持原始节点，只检查"至少有一种语言能解析出文本"
        const JsonValue* titleNode = findMember(object, "title");
        if (!titleNode || aUiResolveText(*titleNode, StringView()).empty())
        {
            aWarning(_("插件 '%s' 的工具 '%s' 缺少可用的 title，已跳过"),
                     pluginPath.c_str(), id.c_str());
            continue;
        }

        JsonValue tooltip;
        if (const JsonValue* node = findMember(object, "tooltip"))
            tooltip = *node;

        int order = 100;
        if (const JsonValue* node = findMember(object, "order"))
            order = node->toInt(100);

        std::string iconPath;
        if (const JsonValue* node = findMember(object, "icon"))
            iconPath = resolveIconPath(pluginPath, node->toString());

        // 构造函数是私有的，只能在这里构造后移动进容器
        // （emplace_back 由 allocator 直接构造，访问不到私有构造）
        out.push_back(UiToolPluginEntry(id, *titleNode, std::move(tooltip),
                                        std::move(iconPath), order, api));
    }
}


UiToolPluginEntry::UiToolPluginEntry(std::string id,
                                     JsonValue title,
                                     JsonValue tooltip,
                                     std::string iconPath,
                                     int order,
                                     int api)
    : id_(std::move(id))
    , title_(std::move(title))
    , tooltip_(std::move(tooltip))
    , iconPath_(std::move(iconPath))
    , order_(order)
    , api_(api)
{
}


std::string UiToolPluginEntry::title(StringView lang) const
{
    return aUiResolveText(title_, lang);
}


std::string UiToolPluginEntry::title() const
{
    return title(aUiCurrentLanguage());
}


std::string UiToolPluginEntry::tooltip(StringView lang) const
{
    return aUiResolveText(tooltip_, lang);
}


std::string UiToolPluginEntry::tooltip() const
{
    return tooltip(aUiCurrentLanguage());
}


std::string UiToolPluginEntry::pluginPath() const
{
    return plugin_ ? plugin_->filepath() : std::string();
}


UiToolPluginManager& UiToolPluginManager::Instance()
{
    static UiToolPluginManager instance;
    return instance;
}


errc_t UiToolPluginManager::refresh()
{
    aEnsurePluginsScanned();

    // 只重建清单：已加载的库不会卸载，刷新前仍存在的工具保留其运行期状态
    std::vector<UiToolPluginEntry> previous = std::move(tools_);
    tools_.clear();

    std::vector<PluginLoader*> plugins = aPluginRegistry().plugins<IUiToolProvider>();
    for (PluginLoader* loader : plugins)
    {
        if (!loader)
            continue;

        std::vector<UiToolPluginEntry> parsed;
        ParseTools(loader->filepath(), loader->metadata(), parsed);

        for (UiToolPluginEntry& info : parsed)
        {
            if (const UiToolPluginEntry* duplicate = findTool(info.id()))
            {
                aWarning(_("界面工具 id 重复：'%s'（'%s' 与 '%s'），后注册者被忽略"),
                         info.id().c_str(),
                         duplicate->pluginPath().c_str(),
                         loader->filepath().c_str());
                continue;
            }

            info.plugin_ = loader;

            for (const UiToolPluginEntry& old : previous)
            {
                if (old.id() == info.id())
                {
                    info.state_ = old.state_;
                    info.error_ = old.error_;
                    info.enabled_ = old.enabled_;
                    break;
                }
            }

            tools_.push_back(std::move(info));
        }
    }

    std::stable_sort(tools_.begin(), tools_.end(),
                     [](const UiToolPluginEntry& lhs, const UiToolPluginEntry& rhs) {
                         if (lhs.order() != rhs.order())
                             return lhs.order() < rhs.order();
                         return lhs.id() < rhs.id();
                     });

    return eNoError;
}


UiToolPluginEntry* UiToolPluginManager::findTool(StringView id)
{
    const std::string key(id);
    for (UiToolPluginEntry& info : tools_)
    {
        if (info.id() == key)
            return &info;
    }
    return nullptr;
}


const UiToolPluginEntry* UiToolPluginManager::findTool(StringView id) const
{
    const std::string key(id);
    for (const UiToolPluginEntry& info : tools_)
    {
        if (info.id() == key)
            return &info;
    }
    return nullptr;
}


const UiToolPluginEntry* UiToolPluginManager::tool(StringView id) const
{
    return findTool(id);
}


EUiToolState UiToolPluginManager::state(StringView id) const
{
    const UiToolPluginEntry* info = findTool(id);
    if (!info)
        return EUiToolState::eNotLoaded;

    if (!info->enabled())
        return EUiToolState::eDisabled;

    // 插件可能被别的调用方提前加载过，这里按实际句柄补一个判断
    if (info->state() == EUiToolState::eNotLoaded && info->plugin_ && info->plugin_->isLoaded())
        return EUiToolState::eLoaded;

    return info->state();
}


std::string UiToolPluginManager::errorText(StringView id) const
{
    const UiToolPluginEntry* info = findTool(id);
    return info ? info->error() : std::string();
}


void UiToolPluginManager::setEnabled(StringView id, bool enabled)
{
    if (UiToolPluginEntry* info = findTool(id))
        info->enabled_ = enabled;
}


bool UiToolPluginManager::isEnabled(StringView id) const
{
    const UiToolPluginEntry* info = findTool(id);
    return info && info->enabled();
}


bool UiToolPluginManager::invoke(StringView key, QWidget* parent)
{
    // 调用期间插件可能弹出模态对话框，Qt 会继续分发事件，
    // 没有这个标志的话用户可以在加载过程中再点一次同一个菜单项
    if (invoking_)
        return false;

    UiToolPluginEntry* info = findTool(key);
    if (!info)
        return false;

    if (!info->enabled())
    {
        info->error_ = "该工具已被禁用";
        return false;
    }

    PluginLoader* loader = info->plugin_;
    if (!loader)
    {
        info->state_ = EUiToolState::eFailed;
        info->error_ = "插件加载器为空";
        return false;
    }

    invoking_ = true;
    struct ReentryGuard
    {
        bool& flag;
        ~ReentryGuard() { flag = false; }
    } guard{invoking_};

    // 插件代码可能间接调用 refresh()，使 info 失效，之后一律按 key 重新查找
    auto setFailure = [this, key](const std::string& reason) {
        if (UiToolPluginEntry* entry = findTool(key))
        {
            entry->state_ = EUiToolState::eFailed;
            entry->error_ = reason;
        }
    };

    try
    {
        const errc_t rc = loader->load();
        if (rc != eNoError)
        {
            const char* reason = aGetLoadError();
            setFailure(std::string("动态库加载失败：") + loader->filepath() +
                       (reason ? std::string("：") + reason : std::string()));
            aWarning(_("界面工具插件加载失败：'%s'"), loader->filepath().c_str());
            return false;
        }

        PluginInstance* instance = loader->instance();
        if (!instance)
        {
            setFailure(std::string("插件入口缺失：") + loader->filepath());
            return false;
        }

        IUiToolProvider* provider = aPluginCast<IUiToolProvider>(instance);
        if (!provider)
        {
            setFailure(std::string("插件未实现 IUiToolProvider：") + loader->filepath());
            return false;
        }

        provider->invoke(key, parent);
    }
    catch (...)
    {
        // 插件是任意代码，异常不能穿过 DLL 边界传播
        setFailure(std::string("插件执行时抛出异常：") + loader->filepath());
        aWarning(_("界面工具插件执行异常：'%s'"), loader->filepath().c_str());
        return false;
    }

    if (UiToolPluginEntry* entry = findTool(key))
    {
        entry->state_ = EUiToolState::eLoaded;
        entry->error_.clear();
    }
    return true;
}

AST_NAMESPACE_END
