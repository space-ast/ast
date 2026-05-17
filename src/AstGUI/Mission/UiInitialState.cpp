///
/// @file      UiInitialState.cpp
/// @brief     InitialState 段编辑器实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
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

#include "UiInitialState.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstGUI/UiStateCartesian.hpp"
#include "AstGUI/UiStateKeplerian.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>

AST_NAMESPACE_BEGIN

UiInitialState::UiInitialState(Object* object, QWidget* parent)
    : UiInitialState(parent)
{
    setInitialState(aobject_cast<InitialState*>(object));
}

UiInitialState::UiInitialState(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiInitialState::~UiInitialState() = default;

void UiInitialState::setupUi()
{
    auto* layout = new QVBoxLayout(this);

    // 状态类型选择
    auto* typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel(tr("状态类型"), this));
    stateTypeCombo_ = new QComboBox(this);
    stateTypeCombo_->addItem(tr("笛卡尔 (Cartesian)"), 0);
    stateTypeCombo_->addItem(tr("开普勒 (Keplerian)"), 1);
    typeLayout->addWidget(stateTypeCombo_);
    layout->addLayout(typeLayout);

    // 状态编辑器 (QStackedWidget 切换两种编辑器)
    stateEditor_ = new QStackedWidget(this);

    cartesianEdit_ = new UiStateCartesian(this);
    cartesianEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    stateEditor_->addWidget(cartesianEdit_);  // index 0

    keplerianEdit_ = new UiStateKeplerian(this);
    keplerianEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    stateEditor_->addWidget(keplerianEdit_);  // index 1

    layout->addWidget(stateEditor_);

    // 连接信号
    connect(stateTypeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UiInitialState::onStateTypeChanged);
    connect(cartesianEdit_, &UiStateCartesian::stateCartesianChanged,
            this, &UiInitialState::onStateCartesianChanged);
    connect(keplerianEdit_, &UiStateKeplerian::stateKeplerianChanged,
            this, &UiInitialState::onStateKeplerianChanged);
}

// ============================================================================
// 公共接口
// ============================================================================

void UiInitialState::setInitialState(InitialState* initState)
{
    if (!initState)
        return;

    setObject(initState);

    auto* scState = initState->getInitialState();
    if (!scState)
    {
        // 若 InitialState 尚无 SpacecraftState，创建一个默认的
        scState = SpacecraftState::NewDefault();
        // InitialState 的输出状态需要显式设置
        // 通过 RTTI setProperty 或直接访问 OutputState 设置
    }

    rebuildStateEditor(scState);
}

InitialState* UiInitialState::getInitialState() const
{
    return aobject_cast<InitialState*>(getObject());
}

// ============================================================================
// 内部
// ============================================================================

void UiInitialState::rebuildStateEditor(SpacecraftState* scState)
{
    if (!scState)
        return;

    State* orbitState = scState->getOrbitState();
    if (!orbitState)
        return;

    // 根据状态类型切换编辑器
    if (auto* cartState = aobject_cast<StateCartesian*>(orbitState))
    {
        stateTypeCombo_->blockSignals(true);
        stateTypeCombo_->setCurrentIndex(0);
        stateTypeCombo_->blockSignals(false);

        cartesianEdit_->setStateCartesian(cartState);
        stateEditor_->setCurrentIndex(0);
    }
    else if (auto* kepState = aobject_cast<StateKeplerian*>(orbitState))
    {
        stateTypeCombo_->blockSignals(true);
        stateTypeCombo_->setCurrentIndex(1);
        stateTypeCombo_->blockSignals(false);

        keplerianEdit_->setStateKeplerian(kepState);
        stateEditor_->setCurrentIndex(1);
    }
}

// ============================================================================
// 槽
// ============================================================================

void UiInitialState::onStateTypeChanged(int index)
{
    auto* initState = getInitialState();
    if (!initState)
        return;

    auto* scState = initState->getInitialState();
    if (!scState)
        return;

    State* oldState = scState->getOrbitState();
    EStateType currentType = scState->getStateType();
    EStateType newType = (index == 0) ? EStateType::eCartesian : EStateType::eKeplerian;

    if (currentType == newType)
        return;

    // 从旧状态获取当前值用于初始化新状态
    CartState cart;
    ModOrbElem orbElem;
    bool hasCart = (oldState && scState->getState(cart) == eNoError);
    bool hasOrb = (oldState && scState->getState(orbElem) == eNoError);

    if (newType == EStateType::eCartesian)
    {
        auto* newState = hasCart
            ? StateCartesian::New(cart)
            : StateCartesian::New();
        scState->setOrbitState(newState);
        cartesianEdit_->setStateCartesian(newState);
        stateEditor_->setCurrentIndex(0);
    }
    else
    {
        auto* newState = hasOrb
            ? StateKeplerian::New(orbElem)
            : StateKeplerian::NewDefault();
        scState->setOrbitState(newState);
        keplerianEdit_->setStateKeplerian(newState);
        stateEditor_->setCurrentIndex(1);
    }
}

void UiInitialState::onStateCartesianChanged(StateCartesian* state)
{
    Q_UNUSED(state);
    // 值已由 UiStateCartesian 内部即时写入，此处仅做占位
    // 未来可在此处刷新关联参数（如坐标系变化后更新显示）
}

void UiInitialState::onStateKeplerianChanged(StateKeplerian* state)
{
    Q_UNUSED(state);
}

AST_NAMESPACE_END
