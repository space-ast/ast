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
#include "AstGUI/ObjectIcons.hpp"
#include "AstGUI/UiObjectTree.hpp"

#include <QApplication>
#include <QButtonGroup>
#include <QCoreApplication>
#include <QDir>
#include <QDockWidget>
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QSlider>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>

AST_NAMESPACE_BEGIN

// ============================================================================
// 构造 / 析构
// ============================================================================

UiMainWindow::UiMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("SpaceAST"));
    resize(1280, 800);
    setupUi();
    applyTheme(QStringLiteral("default"));
}

UiMainWindow::~UiMainWindow() = default;

// ============================================================================
// 主界面搭建
// ============================================================================

void UiMainWindow::setupUi()
{
    // 顶部 Ribbon
    QWidget* ribbon = setupRibbon();
    setMenuWidget(ribbon);

    // 中央画布
    setupCentralCanvas();

    // 左侧对象面板
    setupObjectDock();

    // 底部时间视图
    setupTimeDock();

    // 状态栏
    setupStatusBar();
}

// ============================================================================
// 顶部 Ribbon
// ============================================================================

static const char* kRibbonTabs[] = {
    "开始", "编辑", "视图", "插入", "输出", "工具",  "集成", "关于"
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

    // 时间控制条
    auto* timeBar = new QWidget(ribbonWidget);
    timeBar->setObjectName(QStringLiteral("TimeControlBar"));
    auto* timeLayout = new QHBoxLayout(timeBar);
    timeLayout->setContentsMargins(4, 2, 4, 2);

    auto addTb = [&](QStyle::StandardPixmap icon, const QString& tip) {
        auto* btn = new QToolButton(timeBar);
        btn->setIcon(style()->standardIcon(icon));
        btn->setToolTip(tip);
        btn->setAutoRaise(true);
        timeLayout->addWidget(btn);
        return btn;
    };

    addTb(QStyle::SP_MediaSkipBackward,  tr("跳到起点"));
    addTb(QStyle::SP_MediaSeekBackward,  tr("后退"));
    addTb(QStyle::SP_MediaPlay,           tr("播放"));
    addTb(QStyle::SP_MediaSeekForward,   tr("前进"));
    addTb(QStyle::SP_MediaSkipForward,   tr("跳到终点"));

    timeLayout->addSpacing(12);

    auto* epochLabel = new QLabel(tr("当前历元: 2026-01-01 00:00:00.000 UTCG"), timeBar);
    timeLayout->addWidget(epochLabel);

    timeLayout->addStretch();

    auto* stepLabel = new QLabel(tr("步长: 60 s"), timeBar);
    timeLayout->addWidget(stepLabel);

    ribbonLayout->addWidget(timeBar);

    return ribbonWidget;
}

QToolButton* UiMainWindow::createRibbonButton(const QString& text, const QString& iconName, QWidget* parent)
{
    auto* btn = new QToolButton(parent);
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setIconSize(QSize(24, 24));
    btn->setMinimumWidth(56);

    if (!iconName.isEmpty())
    {
        QIcon icon = loadIcon(iconName);
        if (!icon.isNull())
            btn->setIcon(icon);
    }
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
    case 0: // 开始
        layout->addWidget(createRibbonButton(tr("新建"),   QStringLiteral("Object"),     page));
        layout->addWidget(createRibbonButton(tr("打开"),   QString(),                    page));
        layout->addWidget(createRibbonButton(tr("保存"),   QString(),                    page));
        addRibbonSeparator(layout);
        layout->addWidget(createRibbonButton(tr("场景"),   QStringLiteral("Object"),     page));
        layout->addWidget(createRibbonButton(tr("卫星"),   QStringLiteral("Satellite"),  page));
        layout->addWidget(createRibbonButton(tr("设施"),   QStringLiteral("Facility"),   page));
        layout->addWidget(createRibbonButton(tr("传感器"), QStringLiteral("Sensor"),     page));
        break;

    case 1: // 编辑
        layout->addWidget(createRibbonButton(tr("撤销"), QString(), page));
        layout->addWidget(createRibbonButton(tr("重做"), QString(), page));
        addRibbonSeparator(layout);
        layout->addWidget(createRibbonButton(tr("剪切"), QString(), page));
        layout->addWidget(createRibbonButton(tr("复制"), QString(), page));
        layout->addWidget(createRibbonButton(tr("粘贴"), QString(), page));
        addRibbonSeparator(layout);
        layout->addWidget(createRibbonButton(tr("删除"), QString(), page));
        break;

    case 2: // 视图
        layout->addWidget(createRibbonButton(tr("三维视图"), QStringLiteral("Body"), page));
        layout->addWidget(createRibbonButton(tr("二维视图"), QStringLiteral("Plane"), page));
        addRibbonSeparator(layout);
        layout->addWidget(createRibbonButton(tr("缩放"),     QString(), page));
        layout->addWidget(createRibbonButton(tr("适应窗口"), QString(), page));

        // 主题切换
        {
            addRibbonSeparator(layout);

            QDir themeDir(QCoreApplication::applicationDirPath() + QStringLiteral("/data/style"));
            auto themeFiles = themeDir.entryList({QStringLiteral("*.qss")}, QDir::Files, QDir::Name);

            auto* themeGroup = new QButtonGroup(page);
            themeGroup->setExclusive(true);

            for (const auto& file : themeFiles)
            {
                QString name = file.chopped(4); // remove ".qss"
                auto* btn = createRibbonButton(name, QString(), page);
                btn->setCheckable(true);
                themeGroup->addButton(btn);

                if (name == currentTheme_)
                    btn->setChecked(true);

                connect(btn, &QToolButton::clicked, this, [this, name]() {
                    applyTheme(name);
                });

                layout->addWidget(btn);
            }
        }
        break;

    case 3: // 插入
        layout->addWidget(createRibbonButton(tr("航天器"), QStringLiteral("Spacecraft"),    page));
        layout->addWidget(createRibbonButton(tr("轨道"),   QStringLiteral("OrbitState"),     page));
        layout->addWidget(createRibbonButton(tr("机动"),   QStringLiteral("Maneuver"),       page));
        layout->addWidget(createRibbonButton(tr("序列"),   QStringLiteral("Sequence"),        page));
        break;

    case 4: // 输出
        layout->addWidget(createRibbonButton(tr("报告"), QString(), page));
        layout->addWidget(createRibbonButton(tr("图表"), QString(), page));
        layout->addWidget(createRibbonButton(tr("导出"), QString(), page));
        break;

    case 5: // 工具
        layout->addWidget(createRibbonButton(tr("传播"), QStringLiteral("Propagate"), page));
        layout->addWidget(createRibbonButton(tr("分析"), QString(),                  page));
        layout->addWidget(createRibbonButton(tr("设置"), QString(),                  page));
        break;

    case 6: // 集成
        layout->addWidget(createRibbonButton(tr("脚本"),     QString(), page));
        layout->addWidget(createRibbonButton(tr("外部工具"), QString(), page));
        layout->addWidget(createRibbonButton(tr("数据导入"), QString(), page));
        break;

    case 7: // 关于
        layout->addWidget(createRibbonButton(tr("帮助"), QString(), page));
        layout->addWidget(createRibbonButton(tr("关于"), QString(), page));
        break;
    }

    layout->addStretch();
    return page;
}

// ============================================================================
// 中央画布
// ============================================================================

void UiMainWindow::setupCentralCanvas()
{
    canvasFrame_ = new QFrame(this);
    canvasFrame_->setObjectName(QStringLiteral("CentralCanvas"));
    canvasFrame_->setFrameShape(QFrame::NoFrame);

    auto* layout = new QHBoxLayout(canvasFrame_);
    auto* hint = new QLabel(tr("3D / 2D 场景视图"), canvasFrame_);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet(QStringLiteral("color: #c0c0c0; font-size: 18px;"));
    layout->addWidget(hint);

    setCentralWidget(canvasFrame_);
}

// ============================================================================
// 左侧对象 Dock
// ============================================================================

void UiMainWindow::setupObjectDock()
{
    objectDock_ = new QDockWidget(tr("对象"), this);
    objectDock_->setObjectName(QStringLiteral("ObjectDock"));
    objectDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    objectDock_->setMinimumWidth(220);

    auto* dockContent = new QWidget(objectDock_);
    auto* dockLayout = new QVBoxLayout(dockContent);
    dockLayout->setContentsMargins(0, 0, 0, 0);
    dockLayout->setSpacing(0);

    // 工具按钮行
    auto* toolRow = new QWidget(dockContent);
    auto* toolRowLayout = new QHBoxLayout(toolRow);
    toolRowLayout->setContentsMargins(2, 2, 2, 2);
    toolRowLayout->setSpacing(1);

    auto addDockBtn = [&](QStyle::StandardPixmap icon, const QString& tip) {
        auto* btn = new QToolButton(toolRow);
        btn->setIcon(style()->standardIcon(icon));
        btn->setToolTip(tip);
        btn->setAutoRaise(true);
        btn->setIconSize(QSize(16, 16));
        toolRowLayout->addWidget(btn);
    };

    addDockBtn(QStyle::SP_BrowserReload, tr("刷新"));
    addDockBtn(QStyle::SP_FileIcon,      tr("新建"));
    addDockBtn(QStyle::SP_TrashIcon,     tr("删除"));
    addDockBtn(QStyle::SP_FileDialogDetailedView, tr("属性"));

    toolRowLayout->addStretch();
    dockLayout->addWidget(toolRow);

    // 页签
    auto* tabWidget = new QTabWidget(dockContent);

    objectTree_ = new UiObjectTree(tabWidget);
    objectTree_->refresh();
    tabWidget->addTab(objectTree_, tr("对象视图"));

    groupTree_ = new QTreeWidget(tabWidget);
    groupTree_->setHeaderHidden(true);
    tabWidget->addTab(groupTree_, tr("分组视图"));

    dockLayout->addWidget(tabWidget);
    objectDock_->setWidget(dockContent);
    addDockWidget(Qt::LeftDockWidgetArea, objectDock_);
}

// ============================================================================
// 底部时间视图 Dock
// ============================================================================

void UiMainWindow::setupTimeDock()
{
    timeDock_ = new QDockWidget(tr("时间视图"), this);
    timeDock_->setObjectName(QStringLiteral("TimeDock"));
    timeDock_->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    timeDock_->setMinimumHeight(100);

    auto* dockContent = new QWidget(timeDock_);
    auto* dockLayout = new QVBoxLayout(dockContent);
    dockLayout->setContentsMargins(6, 4, 6, 4);

    // 历元标签行
    auto* labelRow = new QWidget(dockContent);
    auto* labelLayout = new QHBoxLayout(labelRow);
    labelLayout->setContentsMargins(0, 0, 0, 0);

    startEpochLabel_ = new QLabel(tr("开始历元: 2026-01-01 00:00:00.000 UTCG"), labelRow);
    labelLayout->addWidget(startEpochLabel_);

    labelLayout->addStretch();

    currentEpochLabel_ = new QLabel(tr("当前历元: 2026-01-01 00:00:00.000 UTCG"), labelRow);
    labelLayout->addWidget(currentEpochLabel_);

    labelLayout->addStretch();

    endEpochLabel_ = new QLabel(tr("结束历元: 2026-01-02 00:00:00.000 UTCG"), labelRow);
    labelLayout->addWidget(endEpochLabel_);

    dockLayout->addWidget(labelRow);

    // 时间轴
    timeSlider_ = new QSlider(Qt::Horizontal, dockContent);
    timeSlider_->setRange(0, 1000);
    timeSlider_->setValue(0);
    timeSlider_->setTickPosition(QSlider::TicksBelow);
    timeSlider_->setTickInterval(100);
    dockLayout->addWidget(timeSlider_);

    // 刻度标签
    auto* tickRow = new QWidget(dockContent);
    auto* tickLayout = new QHBoxLayout(tickRow);
    tickLayout->setContentsMargins(0, 0, 0, 0);
    tickLayout->addWidget(new QLabel(QStringLiteral("0%"), tickRow));
    tickLayout->addStretch();
    tickLayout->addWidget(new QLabel(QStringLiteral("25%"), tickRow));
    tickLayout->addStretch();
    tickLayout->addWidget(new QLabel(QStringLiteral("50%"), tickRow));
    tickLayout->addStretch();
    tickLayout->addWidget(new QLabel(QStringLiteral("75%"), tickRow));
    tickLayout->addStretch();
    tickLayout->addWidget(new QLabel(QStringLiteral("100%"), tickRow));
    dockLayout->addWidget(tickRow);

    timeDock_->setWidget(dockContent);
    addDockWidget(Qt::BottomDockWidgetArea, timeDock_);
}

// ============================================================================
// 状态栏
// ============================================================================

void UiMainWindow::setupStatusBar()
{
    statusBar()->setObjectName(QStringLiteral("MainStatusBar"));

    statusReadyLabel_  = new QLabel(tr("就绪"), this);
    statusSceneLabel_  = new QLabel(tr("场景: 未加载"), this);
    statusObjectLabel_ = new QLabel(tr("对象: 0"), this);
    statusCoordLabel_  = new QLabel(tr("坐标系: J2000"), this);
    statusUnitLabel_   = new QLabel(tr("单位: km/s"), this);
    statusTimeLabel_   = new QLabel(tr("时间: UTCG"), this);

    statusBar()->addWidget(statusReadyLabel_);
    statusBar()->addWidget(statusSceneLabel_);
    statusBar()->addWidget(statusObjectLabel_);
    statusBar()->addWidget(statusCoordLabel_);
    statusBar()->addWidget(statusUnitLabel_);

    statusBar()->addPermanentWidget(statusTimeLabel_);
}

// ============================================================================
// 样式
// ============================================================================

void UiMainWindow::applyTheme(const QString& name)
{
    currentTheme_ = name;
    QString path = QCoreApplication::applicationDirPath()
                 + QStringLiteral("/data/style/") + name + QStringLiteral(".qss");
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        setStyleSheet(QString::fromUtf8(file.readAll()));
        file.close();
    }
}

AST_NAMESPACE_END
