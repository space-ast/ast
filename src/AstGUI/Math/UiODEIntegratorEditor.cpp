///
/// @file      UiODEIntegratorEditor.cpp
/// @brief     ODE 积分器编辑器实现 — RTTI 类型分发
/// @author    axel
/// @date      2026-05-18
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

#include "UiODEIntegratorEditor.hpp"
#include "AstMath/ODEIntegrator.hpp"
#include "AstMath/ODEFixedStepIntegrator.hpp"
#include "AstMath/ODEVarStepIntegrator.hpp"
#include "AstGUI/UiODEVarStepIntegrator.hpp"
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QDoubleSpinBox>
#include <QLabel>

AST_NAMESPACE_BEGIN

UiODEIntegratorEditor::UiODEIntegratorEditor(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

UiODEIntegratorEditor::~UiODEIntegratorEditor() = default;

void UiODEIntegratorEditor::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    stack_ = new QStackedWidget(this);

    // 占位页面 (index 0)
    auto* placeholder = new QWidget(this);
    auto* phLayout = new QVBoxLayout(placeholder);
    auto* hint = new QLabel(tr(u8"选择积分器以编辑参数"), placeholder);
    hint->setAlignment(Qt::AlignCenter);
    hint->setWordWrap(true);
    phLayout->addWidget(hint);
    stack_->addWidget(placeholder);

    // 定步长页面
    auto* fixedPage = new QWidget(this);
    auto* fixedLayout = new QVBoxLayout(fixedPage);
    fixedLayout->setContentsMargins(0, 0, 0, 0);

    auto* fixedRow = new QHBoxLayout();
    fixedRow->addWidget(new QLabel(tr(u8"步长"), this));
    fixedStepEdit_ = new QDoubleSpinBox(this);
    fixedStepEdit_->setRange(1e-6, 86400);
    fixedStepEdit_->setValue(60);
    fixedStepEdit_->setDecimals(6);
    fixedStepEdit_->setSuffix(QStringLiteral(" s"));
    fixedRow->addWidget(fixedStepEdit_);
    fixedLayout->addLayout(fixedRow);
    fixedLayout->addStretch();

    fixedStepIdx_ = stack_->addWidget(fixedPage);

    // 变步长页面 — 复用已有 UiODEVarStepIntegrator
    varStepPage_ = new UiODEVarStepIntegrator(this);
    varStepIdx_ = stack_->addWidget(varStepPage_);

    layout->addWidget(stack_);
    stack_->setCurrentIndex(0);
}

void UiODEIntegratorEditor::setIntegrator(ODEIntegrator* integrator)
{
    if (!integrator)
    {
        clear();
        return;
    }

    if (auto* varStep = dynamic_cast<ODEVarStepIntegrator*>(integrator))
    {
        varStepPage_->setODEVarStepIntegrator(varStep);
        stack_->setCurrentIndex(varStepIdx_);
    }
    else if (auto* fixedStep = dynamic_cast<ODEFixedStepIntegrator*>(integrator))
    {
        fixedStepEdit_->blockSignals(true);
        fixedStepEdit_->setValue(fixedStep->getStepSize());
        fixedStepEdit_->blockSignals(false);
        stack_->setCurrentIndex(fixedStepIdx_);
    }
}

ODEIntegrator* UiODEIntegratorEditor::getIntegrator() const
{
    int idx = stack_->currentIndex();
    if (idx == varStepIdx_)
        return varStepPage_->getODEVarStepIntegrator();
    return nullptr;
}

void UiODEIntegratorEditor::apply()
{
    if (stack_->currentIndex() == varStepIdx_)
        varStepPage_->apply();
}

void UiODEIntegratorEditor::clear()
{
    stack_->setCurrentIndex(0);
}

AST_NAMESPACE_END
