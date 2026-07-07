///
/// @file      UiStateEditor.cpp
/// @brief     状态编辑器实现
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

#include "UiStateEditor.hpp"
#include "AstCore/State.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstGUI/UiStateCartesian.hpp"
#include "AstGUI/UiStateKeplerian.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>

AST_NAMESPACE_BEGIN

UiStateEditor::UiStateEditor(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

UiStateEditor::~UiStateEditor() = default;

void UiStateEditor::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 占位页面
    auto* placeholder = new QWidget(this);
    auto* phLayout = new QVBoxLayout(placeholder);
    auto* hint = new QLabel(tr("无状态数据"), placeholder);
    hint->setAlignment(Qt::AlignCenter);
    phLayout->addWidget(hint);
    stack_ = new QStackedWidget(this);
    stack_->addWidget(placeholder);   // index 0

    cartesianEdit_ = new UiStateCartesian(this);
    cartesianEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cartesianIdx_ = stack_->addWidget(cartesianEdit_);

    keplerianEdit_ = new UiStateKeplerian(this);
    keplerianEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    keplerianIdx_ = stack_->addWidget(keplerianEdit_);

    layout->addWidget(stack_);
    stack_->setCurrentIndex(0);

    // 转发子编辑器信号
    connect(cartesianEdit_, &UiStateCartesian::stateCartesianChanged,
            this, &UiStateEditor::forwardCartesianChanged);
    connect(keplerianEdit_, &UiStateKeplerian::stateKeplerianChanged,
            this, &UiStateEditor::forwardKeplerianChanged);
}

// ============================================================================
// 公共接口
// ============================================================================

void UiStateEditor::setState(State* state)
{
    if (!state)
    {
        stack_->setCurrentIndex(0);
        return;
    }

    if (auto* cart = aobject_cast<StateCartesian*>(state))
    {
        cartesianEdit_->setStateCartesian(cart);
        stack_->setCurrentIndex(cartesianIdx_);
    }
    else if (auto* kep = aobject_cast<StateKeplerian*>(state))
    {
        keplerianEdit_->setStateKeplerian(kep);
        stack_->setCurrentIndex(keplerianIdx_);
    }
}

State* UiStateEditor::getState() const
{
    if (stack_->currentIndex() == cartesianIdx_)
        return cartesianEdit_->getStateCartesian();
    if (stack_->currentIndex() == keplerianIdx_)
        return keplerianEdit_->getStateKeplerian();
    return nullptr;
}

// ============================================================================
// 信号转发
// ============================================================================

void UiStateEditor::forwardCartesianChanged(StateCartesian* s)
{
    emit stateChanged(s);
}

void UiStateEditor::forwardKeplerianChanged(StateKeplerian* s)
{
    emit stateChanged(s);
}

AST_NAMESPACE_END
