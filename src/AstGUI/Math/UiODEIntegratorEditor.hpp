///
/// @file      UiODEIntegratorEditor.hpp
/// @brief     ODE 积分器编辑器 — RTTI 类型分发
/// @details   根据 ODEIntegrator 子类类型自动切换定步长/变步长编辑页面
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
#include <QWidget>

class QStackedWidget;
class QDoubleSpinBox;

AST_NAMESPACE_BEGIN

class ODEIntegrator;
class ODEVarStepIntegrator;
class UiODEVarStepIntegrator;

/// @brief ODE 积分器编辑器 — RTTI 类型分发 + 信号转发
class AST_GUI_API UiODEIntegratorEditor : public QWidget
{
    Q_OBJECT
public:
    explicit UiODEIntegratorEditor(QWidget* parent = nullptr);
    ~UiODEIntegratorEditor() override;

    void setIntegrator(ODEIntegrator* integrator);
    ODEIntegrator* getIntegrator() const;
    void apply();
    void clear();

signals:
    void integratorChanged(ODEIntegrator* integrator);

private:
    void setupUi();

    QStackedWidget*         stack_ = nullptr;
    QDoubleSpinBox*         fixedStepEdit_ = nullptr;
    int                     fixedStepIdx_ = -1;
    UiODEVarStepIntegrator* varStepPage_ = nullptr;
    int                     varStepIdx_ = -1;
};

AST_NAMESPACE_END
