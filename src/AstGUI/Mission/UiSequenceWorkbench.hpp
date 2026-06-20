///
/// @file      UiSequenceWorkbench.hpp
/// @brief     任务序列编辑主面板
/// @details   组装任务树、段编辑器、工具栏和输出区域
/// @author    axel
/// @date      2026-05-17
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
#include "AstGUI/UiObject.hpp"
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QMenu>

class QSplitter;
class QPlainTextEdit;

AST_NAMESPACE_BEGIN

class Sequence;
class Command;
class UiCommandTree;
class UiCommandEditor;

/// @brief 任务序列编辑主面板（QWidget），由调用方嵌入容器
class AST_GUI_API UiSequenceWorkbench : public UiObject
{
    Q_OBJECT
public:
    explicit UiSequenceWorkbench(QWidget* parent = nullptr);
    explicit UiSequenceWorkbench(Object* sequence, QWidget* parent = nullptr);
    ~UiSequenceWorkbench() override;

    /// @brief 设置外部 MainSequence（裸指针，不持有所有权）
    void setSequence(Sequence* sequence);
    
    /// @brief 获取当前 MainSequence
    Sequence* sequence() const;


private slots:
    void onCommandSelected(Command* cmd);
    void onAddInitialState();
    void onAddPropagate();
    void onAddManeuver();
    void onAddSequence();
    void onAddTargeterSequence();
    void onAddLandingSite();
    void onDeleteSegment();
    void onOpenFile();
    void onSaveFile();
    void onRun();
    void onStop();

private:
    void setupUi();
    void setupToolBar();
    void setupConnections();
    void appendOutput(const QString& text);

    // 工具栏和动作
    QToolBar*    toolBar_ = nullptr;
    QMenu*       addMenu_ = nullptr;
    QAction*     openAction_ = nullptr;
    QAction*     saveAction_ = nullptr;
    QAction*     deleteAction_ = nullptr;
    QAction*     runAction_ = nullptr;
    QAction*     stopAction_ = nullptr;

    // 子组件
    QSplitter*       mainSplitter_ = nullptr;
    UiCommandTree*   commandTree_ = nullptr;
    UiCommandEditor* commandEditor_ = nullptr;
    QPlainTextEdit*  outputView_ = nullptr;

};

AST_NAMESPACE_END
