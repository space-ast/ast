///
/// @file      UiInitialState.cpp
/// @brief     InitialState 段编辑器实现
/// @details   组装轨道参数、航天器参数、燃料储罐三个标签页
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
#include "AstCore/State.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstGUI/UiStateEditor.hpp"
#include "AstGUI/UiSpacecraftParams.hpp"
#include "AstGUI/UiFuelTank.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>

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
    layout->setContentsMargins(0, 0, 0, 0);

    // 标签页
    tabWidget_ = new QTabWidget(this);

    // 标签页1: 轨道参数
    auto* elementsTab = new QWidget();
    setupElementsTab(elementsTab);
    tabWidget_->addTab(elementsTab, tr("轨道参数"));

    // 标签页2: 航天器参数
    scParamsEditor_ = new UiSpacecraftParams();
    tabWidget_->addTab(scParamsEditor_, tr("航天器参数"));

    // 标签页3: 燃料储罐
    fuelTankEditor_ = new UiFuelTank();
    tabWidget_->addTab(fuelTankEditor_, tr("燃料储罐"));

    layout->addWidget(tabWidget_);
}

void UiInitialState::setupElementsTab(QWidget* tab)
{
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(6, 6, 6, 6);

    // 状态类型选择
    auto* typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel(tr("状态类型"), tab));
    stateTypeCombo_ = new QComboBox(tab);
    stateTypeCombo_->addItem(tr("笛卡尔 (Cartesian)"), static_cast<int>(EStateType::eCartesian));
    stateTypeCombo_->addItem(tr("开普勒 (Keplerian)"), static_cast<int>(EStateType::eKeplerian));
    typeLayout->addWidget(stateTypeCombo_);
    layout->addLayout(typeLayout);

    // UiStateEditor（RTTI 分发）
    stateEditor_ = new UiStateEditor(tab);
    layout->addWidget(stateEditor_);

    connect(stateTypeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UiInitialState::onStateTypeChanged);
    connect(stateEditor_, &UiStateEditor::stateChanged,
            this, &UiInitialState::onStateChanged);
}

// ============================================================================
// 公共接口
// ============================================================================

void UiInitialState::setInitialState(InitialState* initState)
{
    if (!initState)
        return;

    setObject(initState);
    rebuildFromSpacecraftState(initState->getInitialState());
}

InitialState* UiInitialState::getInitialState() const
{
    return aobject_cast<InitialState*>(getObject());
}

// ============================================================================
// 内部
// ============================================================================

void UiInitialState::rebuildFromSpacecraftState(SpacecraftState* scState)
{
    if (!scState)
        return;

    // ---------- 轨道状态 ----------
    auto* orbitState = scState->getOrbitState();
    if (orbitState)
    {
        // 同步 combo
        stateTypeCombo_->blockSignals(true);
        if (orbitState->getStateType() == EStateType::eCartesian)
            stateTypeCombo_->setCurrentIndex(0);
        else
            stateTypeCombo_->setCurrentIndex(1);
        stateTypeCombo_->blockSignals(false);

        stateEditor_->setState(orbitState);
    }

    // ---------- 航天器参数 & 燃料储罐 ----------
    scParamsEditor_->setSpacecraftState(scState);
    fuelTankEditor_->setSpacecraftState(scState);
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

    EStateType stateType = static_cast<EStateType>(index);
    if (stateType == scState->getStateType())
        return;

    scState->setStateType(stateType);
    stateEditor_->setState(scState->getOrbitState());
}

void UiInitialState::onStateChanged(State* /*state*/)
{
    // 子编辑器内容变更，值已由 UiStateCartesian/Keplerian 即时写入
}

AST_NAMESPACE_END
