///
/// @file      UiAnalyzerMainWindow.cpp
/// @brief     Analyzer 模块主窗口实现
/// @details   顶部 Ribbon 菜单 + 中央 UiStudyWorkbench 编辑区 + 底部状态栏
/// @author    axel
/// @date      2026-06-03
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

#include "UiAnalyzerMainWindow.hpp"
#include "AstAnalyzer/StudyWorkbench.hpp"
#include "AstAnalyzer/SweepStudy.hpp"
#include "AstGUI/UiStudyWorkbench.hpp"
#include "AstGUI/UiSweepStudy.hpp"
#include "AstGUI/MissionIcons.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabBar>
#include <QStackedWidget>
#include <QToolButton>
#include <QFrame>
#include <QStatusBar>

AST_NAMESPACE_BEGIN

UiAnalyzerMainWindow::UiAnalyzerMainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
}

UiAnalyzerMainWindow::~UiAnalyzerMainWindow() = default;

void UiAnalyzerMainWindow::setupUi()
{
    // 顶部 Ribbon
    QWidget* ribbon = setupRibbon();
    setMenuWidget(ribbon);

    // 中央编辑区域（堆栈式，可切换 StudyWorkbench / SweepStudy）
    editorStack_ = new QStackedWidget(this);

    basicEditor_ = new UiStudyWorkbench(editorStack_);
    editorStack_->addWidget(basicEditor_); // index 0

    traverseEditor_ = new UiSweepStudy(editorStack_);
    editorStack_->addWidget(traverseEditor_); // index 1

    editorStack_->setCurrentIndex(0);
    setCentralWidget(editorStack_);

    // 状态栏（空白占位）
    statusBar();
}

// ============================================================================
// Ribbon
// ============================================================================

static const char* kRibbonTabs[] = {
    u8"分析", u8"建模"
};

QWidget* UiAnalyzerMainWindow::setupRibbon()
{
    auto* ribbonWidget = new QWidget(this);
    ribbonWidget->setObjectName(QStringLiteral("RibbonWidget"));
    auto* ribbonLayout = new QVBoxLayout(ribbonWidget);
    ribbonLayout->setContentsMargins(0, 0, 0, 0);
    ribbonLayout->setSpacing(0);

    // 页签条
    ribbonTabBar_ = new QTabBar(ribbonWidget);
    ribbonTabBar_->setObjectName(QStringLiteral("RibbonTabBar"));
    ribbonTabBar_->setExpanding(false);
    ribbonTabBar_->setDrawBase(false);
    for (const char* tab : kRibbonTabs)
        ribbonTabBar_->addTab(QString::fromUtf8(tab));
    ribbonLayout->addWidget(ribbonTabBar_);

    // 页签内容
    ribbonStack_ = new QStackedWidget(ribbonWidget);
    ribbonStack_->setObjectName(QStringLiteral("RibbonStack"));
    for (int i = 0; i < ribbonTabBar_->count(); ++i)
        ribbonStack_->addWidget(createRibbonPage(i));
    ribbonLayout->addWidget(ribbonStack_);

    connect(ribbonTabBar_, &QTabBar::currentChanged,
            ribbonStack_, &QStackedWidget::setCurrentIndex);

    return ribbonWidget;
}

QToolButton* UiAnalyzerMainWindow::createRibbonButton(const QString& text,
    QStyle::StandardPixmap stdIcon, QWidget* parent)
{
    auto* btn = new QToolButton(parent);
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setIconSize(QSize(24, 24));
    btn->setMinimumWidth(56);
    btn->setIcon(style()->standardIcon(stdIcon));
    return btn;
}

QToolButton* UiAnalyzerMainWindow::createRibbonButton(const QString& text,
    const QIcon& icon, QWidget* parent)
{
    auto* btn = new QToolButton(parent);
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setIconSize(QSize(24, 24));
    btn->setMinimumWidth(56);
    btn->setIcon(icon);
    return btn;
}

static void addRibbonSeparator(QHBoxLayout* layout)
{
    auto* sep = new QFrame();
    sep->setFrameShape(QFrame::VLine);
    sep->setFrameShadow(QFrame::Sunken);
    layout->addWidget(sep);
}

QWidget* UiAnalyzerMainWindow::createRibbonPage(int index)
{
    auto* page = new QWidget();
    auto* layout = new QHBoxLayout(page);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(2);

    switch (index)
    {
    case 0: // 分析
        layout->addWidget(createRibbonButton(tr("参数研究"), missionIcon("ParameterStudy"), page));
        layout->addWidget(createRibbonButton(tr("地毯图研究"), missionIcon("CarpetPlot"), page));
        layout->addWidget(createRibbonButton(tr("区间分析"), missionIcon("IntervalAnalysis"), page));
        layout->addWidget(createRibbonButton(tr("优化打靶"), missionIcon("OptimalTargeting"), page));
        layout->addWidget(createRibbonButton(tr("不确定性分析"), missionIcon("UncertaintyAnalysis"), page));
        break;

    case 1: // 建模
        layout->addWidget(createRibbonButton(tr("类型管理"), missionIcon("TypeManagement"), page));
        layout->addWidget(createRibbonButton(tr("对象管理"), missionIcon("ObjectManagement"), page));
        layout->addWidget(createRibbonButton(tr("组件管理"), missionIcon("ComponentManagement"), page));
        break;
    }

    layout->addStretch();
    return page;
}

// ============================================================================
// 公共接口
// ============================================================================

void UiAnalyzerMainWindow::setStudyWorkbench(StudyWorkbench* analyzer)
{
    basicEditor_->setStudyWorkbench(analyzer);
    editorStack_->setCurrentIndex(0);
}

void UiAnalyzerMainWindow::setSweepStudy(SweepStudy* analyzer)
{
    traverseEditor_->setAnalyzer(analyzer);
    editorStack_->setCurrentIndex(1);
}

AST_NAMESPACE_END
