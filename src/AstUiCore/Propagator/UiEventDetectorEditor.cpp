///
/// @file      UiEventDetectorEditor.cpp
/// @brief     事件检测器编辑器实现 — RTTI 调度
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

#include "UiEventDetectorEditor.hpp"
#include "UiEventDetector.hpp"
#include "AstCore/EventDetector.hpp"
#include <QVBoxLayout>
#include <QLabel>

AST_NAMESPACE_BEGIN

UiEventDetectorEditor::UiEventDetectorEditor(QWidget* parent)
    : QStackedWidget(parent)
{
    setupUi();
}

UiEventDetectorEditor::~UiEventDetectorEditor() = default;

void UiEventDetectorEditor::setupUi()
{
    // 占位页面
    auto* placeholder = new QWidget(this);
    auto* placeholderLayout = new QVBoxLayout(placeholder);
    auto* hint = new QLabel(tr("选择事件检测器以编辑属性"), placeholder);
    hint->setAlignment(Qt::AlignCenter);
    hint->setWordWrap(true);
    placeholderLayout->addWidget(hint);
    placeholderLayout->addStretch();
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    addWidget(placeholder);

    // 基类页面
    basePage_ = new UiEventDetector(this);
    baseIdx_ = addWidget(basePage_);

    setCurrentIndex(0);

    connect(basePage_, &UiEventDetector::detectorChanged,
            this, &UiEventDetectorEditor::detectorChanged);
}

void UiEventDetectorEditor::setDetector(EventDetector* det)
{
    if (!det)
    {
        clear();
        return;
    }

    basePage_->setEventDetector(det);
    setCurrentIndex(baseIdx_);
}

EventDetector* UiEventDetectorEditor::getDetector() const
{
    return basePage_->getEventDetector();
}

void UiEventDetectorEditor::clear()
{
    setCurrentIndex(0);
}

AST_NAMESPACE_END
