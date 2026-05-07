#include "UiPointMassForce.hpp"
#include "AstCore/PointMassForce.hpp"
#include "AstCore/ForceModel.hpp"

AST_NAMESPACE_BEGIN

UiPointMassForce::UiPointMassForce(Object* object, QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiPointMassForce");
    setupUi();
    if (object) {
        setObject(object);
        setPointMassForce(dynamic_cast<PointMassForce*>(object));
    }
}

UiPointMassForce::UiPointMassForce(QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiPointMassForce");
    setupUi();
}

void UiPointMassForce::setupUi()
{
    // 主布局
    mainLayout_ = new QVBoxLayout(this);
    
    // 引力常数来源
    gmSourceLayout_ = new QHBoxLayout();
    gmSourceLabel_ = new QLabel("引力常数来源:", this);
    gmSourceCombo_ = new QComboBox(this);
    gmSourceLayout_->addWidget(gmSourceLabel_);
    gmSourceLayout_->addWidget(gmSourceCombo_);
    mainLayout_->addLayout(gmSourceLayout_);
    
    // 指定引力常数
    specifiedGMLayout_ = new QHBoxLayout();
    specifiedGMLabel_ = new QLabel("指定引力常数:", this);
    specifiedGMEdit_ = new UiQuantity(this);
    specifiedGMEdit_->setValueInUnit(0, Unit("m^3/s^2"));
    specifiedGMLayout_->addWidget(specifiedGMLabel_);
    specifiedGMLayout_->addWidget(specifiedGMEdit_);
    mainLayout_->addLayout(specifiedGMLayout_);
    
    mainLayout_->addStretch();
    
    // 连接信号槽
    connect(gmSourceCombo_, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &UiPointMassForce::onGMSourceChanged);
    
    // 刷新UI
    refreshGMSource();
    onGMSourceChanged(0);
}

PointMassForce* UiPointMassForce::getPointMassForce() const
{
    return dynamic_cast<PointMassForce*>(getObject());
}

void UiPointMassForce::setPointMassForce(PointMassForce* pointMass)
{
    if (pointMass) {
        setObject(pointMass);
        refreshUi();
    }
}

void UiPointMassForce::refreshUi()
{
    auto pointMass = getPointMassForce();
    if (!pointMass) return;
    
    // 刷新引力常数来源
    gmSourceCombo_->setCurrentIndex(static_cast<int>(pointMass->gmSource_));
    
    // 刷新指定引力常数
    specifiedGMEdit_->setValue(pointMass->specifiedGM_);
    
    // 更新指定引力常数输入框的可见性
    onGMSourceChanged(gmSourceCombo_->currentIndex());
}

void UiPointMassForce::apply()
{
    auto pointMass = getPointMassForce();
    if (pointMass) {
        applyTo(pointMass);
    }
}

void UiPointMassForce::applyTo(PointMassForce* pointMass)
{
    if (!pointMass) return;
    
    // 应用引力常数来源
    pointMass->gmSource_ = static_cast<EGMSource>(gmSourceCombo_->currentIndex());
    
    // 应用指定引力常数
    pointMass->specifiedGM_ = specifiedGMEdit_->value();
}

void UiPointMassForce::refreshGMSource()
{
    gmSourceCombo_->clear();
    gmSourceCombo_->addItem("来自天体重力场");
    gmSourceCombo_->addItem("来自JPL DE文件");
    gmSourceCombo_->addItem("用户指定的引力常数");
}

void UiPointMassForce::onGMSourceChanged(int index)
{
    // 只有当引力常数来源为用户指定时，才显示指定引力常数输入框
    bool showSpecifiedGM = (index == static_cast<int>(EGMSource::eSpecifiedValue));
    specifiedGMLabel_->setVisible(showSpecifiedGM);
    specifiedGMEdit_->setVisible(showSpecifiedGM);
}

AST_NAMESPACE_END