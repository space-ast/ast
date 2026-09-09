///
/// @file      UiMissionAnalysisWindow.cpp
/// @brief     航天任务分析主窗口实现
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
#include "AstGUI/UiScenarioWizard.hpp"
#include "AstGUI/UiInsertObjectDialog.hpp"
#include "AstGUI/UiObjectTree.hpp"
#include "AstGUI/UiTimelineView.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstCore/DateTime.hpp"

#include <QAction>
#include <QColor>
#include <QComboBox>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPalette>
#include <QPixmap>
#include <QStatusBar>
#include <QStyle>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 将时间点格式化为窗口使用的时钟时间（如 "7 Sep 2026 04:00:00.000"）
QString formatWallTime(const TimePoint& tp)
{
    DateTime dt{};
    aTimePointToUTC(tp, dt);
    std::string str;
    aDateTimeFormatGregorianEn(dt, str, 3);
    return QString::fromStdString(str);
}

} // namespace

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
    setupMenuBar();
    setupToolBar();
    setupObjectDock();
    setupCentral();
    setupTimelineDock();
    setupStatusBar();
}

// ============================================================================
// 顶部菜单栏
// ============================================================================

void UiMissionAnalysisWindow::setupMenuBar()
{
    auto* fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    auto* newAction = fileMenu->addAction(tr("新建"));
    newAction->setShortcut(QKeySequence::New);
    fileMenu->addAction(tr("打开"))->setShortcut(QKeySequence::Open);
    fileMenu->addAction(tr("保存"))->setShortcut(QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("退出"))->setShortcut(QKeySequence::Quit);
    QObject::connect(newAction, &QAction::triggered, this, [this]() {
        createScenario();
    });

    menuBar()->addMenu(tr("编辑(&E)"));
    menuBar()->addMenu(tr("视图(&V)"));
    auto* insertMenu = menuBar()->addMenu(tr("插入(&I)"));
    auto* insertObjectAction = insertMenu->addAction(tr("对象(&O)..."));
    QObject::connect(insertObjectAction, &QAction::triggered, this, [this]() { insertObject(); });
    menuBar()->addMenu(tr("分析(&A)"));
    menuBar()->addMenu(tr("卫星(&S)"));
    menuBar()->addMenu(tr("工具(&T)"));
    menuBar()->addMenu(tr("窗口(&W)"));
    menuBar()->addMenu(tr("帮助(&H)"));
}

// ============================================================================
// 顶部工具栏（多个可拖动工具组）
// ============================================================================

void UiMissionAnalysisWindow::setupToolBar()
{
    // 每个工具组都是独立的 QToolBar：可整体拖动（drag 手柄）、停靠到上/下/左/右、
    // 或拖出成浮动窗口，对应主窗口内可自由排列的工具组。
    auto makeGroup = [this](const QString& title, Qt::ToolBarArea area) {
        auto* group = new QToolBar(title, this);
        group->setObjectName(title);
        group->setMovable(true);          // 允许拖动
        group->setFloatable(true);        // 允许拖出为浮动窗口
        group->setAllowedAreas(Qt::AllToolBarAreas);  // 可停靠区域
        addToolBar(area, group);
        return group;
    };

    auto addIconAction = [](QToolBar* group, QStyle::StandardPixmap stdIcon,
                            const QString& text) {
        auto* action = group->addAction(group->style()->standardIcon(stdIcon), text);
        action->setToolTip(text);
        return action;
    };

    // 文件组
    QToolBar* fileGroup = makeGroup(tr("文件组"), Qt::TopToolBarArea);
    auto* fileNewAction = addIconAction(fileGroup, QStyle::SP_FileIcon, tr("新建"));
    connect(fileNewAction, &QAction::triggered, this, [this]() { createScenario(); });
    addIconAction(fileGroup, QStyle::SP_DialogOpenButton, tr("打开"));
    addIconAction(fileGroup, QStyle::SP_DialogSaveButton, tr("保存"));
    addIconAction(fileGroup, QStyle::SP_DirOpenIcon, tr("最近的"));

    // 编辑组
    QToolBar* editGroup = makeGroup(tr("编辑组"), Qt::TopToolBarArea);
    addIconAction(editGroup, QStyle::SP_TrashIcon, tr("剪切"));
    addIconAction(editGroup, QStyle::SP_FileDialogContentsView, tr("复制"));
    addIconAction(editGroup, QStyle::SP_DialogResetButton, tr("粘贴"));
    addIconAction(editGroup, QStyle::SP_BrowserReload, tr("重做"));

    // 导航组
    QToolBar* navGroup = makeGroup(tr("导航组"), Qt::TopToolBarArea);
    addIconAction(navGroup, QStyle::SP_ArrowLeft, tr("后退"));
    addIconAction(navGroup, QStyle::SP_ArrowRight, tr("前进"));
    addIconAction(navGroup, QStyle::SP_ArrowUp, tr("上移"));

    // 时间组（当前模拟时刻 + 访问指示）
    QToolBar* timeGroup = makeGroup(tr("时间组"), Qt::TopToolBarArea);
    timeEdit_ = new QLineEdit(timeGroup);
    timeEdit_->setText(QStringLiteral("7 Sep 2025 04:00:00.000"));
    timeEdit_->setMinimumWidth(180);
    timeEdit_->setToolTip(tr("当前模拟时刻"));
    timeGroup->addWidget(timeEdit_);

    auto* accessLabel = new QLabel(timeGroup);
    accessLabel->setPixmap(style()->standardIcon(QStyle::SP_DriveFDIcon).pixmap(18, 18));
    accessLabel->setToolTip(tr("访问"));
    timeGroup->addWidget(accessLabel);
}

// ============================================================================
// 左侧对象树 Dock
// ============================================================================

void UiMissionAnalysisWindow::setupObjectDock()
{
    objectDock_ = new QDockWidget(tr("对象浏览"), this);
    objectDock_->setObjectName(QStringLiteral("ObjectBrowserDock"));
    objectDock_->setAllowedAreas(Qt::DockWidgetAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea));
    objectDock_->setMinimumWidth(190);

    auto* container = new QWidget(objectDock_);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 对象树顶部小图标工具栏
    auto* objBar = new QToolBar(container);
    objBar->setMovable(false);
    objBar->setIconSize(QSize(16, 16));
    objBar->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder), tr("新建对象"));
    objBar->addAction(style()->standardIcon(QStyle::SP_DirLinkIcon), tr("添加子对象"));
    objBar->addAction(style()->standardIcon(QStyle::SP_TrashIcon), tr("删除"));
    objBar->addAction(style()->standardIcon(QStyle::SP_BrowserReload), tr("刷新"));
    layout->addWidget(objBar);

    objectTree_ = new UiObjectTree(container);
    objectTree_->refresh();
    layout->addWidget(objectTree_, 1);

    objectDock_->setWidget(container);

    addDockWidget(Qt::LeftDockWidgetArea, objectDock_);
}

// ============================================================================
// 中央多标签工作区（3D / 2D / 报告 占位）
// ============================================================================

void UiMissionAnalysisWindow::setupCentral()
{
    centralTabs_ = new QTabWidget(this);
    centralTabs_->setTabsClosable(true);
    centralTabs_->setMovable(true);

    auto makePlaceholder = [this](const QString& title) {
        auto* page = new QWidget(centralTabs_);
        page->setAutoFillBackground(true);
        QPalette pal = page->palette();
        pal.setColor(QPalette::Window, QColor(0xBF, 0xBF, 0xBF));
        page->setPalette(pal);
        int idx = centralTabs_->addTab(page, title);
        return idx;
    };

    makePlaceholder(tr("3D"));
    makePlaceholder(tr("2D"));
    makePlaceholder(tr("报告"));

    // 关闭标签页时删除对应的占位控件
    connect(centralTabs_, &QTabWidget::tabCloseRequested,
            this, [this](int index) {
        QWidget* w = centralTabs_->widget(index);
        centralTabs_->removeTab(index);
        delete w;
    });

    setCentralWidget(centralTabs_);
}

// ============================================================================
// 底部时间轴 Dock
// ============================================================================

void UiMissionAnalysisWindow::setupTimelineDock()
{
    timelineDock_ = new QDockWidget(tr("时间轴视图"), this);
    timelineDock_->setObjectName(QStringLiteral("TimelineViewDock"));

    auto* container = new QWidget(timelineDock_);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 工具栏行：图标 + 时间段下拉框
    auto* bar = new QToolBar(container);
    bar->setMovable(false);
    bar->setIconSize(QSize(16, 16));
    bar->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder), tr("新增"));
    bar->addAction(style()->standardIcon(QStyle::SP_DirHomeIcon), tr("添加时间轴"));
    bar->addAction(style()->standardIcon(QStyle::SP_DialogResetButton), tr("删除时间轴"));
    bar->addWidget(new QLabel(tr("Scenario Availability:"), bar));

    availabilityCombo_ = new QComboBox(bar);
    availabilityCombo_->addItems({tr("Scenario Availability"), tr("Scenario17 Availability")});
    bar->addWidget(availabilityCombo_);

    bar->addWidget(new QLabel(tr("Scenario Analysis Period:"), bar));
    analysisPeriodCombo_ = new QComboBox(bar);
    analysisPeriodCombo_->addItems({tr("Whole Run"), tr("Scenario17 Availability")});
    bar->addWidget(analysisPeriodCombo_);

    bar->addSeparator();
    bar->addAction(style()->standardIcon(QStyle::SP_DialogCancelButton), tr("滚动"));
    bar->addAction(style()->standardIcon(QStyle::SP_DialogDiscardButton), tr("实时"));
    layout->addWidget(bar);

    // 自绘时间轴
    timelineView_ = new UiTimelineView(container);
    layout->addWidget(timelineView_, 1);

    // 默认时间跨度（当前模拟时刻起 22 小时）
    TimePoint start = TimePoint::FromUTC(2026, 9, 7, 4, 0, 0);
    TimePoint stop  = start + (22 * 3600.0);
    timelineView_->setTimeSpan(TimeInterval(start, stop));
    timelineView_->setCurrentTime(start);

    std::vector<UiTimelineView::Bar> bars;
    bars.push_back({ start, start + (6 * 3600.0),
                     QStringLiteral("Scenario17 AvailabilityIntervals"),
                     QColor(0xE8, 0x8A, 0x28) });
    timelineView_->setBars(bars);

    timelineDock_->setWidget(container);
    addDockWidget(Qt::BottomDockWidgetArea, timelineDock_);
}

// ============================================================================
// 底部状态栏
// ============================================================================

void UiMissionAnalysisWindow::setupStatusBar()
{
    statusReadyLabel_ = new QLabel(tr("就绪"), this);
    statusBar()->addWidget(statusReadyLabel_);

    auto makeStatusItem = [this](const QIcon& icon, const QString& text) {
        auto* widget = new QWidget(this);
        auto* hbox = new QHBoxLayout(widget);
        hbox->setContentsMargins(4, 0, 4, 0);
        hbox->setSpacing(4);
        if (!icon.isNull())
        {
            auto* iconLabel = new QLabel(widget);
            iconLabel->setPixmap(icon.pixmap(16, 16));
            hbox->addWidget(iconLabel);
        }
        auto* textLabel = new QLabel(text, widget);
        hbox->addWidget(textLabel);
        return widget;
    };

    statusBar()->addPermanentWidget(
        makeStatusItem(style()->standardIcon(QStyle::SP_ComputerIcon), tr("Satellite1")));

    statusCoordLabel_ = new QLabel(tr("(-77.14286, -177.85714)"), this);
    statusBar()->addPermanentWidget(statusCoordLabel_);

    statusTimeLabel_ = new QLabel(tr("7 Sep 2026 04:00:00.000"), this);
    statusBar()->addPermanentWidget(statusTimeLabel_);

    statusTimeStepLabel_ = new QLabel(tr("Time Step: 10.00 sec"), this);
    statusBar()->addPermanentWidget(statusTimeStepLabel_);
}

// ============================================================================
// 新建场景
// ============================================================================

void UiMissionAnalysisWindow::createScenario()
{
    UiScenarioWizard dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const QString  name  = dlg.scenarioName();
    const TimePoint start = dlg.startTime();
    const TimePoint stop  = dlg.stopTime();
    const QString  startStr = formatWallTime(start);

    // 刷新当前模拟时刻显示（工具栏 + 状态栏）
    timeEdit_->setText(startStr);
    statusTimeLabel_->setText(startStr);

    // 更新底部时间轴为新建场景的分析区间
    if (timelineView_)
    {
        timelineView_->setTimeSpan(TimeInterval(start, stop));
        timelineView_->setCurrentTime(start);

        std::vector<UiTimelineView::Bar> bars;
        bars.push_back({ start, start + (6 * 3600.0),
                         tr("%1 可用区间").arg(name),
                         QColor(0xE8, 0x8A, 0x28) });
        timelineView_->setBars(bars);
    }

    // 时间段下拉框加入新场景
    if (availabilityCombo_)
    {
        availabilityCombo_->addItem(tr("场景可用性：%1").arg(name));
        availabilityCombo_->setCurrentIndex(availabilityCombo_->count() - 1);
    }
    if (analysisPeriodCombo_)
    {
        analysisPeriodCombo_->addItem(tr("整个运行区间：%1").arg(name));
        analysisPeriodCombo_->setCurrentIndex(analysisPeriodCombo_->count() - 1);
    }

    statusReadyLabel_->setText(tr("场景已创建: %1").arg(name));
}

// ============================================================================
// 插入对象
// ============================================================================

void UiMissionAnalysisWindow::insertObject()
{
    UiInsertObjectDialog dlg(this);
    connect(&dlg, &UiInsertObjectDialog::objectInsertRequested, this,
            [this](const QString& objectType, const QString& method, const QString& centralBody) {
                statusBar()->showMessage(tr("以“%1”方式插入 %2（中心天体 %3）")
                                             .arg(method, objectType, centralBody));
            });
    dlg.exec();
}

AST_NAMESPACE_END
