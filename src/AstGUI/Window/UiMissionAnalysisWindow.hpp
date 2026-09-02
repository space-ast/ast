///
/// @file      UiMissionAnalysisWindow.hpp
/// @brief     航天任务分析主窗口
/// @details   任务分析顶层界面：顶部 Ribbon 菜单 + 左侧对象树 Dock
///            + 中央多标签工作区（3D/2D/报告）+ 底部状态栏。
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
class QLabel;
class QDockWidget;

AST_NAMESPACE_BEGIN

class UiObjectTree;

/// @brief STK 风格任务分析主窗口
///
/// 界面布局：
/// - 顶部 Ribbon 标签页菜单（透视、轨道工具、场景、访问分析等）
/// - 左侧对象树 Dock（Object Browser，管理场景/航天器/传感器等对象）
/// - 中央多标签工作区（3D 窗口、2D 地图窗口、时间轴、报告）
/// - 底部状态栏（就绪提示、单位/坐标信息）
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

    // 顶部 Ribbon 菜单
    QTabBar*        ribbonTabBar_ = nullptr;
    QStackedWidget* ribbonStack_ = nullptr;

    // 左侧对象浏览树 Dock
    QDockWidget* objectDock_ = nullptr;
    UiObjectTree* objectTree_ = nullptr;

    // 中央多标签工作区
    QTabWidget* centralTabs_ = nullptr;

    // 底部状态栏
    QLabel* statusReadyLabel_ = nullptr;
};

AST_NAMESPACE_END
