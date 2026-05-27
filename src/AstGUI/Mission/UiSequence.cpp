///
/// @file      UiSequence.cpp
/// @brief     Sequence / TargeterSequence 段编辑器实现
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

#include "UiSequence.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/TargeterSequence.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

AST_NAMESPACE_BEGIN

// =============================================================================
// UiSequence
// =============================================================================

UiSequence::UiSequence(Object* object, QWidget* parent)
    : UiSequence(parent)
{
    setSequence(aobject_cast<Sequence*>(object));
}

UiSequence::UiSequence(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiSequence::~UiSequence() = default;

void UiSequence::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* repeatLayout = new QHBoxLayout();
    repeatLayout->addWidget(new QLabel(tr(u8"重复次数"), this));
    repeatCountSpin_ = new QSpinBox(this);
    repeatCountSpin_->setRange(1, 9999);
    repeatCountSpin_->setValue(1);
    repeatLayout->addWidget(repeatCountSpin_);
    layout->addLayout(repeatLayout);

    layout->addStretch();

    connect(repeatCountSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &UiSequence::onRepeatCountChanged);
}

void UiSequence::setSequence(Sequence* seq)
{
    if (!seq)
        return;
    setObject(seq);

    repeatCountSpin_->blockSignals(true);
    repeatCountSpin_->setValue(seq->repeatCount());
    repeatCountSpin_->blockSignals(false);
}

Sequence* UiSequence::getSequence() const
{
    return aobject_cast<Sequence*>(getObject());
}

void UiSequence::onRepeatCountChanged(int value)
{
    if (auto* seq = getSequence())
        seq->setRepeatCount(value);
}

// =============================================================================
// UiTargeterSequence
// =============================================================================

UiTargeterSequence::UiTargeterSequence(Object* object, QWidget* parent)
    : UiTargeterSequence(parent)
{
    setTargeterSequence(aobject_cast<TargeterSequence*>(object));
}

UiTargeterSequence::UiTargeterSequence(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiTargeterSequence::~UiTargeterSequence() = default;

void UiTargeterSequence::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* repeatLayout = new QHBoxLayout();
    repeatLayout->addWidget(new QLabel(tr(u8"重复次数"), this));
    repeatCountSpin_ = new QSpinBox(this);
    repeatCountSpin_->setRange(1, 9999);
    repeatCountSpin_->setValue(1);
    repeatLayout->addWidget(repeatCountSpin_);
    layout->addLayout(repeatLayout);

    auto* iterLayout = new QHBoxLayout();
    iterLayout->addWidget(new QLabel(tr(u8"最大迭代次数"), this));
    maxIterSpin_ = new QSpinBox(this);
    maxIterSpin_->setRange(1, 9999);
    maxIterSpin_->setValue(50);
    iterLayout->addWidget(maxIterSpin_);
    layout->addLayout(iterLayout);

    layout->addStretch();

    connect(repeatCountSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int v)
    {
        if (auto* s = getTargeterSequence())
            s->setRepeatCount(v);
    });
}

void UiTargeterSequence::setTargeterSequence(TargeterSequence* seq)
{
    if (!seq)
        return;
    setObject(seq);

    repeatCountSpin_->blockSignals(true);
    repeatCountSpin_->setValue(seq->repeatCount());
    repeatCountSpin_->blockSignals(false);
}

TargeterSequence* UiTargeterSequence::getTargeterSequence() const
{
    return aobject_cast<TargeterSequence*>(getObject());
}

AST_NAMESPACE_END
