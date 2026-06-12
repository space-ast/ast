///
/// @file      PilotAgent.hpp
/// @brief     Qt控件生命周期管理、ref映射、快照生成
/// @author    axel
/// @date      2026-06-12
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
#include <QObject>
#include <QHash>
#include <string>
#include <sstream>
#include <functional>

class QWidget;
class QAction;
class QMenu;
class QRubberBand;

AST_NAMESPACE_BEGIN

/*!
    @defgroup UiPilot
    @brief AI驱动的Qt GUI自动化测试
    @{
*/

/// @brief Qt控件生命周期管理与界面快照生成
/// @details 维护 QWidget*/QAction* ↔ ref 的双向映射，ref 在控件整个生命周期内稳定不变。
///          提供 snapshot() 生成结构化控件树文本供 LLM 消费。
class AST_UIPILOT_API PilotAgent : public QObject
{
    Q_OBJECT

public:
    /// @brief 获取全局单例
    static PilotAgent* instance();

    /// @brief 构造函数
    explicit PilotAgent(QObject* parent = nullptr);

    /// @brief 析构函数
    ~PilotAgent() override;

    // ---- ref 管理 ----

    /// @brief 获取或分配 QWidget 的唯一ref（生命周期绑定）
    int ref(QWidget* widget);

    /// @brief 获取或分配 QAction 的唯一ref
    int ref(QAction* action);

    /// @brief 通过ref查找控件
    QWidget* widget(int refId) const;

    /// @brief 通过ref查找 QAction
    QAction* action(int refId) const;

    /// @brief 通过ref查找任意 QObject（先查 widget，再查 action）
    QObject* object(int refId) const;

    /// @brief 获取ref对应的对象类型名
    std::string objectType(int refId) const;

    /// @brief 检查ref是否有效
    bool isValidRef(int refId) const;

    // ---- 快照 ----

    /// @brief 生成结构化控件树快照
    std::string snapshot(int maxDepth = 8);

    /// @brief 对指定控件生成局部快照
    std::string snapshotOf(QWidget* root, int maxDepth = 8);

    /// @brief 检测并返回当前活动的模态对话框
    QWidget* activeModalDialog() const;

    // ---- 操作 ----

    /// @brief 高亮控件（红色边框，调试用）
    void highlight(QWidget* widget, int durationMs = 2000);

    /// @brief 等待界面稳定
    void waitForIdle(int timeoutMs = 5000);

    /// @brief 获取所有顶层窗口
    QList<QWidget*> topLevelWidgets() const;

    /// @brief 点击QAction（模拟触发）
    bool triggerAction(QAction* action);

    /// @brief 展开QMenu（模拟悬停以在快照中看到子项）
    /// @return 展开后的QMenu子项（QActions）
    QList<QAction*> expandMenu(QMenu* menu);

    /// @brief 是否自动生成快照
    bool isAutoSnapshot() const{return autoSnapshot_;}

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    // ---- 剪枝 ----
    static bool isWorthReporting(QWidget* w);
    static bool isInteractive(QWidget* w);

    // ---- 快照输出 ----
    void formatWidget(QWidget* w, int depth, std::ostringstream& out);
    void formatAction(QAction* a, int depth, std::ostringstream& out);
    void walkWidgetTree(QWidget* root, int depth, int maxDepth, std::ostringstream& out);
    void formatWidgetValue(QWidget* w, std::ostringstream& out);
    static QString statusTags(QWidget* w);
    static QString friendlyName(QWidget* w);

    // ---- 内存清理 ----
    void cleanupRef(int refId);

    bool autoSnapshot_ = false;          ///< 是否自动生成快照

    // ---- ref 映射 ----
    QHash<QWidget*, int> widgetToRef_;   ///< widget* → ref编号
    QHash<int, QWidget*> refToWidget_;   ///< ref编号 → widget*
    QHash<QAction*, int> actionToRef_;   ///< action* → ref编号
    QHash<int, QAction*> refToAction_;   ///< ref编号 → action*

    int nextRef_ = 1;                    ///< 下一个可用的ref编号
    static PilotAgent* s_instance;      ///< 全局单例
};

/*! @} */

AST_NAMESPACE_END
