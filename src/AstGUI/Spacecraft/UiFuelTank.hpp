///
/// @file      UiFuelTank.hpp
/// @brief     燃料储罐编辑器
/// @details   编辑航天器燃料储罐参数（压力、温度、密度、质量等）
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

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include <QWidget>

AST_NAMESPACE_BEGIN

class SpacecraftState;
class UiQuantity;

/// @brief 燃料储罐编辑器
class AST_GUI_API UiFuelTank : public UiObject
{
    Q_OBJECT
public:
    explicit UiFuelTank(Object* object, QWidget* parent = nullptr);
    explicit UiFuelTank(QWidget* parent = nullptr);
    ~UiFuelTank() override;
    UiFuelTank(const UiFuelTank&) = delete;
    UiFuelTank& operator=(const UiFuelTank&) = delete;

    void setSpacecraftState(SpacecraftState* state);
    SpacecraftState* getSpacecraftState() const;

private:
    void setupUi();
    void refreshFromState(SpacecraftState* state);

    // 燃料储罐参数控件
    UiQuantity*     tankPressureEdit_ = nullptr;
    UiQuantity*     tankTemperatureEdit_ = nullptr;
    UiQuantity*     fuelDensityEdit_ = nullptr;
    UiQuantity*     fuelMassEdit_ = nullptr;
};

AST_NAMESPACE_END
