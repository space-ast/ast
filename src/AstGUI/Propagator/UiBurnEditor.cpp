///
/// @file      UiBurnEditor.cpp
/// @brief     发动机点火编辑器实现 — RTTI 类型分发
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

#include "UiBurnEditor.hpp"
#include "UiBurnImpulsive.hpp"
#include "AstCore/Burn.hpp"
#include "AstCore/BurnImpulsive.hpp"
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>

AST_NAMESPACE_BEGIN

UiBurnEditor::UiBurnEditor(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

UiBurnEditor::~UiBurnEditor() = default;

void UiBurnEditor::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    stack_ = new QStackedWidget(this);

    // 占位页面 (index 0)
    auto* placeholder = new QWidget(this);
    auto* phLayout = new QVBoxLayout(placeholder);
    auto* hint = new QLabel(tr(u8"选择发动机类型以编辑参数"), placeholder);
    hint->setAlignment(Qt::AlignCenter);
    hint->setWordWrap(true);
    phLayout->addWidget(hint);
    stack_->addWidget(placeholder);

    // 脉冲推力页面
    impulsivePage_ = new UiBurnImpulsive(this);
    impulsiveIdx_ = stack_->addWidget(impulsivePage_);

    layout->addWidget(stack_);
    stack_->setCurrentIndex(0);

    // 转发信号
    connect(impulsivePage_, &UiBurnImpulsive::burnChanged,
            this, &UiBurnEditor::burnChanged);
}

void UiBurnEditor::setBurn(Burn* burn)
{
    if (!burn)
    {
        clear();
        return;
    }

    if (auto* imp = dynamic_cast<BurnImpulsive*>(burn))
    {
        impulsivePage_->setBurn(imp);
        stack_->setCurrentIndex(impulsiveIdx_);
    }
}

void UiBurnEditor::clear()
{
    stack_->setCurrentIndex(0);
}

AST_NAMESPACE_END
