///
/// @file      UiStateCartesian.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-28
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "AstGUI/UiState.hpp"
#include "AstGUI/UiQuantity.hpp"
#include "AstGUI/UiTimePoint.hpp"
#include <QLineEdit>

class QPushButton;

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class StateCartesian;
class Frame;

class AST_GUI_API UiStateCartesian: public UiState
{
    Q_OBJECT
public:
    explicit UiStateCartesian(Object* object, QWidget *parent = nullptr);
    explicit UiStateCartesian(QWidget *parent = nullptr);
    ~UiStateCartesian() = default;

    void refreshUi();
    void apply();
    void applyTo(StateCartesian* state);

    void setStateCartesian(StateCartesian* state);
    StateCartesian* getStateCartesian() const;

private slots:
    void onSelectFrame();

signals:
    void stateCartesianChanged(StateCartesian* state);
private:
    // 左侧控件
    UiTimePoint* epochEdit_{nullptr};
    QLineEdit*   frameEdit_{nullptr};
    QPushButton* frameSelectBtn_{nullptr};

    // 右侧控件
    UiQuantity* posXEdit_{nullptr};
    UiQuantity* posYEdit_{nullptr};
    UiQuantity* posZEdit_{nullptr};
    UiQuantity* velXEdit_{nullptr};
    UiQuantity* velYEdit_{nullptr};
    UiQuantity* velZEdit_{nullptr};
};


/*! @} */

AST_NAMESPACE_END