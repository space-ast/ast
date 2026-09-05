///
/// @file      UiMissionAnalysisWindow.cpp
/// @brief     航天任务分析主窗口
/// @details   任务分析顶层界面：顶部 Ribbon 菜单 + 左侧对象树 Dock
///            + 中央多标签工作区（3D/2D/报告）+ 底部状态栏。
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

#include "UiMissionAnalysisWindow.hpp"

#include <QLabel>
#include <QStatusBar>
#include <QTabWidget>

AST_NAMESPACE_BEGIN

// ============================================================================
// 构造 / 析构
// ============================================================================

UiMissionAnalysisWindow::UiMissionAnalysisWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("任务分析"));
    resize(1600, 900);
    setupUi();
}

UiMissionAnalysisWindow::~UiMissionAnalysisWindow() = default;

// ============================================================================
// 主界面搭建
// ============================================================================

void UiMissionAnalysisWindow::setupUi()
{
    // --- 中央多标签工作区（对标 STK 的 3D 窗口 / 2D 地图 / 报告） ---
    centralTabs_ = new QTabWidget(this);
    centralTabs_->setTabsClosable(true);
    centralTabs_->setMovable(true);
    setCentralWidget(centralTabs_);

    // --- 底部状态栏 ---
    statusReadyLabel_ = new QLabel(tr("就绪"), this);
    statusBar()->addWidget(statusReadyLabel_);

    // TODO: 后续补齐
    //  - setupRibbon():   顶部 Ribbon 标签页菜单（透视/轨道工具/访问分析）
    //  - setupObjectDock(): 左侧对象树 Dock（场景/航天器/传感器）
    //  - timeAxisDock / 动画控制等
}

AST_NAMESPACE_END
