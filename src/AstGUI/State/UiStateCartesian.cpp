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
#include "AstCore/State.hpp"
#include "AstCore/Frame.hpp"
#include "AstGUI/UiSelectFrame.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>


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
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setColumnStretch(1, 1);

    int row = 0;

    // 轨道历元
    QLabel* epochLabel = new QLabel(tr("轨道历元"), this);
    epochEdit_ = new UiTimePoint(this);
    mainLayout->addWidget(epochLabel, row, 0);
    mainLayout->addWidget(epochEdit_, row, 1);
    row++;

    // 坐标系
    QLabel* frameLabel = new QLabel(tr("坐标系"), this);
    frameEdit_ = new QLineEdit(this);
    frameEdit_->setReadOnly(true);
    frameEdit_->setText(("ICRF"));
    frameSelectBtn_ = new QPushButton(("..."), this);
    frameSelectBtn_->setFixedWidth(30);
    frameSelectBtn_->setToolTip(tr("选择坐标系 (天体 + 类型)"));
    auto* frameWidget = new QWidget(this);
    frameWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* frameBar = new QHBoxLayout(frameWidget);
    frameBar->setContentsMargins(0, 0, 0, 0);
    frameBar->addWidget(frameEdit_);
    frameBar->addWidget(frameSelectBtn_);
    mainLayout->addWidget(frameLabel, row, 0);
    mainLayout->addWidget(frameWidget, row, 1);
    row++;

    // 位置X
    QLabel* posXLabel = new QLabel(tr("位置X"), this);
    posXEdit_ = new UiQuantity(this);
    posXEdit_->setDimension(Dimension::Length());
    mainLayout->addWidget(posXLabel, row, 0);
    mainLayout->addWidget(posXEdit_, row, 1);
    row++;

    // 位置Y
    QLabel* posYLabel = new QLabel(tr("位置Y"), this);
    posYEdit_ = new UiQuantity(this);
    posYEdit_->setDimension(Dimension::Length());
    mainLayout->addWidget(posYLabel, row, 0);
    mainLayout->addWidget(posYEdit_, row, 1);
    row++;

    // 位置Z
    QLabel* posZLabel = new QLabel(tr("位置Z"), this);
    posZEdit_ = new UiQuantity(this);
    posZEdit_->setDimension(Dimension::Length());
    mainLayout->addWidget(posZLabel, row, 0);
    mainLayout->addWidget(posZEdit_, row, 1);
    row++;

    // 速度X
    QLabel* velXLabel = new QLabel(tr("速度X"), this);
    velXEdit_ = new UiQuantity(this);
    velXEdit_->setDimension(Dimension::Speed());
    mainLayout->addWidget(velXLabel, row, 0);
    mainLayout->addWidget(velXEdit_, row, 1);
    row++;

    // 速度Y
    QLabel* velYLabel = new QLabel(tr("速度Y"), this);
    velYEdit_ = new UiQuantity(this);
    velYEdit_->setDimension(Dimension::Speed());
    mainLayout->addWidget(velYLabel, row, 0);
    mainLayout->addWidget(velYEdit_, row, 1);
    row++;

    // 速度Z
    QLabel* velZLabel = new QLabel(tr("速度Z"), this);
    velZEdit_ = new UiQuantity(this);
    velZEdit_->setDimension(Dimension::Speed());
    mainLayout->addWidget(velZLabel, row, 0);
    mainLayout->addWidget(velZEdit_, row, 1);
    row++;

    mainLayout->setRowStretch(row, 1);

    setLayout(mainLayout);

    // 每个控件变更时即时写入对应字段
    connect(posXEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getStateCartesian()) {
            state->setX(posXEdit_->getValueSI());
            emit stateCartesianChanged(state);
        }
    });
    connect(posYEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getStateCartesian()) {
            state->setY(posYEdit_->getValueSI());
            emit stateCartesianChanged(state);
        }
    });
    connect(posZEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getStateCartesian()) {
            state->setZ(posZEdit_->getValueSI());
            emit stateCartesianChanged(state);
        }
    });
    connect(velXEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getStateCartesian()) {
            state->setVx(velXEdit_->getValueSI());
            emit stateCartesianChanged(state);
        }
    });
    connect(velYEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getStateCartesian()) {
            state->setVy(velYEdit_->getValueSI());
            emit stateCartesianChanged(state);
        }
    });
    connect(velZEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getStateCartesian()) {
            state->setVz(velZEdit_->getValueSI());
            emit stateCartesianChanged(state);
        }
    });
    connect(epochEdit_, &UiTimePoint::timePointChanged, this, [this](const TimePoint& tp) {
        if (auto* state = getStateCartesian()) {
            state->setStateEpoch(tp);
            emit stateCartesianChanged(state);
        }
    });
    connect(frameSelectBtn_, &QPushButton::clicked, this, &UiStateCartesian::onSelectFrame);
}

void UiStateCartesian::refreshUi()
{
    if(auto state = getStateCartesian()){
        // 设置位置和速度值
        posXEdit_->setValueSI(state->x());
        posYEdit_->setValueSI(state->y());
        posZEdit_->setValueSI(state->z());
        velXEdit_->setValueSI(state->vx());
        velYEdit_->setValueSI(state->vy());
        velZEdit_->setValueSI(state->vz());
        
        // 设置轨道历元
        TimePoint timePoint = state->getStateEpoch_TimePoint();
        epochEdit_->setTimePoint(timePoint);

        // 更新坐标系显示
        if (auto* frame = state->getFrame())
            frameEdit_->setText(QString::fromUtf8(frame->getRepresentation().c_str()));
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
}

void UiStateCartesian::onSelectFrame()
{
    auto* state = getStateCartesian();
    if (!state)
        return;

    auto* frame = aUiSelectFrame();
    if (frame)
    {
        state->changeFrame(frame);
        frameEdit_->setText(QString::fromUtf8(frame->getRepresentation().c_str()));
        this->refreshUi();
        emit stateCartesianChanged(state);
    }
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