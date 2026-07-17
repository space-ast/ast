///
/// @file      UiEventDetector.hpp
/// @brief     事件检测器基类编辑器
/// @details   编辑 EventDetector 的公共属性：激活、重复次数、方向、阈值、目标值
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
#include "AstGUI/UiObject.hpp"
#include <QWidget>

class QComboBox;

AST_NAMESPACE_BEGIN

class EventDetector;
class UiQuantity;

/// @brief 事件检测器基类编辑器 — 编辑公共属性
class UiInteger;

class AST_GUI_API UiEventDetector : public UiObject
{
    Q_OBJECT
public:
    explicit UiEventDetector(Object* object, QWidget* parent = nullptr);
    explicit UiEventDetector(QWidget* parent = nullptr);
    ~UiEventDetector() override;
    UiEventDetector(const UiEventDetector&) = delete;
    UiEventDetector& operator=(const UiEventDetector&) = delete;

    void setEventDetector(EventDetector* det);
    EventDetector* getEventDetector() const;

signals:
    void detectorChanged(EventDetector* det);

private:
    void setupUi();
    void refreshFromDetector(EventDetector* det);

    UiInteger*  repeatCountEdit_ = nullptr;
    QComboBox*  directionCombo_ = nullptr;
    UiQuantity* thresholdEdit_ = nullptr;
    UiQuantity* goalEdit_ = nullptr;
};

AST_NAMESPACE_END
