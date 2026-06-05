///
/// @file      UiMainWindow.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "UiMainWindow.hpp"
#include "UiNewObjectQuickDialog.hpp"
#include "AstGUI/MissionIcons.hpp"
#include "AstGUI/ObjectEditRegistry.hpp"
#include "AstGUI/UiCommon.hpp"
#include "AstGUI/UiObjectTree.hpp"
#include "AstUtil/RTTIAPI.hpp"

#include <QApplication>
#include <QDockWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>

AST_NAMESPACE_BEGIN

// ============================================================================
// 构造 / 析构
// ============================================================================

UiMainWindow::UiMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("航天仿真"));
    resize(1290, 768);
    setupUi();
}

UiMainWindow::~UiMainWindow() = default;

// ============================================================================
// 主界面搭建
// ============================================================================

void UiMainWindow::setupUi()
{
    // 顶部 Ribbon 菜单
    QWidget* ribbon = setupRibbon();
    setMenuWidget(ribbon);

    // 中央多标签页
    centralTabs_ = new QTabWidget(this);
    centralTabs_->setTabsClosable(true);
    centralTabs_->setMovable(true);
    setCentralWidget(centralTabs_);

    // 关闭标签页时删除对应的编辑控件
    connect(centralTabs_, &QTabWidget::tabCloseRequested,
            this, [this](int index)
    {
        QWidget* w = centralTabs_->widget(index);
        centralTabs_->removeTab(index);
        delete w;
    });

    // 左侧 Dock
    setupObjectDock();

    // 状态栏
    setupStatusBar();
}

// ============================================================================
// 顶部 Ribbon
// ============================================================================

static const char* kRibbonTabs[] = {
    u8"建模", u8"分析"
};

QWidget* UiMainWindow::setupRibbon()
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

QToolButton* UiMainWindow::createRibbonButton(const QString& text, const QIcon& icon,
                                               QWidget* parent)
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

QWidget* UiMainWindow::createRibbonPage(int index)
{
    auto* page = new QWidget();
    auto* layout = new QHBoxLayout(page);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(2);

    switch (index)
    {
    case 0: // 建模
        {
            auto* newBtn = createRibbonButton(tr("新建"), missionIcon("New"), page);
            connect(newBtn, &QToolButton::clicked, this, [this]() {
                UiNewObjectQuickDialog dlg(this);
                if (dlg.exec() != QDialog::Accepted)
                    return;
                objectTree_->refresh();
                statusReadyLabel_->setText(tr("已创建: %1").arg(dlg.createdObjectName()));
            });
            layout->addWidget(newBtn);
            addRibbonSeparator(layout);
        }
        layout->addWidget(createRibbonButton(tr("类型管理"), missionIcon("TypeManagement"), page));
        layout->addWidget(createRibbonButton(tr("对象管理"), missionIcon("ObjectManagement"), page));
        layout->addWidget(createRibbonButton(tr("组件管理"), missionIcon("ComponentManagement"), page));
        break;

    case 1: // 分析
        layout->addWidget(createRibbonButton(tr("参数研究"), missionIcon("ParameterStudy"), page));
        layout->addWidget(createRibbonButton(tr("地毯图研究"), missionIcon("CarpetPlot"), page));
        layout->addWidget(createRibbonButton(tr("区间分析"), missionIcon("IntervalAnalysis"), page));
        layout->addWidget(createRibbonButton(tr("优化打靶"), missionIcon("OptimalTargeting"), page));
        layout->addWidget(createRibbonButton(tr("不确定性分析"), missionIcon("UncertaintyAnalysis"), page));
        break;
    }

    layout->addStretch();
    return page;
}

// ============================================================================
// 左侧对象 Dock
// ============================================================================

void UiMainWindow::setupObjectDock()
{
    objectDock_ = new QDockWidget(tr("对象"), this);
    objectDock_->setObjectName(QStringLiteral("ObjectDock"));
    objectDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    objectDock_->setMinimumWidth(190);

    objectTree_ = new UiObjectTree(objectDock_);
    objectTree_->refresh();
    objectDock_->setWidget(objectTree_);

    // 双击对象树节点时，在中央标签页中打开对应的编辑控件
    connect(objectTree_, &UiObjectTree::objectDoubleClicked,
            this, [this](Object* obj)
    {
        // 检查是否已经为该对象打开了编辑标签页
        for (int i = 0; i < centralTabs_->count(); ++i)
        {
            if (centralTabs_->tabToolTip(i) == QString::fromStdString(obj->typeName())
                && centralTabs_->tabText(i) == aUiObjectDisplayName(obj))
            {
                centralTabs_->setCurrentIndex(i);
                return;
            }
        }

        QWidget* editWidget = ObjectEditRegistry::Instance().newEditWidget(obj);
        if (!editWidget)
            return;
        int idx = centralTabs_->addTab(editWidget, objectIcon(obj),
                                       aUiObjectDisplayName(obj));
        centralTabs_->setTabToolTip(idx, QString::fromStdString(obj->typeName()));
        centralTabs_->setCurrentIndex(idx);
    });

    addDockWidget(Qt::LeftDockWidgetArea, objectDock_);
}

// ============================================================================
// 状态栏
// ============================================================================

void UiMainWindow::setupStatusBar()
{
    statusBar()->setObjectName(QStringLiteral("MainStatusBar"));
    statusReadyLabel_ = new QLabel(tr("就绪"), this);
    statusBar()->addWidget(statusReadyLabel_);
}

AST_NAMESPACE_END
