///
/// @file      UiStartPage.cpp
/// @brief     SpaceAST 起始页
/// @details   提供启动后的新建、示例和学习入口
/// @author    axel
/// @date      2026-05-27
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

#include "UiStartPage.hpp"
#include "AstGUI/ObjectIcons.hpp"

#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStyle>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>

AST_NAMESPACE_BEGIN

namespace
{
struct StartCardSpec
{
    const char* id;
    const char* title;
    const char* description;
    const char* iconName;
    QStyle::StandardPixmap fallbackIcon;
};

QString utf8(const char* text)
{
    return QString::fromUtf8(text);
}
}

UiStartPage::UiStartPage(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

UiStartPage::~UiStartPage() = default;

void UiStartPage::setupUi()
{
    setObjectName(QStringLiteral("StartPage"));

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(createSidebar());
    layout->addWidget(createMainArea(), 1);
}

QWidget* UiStartPage::createSidebar()
{
    auto* sidebar = new QFrame(this);
    sidebar->setObjectName(QStringLiteral("StartPageSidebar"));
    sidebar->setFixedWidth(240);

    auto* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(22, 18, 14, 12);
    layout->setSpacing(8);

    auto* logo = new QLabel(QStringLiteral("SpaceAST"), sidebar);
    logo->setObjectName(QStringLiteral("StartPageLogo"));
    layout->addWidget(logo);

    auto* subtitle = new QLabel(tr("航天任务分析与仿真平台"), sidebar);
    subtitle->setObjectName(QStringLiteral("StartPageSubtitle"));
    layout->addWidget(subtitle);

    layout->addSpacing(18);

    auto* newButton = createSidebarButton(tr("新建任务"), QStringLiteral("Object"), QStyle::SP_FileIcon);
    auto* openButton = createSidebarButton(tr("打开任务"), QString(), QStyle::SP_DialogOpenButton);
    auto* exampleButton = createSidebarButton(tr("打开示例"), QStringLiteral("Sequence"), QStyle::SP_DirOpenIcon);
    layout->addWidget(newButton);
    layout->addWidget(openButton);
    layout->addWidget(exampleButton);

    connect(newButton, &QToolButton::clicked, this, &UiStartPage::newTaskRequested);
    connect(openButton, &QToolButton::clicked, this, &UiStartPage::openTaskRequested);
    connect(exampleButton, &QToolButton::clicked, this, [this]() {
        emit exampleRequested(QStringLiteral("examples"));
    });

    layout->addSpacing(14);

    auto addTitle = [sidebar, layout](const QString& text) {
        auto* label = new QLabel(text, sidebar);
        label->setObjectName(QStringLiteral("StartPageSectionTitle"));
        layout->addWidget(label);
    };

    addTitle(tr("最近项目"));
    const char* recentItems[] = {"LEO_Orbit.ast", "LunarTransfer.ast", "CoverageDemo.ast"};
    for (const char* item : recentItems)
    {
        auto* button = createSidebarButton(QString::fromLatin1(item), QStringLiteral("OrbitState"), QStyle::SP_FileIcon);
        button->setObjectName(QStringLiteral("StartPageLinkButton"));
        layout->addWidget(button);
        connect(button, &QToolButton::clicked, this, [this, item]() {
            emit cardActivated(QString::fromLatin1(item));
        });
    }

    layout->addSpacing(12);
    addTitle(tr("社区 / 学习"));

    const char* links[] = {u8"文档中心", u8"示例库", u8"课程学习", u8"反馈建议"};
    for (const char* link : links)
    {
        auto* button = createSidebarButton(utf8(link), QString(), QStyle::SP_DialogHelpButton);
        button->setObjectName(QStringLiteral("StartPageLinkButton"));
        layout->addWidget(button);
        connect(button, &QToolButton::clicked, this, [this, link]() {
            emit cardActivated(utf8(link));
        });
    }

    layout->addStretch();

    auto* startupCheck = new QCheckBox(tr("启动时显示此页面"), sidebar);
    startupCheck->setObjectName(QStringLiteral("StartPageStartupCheck"));
    startupCheck->setChecked(true);
    layout->addWidget(startupCheck);

    return sidebar;
}

QWidget* UiStartPage::createMainArea()
{
    auto* main = new QFrame(this);
    main->setObjectName(QStringLiteral("StartPageMainArea"));

    auto* layout = new QVBoxLayout(main);
    layout->setContentsMargins(18, 14, 18, 18);
    layout->setSpacing(0);

    auto* tabs = new QTabWidget(main);
    tabs->setObjectName(QStringLiteral("StartPageTabWidget"));
    tabs->addTab(createNewTab(), tr("新建"));
    tabs->addTab(createExampleTab(), tr("示例"));
    tabs->addTab(createLearningTab(), tr("学习"));
    layout->addWidget(tabs);

    return main;
}

QWidget* UiStartPage::createNewTab()
{
    auto* content = new QWidget();
    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 14, 0, 0);
    layout->setSpacing(18);

    const StartCardSpec spaceAstCards[] = {
        {"scene",       u8"场景",     u8"创建新的任务场景",       "Object",      QStyle::SP_FileIcon},
        {"spacecraft",  u8"航天器",   u8"配置平台、质量和载荷",   "Spacecraft",  QStyle::SP_ComputerIcon},
        {"orbit",       u8"轨道状态", u8"定义初始轨道与坐标系",   "OrbitState",  QStyle::SP_ComputerIcon},
        {"maneuver",    u8"机动",     u8"添加脉冲或有限推力机动", "Maneuver",    QStyle::SP_ArrowForward},
        {"sequence",    u8"序列",     u8"编排任务段和执行流程",   "Sequence",    QStyle::SP_FileDialogDetailedView},
        {"propagate",   u8"传播分析", u8"设置传播器与力模型",     "Propagate",   QStyle::SP_MediaPlay},
        {"coverage",    u8"覆盖分析", u8"评估目标可见性与覆盖",   "Sensor",      QStyle::SP_ComputerIcon},
        {"report",      u8"报告",     u8"生成结果表格和分析报告", "",           QStyle::SP_FileDialogDetailedView}
    };

    const StartCardSpec sysblockCards[] = {
        {"dynamics",    u8"动力学模块", u8"搭建动力学计算模块", "Force",    QStyle::SP_ComputerIcon},
        {"control",     u8"控制系统",   u8"创建控制律与执行器",   "Vector",   QStyle::SP_CommandLink},
        {"stateMachine", u8"状态机",     u8"描述任务状态转换",     "Sequence", QStyle::SP_FileDialogListView},
        {"workflow",    u8"仿真流程",   u8"组织仿真执行链路",     "Segment",  QStyle::SP_FileDialogContentsView}
    };

    QVector<QWidget*> spaceAstWidgets;
    for (const auto& card : spaceAstCards)
        spaceAstWidgets.push_back(createCard(QString::fromLatin1(card.id), utf8(card.iconName), card.fallbackIcon, utf8(card.title), utf8(card.description)));
    layout->addWidget(createSection(QStringLiteral("SpaceAST"), spaceAstWidgets));

    QVector<QWidget*> sysblockWidgets;
    for (const auto& card : sysblockCards)
        sysblockWidgets.push_back(createCard(QString::fromLatin1(card.id), utf8(card.iconName), card.fallbackIcon, utf8(card.title), utf8(card.description)));
    layout->addWidget(createSection(QStringLiteral("Sysblock"), sysblockWidgets));

    layout->addStretch();
    return createScrollPage(content);
}

QWidget* UiStartPage::createExampleTab()
{
    auto* content = new QWidget();
    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 14, 0, 0);

    const StartCardSpec cards[] = {
        {"leoExample",      u8"近地轨道卫星", u8"快速打开 LEO 传播示例",       "Satellite", QStyle::SP_ComputerIcon},
        {"lunarTransfer",  u8"地月转移任务", u8"查看转移轨道和中途修正",     "OrbitState", QStyle::SP_ComputerIcon},
        {"coverageExample",u8"覆盖分析",     u8"学习区域覆盖和可见性分析",   "Sensor", QStyle::SP_ComputerIcon},
        {"formation",      u8"编队飞行",     u8"查看相对运动与编队保持",     "Spacecraft", QStyle::SP_ComputerIcon},
        {"reentry",        u8"再入分析",     u8"体验再入轨迹和热环境分析",   "Body", QStyle::SP_ComputerIcon}
    };

    QVector<QWidget*> widgets;
    for (const auto& card : cards)
        widgets.push_back(createCard(QString::fromLatin1(card.id), utf8(card.iconName), card.fallbackIcon, utf8(card.title), utf8(card.description)));
    layout->addWidget(createSection(tr("任务示例"), widgets));
    layout->addStretch();
    return createScrollPage(content);
}

QWidget* UiStartPage::createLearningTab()
{
    auto* content = new QWidget();
    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 14, 0, 0);

    const StartCardSpec cards[] = {
        {"quickStart", u8"快速入门",     u8"了解 SpaceAST 的基本工作流",   "Object", QStyle::SP_DialogHelpButton},
        {"modeling",   u8"任务建模教程", u8"学习对象、任务段和约束建模",   "Sequence", QStyle::SP_FileDialogDetailedView},
        {"propagation",u8"轨道传播说明", u8"理解传播器、力模型和步长设置", "Propagate", QStyle::SP_MediaPlay},
        {"script",     u8"脚本自动化",   u8"使用脚本批量构建和运行任务",   "", QStyle::SP_CommandLink},
        {"analysis",   u8"结果分析",     u8"查看报告、图表和参数扫描结果", "", QStyle::SP_FileDialogContentsView}
    };

    QVector<QWidget*> widgets;
    for (const auto& card : cards)
        widgets.push_back(createCard(QString::fromLatin1(card.id), utf8(card.iconName), card.fallbackIcon, utf8(card.title), utf8(card.description)));
    layout->addWidget(createSection(tr("学习资源"), widgets));
    layout->addStretch();
    return createScrollPage(content);
}

QWidget* UiStartPage::createScrollPage(QWidget* content)
{
    auto* scroll = new QScrollArea();
    scroll->setWidget(content);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    return scroll;
}

QWidget* UiStartPage::createSection(const QString& title, const QVector<QWidget*>& cards)
{
    auto* section = new QWidget();
    auto* layout = new QVBoxLayout(section);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    auto* titleLabel = new QLabel(title, section);
    titleLabel->setObjectName(QStringLiteral("StartPageSectionTitle"));
    layout->addWidget(titleLabel);

    auto* gridWidget = new QWidget(section);
    auto* grid = new QGridLayout(gridWidget);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(14);

    const int columns = 4;
    for (int i = 0; i < cards.size(); ++i)
        grid->addWidget(cards.at(i), i / columns, i % columns);
    for (int column = 0; column < columns; ++column)
        grid->setColumnStretch(column, 1);

    layout->addWidget(gridWidget);
    return section;
}

QWidget* UiStartPage::createCard(const QString& id, const QString& iconName, QStyle::StandardPixmap fallbackIcon,
                                 const QString& title, const QString& description)
{
    auto* button = new QToolButton();
    button->setObjectName(QStringLiteral("StartPageCard"));
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIconSize(QSize(42, 42));
    button->setMinimumSize(170, 116);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setText(title + QStringLiteral("\n") + description);
    button->setToolTip(description);

    QIcon icon;
    if (!iconName.isEmpty())
        icon = loadIcon(iconName);
    if (icon.isNull())
        icon = style()->standardIcon(fallbackIcon);
    button->setIcon(icon);

    connect(button, &QToolButton::clicked, this, [this, id]() {
        emit cardActivated(id);
    });

    return button;
}

QToolButton* UiStartPage::createSidebarButton(const QString& text, const QString& iconName,
                                              QStyle::StandardPixmap fallbackIcon)
{
    auto* button = new QToolButton();
    button->setObjectName(QStringLiteral("StartPageActionButton"));
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setIconSize(QSize(16, 16));
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setText(text);

    QIcon icon;
    if (!iconName.isEmpty())
        icon = loadIcon(iconName);
    if (icon.isNull())
        icon = style()->standardIcon(fallbackIcon);
    button->setIcon(icon);

    return button;
}

AST_NAMESPACE_END
