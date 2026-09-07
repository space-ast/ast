///
/// @file      UiInsertObjectDialog.cpp
/// @brief     插入对象对话框实现
/// @details   ~
/// @author    axel
/// @date      2026-09-07
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

#include "UiInsertObjectDialog.hpp"
#include "AstGUI/UiCommon.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QAbstractItemView>
#include <QSignalBlocker>
#include <QIcon>
#include <QSizePolicy>
#include <QPalette>
#include <QColor>

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 对象类型条目：显示名 + data/icons/ 下的图标名
struct ObjectTypeItem
{
    const char* label;  ///< 显示名（如 "Satellite"）
    const char* icon;   ///< 图标名（如 "Satellite"）
};

/// @brief 插入方法条目：显示名
struct MethodItem
{
    const char* label;
};

/// @brief 中心天体列表：显示中文名，底层保留英文标识（与 ast 内部查询键一致）
struct CentralBodyItem
{
    const char* cn;  ///< 中文显示名
    const char* id;  ///< 英文标识（如 "Earth"）
};

const CentralBodyItem kCentralBodies[] = {
    { "地球", "Earth" }, { "月球", "Moon" }, { "太阳", "Sun" },
    { "水星", "Mercury" }, { "金星", "Venus" }, { "火星", "Mars" },
    { "木星", "Jupiter" }, { "土星", "Saturn" }, { "天王星", "Uranus" },
    { "海王星", "Neptune" }, { "冥王星", "Pluto" }
};

/// @brief 场景对象（可插入到场景顶层的对象类型）
const ObjectTypeItem kScenarioObjects[] = {
    { "Satellite",      "Satellite" },
    { "Ground Station", "GroundStation" }
};

/// @brief 附加对象（依附于其他对象，如传感器）
const ObjectTypeItem kAttachedObjects[] = {
    { "Sensor", "Sensor" }
};

/// @brief 插入方法列表
const MethodItem kMethods[] = {
    { "Insert Default" }
};

/// @brief 加载对象/方法图标，缺失时回退到通用 "Object" 图标
QIcon loadIcon(const char* name)
{
    QIcon icon = aUiIcon(QString::fromUtf8(name));
    if (icon.isNull())
        icon = aUiIcon(QStringLiteral("Object"));
    return icon;
}

} // namespace

// ============================================================================
// 构造 / 析构
// ============================================================================

UiInsertObjectDialog::UiInsertObjectDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("插入对象"));
    resize(640, 460);

    setupUi();
    setupConnections();
}

UiInsertObjectDialog::~UiInsertObjectDialog() = default;

// ============================================================================
// 公共接口
// ============================================================================

QString UiInsertObjectDialog::selectedObjectType() const
{
    return currentObjectType_;
}

QString UiInsertObjectDialog::selectedMethod() const
{
    if (methodList_ && methodList_->currentItem())
        return methodList_->currentItem()->text();
    return QString();
}

QString UiInsertObjectDialog::centralBody() const
{
    return centralBodyCombo_->currentData().toString();
}

// ============================================================================
// 界面搭建
// ============================================================================

void UiInsertObjectDialog::setupUi()
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(12, 12, 12, 12);
    rootLayout->setSpacing(8);

    // ——— 顶部：左右两栏 ———
    auto* mainRow = new QHBoxLayout();
    mainRow->setSpacing(12);

    // —— 左栏：对象类型网格（每行两个条目）——
    auto* leftCol = new QVBoxLayout();
    leftCol->setSpacing(4);
    leftCol->addWidget(new QLabel(tr("选择要插入的对象:"), this));

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* leftContent = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftContent);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(4);

    // 生成对象表格：每行两个条目（QTableWidget 自带白色列表背景，无需额外样式表）
    auto makeObjectTable = [this](QWidget* parent, const ObjectTypeItem* items, int count) {
        const int rows = (count + 1) / 2;  // 每行两条目，向上取整行数
        auto* table = new QTableWidget(rows, 2, parent);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setSelectionBehavior(QAbstractItemView::SelectItems);
        table->setShowGrid(false);
        table->setWordWrap(false);
        table->setFocusPolicy(Qt::NoFocus);
        table->setFrameShape(QFrame::NoFrame);
        table->setIconSize(QSize(16, 16));
        table->setCornerButtonEnabled(false);
        table->horizontalHeader()->hide();
        table->verticalHeader()->hide();
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        // 让表格在父容器中自动撑满（水平、垂直都扩展）
        table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // 设置选中高亮色（通过调色板，避免样式表）；淡蓝高亮、文字保持原色
        QPalette tablePal = table->palette();
        tablePal.setColor(QPalette::Highlight, QColor(0xcd, 0xe6, 0xf7));
        table->setPalette(tablePal);

        const int cellCount = rows * 2;
        for (int i = 0; i < cellCount; ++i)
        {
            if (i < count)
            {
                const ObjectTypeItem& item = items[i];
                auto* cell = new QTableWidgetItem(loadIcon(item.icon),
                                                  QString::fromUtf8(item.label));
                table->setItem(i / 2, i % 2, cell);
            }
            else
            {
                // 空出来的单元格设为不可选，避免误点
                auto* cell = new QTableWidgetItem;
                cell->setFlags(Qt::NoItemFlags);
                table->setItem(i / 2, i % 2, cell);
            }
        }
        return table;
    };

    // 场景对象
    leftLayout->addWidget(new QLabel(tr("场景对象"), leftContent));
    objectTable_ = makeObjectTable(leftContent, kScenarioObjects,
                                   static_cast<int>(sizeof(kScenarioObjects) / sizeof(kScenarioObjects[0])));
    connect(objectTable_, &QTableWidget::itemSelectionChanged,
            this, &UiInsertObjectDialog::onObjectTableSelectionChanged);
    leftLayout->addWidget(objectTable_, 2);

    // 附加对象
    leftLayout->addWidget(new QLabel(tr("附加对象"), leftContent));
    attachedTable_ = makeObjectTable(leftContent, kAttachedObjects,
                                     static_cast<int>(sizeof(kAttachedObjects) / sizeof(kAttachedObjects[0])));
    connect(attachedTable_, &QTableWidget::itemSelectionChanged,
            this, &UiInsertObjectDialog::onObjectTableSelectionChanged);
    leftLayout->addWidget(attachedTable_, 1);

    scroll->setWidget(leftContent);
    leftCol->addWidget(scroll, 1);

    // 左栏底部：中心天体
    auto* cbRow = new QHBoxLayout();
    cbRow->setSpacing(8);
    cbRow->addWidget(new QLabel(tr("中心天体:"), this));
    centralBodyCombo_ = new QComboBox(this);
    for (const auto& item : kCentralBodies)
        centralBodyCombo_->addItem(QString::fromUtf8(item.cn), QString::fromUtf8(item.id));
    int earthIdx = centralBodyCombo_->findData(QStringLiteral("Earth"));
    if (earthIdx >= 0)
        centralBodyCombo_->setCurrentIndex(earthIdx);
    cbRow->addWidget(centralBodyCombo_, 1);
    leftCol->addLayout(cbRow);

    mainRow->addLayout(leftCol, 3);

    // —— 右栏：插入方法列表 ——
    auto* rightCol = new QVBoxLayout();
    rightCol->setSpacing(4);
    rightCol->addWidget(new QLabel(tr("选择插入方法:"), this));
    methodList_ = new QListWidget(this);
    methodList_->setSelectionMode(QAbstractItemView::SingleSelection);
    for (const auto& item : kMethods)
    {
        auto* listItem = new QListWidgetItem(QString::fromUtf8(item.label));
        methodList_->addItem(listItem);
    }
    rightCol->addWidget(methodList_, 1);

    mainRow->addLayout(rightCol, 2);

    rootLayout->addLayout(mainRow, 1);

    // ——— 分隔线 ———
    auto* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    rootLayout->addWidget(line);

    // ——— 底部按钮行 ———
    auto* bottom = new QHBoxLayout();
    bottom->setSpacing(8);
    bottom->addStretch(1);

    insertButton_ = new QPushButton(tr("插入..."), this);
    closeButton_ = new QPushButton(tr("关闭"), this);
    insertButton_->setDefault(true);
    bottom->addWidget(insertButton_);
    bottom->addWidget(closeButton_);
    rootLayout->addLayout(bottom);
}

// ============================================================================
// 信号连接与状态
// ============================================================================

void UiInsertObjectDialog::setupConnections()
{
    connect(methodList_, &QListWidget::itemSelectionChanged, this, [this]() {
        insertButton_->setEnabled(!currentObjectType_.isEmpty() && !selectedMethod().isEmpty());
    });

    connect(closeButton_, &QPushButton::clicked, this, &QDialog::reject);
    connect(insertButton_, &QPushButton::clicked, this, &UiInsertObjectDialog::onInsertClicked);

    // 默认选中：场景对象中的 "Satellite" 以及首个插入方法，使插入按钮立即可用
    selectObjectByType(QStringLiteral("Satellite"));
    if (methodList_->count() > 0)
        methodList_->setCurrentItem(methodList_->item(0));

    insertButton_->setEnabled(!currentObjectType_.isEmpty() && !selectedMethod().isEmpty());
}

// ============================================================================
// 对象选择联动
// ============================================================================

void UiInsertObjectDialog::onObjectTableSelectionChanged()
{
    // 两个对象表格之间保持单选：选中某格时清空另一表格的选中
    auto* sender = qobject_cast<QTableWidget*>(QObject::sender());
    QTableWidget* other = nullptr;
    if (sender == objectTable_)
        other = attachedTable_;
    else if (sender == attachedTable_)
        other = objectTable_;

    if (other && other->currentItem())
    {
        // 屏蔽信号，避免触发另一表格的 itemSelectionChanged 造成递归清空
        const QSignalBlocker blocker(other);
        other->clearSelection();
        other->setCurrentItem(nullptr);
    }

    QTableWidget* table = sender;
    currentObjectType_ = (table && table->currentItem()) ? table->currentItem()->text() : QString();
    insertButton_->setEnabled(!currentObjectType_.isEmpty() && !selectedMethod().isEmpty());
}

void UiInsertObjectDialog::selectObjectByType(const QString& type)
{
    if (!objectTable_)
        return;

    for (int r = 0; r < objectTable_->rowCount(); ++r)
    {
        for (int c = 0; c < objectTable_->columnCount(); ++c)
        {
            QTableWidgetItem* it = objectTable_->item(r, c);
            if (it && it->text() == type && (it->flags() & Qt::ItemIsSelectable))
            {
                const QSignalBlocker blocker(objectTable_);
                objectTable_->setCurrentItem(it);
                currentObjectType_ = type;
                return;
            }
        }
    }
}

// ============================================================================
// 按钮行为
// ============================================================================

void UiInsertObjectDialog::onInsertClicked()
{
    if (currentObjectType_.isEmpty() || selectedMethod().isEmpty())
        return;

    emit objectInsertRequested(currentObjectType_, selectedMethod(), centralBody());
    accept();
}

AST_NAMESPACE_END
