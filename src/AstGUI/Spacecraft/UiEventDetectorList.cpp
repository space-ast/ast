///
/// @file      UiEventDetectorList.cpp
/// @brief     事件检测器列表管理组件实现
/// @author    axel
/// @date      2026-05-18
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

#include "UiEventDetectorList.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/EventDetector.hpp"
#include "AstCore/DetectorAllHeaders.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstGUI/UiEventDetectorEditor.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QCheckBox>
#include <QMenu>
#include <QPushButton>

AST_NAMESPACE_BEGIN

namespace {

struct DetectorTypeInfo {
    const char* className;
    const char* displayName;
};

const DetectorTypeInfo kDetectorTypes[] = {
    {"DetectorApoapsis",         "远地点"},
    {"DetectorPeriapsis",        "近地点"},
    {"DetectorAltitude",         "高度"},
    {"DetectorDuration",         "时长"},
    {"DetectorEpoch",            "历元"},
    {"DetectorTrueAnomaly",      "真近点角"},
    {"DetectorMeanAnomaly",      "平近点角"},
    {"DetectorAscendingNode",    "升交点"},
    {"DetectorDescendingNode",   "降交点"},
    {"DetectorCartesian",        "笛卡尔坐标"},
    {"DetectorDeltaV",           "速度增量 (ΔV)"},
    {"DetectorRMagnitude",       "地心距"},
    {"DetectorXYPlaneCross",     "XY 平面穿越"},
    {"DetectorYZPlaneCross",     "YZ 平面穿越"},
    {"DetectorZXPlaneCross",     "ZX 平面穿越"},
    {"DetectorUserSelect",       "用户自定义"},
    {"DetectorLighting",         "光照条件"},
    {"DetectorAlwaysTripped",    "始终触发"},
};

static EventDetector* createDetector(const std::string& name)
{
    if (name == "DetectorApoapsis")           return aNewObject<DetectorApoapsis>();
    if (name == "DetectorPeriapsis")          return aNewObject<DetectorPeriapsis>();
    if (name == "DetectorAltitude")           return aNewObject<DetectorAltitude>();
    if (name == "DetectorDuration")           return aNewObject<DetectorDuration>();
    if (name == "DetectorEpoch")              return aNewObject<DetectorEpoch>();
    if (name == "DetectorTrueAnomaly")        return aNewObject<DetectorTrueAnomaly>();
    if (name == "DetectorMeanAnomaly")        return aNewObject<DetectorMeanAnomaly>();
    if (name == "DetectorAscendingNode")      return aNewObject<DetectorAscendingNode>();
    if (name == "DetectorDescendingNode")     return aNewObject<DetectorDescendingNode>();
    if (name == "DetectorCartesian")          return aNewObject<DetectorCartesian>();
    if (name == "DetectorDeltaV")             return aNewObject<DetectorDeltaV>();
    if (name == "DetectorRMagnitude")         return aNewObject<DetectorRMagnitude>();
    if (name == "DetectorXYPlaneCross")       return aNewObject<DetectorXYPlaneCross>();
    if (name == "DetectorYZPlaneCross")       return aNewObject<DetectorYZPlaneCross>();
    if (name == "DetectorZXPlaneCross")       return aNewObject<DetectorZXPlaneCross>();
    if (name == "DetectorUserSelect")         return aNewObject<DetectorUserSelect>();
    if (name == "DetectorLighting")           return aNewObject<DetectorLighting>();
    if (name == "DetectorAlwaysTripped")      return aNewObject<DetectorAlwaysTripped>();
    return nullptr;
}

static const int kColActive = 0;
static const int kColName  = 1;
static const int kColType  = 2;

} // namespace

UiEventDetectorList::UiEventDetectorList(Object* object, QWidget* parent)
    : UiEventDetectorList(parent)
{
    setPropagate(aobject_cast<Propagate*>(object));
}

UiEventDetectorList::UiEventDetectorList(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiEventDetectorList::~UiEventDetectorList() = default;

void UiEventDetectorList::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 表格
    table_ = new QTableWidget(0, 3, this);
    table_->setAlternatingRowColors(true);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);

    // 表头
    QStringList headers;
    headers << tr("激活") << tr("名称") << tr("类型");
    table_->setHorizontalHeaderLabels(headers);
    table_->setColumnWidth(kColActive, 70);
    table_->setColumnWidth(kColName, 240);
    table_->setColumnWidth(kColType, 120);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->verticalHeader()->setVisible(false);

    layout->addWidget(table_);

    // 按钮栏
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 4, 0, 0);

    addBtn_ = new QPushButton(tr("+ 添加"), this);
    auto* addMenu = new QMenu(this);
    for (const auto& info : kDetectorTypes)
        addMenu->addAction(QString::fromUtf8(info.displayName))->setData(QString::fromUtf8(info.className));
    addBtn_->setMenu(addMenu);
    btnLayout->addWidget(addBtn_);

    removeBtn_ = new QPushButton(tr("- 删除"), this);
    removeBtn_->setEnabled(false);
    btnLayout->addWidget(removeBtn_);

    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    // 编辑器
    editor_ = new UiEventDetectorEditor(this);
    layout->addWidget(editor_);

    // 连接
    connect(addMenu, &QMenu::triggered, this, [this](QAction* action) {
        onAddDetector(action->data().toString());
    });
    connect(removeBtn_, &QPushButton::clicked, this, &UiEventDetectorList::onRemoveDetector);
    connect(table_, &QTableWidget::itemSelectionChanged, this, [this]() {
        int row = table_->currentRow();
        removeBtn_->setEnabled(row >= 0);

        if (row >= 0)
        {
            auto* prop = getPropagate();
            if (prop)
            {
                const auto& detectors = prop->eventDetectors();
                if (row < (int)detectors.size())
                {
                    auto* det = detectors[row].get();
                    editor_->setDetector(det);
                    emit detectorSelected(det);
                }
            }
        }
        else
        {
            editor_->clear();
        }
    });
    connect(table_, &QTableWidget::cellChanged, this, &UiEventDetectorList::onNameChanged);
}

// ============================================================================
// 公共接口
// ============================================================================

void UiEventDetectorList::setPropagate(Propagate* prop)
{
    if (!prop)
        return;
    setObject(prop);
    refreshList();
}

Propagate* UiEventDetectorList::getPropagate() const
{
    return aobject_cast<Propagate*>(getObject());
}

// ============================================================================
// 槽
// ============================================================================

void UiEventDetectorList::onAddDetector(const QString& className)
{
    auto* prop = getPropagate();
    if (!prop)
        return;

    std::string clsName = className.toUtf8().constData();
    auto* det = createDetector(clsName);
    if (!det)
        return;

    det->setName(clsName);

    auto detectors = prop->eventDetectors();
    detectors.push_back(SharedPtr<EventDetector>(det));
    prop->setEventDetectors(detectors);

    refreshList();
}

void UiEventDetectorList::onRemoveDetector()
{
    auto* prop = getPropagate();
    if (!prop)
        return;

    int row = table_->currentRow();
    if (row < 0)
        return;

    auto detectors = prop->eventDetectors();
    if (row < (int)detectors.size())
    {
        detectors.erase(detectors.begin() + row);
        prop->setEventDetectors(detectors);
    }

    refreshList();
}

void UiEventDetectorList::onActiveToggled(int row, bool checked)
{
    auto* prop = getPropagate();
    if (!prop)
        return;

    const auto& detectors = prop->eventDetectors();
    if (row < (int)detectors.size() && detectors[row])
        detectors[row]->setActive(checked);
}

void UiEventDetectorList::onNameChanged(int row, int col)
{
    if (col != kColName)
        return;

    auto* prop = getPropagate();
    if (!prop)
        return;

    const auto& detectors = prop->eventDetectors();
    if (row < (int)detectors.size() && detectors[row])
    {
        auto* item = table_->item(row, col);
        if (item)
            detectors[row]->setName(item->text().toUtf8().constData());
    }
}

// ============================================================================
// 内部
// ============================================================================

void UiEventDetectorList::refreshList()
{
    auto* prop = getPropagate();

    // 断开 cellChanged 避免 refresh 时触发写入
    table_->blockSignals(true);
    table_->setRowCount(0);

    if (!prop)
    {
        table_->blockSignals(false);
        return;
    }

    const auto& detectors = prop->eventDetectors();
    table_->setRowCount((int)detectors.size());

    for (int i = 0; i < (int)detectors.size(); ++i)
    {
        auto* det = detectors[i].get();
        if (!det)
            continue;

        // 激活列 — QCheckBox（居中）
        auto* cb = new QCheckBox();
        cb->setChecked(det->active());
        auto* cbContainer = new QWidget();
        auto* cbLayout = new QHBoxLayout(cbContainer);
        cbLayout->setContentsMargins(0, 0, 0, 0);
        cbLayout->setAlignment(Qt::AlignCenter);
        cbLayout->addWidget(cb);
        table_->setCellWidget(i, kColActive, cbContainer);
        connect(cb, &QCheckBox::toggled, this, [this, i](bool checked) {
            onActiveToggled(i, checked);
        });

        // 名称列
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(det->getName()));
        table_->setItem(i, kColName, nameItem);

        // 类型列（只读）
        auto* typeItem = new QTableWidgetItem(typeDisplayName(det));
        typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);
        table_->setItem(i, kColType, typeItem);
    }

    table_->blockSignals(false);
}

QString UiEventDetectorList::typeDisplayName(EventDetector* det) const
{
    if (!det)
        return {};

    std::string typeName = det->getType()->name();
    for (const auto& info : kDetectorTypes)
    {
        if (typeName == info.className)
            return QString::fromUtf8(info.displayName);
    }
    return QString::fromStdString(typeName);
}

AST_NAMESPACE_END
