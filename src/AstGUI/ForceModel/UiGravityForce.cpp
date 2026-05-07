#include "UiGravityForce.hpp"
#include "AstCore/GravityForce.hpp"
#include <QGroupBox>

AST_NAMESPACE_BEGIN

UiGravityForce::UiGravityForce(Object* object, QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiGravityForce");
    setupUi();
    if (object) {
        setObject(object);
        setGravityForce(dynamic_cast<GravityForce*>(object));
    }
}

UiGravityForce::UiGravityForce(QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiGravityForce");
    setupUi();
}

void UiGravityForce::setupUi()
{
    // 主布局
    mainLayout_ = new QVBoxLayout(this);
    
    // 中心天体重力场配置
    centralBodyGravityGroup_ = new QGroupBox("中心天体重力场", this);
    centralBodyGravityLayout_ = new QGridLayout(centralBodyGravityGroup_);
    
    modelLayout_ = new QHBoxLayout();
    modelLabel_ = new QLabel("模型:", this);
    modelCombo_ = new QComboBox(this);
    modelFileEdit_ = new UiFilePath(this);
    // modelFileEdit_->setFileMode(UiFilePath::FileMode::OpenFile);
    modelFileEdit_->setFilter("Gravity files (*.grv);;All files (*.*)");
    modelLayout_->addWidget(modelLabel_);
    modelLayout_->addWidget(modelCombo_);
    modelLayout_->addWidget(modelFileEdit_);
    centralBodyGravityLayout_->addLayout(modelLayout_, 0, 0, 1, 2);
    
    degreeLayout_ = new QHBoxLayout();
    degreeLabel_ = new QLabel("阶数:", this);
    degreeEdit_ = new UiInteger(this);
    degreeEdit_->setValue(2);
    degreeLayout_->addWidget(degreeLabel_);
    degreeLayout_->addWidget(degreeEdit_);
    centralBodyGravityLayout_->addLayout(degreeLayout_, 1, 0);
    
    orderLayout_ = new QHBoxLayout();
    orderLabel_ = new QLabel("次数:", this);
    orderEdit_ = new UiInteger(this);
    orderEdit_->setValue(0);
    orderLayout_->addWidget(orderLabel_);
    orderLayout_->addWidget(orderEdit_);
    centralBodyGravityLayout_->addLayout(orderLayout_, 1, 1);
    
    secularVariationsLayout_ = new QHBoxLayout();
    secularVariationsCheck_ = new QCheckBox("包含引力场长期变化", this);
    secularVariationsLayout_->addWidget(secularVariationsCheck_);
    centralBodyGravityLayout_->addLayout(secularVariationsLayout_, 2, 0, 1, 2);
    
    // 固体潮汐配置
    solidTidesGroup_ = new QGroupBox("固体潮汐", this);
    solidTidesLayout_ = new QGridLayout(solidTidesGroup_);
    
    solidTideTypeLayout_ = new QHBoxLayout();
    solidTideTypeLabel_ = new QLabel("类型:", this);
    solidTideTypeCombo_ = new QComboBox(this);
    solidTideTypeLayout_->addWidget(solidTideTypeLabel_);
    solidTideTypeLayout_->addWidget(solidTideTypeCombo_);
    solidTidesLayout_->addLayout(solidTideTypeLayout_, 0, 0, 1, 2);
    
    timeDependentLayout_ = new QHBoxLayout();
    timeDependentCheck_ = new QCheckBox("包含时间依赖项", this);
    timeDependentLayout_->addWidget(timeDependentCheck_);
    solidTidesLayout_->addLayout(timeDependentLayout_, 1, 0, 1, 2);
    
    minAmplitudeLayout_ = new QHBoxLayout();
    minAmplitudeLabel_ = new QLabel("最小振幅:", this);
    minAmplitudeEdit_ = new UiQuantity(this);
    minAmplitudeEdit_->setValueInUnit(0.0, Unit("m"));
    minAmplitudeLayout_->addWidget(minAmplitudeLabel_);
    minAmplitudeLayout_->addWidget(minAmplitudeEdit_);
    solidTidesLayout_->addLayout(minAmplitudeLayout_, 2, 0, 1, 2);
    
    truncateLayout_ = new QHBoxLayout();
    truncateCheck_ = new QCheckBox("截断到引力场大小", this);
    truncateLayout_->addWidget(truncateCheck_);
    solidTidesLayout_->addLayout(truncateLayout_, 3, 0, 1, 2);
    
    // 海洋潮汐配置
    oceanTidesGroup_ = new QGroupBox("海洋潮汐", this);
    oceanTidesLayout_ = new QGridLayout(oceanTidesGroup_);
    
    useOceanTidesLayout_ = new QHBoxLayout();
    useOceanTidesCheck_ = new QCheckBox("使用海洋潮汐", this);
    useOceanTidesLayout_->addWidget(useOceanTidesCheck_);
    oceanTidesLayout_->addLayout(useOceanTidesLayout_, 0, 0, 1, 2);
    
    degreeOceanLayout_ = new QHBoxLayout();
    degreeOceanLabel_ = new QLabel("阶数:", this);
    degreeOceanEdit_ = new UiInteger(this);
    degreeOceanEdit_->setValue(2);
    degreeOceanLayout_->addWidget(degreeOceanLabel_);
    degreeOceanLayout_->addWidget(degreeOceanEdit_);
    oceanTidesLayout_->addLayout(degreeOceanLayout_, 1, 0);
    
    orderOceanLayout_ = new QHBoxLayout();
    orderOceanLabel_ = new QLabel("次数:", this);
    orderOceanEdit_ = new UiInteger(this);
    orderOceanEdit_->setValue(0);
    orderOceanLayout_->addWidget(orderOceanLabel_);
    orderOceanLayout_->addWidget(orderOceanEdit_);
    oceanTidesLayout_->addLayout(orderOceanLayout_, 1, 1);
    
    minAmplitudeOceanLayout_ = new QHBoxLayout();
    minAmplitudeOceanLabel_ = new QLabel("最小振幅:", this);
    minAmplitudeOceanEdit_ = new UiQuantity(this);
    minAmplitudeOceanEdit_->setValueInUnit(0.0, Unit("m"));
    minAmplitudeOceanLayout_->addWidget(minAmplitudeOceanLabel_);
    minAmplitudeOceanLayout_->addWidget(minAmplitudeOceanEdit_);
    oceanTidesLayout_->addLayout(minAmplitudeOceanLayout_, 2, 0, 1, 2);
    
    // 潮汐布局
    QHBoxLayout* tideLayout = new QHBoxLayout();
    tideLayout->addWidget(solidTidesGroup_, 1);
    tideLayout->addWidget(oceanTidesGroup_, 1);
    
    // 添加到主布局
    mainLayout_->addWidget(centralBodyGravityGroup_);
    mainLayout_->addLayout(tideLayout);
    mainLayout_->addStretch();
    
    // 刷新UI
    refreshModel();
    refreshSolidTideType();
}

GravityForce* UiGravityForce::getGravityForce() const
{
    return dynamic_cast<GravityForce*>(getObject());
}

void UiGravityForce::setGravityForce(GravityForce* gravity)
{
    if (gravity) {
        setObject(gravity);
        refreshUi();
    }
}

void UiGravityForce::refreshUi()
{
    auto gravity = getGravityForce();
    if (!gravity) return;
    
    // 中心天体重力场配置
    modelFileEdit_->setPath(gravity->model_);
    degreeEdit_->setValue(gravity->maxDegree_);
    orderEdit_->setValue(gravity->maxOrder_);
    secularVariationsCheck_->setChecked(gravity->useSecularVariations_);
    
    // 固体潮汐配置
    solidTideTypeCombo_->setCurrentIndex(static_cast<int>(gravity->solidTideType_));
    timeDependentCheck_->setChecked(gravity->includeTimeDependentSolidTides_);
    minAmplitudeEdit_->setValue(gravity->minAmplitudeSolidTides_);
    truncateCheck_->setChecked(gravity->truncateSolidTides_);
    
    // 海洋潮汐配置
    useOceanTidesCheck_->setChecked(gravity->useOceanTides_);
    degreeOceanEdit_->setValue(gravity->maxDegreeOceanTides_);
    orderOceanEdit_->setValue(gravity->maxOrderOceanTides_);
    minAmplitudeOceanEdit_->setValue(gravity->minAmplitudeOceanTides_);
}

void UiGravityForce::apply()
{
    auto gravity = getGravityForce();
    if (gravity) {
        applyTo(gravity);
    }
}

void UiGravityForce::applyTo(GravityForce* gravity)
{
    if (!gravity) return;
    
    // 中心天体重力场配置
    gravity->model_ = modelFileEdit_->path().toUtf8().data();
    gravity->maxDegree_ = degreeEdit_->value();
    gravity->maxOrder_ = orderEdit_->value();
    gravity->useSecularVariations_ = secularVariationsCheck_->isChecked();
    
    // 固体潮汐配置
    gravity->solidTideType_ = static_cast<ESolidTideType>(solidTideTypeCombo_->currentIndex());
    gravity->includeTimeDependentSolidTides_ = timeDependentCheck_->isChecked();
    gravity->minAmplitudeSolidTides_ = minAmplitudeEdit_->value();
    gravity->truncateSolidTides_ = truncateCheck_->isChecked();
    
    // 海洋潮汐配置
    gravity->useOceanTides_ = useOceanTidesCheck_->isChecked();
    gravity->maxDegreeOceanTides_ = degreeOceanEdit_->value();
    gravity->maxOrderOceanTides_ = orderOceanEdit_->value();
    gravity->minAmplitudeOceanTides_ = minAmplitudeOceanEdit_->value();
}

void UiGravityForce::refreshModel()
{
    modelCombo_->clear();
    modelCombo_->addItem("WGS84");
    modelCombo_->addItem("EGM2008");
    modelCombo_->addItem("自定义文件");
}

void UiGravityForce::refreshSolidTideType()
{
    solidTideTypeCombo_->clear();
    solidTideTypeCombo_->addItem("无");
    solidTideTypeCombo_->addItem("仅永久潮汐");
    solidTideTypeCombo_->addItem("完整潮汐");
}

AST_NAMESPACE_END