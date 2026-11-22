///
/// @file      UiToolProvider.hpp
/// @brief     界面工具插件接口
/// @details   插件根对象通过实现本接口，向主程序提供可挂到菜单里的工具
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
#include "AstUtil/PluginInterface.hpp"
#include "AstUtil/StringView.hpp"

class QWidget;

AST_NAMESPACE_BEGIN

/*!
    @addtogroup ToolPlugin
    @{
*/

/// @brief 界面工具插件接口
/// @details
/// 插件根对象同时继承 PluginInstance 与本接口，主程序按工具键调用 invoke()。
/// 一个插件可以通过元数据声明多个工具，invoke() 需要根据 key 分派到具体的实现。
///
/// @par 元数据示例
/// 界面插件用 AST_PLUGIN_METADATA_JSON 声明工具清单，扫描阶段直接读文件字节，不需要加载动态库：
/// @code{.json}
/// {
///   "api": 1,
///   "tools": {
///     "ephem": {
///       "title":   { "zh": "星历生成", "en": "Ephemeris Generation" },
///       "tooltip": { "zh": "生成SPK星历", "en": "Generate SPK ephemeris" },
///       "order": 20,
///       "icon": "icons/ephem.png"
///     },
///     "report": {
///       "title": { "zh": "报告导出", "en": "Report Export" }
///     }
///   }
/// }
/// @endcode
///
/// 字段：
/// - `tools`：必填对象。键就是工具键，会原样传给 invoke()；同一个键不能重复
/// - `title`：必填。多语言对象（如 `{"zh":"…","en":"…"}`）或纯字符串
/// - `tooltip`：可选，格式同 `title`
/// - `order`：可选，菜单排序权重，默认 100
/// - `icon`：可选，相对插件动态库所在目录的图标路径；没有时菜单不带图标
/// - `api`：可选，接口版本，默认 1
///
/// @par 插件声明方式
/// @code{.cpp}
/// AST_PLUGIN_METADATA_JSON(IUiToolProvider, R"json(
/// { "tools": { "hello": { "title": { "zh": "你好", "en": "Hello" } } } }
/// )json")
/// AST_PLUGIN_INSTANCE(MyToolProvider)
/// @endcode
///
/// @note 多语言文本按"当前语言 → en → 字母序第一个"的顺序解析；
///       条目本身不保存解析结果，切换语言后宿主重建一次菜单即可
/// @note 窗口的创建、显示与销毁由插件自己负责，parent 只用于给对话框定位
class AST_UIUTIL_API IUiToolProvider
{
public:
    virtual ~IUiToolProvider() = default;

    /// @brief 调用一个工具
    /// @details 
    /// - 在打开对话框/面板后立即返回
    /// - 或打开模态对话框阻塞到用户操作完成
    /// - 或直接执行工具逻辑后返回
    /// @param key 元数据 tools 对象的键
    /// @param parent 主窗口，仅用作对话框父对象，可为空
    virtual void invoke(StringView key, QWidget* parent) = 0;
};

/// @brief 界面工具插件接口的唯一 ID（插件元数据里的 iid 必须是此值）
#define AST_IID_IUiToolProvider "space.ast.AstUiUtil.IUiToolProvider:1"

AST_DECLARE_INTERFACE(IUiToolProvider, AST_IID_IUiToolProvider)

/*! @} */

AST_NAMESPACE_END
