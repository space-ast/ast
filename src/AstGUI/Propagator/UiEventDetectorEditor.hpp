///
/// @file      UiEventDetectorEditor.hpp
/// @brief     事件检测器编辑器 — RTTI 调度
/// @details   根据 EventDetector 子类类型自动切换对应的编辑器页面
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

#pragma once

#include "AstGlobal.h"
#include <QStackedWidget>

AST_NAMESPACE_BEGIN

class EventDetector;
class UiEventDetector;

/// @brief 事件检测器编辑器 — 根据 RTTI 类型切换编辑页面
class AST_GUI_API UiEventDetectorEditor : public QStackedWidget
{
    Q_OBJECT
public:
    explicit UiEventDetectorEditor(QWidget* parent = nullptr);
    ~UiEventDetectorEditor() override;

    void setDetector(EventDetector* det);
    EventDetector* getDetector() const;
    void clear();

signals:
    void detectorChanged(EventDetector* det);

private:
    void setupUi();

    UiEventDetector* basePage_ = nullptr;
    int baseIdx_ = -1;
};

AST_NAMESPACE_END
