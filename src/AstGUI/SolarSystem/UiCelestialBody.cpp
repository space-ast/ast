///
/// @file      UiCelestialBody.cpp
/// @brief     天体编辑界面实现
/// @author    Aist
/// @date      2026-04-10
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#include "UiCelestialBody.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstCore/BodyEphemeris.hpp"
#include "AstCore/BodyOrientation.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>

AST_NAMESPACE_BEGIN

using namespace units;

UiCelestialBody::UiCelestialBody(Object* object, QWidget* parent)
    : UiCelestialBody(parent)
{
    setCelestialBody(dynamic_cast<CelestialBody*>(object));
}

UiCelestialBody::UiCelestialBody(QWidget* parent)
    : UiObject(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // ============ 基本信息组 ============
    QGroupBox* basicGroup = new QGroupBox(tr("基本信息"), this);
    QGridLayout* basicLayout = new QGridLayout(basicGroup);
    
    // 名称
    basicLayout->addWidget(new QLabel(tr("名称"), this), 0, 0);
    nameEdit_ = new QLineEdit(this);
    basicLayout->addWidget(nameEdit_, 0, 1);
    
    mainLayout->addWidget(basicGroup);
    
    // ============ 物理参数组 ============
    QGroupBox* physicsGroup = new QGroupBox(tr("物理参数"), this);
    QGridLayout* physicsLayout = new QGridLayout(physicsGroup);
    
    // 引力参数 (GM)
    physicsLayout->addWidget(new QLabel(tr("引力参数 (GM)"), this), 0, 0);
    gmEdit_ = new UiQuantity(this);
    gmEdit_->setDimension(Dimension::Volume() / Dimension::Time().pow(2));
    physicsLayout->addWidget(gmEdit_, 0, 1);
    
    // 母天体
    physicsLayout->addWidget(new QLabel(tr("母天体"), this), 1, 0);
    parentLabel_ = new QLabel(this);
    parentLabel_->setText("-");
    physicsLayout->addWidget(parentLabel_, 1, 1);
    
    // 子天体
    physicsLayout->addWidget(new QLabel(tr("子天体"), this), 2, 0);
    childrenLabel_ = new QLabel(this);
    childrenLabel_->setText("-");
    physicsLayout->addWidget(childrenLabel_, 2, 1);
    
    mainLayout->addWidget(physicsGroup);
    
    // ============ 引力模型组 ============
    QGroupBox* gravityGroup = new QGroupBox(tr("引力模型"), this);
    QHBoxLayout* gravityLayout = new QHBoxLayout(gravityGroup);
    
    gravityModelCombo_ = new QComboBox(this);
    gravityModelCombo_->addItem(tr("点质量"));
    gravityModelCombo_->addItem(tr("J2带谐项"));
    gravityModelCombo_->addItem(tr("J4带谐项"));
    gravityLayout->addWidget(gravityModelCombo_);
    
    gravityModelDetailsBtn_ = new QPushButton(tr("详情..."), this);
    gravityLayout->addWidget(gravityModelDetailsBtn_);
    gravityLayout->addStretch();
    
    mainLayout->addWidget(gravityGroup);
    
    // ============ 形状组 ============
    QGroupBox* shapeGroup = new QGroupBox(tr("形状"), this);
    QGridLayout* shapeLayout = new QGridLayout(shapeGroup);
    
    // 形状类型
    shapeLayout->addWidget(new QLabel(tr("类型"), this), 0, 0);
    shapeCombo_ = new QComboBox(this);
    shapeCombo_->addItem(tr("球体"));
    shapeCombo_->addItem(tr("椭球"));
    shapeLayout->addWidget(shapeCombo_, 0, 1);
    
    // 半长轴
    shapeLayout->addWidget(new QLabel(tr("半长轴"), this), 1, 0);
    semiMajorAxisEdit_ = new UiQuantity(this);
    semiMajorAxisEdit_->setDimension(Dimension::Length());
    shapeLayout->addWidget(semiMajorAxisEdit_, 1, 1);
    
    // 半中轴
    shapeLayout->addWidget(new QLabel(tr("半中轴"), this), 2, 0);
    semiIntermediateAxisEdit_ = new UiQuantity(this);
    semiIntermediateAxisEdit_->setDimension(Dimension::Length());
    shapeLayout->addWidget(semiIntermediateAxisEdit_, 2, 1);
    
    // 半短轴
    shapeLayout->addWidget(new QLabel(tr("半短轴"), this), 3, 0);
    semiMinorAxisEdit_ = new UiQuantity(this);
    semiMinorAxisEdit_->setDimension(Dimension::Length());
    shapeLayout->addWidget(semiMinorAxisEdit_, 3, 1);
    
    mainLayout->addWidget(shapeGroup);
    
    // ============ 姿态组 ============
    QGroupBox* orientationGroup = new QGroupBox(tr("姿态"), this);
    QHBoxLayout* orientationLayout = new QHBoxLayout(orientationGroup);
    
    orientationCombo_ = new QComboBox(this);
    orientationCombo_->addItem(tr("旋转系数文件"));
    orientationLayout->addWidget(orientationCombo_);
    
    orientationDetailsBtn_ = new QPushButton(tr("详情..."), this);
    orientationLayout->addWidget(orientationDetailsBtn_);
    orientationLayout->addStretch();
    
    mainLayout->addWidget(orientationGroup);
    
    // ============ 星历组 ============
    QGroupBox* ephemerisGroup = new QGroupBox(tr("星历"), this);
    QGridLayout* ephemerisLayout = new QGridLayout(ephemerisGroup);
    
    // 星历类型
    ephemerisLayout->addWidget(new QLabel(tr("类型"), this), 0, 0);
    ephemerisCombo_ = new QComboBox(this);
    ephemerisCombo_->addItem(("JPL SPICE"));
    ephemerisLayout->addWidget(ephemerisCombo_, 0, 1);
    
    // SPICE ID
    ephemerisLayout->addWidget(new QLabel(tr("SPICE 编号"), this), 1, 0);
    spiceIdLabel_ = new QLabel(this);
    ephemerisLayout->addWidget(spiceIdLabel_, 1, 1);
    
    mainLayout->addWidget(ephemerisGroup);
    
    mainLayout->addStretch();
    setLayout(mainLayout);
    
    // 连接信号
    connect(nameEdit_, &QLineEdit::textChanged, this, &UiCelestialBody::apply);
    connect(gravityModelCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UiCelestialBody::apply);
    connect(gravityModelDetailsBtn_, &QPushButton::clicked, this, &UiCelestialBody::onGravityModelDetails);
    connect(orientationDetailsBtn_, &QPushButton::clicked, this, &UiCelestialBody::onOrientationDetails);
}

void UiCelestialBody::refreshUi()
{
    if (auto body = getCelestialBody())
    {
        // 基本信息
        nameEdit_->blockSignals(true);
        nameEdit_->setText(QString::fromStdString(body->getName()));
        nameEdit_->blockSignals(false);
        
        // 物理参数
        gmEdit_->blockSignals(true);
        gmEdit_->setValueSI(body->getGM());
        gmEdit_->blockSignals(false);
        
        // 母天体
        if (auto parent = body->getParent())
        {
            parentLabel_->setText(QString::fromStdString(parent->getName()));
        }
        else
        {
            parentLabel_->setText("-");
        }
        
        // 子天体
        childrenLabel_->setText("-");
        
        // 引力模型
        int modelIndex = gravityModelCombo_->findText(QString::fromStdString(body->getGravityModel()));
        if (modelIndex >= 0)
        {
            gravityModelCombo_->blockSignals(true);
            gravityModelCombo_->setCurrentIndex(modelIndex);
            gravityModelCombo_->blockSignals(false);
        }
        
        // 形状
        if (auto shape = body->getShape())
        {
            A_UNUSED(shape);
            semiMajorAxisEdit_->blockSignals(true);
            semiMajorAxisEdit_->setValueSI(body->getRadius());
            semiMajorAxisEdit_->blockSignals(false);

            semiIntermediateAxisEdit_->blockSignals(true);
            semiIntermediateAxisEdit_->setValueSI(body->getRadius());
            semiIntermediateAxisEdit_->blockSignals(false);

            semiMinorAxisEdit_->blockSignals(true);
            semiMinorAxisEdit_->setValueSI(body->getRadius());
            semiMinorAxisEdit_->blockSignals(false);
        }
        
        // SPICE ID
        spiceIdLabel_->setText(QString::number(body->getJplSpiceId()));
    }
}

void UiCelestialBody::apply()
{
    if (auto body = getCelestialBody())
    {
        body->setName(nameEdit_->text().toStdString());
        body->setJplSpiceId(spiceIdLabel_->text().toInt());
        emit celestialBodyChanged(body);
    }
}

void UiCelestialBody::onGravityModelDetails()
{
    QMessageBox::information(this, tr("引力模型详情"), tr("引力模型配置对话框（待实现）"));
}

void UiCelestialBody::onOrientationDetails()
{
    QMessageBox::information(this, tr("姿态详情"), tr("姿态配置对话框（待实现）"));
}

void UiCelestialBody::setCelestialBody(CelestialBody* body)
{
    if (body)
    {
        setObject(body);
        refreshUi();
    }
}

CelestialBody* UiCelestialBody::getCelestialBody() const
{
    return dynamic_cast<CelestialBody*>(getObject());
}

AST_NAMESPACE_END
