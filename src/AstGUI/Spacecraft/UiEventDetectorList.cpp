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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
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

    // 列表
    listWidget_ = new QListWidget(this);
    listWidget_->setMaximumHeight(140);
    listWidget_->setAlternatingRowColors(true);
    layout->addWidget(listWidget_);

    // 按钮栏
    auto* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    addBtn_ = new QPushButton(tr("+ 添加"), this);
    auto* addMenu = new QMenu(this);
    for (const auto& info : kDetectorTypes)
        addMenu->addAction(QString::fromUtf8(info.displayName))->setData(QString::fromUtf8(info.className));
    addBtn_->setMenu(addMenu);
    btnLayout->addWidget(addBtn_);

    removeBtn_ = new QPushButton(tr("- 删除"), this);
    removeBtn_->setEnabled(false);
    btnLayout->addWidget(removeBtn_);

    layout->addLayout(btnLayout);

    // 连接
    connect(addMenu, &QMenu::triggered, this, [this](QAction* action) {
        onAddDetector(action->data().toString());
    });
    connect(removeBtn_, &QPushButton::clicked, this, &UiEventDetectorList::onRemoveDetector);
    connect(listWidget_, &QListWidget::itemSelectionChanged, this, [this]() {
        removeBtn_->setEnabled(listWidget_->currentItem() != nullptr);
    });
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

    // 通过 RTTI 类型名创建实例
    EventDetector* det = nullptr;
    std::string name = className.toUtf8().constData();

    if (name == "DetectorApoapsis")           det = aNewObject<DetectorApoapsis>();
    else if (name == "DetectorPeriapsis")      det = aNewObject<DetectorPeriapsis>();
    else if (name == "DetectorAltitude")       det = aNewObject<DetectorAltitude>();
    else if (name == "DetectorDuration")       det = aNewObject<DetectorDuration>();
    else if (name == "DetectorEpoch")          det = aNewObject<DetectorEpoch>();
    else if (name == "DetectorTrueAnomaly")    det = aNewObject<DetectorTrueAnomaly>();
    else if (name == "DetectorMeanAnomaly")    det = aNewObject<DetectorMeanAnomaly>();
    else if (name == "DetectorAscendingNode")  det = aNewObject<DetectorAscendingNode>();
    else if (name == "DetectorDescendingNode") det = aNewObject<DetectorDescendingNode>();
    else if (name == "DetectorCartesian")      det = aNewObject<DetectorCartesian>();
    else if (name == "DetectorDeltaV")         det = aNewObject<DetectorDeltaV>();
    else if (name == "DetectorRMagnitude")     det = aNewObject<DetectorRMagnitude>();
    else if (name == "DetectorXYPlaneCross")   det = aNewObject<DetectorXYPlaneCross>();
    else if (name == "DetectorYZPlaneCross")   det = aNewObject<DetectorYZPlaneCross>();
    else if (name == "DetectorZXPlaneCross")   det = aNewObject<DetectorZXPlaneCross>();
    else if (name == "DetectorUserSelect")     det = aNewObject<DetectorUserSelect>();
    else if (name == "DetectorLighting")       det = aNewObject<DetectorLighting>();
    else if (name == "DetectorAlwaysTripped")  det = aNewObject<DetectorAlwaysTripped>();

    if (!det)
        return;

    det->setName(name);

    // 即时写入 Propagate
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

    int row = listWidget_->currentRow();
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

// ============================================================================
// 内部
// ============================================================================

void UiEventDetectorList::refreshList()
{
    auto* prop = getPropagate();
    listWidget_->clear();

    if (!prop)
        return;

    for (const auto& det : prop->eventDetectors())
    {
        if (!det)
            continue;
        listWidget_->addItem(detectorDisplayName(det.get()));
    }
}

QString UiEventDetectorList::detectorDisplayName(EventDetector* det) const
{
    if (!det)
        return {};

    // 取 RTTI 类型名，匹配中文名
    std::string typeName = det->getType()->name();
    for (const auto& info : kDetectorTypes)
    {
        if (typeName == info.className)
            return QString::fromUtf8(info.displayName);
    }
    return QString::fromStdString(typeName);
}

AST_NAMESPACE_END
