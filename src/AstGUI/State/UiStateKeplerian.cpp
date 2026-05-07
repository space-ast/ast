///
/// @file      UiStateKeplerian.cpp
/// @brief     开普勒轨道根数状态编辑界面
/// @author    axel
/// @date      2026-03-31
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

#include "UiStateKeplerian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

AST_NAMESPACE_BEGIN

using namespace units;

UiStateKeplerian::UiStateKeplerian(Object *object, QWidget *parent)
    : UiStateKeplerian(parent)
{
    setStateKeplerian(dynamic_cast<StateKeplerian*>(object));
}

UiStateKeplerian::UiStateKeplerian(QWidget *parent) : UiState(parent)
{
    // 创建主布局
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(2, 3);
    
    int row = 0;
    
    // 轨道历元
    QLabel* epochLabel = new QLabel(tr("轨道历元"), this);
    epochEdit_ = new UiTimePoint(this);
    mainLayout->addWidget(epochLabel, row, 0);
    mainLayout->addWidget(epochEdit_, row, 2);
    row++;
    
    // 坐标系
    QLabel* frameLabel = new QLabel(tr("坐标系"), this);
    frameCombo_ = new QComboBox(this);
    frameCombo_->addItem(tr("ICRF"));
    mainLayout->addWidget(frameLabel, row, 0);
    mainLayout->addWidget(frameCombo_, row, 2);
    row++;
    
    // 轨道大小
    sizeLabel_ = new QLabel(tr("轨道大小"), this);
    sizeTypeCombo_ = new QComboBox(this);
    sizeTypeCombo_->addItem(tr("半长轴"), static_cast<int>(ESizeType::eSMA));
    sizeTypeCombo_->addItem(tr("远地点高度"), static_cast<int>(ESizeType::eApoAlt));
    sizeTypeCombo_->addItem(tr("远地点半径"), static_cast<int>(ESizeType::eApoRad));
    sizeTypeCombo_->addItem(tr("近地点高度"), static_cast<int>(ESizeType::ePeriAlt));
    sizeTypeCombo_->addItem(tr("近地点半径"), static_cast<int>(ESizeType::ePeriRad));
    sizeTypeCombo_->addItem(tr("周期"), static_cast<int>(ESizeType::ePeriod));
    sizeTypeCombo_->addItem(tr("平均角速度"), static_cast<int>(ESizeType::eMeanMotion));

    sizeEdit_ = new UiQuantity(this);
    sizeEdit_->setQuantity(Quantity(6678137));
    mainLayout->addWidget(sizeLabel_, row, 0);
    mainLayout->addWidget(sizeTypeCombo_, row, 1);
    mainLayout->addWidget(sizeEdit_, row, 2);
    row++;
    
    // 轨道形状
    shapeLabel_ = new QLabel(tr("轨道形状"), this);
    shapeTypeCombo_ = new QComboBox(this);
    shapeTypeCombo_->addItem(tr("偏心率"), static_cast<int>(EShapeType::eEcc));
    shapeTypeCombo_->addItem(tr("远地点高度"), static_cast<int>(EShapeType::eApoAlt));
    shapeTypeCombo_->addItem(tr("远地点半径"), static_cast<int>(EShapeType::eApoRad));
    shapeTypeCombo_->addItem(tr("近地点高度"), static_cast<int>(EShapeType::ePeriAlt));
    shapeTypeCombo_->addItem(tr("近地点半径"), static_cast<int>(EShapeType::ePeriRad));

    shapeEdit_ = new UiQuantity(this);
    shapeEdit_->setQuantity(Quantity(0.0));
    mainLayout->addWidget(shapeLabel_, row, 0);
    mainLayout->addWidget(shapeTypeCombo_, row, 1);
    mainLayout->addWidget(shapeEdit_, row, 2);
    row++;
    
    // 倾角
    incLabel_ = new QLabel(tr("倾角"), this);
    incEdit_ = new UiQuantity(this);
    incEdit_->setQuantity(Quantity(0.0, deg));
    mainLayout->addWidget(incLabel_, row, 0);
    mainLayout->addWidget(incEdit_, row, 2);
    row++;
    
    // 轨道面方向
    orientationLabel_ = new QLabel(tr("轨道面方向"), this);
    orientationTypeCombo_ = new QComboBox(this);
    orientationTypeCombo_->addItem(tr("升交点赤经"), static_cast<int>(EOrientationType::eRAAN));
    orientationTypeCombo_->addItem(tr("升交点经度"), static_cast<int>(EOrientationType::eLAN));
    
    orientationEdit_ = new UiQuantity(this);
    orientationEdit_->setQuantity(Quantity(0.0, deg));
    mainLayout->addWidget(orientationLabel_, row, 0);
    mainLayout->addWidget(orientationTypeCombo_, row, 1);
    mainLayout->addWidget(orientationEdit_, row, 2);
    row++;
    
    // 近地点幅角
    argPeriLabel_ = new QLabel(tr("近地点幅角"), this);
    argPeriEdit_ = new UiQuantity(this);
    argPeriEdit_->setQuantity(Quantity(0.0, deg));
    mainLayout->addWidget(argPeriLabel_, row, 0);
    mainLayout->addWidget(argPeriEdit_, row, 2);
    row++;
    
    // 轨道位置
    positionLabel_ = new QLabel(tr("轨道位置"), this);
    positionTypeCombo_ = new QComboBox(this);
    positionTypeCombo_->addItem(tr("真近点角"), static_cast<int>(EPositionType::eTrueAnomaly));
    positionTypeCombo_->addItem(tr("平近点角"), static_cast<int>(EPositionType::eMeanAnomaly));
    positionTypeCombo_->addItem(tr("偏近点角"), static_cast<int>(EPositionType::eEccAnomaly));
    positionTypeCombo_->addItem(tr("纬度幅角"), static_cast<int>(EPositionType::eArgLat));
    positionTypeCombo_->addItem(tr("过近地点后时间"), static_cast<int>(EPositionType::eTimePastPeri));
    positionTypeCombo_->addItem(tr("过升交点后时间"), static_cast<int>(EPositionType::eTimePastAscNode));
    // positionTypeCombo_->addItem(tr("过近地点时刻"), static_cast<int>(EPositionType::eTimeOfPeriPassage));
    // positionTypeCombo_->addItem(tr("过升交点时刻"), static_cast<int>(EPositionType::eTimeOfAscNodePassage));

    positionEdit_ = new UiQuantity(this);
    positionEdit_->setQuantity(Quantity(0.0, deg));
    mainLayout->addWidget(positionLabel_, row, 0);
    mainLayout->addWidget(positionTypeCombo_, row, 1);
    mainLayout->addWidget(positionEdit_, row, 2);
    row++;
    
    setLayout(mainLayout);

    // 连接信号槽
    connect(epochEdit_, &UiTimePoint::timePointChanged, this, &UiStateKeplerian::onEpochChanged);
    connect(sizeEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::onSizeParamChanged);
    connect(sizeTypeCombo_, &QComboBox::currentTextChanged, this, &UiStateKeplerian::onSizeTypeChanged);
    connect(shapeEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::onShapeParamChanged);
    connect(shapeTypeCombo_, &QComboBox::currentTextChanged, this, &UiStateKeplerian::onShapeTypeChanged);
    connect(incEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::onIncChanged);
    connect(orientationEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::onOrientationParamChanged);
    connect(orientationTypeCombo_, &QComboBox::currentTextChanged, this, &UiStateKeplerian::onOrientationTypeChanged);
    connect(argPeriEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::onArgPeriChanged);
    connect(positionEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::onPositionParamChanged);
    connect(positionTypeCombo_, &QComboBox::currentTextChanged, this, &UiStateKeplerian::onPositionTypeChanged);
    connect(frameCombo_, &QComboBox::currentTextChanged, this, &UiStateKeplerian::onFrameChanged);
}

void UiStateKeplerian::refreshUi()
{
    refreshSizeType();
    refreshSizeParam();
    refreshShapeType();
    refreshShapeParam();
    refreshOrientationType();
    refreshOrientationParam();
    refreshPositionType();
    refreshPositionParam();
    refreshEpoch();
    refreshFrame();
    refreshInc();
    refreshArgPeri();
}

// void UiStateKeplerian::apply()
// {
//     if (auto state = getStateKeplerian())
//     {
//         emit stateKeplerianChanged(state);
//     }
// }

// void UiStateKeplerian::applyTo(StateKeplerian *state)
// {
//     auto oldState = getStateKeplerian();
//     setObject(state);
//     
// }

void UiStateKeplerian::setStateKeplerian(StateKeplerian* state)
{
    if (state)
    {
        setObject(state);
        refreshUi();
    }
}

StateKeplerian* UiStateKeplerian::getStateKeplerian() const
{
    return dynamic_cast<StateKeplerian*>(getObject());
}

void UiStateKeplerian::onSizeTypeChanged()
{
    if (auto state = getStateKeplerian())
    {
        ESizeType sizeType = static_cast<ESizeType>(sizeTypeCombo_->currentData().toInt());
        state->setSizeType(sizeType);
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onSizeParamChanged()
{
    if (auto state = getStateKeplerian())
    {
        double sizeParam = sizeEdit_->getValueSI();
        state->setSizeParam(sizeParam);
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onShapeTypeChanged()
{
    if (auto state = getStateKeplerian())
    {
        EShapeType shapeType = static_cast<EShapeType>(shapeTypeCombo_->currentData().toInt());
        state->setShapeType(shapeType);
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onShapeParamChanged()
{
    if (auto state = getStateKeplerian())
    {
        double shapeParam = shapeEdit_->getValueSI();
        state->setShapeParam(shapeParam);
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onOrientationTypeChanged()
{
    if (auto state = getStateKeplerian())
    {
        EOrientationType orientationType = static_cast<EOrientationType>(orientationTypeCombo_->currentData().toInt());
        state->setOrientationType(orientationType);
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onOrientationParamChanged()
{
    if (auto state = getStateKeplerian())
    {
        double orientationParam = orientationEdit_->getValueSI();
        state->setOrientationParam(orientationParam);
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onPositionTypeChanged()
{
    if (auto state = getStateKeplerian())
    {
        EPositionType positionType = static_cast<EPositionType>(positionTypeCombo_->currentData().toInt());
        state->setPositionType(positionType);
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onPositionParamChanged()
{
    if (auto state = getStateKeplerian())
    {
        double positionParam = positionEdit_->getValueSI();
        state->setPositionParam(positionParam);
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onEpochChanged()
{
    if (auto state = getStateKeplerian())
    {
        TimePoint timePoint = epochEdit_->getTimePoint();
        state->setStateEpoch(timePoint);
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onFrameChanged()
{
    if (auto state = getStateKeplerian())
    {
        // 坐标系暂时只支持ICRF
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onIncChanged()
{
    if (auto state = getStateKeplerian())
    {
        state->setInc(incEdit_->getValueSI());
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::onArgPeriChanged()
{
    if (auto state = getStateKeplerian())
    {
        state->setArgPeri(argPeriEdit_->getValueSI());
        refreshUi();
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::refreshSizeType()
{
    if (auto state = getStateKeplerian())
    {
        ESizeType sizeType = state->getSizeType();
        int index = sizeTypeCombo_->findData(static_cast<int>(sizeType));
        if (index != -1)
        {
            sizeTypeCombo_->setCurrentIndex(index);
        }else{
            // 处理未知的sizeType
        }
    }
}

void UiStateKeplerian::refreshSizeParam()
{
    if (auto state = getStateKeplerian())
    {
        ESizeType sizeType = state->getSizeType();
        double sizeParam = state->getSizeParam();
        switch(sizeType){
        case ESizeType::eSMA:
        case ESizeType::eApoAlt:
        case ESizeType::ePeriAlt:
        case ESizeType::eApoRad:
        case ESizeType::ePeriRad:
            sizeEdit_->setQuantity(Quantity(sizeParam, m));
            break;
        case ESizeType::ePeriod:
            sizeEdit_->setQuantity(Quantity(sizeParam, s));
            break;
        case ESizeType::eMeanMotion:
            sizeEdit_->setQuantity(Quantity(sizeParam, rad / s));
            break;
        }
    }
}

void UiStateKeplerian::refreshShapeType()
{
    if (auto state = getStateKeplerian())
    {
        EShapeType shapeType = state->getShapeType();
        int index = shapeTypeCombo_->findData(static_cast<int>(shapeType));
        if (index != -1)
        {
            shapeTypeCombo_->setCurrentIndex(index);
        }else{
            // 处理未知的shapeType
        }
    }
}

void UiStateKeplerian::refreshShapeParam()
{
    if (auto state = getStateKeplerian())
    {
        EShapeType shapeType = state->getShapeType();
        double shapeParam = state->getShapeParam();
        switch(shapeType){
        case EShapeType::eEcc:
            shapeEdit_->setQuantity(Quantity(shapeParam));
            break;
        case EShapeType::eApoAlt:
        case EShapeType::ePeriAlt:
        case EShapeType::eApoRad:
        case EShapeType::ePeriRad:
            shapeEdit_->setQuantity(Quantity(shapeParam, m));
            break;
        }
    }
}

void UiStateKeplerian::refreshOrientationType()
{
    if (auto state = getStateKeplerian())
    {
        EOrientationType orientationType = state->getOrientationType();
        int index = orientationTypeCombo_->findData(static_cast<int>(orientationType));
        if (index != -1)
        {
            orientationTypeCombo_->setCurrentIndex(index);
        }else{
            // 处理未知的orientationType
        }
    }
}

void UiStateKeplerian::refreshOrientationParam()
{
    if (auto state = getStateKeplerian())
    {
        double orientationParam = state->getOrientationParam();
        orientationEdit_->setQuantity(Quantity(orientationParam, rad));
    }
}

void UiStateKeplerian::refreshPositionType()
{
    if (auto state = getStateKeplerian())
    {
        EPositionType positionType = state->getPositionType();
        int index = positionTypeCombo_->findData(static_cast<int>(positionType));
        if (index != -1)
        {
            positionTypeCombo_->setCurrentIndex(index);
        }else{
            // 处理未知的positionType
        }
    }
}

void UiStateKeplerian::refreshPositionParam()
{
    if (auto state = getStateKeplerian())
    {
        EPositionType positionType = state->getPositionType();
        double positionParam = state->getPositionParam();
        switch(positionType){
        case EPositionType::eTrueAnomaly:
        case EPositionType::eMeanAnomaly:
        case EPositionType::eEccAnomaly:
        case EPositionType::eArgLat:
            positionEdit_->setQuantity(Quantity(positionParam, rad));
            break;
        case EPositionType::eTimePastPeri:
        case EPositionType::eTimePastAscNode:
            positionEdit_->setQuantity(Quantity(positionParam, s));
            break;
        case EPositionType::eTimeOfPeriPassage:
        case EPositionType::eTimeOfAscNodePassage:
            // 这里需要特殊处理，因为是时间点
            break;
        }
    }
}

void UiStateKeplerian::refreshEpoch()
{
    if (auto state = getStateKeplerian())
    {
        TimePoint timePoint = state->getStateEpoch_TimePoint();
        epochEdit_->setTimePoint(timePoint);
    }
}

void UiStateKeplerian::refreshFrame()
{
    // 坐标系暂时只支持ICRF
    frameCombo_->setCurrentIndex(0);
}

void UiStateKeplerian::refreshInc()
{
    if (auto state = getStateKeplerian())
    {
        if(incEdit_->getUnit().dimension() != Dimension::Angle())
        {
            incEdit_->setUnit(rad);
        }
        incEdit_->setValueSI(state->getInc());
    }
}

void UiStateKeplerian::refreshArgPeri()
{
    if (auto state = getStateKeplerian())
    {
        if(argPeriEdit_->getUnit().dimension() != Dimension::Angle())
        {
            argPeriEdit_->setUnit(rad);
        }
        argPeriEdit_->setValueSI(state->getArgPeri());
    }
}

AST_NAMESPACE_END