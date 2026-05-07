///
/// @file      UiStateCartesian.cpp
/// @brief     笛卡尔状态编辑界面
/// @author    axel
/// @date      2026-03-28
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "UiStateCartesian.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>


AST_NAMESPACE_BEGIN

using namespace units;

UiStateCartesian::UiStateCartesian(Object *object, QWidget *parent)
    : UiStateCartesian(parent)
{
    setStateCartesian(dynamic_cast<StateCartesian*>(object));
}

UiStateCartesian::UiStateCartesian(QWidget *parent) : UiState(parent)
{
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 轨道历元
    QHBoxLayout* epochLayout = new QHBoxLayout();
    QLabel* epochLabel = new QLabel(tr("轨道历元"), this);
    epochEdit_ = new UiTimePoint(this);
    epochLayout->addWidget(epochLabel);
    epochLayout->addWidget(epochEdit_);
    mainLayout->addLayout(epochLayout);
    
    // 坐标系
    QHBoxLayout* frameLayout = new QHBoxLayout();
    QLabel* frameLabel = new QLabel(tr("坐标系"), this);
    frameCombo_ = new QComboBox(this);
    frameCombo_->addItem(tr("ICRF"));
    frameLayout->addWidget(frameLabel);
    frameLayout->addWidget(frameCombo_);
    mainLayout->addLayout(frameLayout);
    
    // 位置X
    QHBoxLayout* posXLayout = new QHBoxLayout();
    QLabel* posXLabel = new QLabel(tr("位置X"), this);
    posXEdit_ = new UiQuantity(this);
    posXEdit_->setQuantity(Quantity(6678137, m));
    posXLayout->addWidget(posXLabel);
    posXLayout->addWidget(posXEdit_);
    mainLayout->addLayout(posXLayout);
    
    // 位置Y
    QHBoxLayout* posYLayout = new QHBoxLayout();
    QLabel* posYLabel = new QLabel(tr("位置Y"), this);
    posYEdit_ = new UiQuantity(this);
    posYEdit_->setQuantity(Quantity(0, m));
    posYLayout->addWidget(posYLabel);
    posYLayout->addWidget(posYEdit_);
    mainLayout->addLayout(posYLayout);
    
    // 位置Z
    QHBoxLayout* posZLayout = new QHBoxLayout();
    QLabel* posZLabel = new QLabel(tr("位置Z"), this);
    posZEdit_ = new UiQuantity(this);
    posZEdit_->setQuantity(Quantity(0, m));
    posZLayout->addWidget(posZLabel);
    posZLayout->addWidget(posZEdit_);
    mainLayout->addLayout(posZLayout);
    
    // 速度X
    QHBoxLayout* velXLayout = new QHBoxLayout();
    QLabel* velXLabel = new QLabel(tr("速度X"), this);
    velXEdit_ = new UiQuantity(this);
    velXEdit_->setQuantity(Quantity(0, m / s));
    velXLayout->addWidget(velXLabel);
    velXLayout->addWidget(velXEdit_);
    mainLayout->addLayout(velXLayout);
    
    // 速度Y
    QHBoxLayout* velYLayout = new QHBoxLayout();
    QLabel* velYLabel = new QLabel(tr("速度Y"), this);
    velYEdit_ = new UiQuantity(this);
    velYEdit_->setQuantity(Quantity(6789.530297717651592, m / s));
    velYLayout->addWidget(velYLabel);
    velYLayout->addWidget(velYEdit_);
    mainLayout->addLayout(velYLayout);
    
    // 速度Z
    QHBoxLayout* velZLayout = new QHBoxLayout();
    QLabel* velZLabel = new QLabel(tr("速度Z"), this);
    velZEdit_ = new UiQuantity(this);
    velZEdit_->setQuantity(Quantity(3686.414173013651634, m / s));
    velZLayout->addWidget(velZLabel);
    velZLayout->addWidget(velZEdit_);
    mainLayout->addLayout(velZLayout);
    
    setLayout(mainLayout);

    connect(posXEdit_, &UiQuantity::quantityChanged, this, &UiStateCartesian::apply);
    connect(posYEdit_, &UiQuantity::quantityChanged, this, &UiStateCartesian::apply);
    connect(posZEdit_, &UiQuantity::quantityChanged, this, &UiStateCartesian::apply);
    connect(velXEdit_, &UiQuantity::quantityChanged, this, &UiStateCartesian::apply);
    connect(velYEdit_, &UiQuantity::quantityChanged, this, &UiStateCartesian::apply);
    connect(velZEdit_, &UiQuantity::quantityChanged, this, &UiStateCartesian::apply);
    connect(epochEdit_, &UiTimePoint::timePointChanged, this, &UiStateCartesian::apply);
}

void UiStateCartesian::refreshUi()
{
    if(auto state = getStateCartesian()){
        // 设置位置和速度值
        posXEdit_->setQuantity(Quantity(state->x(), m));
        posYEdit_->setQuantity(Quantity(state->y(), m));
        posZEdit_->setQuantity(Quantity(state->z(), m));
        velXEdit_->setQuantity(Quantity(state->vx(), m / s));
        velYEdit_->setQuantity(Quantity(state->vy(), m / s));
        velZEdit_->setQuantity(Quantity(state->vz(), m / s));
        
        // 设置轨道历元
        TimePoint timePoint = state->getStateEpoch_TimePoint();
        epochEdit_->setTimePoint(timePoint);

        // 这里可以添加代码，设置坐标系等其他属性
    }
}

void UiStateCartesian::apply()
{
    if (auto state = getStateCartesian())
    {
        applyTo(state);
        emit stateCartesianChanged(state);
    }
}

void UiStateCartesian::applyTo(StateCartesian *state)
{
    // 从界面上获取位置和速度值
    state->setX(posXEdit_->getValueSI());
    state->setY(posYEdit_->getValueSI());
    state->setZ(posZEdit_->getValueSI());
    state->setVx(velXEdit_->getValueSI());
    state->setVy(velYEdit_->getValueSI());
    state->setVz(velZEdit_->getValueSI());
    
    // 获取轨道历元
    TimePoint timePoint = epochEdit_->getTimePoint();
    state->setStateEpoch(timePoint);
    // 这里可以添加代码，获取坐标系等其他属性
}

void UiStateCartesian::setStateCartesian(StateCartesian* state)
{
    if (state)
    {
        setObject(state);
        refreshUi();
    }
}

StateCartesian* UiStateCartesian::getStateCartesian() const
{
    return dynamic_cast<StateCartesian*>(getObject());
}

AST_NAMESPACE_END