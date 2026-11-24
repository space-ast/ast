///
/// @file      UiToolPluginManager.hpp
/// @brief     界面工具插件管理器
/// @details   扫描插件元数据里声明的工具，建立菜单可用的工具清单；
///            只有首次激活某个工具时才加载其所属的动态库
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/PluginInterface.hpp"
#include "AstUtil/StringView.hpp"
#include <string>
#include <vector>

class QWidget;

AST_NAMESPACE_BEGIN

class PluginLoader;

/*!
    @addtogroup ToolPlugin
    @{
*/

/// @brief 工具的运行状态
enum class EUiToolState
{
    eNotLoaded,     ///< 已发现，尚未加载动态库
    eLoaded,        ///< 已加载并成功激活过
    eFailed,        ///< 加载或激活失败
    eIncompatible,  ///< 接口版本不匹配
    eDisabled,      ///< 被会话级禁用
};

/// @brief 工具插件声明的一个工具
class AST_UIUTIL_API UiToolPluginEntry
{
public:
    /// @brief 工具 id，即元数据 tools 对象的键
    const std::string& id() const noexcept { return id_; }

    /// @brief 按指定语言解析标题
    /// @param lang 主语言码，如 "zh"；为空时按"当前语言 → en → 第一个"回退
    std::string title(StringView lang) const;

    /// @brief 按当前界面语言解析标题
    std::string title() const;

    /// @brief 按指定语言解析提示文本
    std::string tooltip(StringView lang) const;

    /// @brief 按当前界面语言解析提示文本；没有时返回空串
    std::string tooltip() const;

    const std::string& iconPath() const noexcept { return iconPath_; }  ///< 图标绝对路径，为空表示没有图标
    int order() const noexcept { return order_; }                       ///< 菜单排序权重，小的在前
    int api() const noexcept { return api_; }                           ///< 插件声明的接口版本

    /// @brief 所属插件动态库的路径
    /// @return 路径；条目尚未关联插件库时返回空串
    std::string pluginPath() const;

    /// @name 运行期状态（只读）
    /// @{
    EUiToolState state() const noexcept { return state_; }        ///< 当前状态
    const std::string& error() const noexcept { return error_; }  ///< 最近一次失败原因
    bool enabled() const noexcept { return enabled_; }            ///< 会话级启用状态
    /// @}

private:
    A_DISABLE_COPY(UiToolPluginEntry);
    friend class UiToolPluginManager;

    /// @brief 由解析器构造条目，外部不可见
    /// @param title 标题节点：形如 {"zh":"中文","en":"English"}，也接受纯字符串
    /// @param tooltip 提示节点，格式同 title，可为空
    UiToolPluginEntry(std::string id,
                      JsonValue title,
                      JsonValue tooltip,
                      std::string iconPath,
                      int order,
                      int api);

    std::string id_{};                              ///< 工具 id
    JsonValue title_{};                             ///< 标题节点（多语言，读取时解析）
    JsonValue tooltip_{};                           ///< 提示节点（多语言，读取时解析）
    std::string iconPath_{};                        ///< 图标路径
    int order_{100};                                ///< 排序权重
    int api_{1};                                    ///< 接口版本

    EUiToolState state_{EUiToolState::eNotLoaded};  ///< 当前状态
    std::string error_{};                           ///< 最近一次失败原因
    bool enabled_{true};                            ///< 会话级启用状态

    /// @internal 所属插件库，仅管理器使用
    PluginLoader* plugin_{nullptr};
};

/// @brief 界面工具插件管理器（进程级单例）
/// @details
/// 扫描阶段只读取插件元数据，不加载动态库；invoke() 命中某个工具时才加载其所属动态库。
/// 已加载的插件库常驻到进程退出，refresh() 只重建工具清单，不会卸载任何动态库。
/// 工具条目不保存解析后的文本：切换语言后宿主重建一次菜单即可拿到新语言的标题，
/// 不需要刷新插件清单。
class AST_UIUTIL_API UiToolPluginManager
{
public:
    /// @brief 获取全局单例
    static UiToolPluginManager& Instance();

    /// @brief 扫描插件并重建工具清单（不加载动态库）
    /// @details 必须在主线程调用；重复调用是幂等的，只重建清单
    /// @return 错误码
    errc_t refresh();

    /// @brief 解析单个插件声明的工具（不加载动态库）
    /// @param pluginPath 插件动态库路径（用于日志与图标相对路径解析）
    /// @param metadata 已解析的插件元数据
    /// @param[out] out 解析结果追加到该列表；坏条目跳过并记日志，不影响同插件其它工具
    /// @note 多语言文本按原始节点保存，不在这里解析，因此条目与语言无关。
    ///       独立成公开静态函数是为了让测试和诊断工具能直接喂元数据
    static void ParseTools(const std::string& pluginPath,
                           const PluginMetadata& metadata,
                           std::vector<UiToolPluginEntry>& out);

    /// @brief 已发现的工具，按 order → id 排序
    const std::vector<UiToolPluginEntry>& tools() const { return tools_; }

    /// @brief 按 id 查找工具条目
    /// @return 工具条目；未命中返回 nullptr
    const UiToolPluginEntry* tool(StringView id) const;

    /// @brief 工具当前状态
    EUiToolState state(StringView id) const;

    /// @brief 最近一次失败原因（UTF-8）；没有失败时返回空串
    std::string errorText(StringView id) const;

    /// @brief 调用工具：首次调用时加载所属动态库并转发给插件
    /// @param key 工具键，即元数据 tools 对象的键
    /// @param parent 主窗口，作为插件对话框的父对象，可为空
    /// @return 成功返回 true；失败时原因可通过 errorText() 取得
    /// @note 只能在主线程调用；失败结果会被记住，不会反复重试
    bool invoke(StringView key, QWidget* parent);

    /// @brief 会话级启用/禁用工具，只影响菜单是否可用
    void setEnabled(StringView id, bool enabled);

    /// @brief 工具是否处于启用状态
    bool isEnabled(StringView id) const;

private:
    UiToolPluginManager() = default;

    A_DISABLE_COPY(UiToolPluginManager)

    /// @brief 按 id 查找工具条目（工具数量很少，直接线性查找）
    UiToolPluginEntry* findTool(StringView id);
    const UiToolPluginEntry* findTool(StringView id) const;

    std::vector<UiToolPluginEntry> tools_{};  ///< 工具清单（已排序，含运行期状态）
    bool invoking_{false};                    ///< 调用重入保护
};

/*! @} */

AST_NAMESPACE_END
