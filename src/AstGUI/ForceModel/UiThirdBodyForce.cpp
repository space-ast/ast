#include "UiThirdBodyForce.hpp"
#include "AstCore/ThirdBodyForce.hpp"
#include "AstCore/CelestialBody.hpp"
#include <QGroupBox>

AST_NAMESPACE_BEGIN

UiThirdBodyForce::UiThirdBodyForce(Object* object, QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiThirdBodyForce");
    setupUi();
    if (object) {
        setObject(object);
        setThirdBodyForce(dynamic_cast<ThirdBodyForce*>(object));
    }
}

UiThirdBodyForce::UiThirdBodyForce(QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiThirdBodyForce");
    setupUi();
}

void UiThirdBodyForce::setupUi()
{
    // 主布局
    mainLayout_ = new QVBoxLayout(this);
    
    // 基本信息
    thirdBodyLayout_ = new QHBoxLayout();
    thirdBodyLabel_ = new QLabel("三体:", this);
    thirdBodyCombo_ = new QComboBox(this);
    modeLabel_ = new QLabel("模式:", this);
    modeCombo_ = new QComboBox(this);
    modeCombo_->addItem("引力场");
    modeCombo_->addItem("点质量");
    thirdBodyLayout_->addWidget(thirdBodyLabel_);
    thirdBodyLayout_->addWidget(thirdBodyCombo_);
    thirdBodyLayout_->addWidget(modeLabel_);
    thirdBodyLayout_->addWidget(modeCombo_);
    mainLayout_->addLayout(thirdBodyLayout_);
    
    // 星历来源
    ephemerisLayout_ = new QHBoxLayout();
    ephemerisLabel_ = new QLabel("星历来源:", this);
    ephemerisCombo_ = new QComboBox(this);
    ephemerisLayout_->addWidget(ephemerisLabel_);
    ephemerisLayout_->addWidget(ephemerisCombo_);
    mainLayout_->addLayout(ephemerisLayout_);
    
    // 引力场配置
    gravityFieldWidget_ = new QWidget(this);
    gravityFieldLayout_ = new QVBoxLayout(gravityFieldWidget_);
    gravityForceWidget_ = new UiGravityForce(gravityFieldWidget_);
    gravityFieldLayout_->addWidget(gravityForceWidget_);
    mainLayout_->addWidget(gravityFieldWidget_);
    
    // 点质量配置
    pointMassWidget_ = new QWidget(this);
    pointMassLayout_ = new QVBoxLayout(pointMassWidget_);
    pointMassForceWidget_ = new UiPointMassForce(pointMassWidget_);
    pointMassLayout_->addWidget(pointMassForceWidget_);
    mainLayout_->addWidget(pointMassWidget_);
    
    // 移除这两个容器布局的默认内边距
    gravityFieldLayout_->setContentsMargins(0, 0, 0, 0);
    pointMassLayout_->setContentsMargins(0, 0, 0, 0);

    // 移除引力场配置的默认内边距
    auto gravityLayout = gravityForceWidget_->layout();
    if (gravityLayout) {
        gravityLayout->setContentsMargins(0, 0, 0, 0);
    }

    // 移除点质量配置的默认内边距
    auto pointMassLayout = pointMassForceWidget_->layout();
    if (pointMassLayout) {
        pointMassLayout->setContentsMargins(0, 0, 0, 0);
    }

    
    // 连接信号槽
    connect(modeCombo_, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &UiThirdBodyForce::onModeChanged);
    
    // 刷新UI
    refreshCelestialBodies();
    refreshEphemerisSource();
    refreshAttractionType();
    
    // 默认显示引力场模式
    onModeChanged(0);
}

void UiThirdBodyForce::onModeChanged(int index)
{
    if (index == 0) {
        // 引力场模式
        gravityFieldWidget_->show();
        pointMassWidget_->hide();
    } else {
        // 点质量模式
        gravityFieldWidget_->hide();
        pointMassWidget_->show();
    }
}

ThirdBodyForce* UiThirdBodyForce::getThirdBodyForce() const
{
    return dynamic_cast<ThirdBodyForce*>(getObject());
}

void UiThirdBodyForce::setThirdBodyForce(ThirdBodyForce* thirdBody)
{
    if (thirdBody) {
        setObject(thirdBody);
        refreshUi();
    }
}

void UiThirdBodyForce::refreshUi()
{
    auto thirdBody = getThirdBodyForce();
    if (!thirdBody) return;
    
    // 刷新三体选择
    auto body = thirdBody->body();
    if (body) {
        for (int i = 0; i < thirdBodyCombo_->count(); ++i) {
            if (thirdBodyCombo_->itemText(i).toStdString() == body->name()) {
                thirdBodyCombo_->setCurrentIndex(i);
                break;
            }
        }
    }
    
    // 刷新模式选择
    modeCombo_->setCurrentIndex(thirdBody->bodyAttractionType() == EBodyAttractionType::ePointMass ? 1 : 0);
    
    // 刷新星历来源
    ephemerisCombo_->setCurrentIndex(static_cast<int>(thirdBody->ephemerisSource()));
    
    // 刷新引力场或点质量配置
    if (thirdBody->bodyAttractionType() == EBodyAttractionType::ePointMass) {
        // 点质量模式
        pointMassForceWidget_->setPointMassForce(&thirdBody->pointMass());
        pointMassForceWidget_->refreshUi();
    } else {
        // 引力场模式
        gravityForceWidget_->setGravityForce(&thirdBody->gravity());
        gravityForceWidget_->refreshUi();
    }
    
    // 切换显示模式
    onModeChanged(modeCombo_->currentIndex());
}

void UiThirdBodyForce::apply()
{
    auto thirdBody = getThirdBodyForce();
    if (thirdBody) {
        applyTo(thirdBody);
    }
}

void UiThirdBodyForce::applyTo(ThirdBodyForce* thirdBody)
{
    if (!thirdBody) return;
    
    // 应用模式选择
    thirdBody->setAttractionType(modeCombo_->currentIndex() == 1 ? EBodyAttractionType::ePointMass : EBodyAttractionType::eGravity);
    
    // 应用星历来源
    thirdBody->setEphemerisSource(static_cast<EEphemerisSource>(ephemerisCombo_->currentIndex()));
    
    // 应用引力场或点质量配置
    if (thirdBody->bodyAttractionType() == EBodyAttractionType::ePointMass) {
        // 点质量模式
        pointMassForceWidget_->apply();
    } else {
        // 引力场模式
        gravityForceWidget_->apply();
    }
}

void UiThirdBodyForce::refreshEphemerisSource()
{
    ephemerisCombo_->clear();
    ephemerisCombo_->addItem("SPICE 天体中心");
    ephemerisCombo_->addItem("JPL DE");
    ephemerisCombo_->addItem("天体");
}

void UiThirdBodyForce::refreshAttractionType()
{
    // 这里需要根据实际的EBodyAttractionType枚举添加相应的选项
}

void UiThirdBodyForce::refreshCelestialBodies()
{
    thirdBodyCombo_->clear();
    thirdBodyCombo_->addItem("Moon");
    thirdBodyCombo_->addItem("Sun");
    thirdBodyCombo_->addItem("Mercury");
    thirdBodyCombo_->addItem("Venus");
    thirdBodyCombo_->addItem("Mars");
    thirdBodyCombo_->addItem("Jupiter");
    thirdBodyCombo_->addItem("Saturn");
    thirdBodyCombo_->addItem("Uranus");
    thirdBodyCombo_->addItem("Neptune");
}

AST_NAMESPACE_END