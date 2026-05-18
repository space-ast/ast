///
/// @file      UiBurnEditor.hpp
/// @brief     发动机点火编辑器 — RTTI 类型分发
/// @details   根据 Burn 子类类型自动切换脉冲/有限推力编辑页面
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

AST_NAMESPACE_BEGIN

class Burn;
class BurnImpulsive;
class UiBurnImpulsive;

/// @brief 发动机点火编辑器 — RTTI 类型分发
class AST_GUI_API UiBurnEditor : public QWidget
{
    Q_OBJECT
public:
    explicit UiBurnEditor(QWidget* parent = nullptr);
    ~UiBurnEditor() override;

    void setBurn(Burn* burn);
    void clear();

signals:
    void burnChanged(Burn* burn);

private:
    void setupUi();

    QStackedWidget* stack_ = nullptr;
    UiBurnImpulsive* impulsivePage_ = nullptr;
    int              impulsiveIdx_ = -1;
};

AST_NAMESPACE_END
