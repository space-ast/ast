#include "UiSolarRadiationPressure.hpp"
#include "AstCore/SolarRadiationPressure.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstUtil/Unit.hpp"
#include <QGroupBox>

AST_NAMESPACE_BEGIN

UiSolarRadiationPressure::UiSolarRadiationPressure(Object* object, QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiSolarRadiationPressure");
    setupUi();
    if (object) {
        setObject(object);
        setSolarRadiationPressure(dynamic_cast<SolarRadiationPressure*>(object));
    }
}

UiSolarRadiationPressure::UiSolarRadiationPressure(QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiSolarRadiationPressure");
    setupUi();
}

void UiSolarRadiationPressure::setupUi()
{
    // 主布局
    mainLayout_ = new QVBoxLayout(this);
    
    // 模型配置
    modelGroup_ = new QGroupBox("模型", this);
    modelLayout_ = new QGridLayout(modelGroup_);
    
    typeLayout_ = new QHBoxLayout();
    typeLabel_ = new QLabel("类型:", this);
    typeCombo_ = new QComboBox(this);
    typeCombo_->addItem("球形");
    typeLayout_->addWidget(typeLabel_);
    typeLayout_->addWidget(typeCombo_);
    modelLayout_->addLayout(typeLayout_, 0, 0, 1, 2);
    
    crLayout_ = new QHBoxLayout();
    crLabel_ = new QLabel("Cr:", this);
    crEdit_ = new UiDouble(this);
    crEdit_->setValue(1.2);
    crLayout_->addWidget(crLabel_);
    crLayout_->addWidget(crEdit_);
    modelLayout_->addLayout(crLayout_, 1, 0);
    
    areaMassLayout_ = new QHBoxLayout();
    areaMassLabel_ = new QLabel("面积/质量:", this);
    areaMassEdit_ = new UiQuantity(this);
    areaMassEdit_->setValueInUnit(0.02, Unit("m^2/kg"));
    areaMassLayout_->addWidget(areaMassLabel_);
    areaMassLayout_->addWidget(areaMassEdit_);
    modelLayout_->addLayout(areaMassLayout_, 1, 1);
    
    // 阴影模型
    shadowGroup_ = new QGroupBox("阴影模型", this);
    shadowLayout_ = new QGridLayout(shadowGroup_);
    
    shadowModelLayout_ = new QHBoxLayout();
    shadowModelLabel_ = new QLabel("阴影模型:", this);
    shadowModelCombo_ = new QComboBox(this);
    shadowModelLayout_->addWidget(shadowModelLabel_);
    shadowModelLayout_->addWidget(shadowModelCombo_);
    shadowLayout_->addLayout(shadowModelLayout_, 0, 0, 1, 2);
    
    boundaryMitigationLayout_ = new QHBoxLayout();
    boundaryMitigationCheck_ = new QCheckBox("使用阴影边界缓解", this);
    boundaryMitigationLayout_->addWidget(boundaryMitigationCheck_);
    shadowLayout_->addLayout(boundaryMitigationLayout_, 1, 0, 1, 2);
    
    sunPositionLayout_ = new QHBoxLayout();
    sunPositionLabel_ = new QLabel("太阳位置计算方法:", this);
    sunPositionCombo_ = new QComboBox(this);
    sunPositionLayout_->addWidget(sunPositionLabel_);
    sunPositionLayout_->addWidget(sunPositionCombo_);
    shadowLayout_->addLayout(sunPositionLayout_, 2, 0, 1, 2);
    
    atmAltLayout_ = new QHBoxLayout();
    atmAltLabel_ = new QLabel("中心天体大气高度:", this);
    atmAltEdit_ = new UiQuantity(this);
    atmAltEdit_->setValueInUnit(0.0, Unit("km"));
    atmAltLayout_->addWidget(atmAltLabel_);
    atmAltLayout_->addWidget(atmAltEdit_);
    shadowLayout_->addLayout(atmAltLayout_, 3, 0, 1, 2);
    
    // 遮挡天体
    eclipsingGroup_ = new QGroupBox("遮挡天体", this);
    eclipsingLayout_ = new QHBoxLayout(eclipsingGroup_);
    
    availableLayout_ = new QVBoxLayout();
    availableLabel_ = new QLabel("可用", this);
    availableList_ = new QListWidget(this);
    availableLayout_->addWidget(availableLabel_);
    availableLayout_->addWidget(availableList_);
    
    buttonsLayout_ = new QVBoxLayout();
    addButton_ = new QPushButton(">>", this);
    removeButton_ = new QPushButton("<<", this);
    buttonsLayout_->addStretch();
    buttonsLayout_->addWidget(addButton_);
    buttonsLayout_->addWidget(removeButton_);
    buttonsLayout_->addStretch();
    
    assignedLayout_ = new QVBoxLayout();
    assignedLabel_ = new QLabel("已分配", this);
    assignedList_ = new QListWidget(this);
    assignedLayout_->addWidget(assignedLabel_);
    assignedLayout_->addWidget(assignedList_);
    
    eclipsingLayout_->addLayout(availableLayout_);
    eclipsingLayout_->addLayout(buttonsLayout_);
    eclipsingLayout_->addLayout(assignedLayout_);
    
    // 添加到主布局
    mainLayout_->addWidget(modelGroup_);
    mainLayout_->addWidget(shadowGroup_);
    mainLayout_->addWidget(eclipsingGroup_);
    mainLayout_->addStretch();
    
    // 连接信号槽
    connect(addButton_, &QPushButton::clicked, this, &UiSolarRadiationPressure::addEclipsingBody);
    connect(removeButton_, &QPushButton::clicked, this, &UiSolarRadiationPressure::removeEclipsingBody);
    
    // 刷新UI
    refreshShadowModel();
    refreshSunPosition();
    refreshCelestialBodies();
}

SolarRadiationPressure* UiSolarRadiationPressure::getSolarRadiationPressure() const
{
    return dynamic_cast<SolarRadiationPressure*>(getObject());
}

void UiSolarRadiationPressure::setSolarRadiationPressure(SolarRadiationPressure* srp)
{
    if (srp) {
        setObject(srp);
        refreshUi();
    }
}

void UiSolarRadiationPressure::refreshUi()
{
    auto srp = getSolarRadiationPressure();
    if (!srp) return;
    
    // 模型配置
    crEdit_->setValue(1.0); // 默认值
    areaMassEdit_->setValue(0.02); // 默认值
    
    // 阴影模型
    shadowModelCombo_->setCurrentIndex(static_cast<int>(srp->shadowModel_));
    boundaryMitigationCheck_->setChecked(srp->detectShadowBoundaries_);
    sunPositionCombo_->setCurrentIndex(static_cast<int>(srp->sunPosition_));
    atmAltEdit_->setValue(srp->atmAltForEclipse_);
    
    // 遮挡天体
    assignedList_->clear();
    for (const auto& body : srp->eclipsingBodies_) {
        if (body) {
            assignedList_->addItem(body->name().c_str());
        }
    }
}

void UiSolarRadiationPressure::apply()
{
    auto srp = getSolarRadiationPressure();
    if (srp) {
        applyTo(srp);
    }
}

void UiSolarRadiationPressure::applyTo(SolarRadiationPressure* srp)
{
    if (!srp) return;
    
    // 模型配置
    // 注意：这里需要根据实际的SolarRadiationPressure类结构添加相应的参数设置
    
    // 阴影模型
    srp->shadowModel_ = static_cast<EShadowModel>(shadowModelCombo_->currentIndex());
    srp->detectShadowBoundaries_ = boundaryMitigationCheck_->isChecked();
    srp->sunPosition_ = static_cast<ESunPosition>(sunPositionCombo_->currentIndex());
    srp->atmAltForEclipse_ = atmAltEdit_->value();
    
    // 遮挡天体
    // 注意：这里需要根据实际的CelestialBody类结构添加相应的处理
}

void UiSolarRadiationPressure::refreshShadowModel()
{
    shadowModelCombo_->clear();
    shadowModelCombo_->addItem("无");
    shadowModelCombo_->addItem("柱形");
    shadowModelCombo_->addItem("双锥");
}

void UiSolarRadiationPressure::refreshSunPosition()
{
    sunPositionCombo_->clear();
    sunPositionCombo_->addItem("真实太阳位置");
    sunPositionCombo_->addItem("视太阳到真实中心天体");
    sunPositionCombo_->addItem("视太阳位置");
}

void UiSolarRadiationPressure::refreshCelestialBodies()
{
    availableList_->clear();
    // 这里应该添加实际的天体列表，暂时添加一些示例
    availableList_->addItem("地球");
    availableList_->addItem("月球");
    availableList_->addItem("太阳");
    availableList_->addItem("水星");
    availableList_->addItem("金星");
    availableList_->addItem("火星");
    availableList_->addItem("木星");
    availableList_->addItem("土星");
}

void UiSolarRadiationPressure::addEclipsingBody()
{
    QListWidgetItem* item = availableList_->currentItem();
    if (item) {
        assignedList_->addItem(item->text());
        delete availableList_->takeItem(availableList_->row(item));
    }
}

void UiSolarRadiationPressure::removeEclipsingBody()
{
    QListWidgetItem* item = assignedList_->currentItem();
    if (item) {
        availableList_->addItem(item->text());
        delete assignedList_->takeItem(assignedList_->row(item));
    }
}

AST_NAMESPACE_END