///
/// @file      UiInitialState.hpp
/// @brief     InitialState 段编辑器
/// @details   封装 UiStateCartesian / UiStateKeplerian，提供状态类型切换
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include <QComboBox>

class QStackedWidget;

AST_NAMESPACE_BEGIN

class InitialState;
class SpacecraftState;
class UiStateCartesian;
class UiStateKeplerian;

/// @brief InitialState 段编辑器，提供状态类型选择和轨道状态编辑
class AST_GUI_API UiInitialState : public UiObject
{
    Q_OBJECT
public:
    explicit UiInitialState(Object* object, QWidget* parent = nullptr);
    explicit UiInitialState(QWidget* parent = nullptr);
    ~UiInitialState() override;

    void setInitialState(InitialState* state);
    InitialState* getInitialState() const;

private slots:
    void onStateTypeChanged(int index);
    void onStateCartesianChanged(StateCartesian* state);
    void onStateKeplerianChanged(StateKeplerian* state);

private:
    void setupUi();
    void rebuildStateEditor(SpacecraftState* scState);

    QComboBox*         stateTypeCombo_ = nullptr;
    QStackedWidget*    stateEditor_ = nullptr;
    UiStateCartesian*  cartesianEdit_ = nullptr;
    UiStateKeplerian*  keplerianEdit_ = nullptr;
};

AST_NAMESPACE_END
