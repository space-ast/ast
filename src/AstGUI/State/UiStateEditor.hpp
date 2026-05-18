///
/// @file      UiStateEditor.hpp
/// @brief     状态编辑器 — 根据 State RTTI 类型转发到 Cartesian/Keplerian 子编辑器
/// @details   不处理类型切换，不含类型选择控件。仅根据传入的 State* 类型自动
///           切换到对应的编辑页面，并将子编辑器的变更信号向外转发。
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
#include <QWidget>

class QStackedWidget;

AST_NAMESPACE_BEGIN

class State;
class StateCartesian;
class StateKeplerian;
class UiStateCartesian;
class UiStateKeplerian;

/// @brief 状态编辑器容器 — RTTI 类型分发 + 信号转发
///
/// 与 UiSegmentEditor 模式一致：
///   1. setState(State*) 按 RTTI 切换 Cartesian/Keplerian 页面
///   2. 子编辑器变更信号统一通过 stateChanged(State*) 向外转发
class AST_GUI_API UiStateEditor : public QWidget
{
    Q_OBJECT
public:
    explicit UiStateEditor(QWidget* parent = nullptr);
    ~UiStateEditor() override;

    /// @brief 设置要编辑的 State，根据 RTTI 类型切换子编辑器
    void setState(State* state);

    /// @brief 获取当前编辑中的 State（来自活跃的子编辑器）
    State* getState() const;

signals:
    /// @brief 子编辑器内容变更（类型切换不触发）
    void stateChanged(State* state);

private:
    void setupUi();
    void forwardCartesianChanged(StateCartesian* s);
    void forwardKeplerianChanged(StateKeplerian* s);

    QStackedWidget*   stack_ = nullptr;
    UiStateCartesian* cartesianEdit_ = nullptr;
    UiStateKeplerian* keplerianEdit_ = nullptr;
    int cartesianIdx_ = -1;
    int keplerianIdx_ = -1;
};

AST_NAMESPACE_END
