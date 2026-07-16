///
/// @file      UiBurnImpulsive.hpp
/// @brief     脉冲推力参数编辑界面
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

class QComboBox;

AST_NAMESPACE_BEGIN

class Burn;
class BurnImpulsive;
class UiQuantity;

/// @brief 脉冲推力参数编辑器
class AST_GUI_API UiBurnImpulsive : public UiObject
{
    Q_OBJECT
public:
    explicit UiBurnImpulsive(Object* object, QWidget* parent = nullptr);
    explicit UiBurnImpulsive(QWidget* parent = nullptr);
    ~UiBurnImpulsive() override = default;
    UiBurnImpulsive(const UiBurnImpulsive&) = delete;
    UiBurnImpulsive& operator=(const UiBurnImpulsive&) = delete;

    void setBurn(Burn* burn);
    BurnImpulsive* getBurnImpulsive() const;
    void refreshUi();
    void apply();
    void applyTo(BurnImpulsive* burn);

signals:
    void burnChanged(BurnImpulsive* burn);

private:
    void setupUi();

    QComboBox*  thrustAxesCombo_ = nullptr;
    UiQuantity* xEdit_ = nullptr;
    UiQuantity* yEdit_ = nullptr;
    UiQuantity* zEdit_ = nullptr;
};

AST_NAMESPACE_END
