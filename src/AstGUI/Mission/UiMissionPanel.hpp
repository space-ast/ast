///
/// @file      UiMissionPanel.hpp
/// @brief     任务序列编辑主面板
/// @details   组装任务树、段编辑器、工具栏和输出区域
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QMenu>

class QSplitter;
class QPlainTextEdit;

AST_NAMESPACE_BEGIN

class MissionModerator;
class MissionCommand;
class UiMissionTree;
class UiSegmentEditor;

/// @brief 任务序列编辑主面板（QWidget），由调用方嵌入容器
class AST_GUI_API UiMissionPanel : public QWidget
{
    Q_OBJECT
public:
    explicit UiMissionPanel(QWidget* parent = nullptr);
    ~UiMissionPanel() override;

    /// @brief 设置外部 MissionModerator（裸指针，不持有所有权）
    void setModerator(MissionModerator* moderator);

    /// @brief 获取当前 Moderator
    MissionModerator* moderator() const;

signals:
    void runRequested();
    void stopped();

private slots:
    void onSegmentSelected(MissionCommand* cmd);
    void onAddInitialState();
    void onAddPropagate();
    void onAddManeuver();
    void onAddSequence();
    void onAddTargeterSequence();
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
    UiMissionTree*   missionTree_ = nullptr;
    UiSegmentEditor* segmentEditor_ = nullptr;
    QPlainTextEdit*  outputView_ = nullptr;

    // 数据
    MissionModerator* moderator_ = nullptr;
};

AST_NAMESPACE_END
