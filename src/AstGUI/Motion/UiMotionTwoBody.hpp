///
/// @file      UiMotionTwoBody.hpp
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
#include "AstGUI/UiMotionProfile.hpp"
#include "AstGUI/UiTimeInterval.hpp"
#include "AstGUI/UiTimePoint.hpp"
#include "AstGUI/UiQuantity.hpp"
#include "AstGUI/UiStateCartesian.hpp"
#include "AstGUI/UiStateKeplerian.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>



AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class MotionTwoBody;

class AST_GUI_API UiMotionTwoBody: public UiMotionProfile
{
    Q_OBJECT
public:
    UiMotionTwoBody(Object* object, QWidget *parent = nullptr);
    UiMotionTwoBody(QWidget *parent = nullptr);
    ~UiMotionTwoBody() = default;
    UiMotionTwoBody(const UiMotionTwoBody&) = delete;
    UiMotionTwoBody& operator=(const UiMotionTwoBody&) = delete;

    MotionTwoBody* getMotionTwoBody() const;
    void setMotionTwoBody(MotionTwoBody* motion);
    void refreshUi();
    void apply();
protected:
    void onStateTypeChanged();
    void onStepSizeChanged();
    void onIntervalChanged();
    void onPropagationFrameChanged();
protected:
    void refreshStateType();
private slots:
    void applyTo(MotionTwoBody* motion);
private:
    // 布局
    QVBoxLayout* mainLayout_{nullptr};
    QGridLayout* gridLayout_{nullptr};
    QVBoxLayout* leftLayout_{nullptr};
    QVBoxLayout* rightLayout_{nullptr};
    
    // 左侧控件
    QHBoxLayout* timeIntervalLayout_{nullptr};
    QLabel* timeIntervalLabel_{nullptr};
    UiTimeInterval* timeIntervalEdit_{nullptr};
    QHBoxLayout* stepSizeLayout_{nullptr};
    QLabel* stepSizeLabel_{nullptr};
    UiQuantity* stepSizeEdit_{nullptr};
    QHBoxLayout* propagationFrameLayout_{nullptr};
    QLabel* propagationFrameLabel_{nullptr};
    QComboBox* propagationFrameCombo_{nullptr};
    QHBoxLayout* stateTypeLayout_{nullptr};
    QLabel* stateTypeLabel_{nullptr};
    QComboBox* stateTypeCombo_{nullptr};
    
    // 右侧控件
    UiStateCartesian* stateCartesianEdit_{nullptr};
    UiStateKeplerian* stateKeplerianEdit_{nullptr};
    
    // 数据
    MotionTwoBody* motionTwoBody_{nullptr};
};




/*! @} */

AST_NAMESPACE_END
