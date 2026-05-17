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
