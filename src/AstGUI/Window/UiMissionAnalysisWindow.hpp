///
/// @file      UiMissionAnalysisWindow.hpp
/// @brief     航天任务分析主窗口
/// @details   任务分析顶层界面：顶部标准菜单栏 + 工具栏 + 左侧对象树 Dock
///            + 中央多标签工作区（3D/2D/报告占位）+ 底部时间轴 Dock + 状态栏。
///            作为 AppMissionAnalysis 应用的主窗口视图，独立于 UiMainWindow。
/// @author    axel
/// @date      2026-09-03
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
#include <QMainWindow>

class QTabBar;
class QStackedWidget;
class QTabWidget;
class QToolBar;
class QLineEdit;
class QLabel;
class QDockWidget;
class QComboBox;

AST_NAMESPACE_BEGIN

class UiObjectTree;
class UiTimelineView;

/// @brief 任务分析主窗口
///
/// 界面布局：
/// - 顶部标准菜单栏（文件、编辑、视图、插入、分析、卫星、工具、窗口、帮助）
/// - 工具栏（新建/打开/保存、剪切/复制/粘贴、撤销/重做 + 时间输入框）
/// - 左侧对象树 Dock（Object Browser，管理场景/航天器/传感器等对象）
/// - 中央多标签工作区（3D 窗口、2D 地图窗口、报告）
/// - 底部时间轴 Dock（Timeline View，含时间段选择 + 小时刻度轴 + 区间条）
/// - 状态栏（就绪提示、对象信息、坐标、时间步长）
class AST_GUI_API UiMissionAnalysisWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UiMissionAnalysisWindow(QWidget* parent = nullptr);
    ~UiMissionAnalysisWindow() override;

    UiMissionAnalysisWindow(const UiMissionAnalysisWindow&) = delete;
    UiMissionAnalysisWindow& operator=(const UiMissionAnalysisWindow&) = delete;

private:
    void setupUi();

    void setupMenuBar();
    void setupToolBar();
    void setupObjectDock();
    void setupCentral();
    void setupTimelineDock();
    void setupStatusBar();

    /// @brief 打开"新建场景向导"并应用新场景（刷新时间轴 / 时间段 / 时刻显示）
    void createScenario();

    /// @brief 打开"插入对象"对话框，并将插入结果反映到状态栏
    void insertObject();

    // 工具栏（由多个可移动工具组组成，QMainWindow 持有所有权）
    QLineEdit* timeEdit_    = nullptr;   ///< 时间输入框（当前模拟时刻）

    // 左侧对象浏览树 Dock
    QDockWidget*  objectDock_ = nullptr;
    UiObjectTree* objectTree_ = nullptr;

    // 中央多标签工作区
    QTabWidget* centralTabs_ = nullptr;

    // 底部时间轴 Dock
    QDockWidget*    timelineDock_  = nullptr;
    UiTimelineView* timelineView_  = nullptr;
    QComboBox*      availabilityCombo_     = nullptr;
    QComboBox*      analysisPeriodCombo_   = nullptr;

    // 底部状态栏
    QLabel* statusReadyLabel_    = nullptr;
    QLabel* statusCoordLabel_    = nullptr;
    QLabel* statusTimeLabel_     = nullptr;
    QLabel* statusTimeStepLabel_ = nullptr;
};

AST_NAMESPACE_END
