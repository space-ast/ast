///
/// @file      UiStateKeplerian.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-31
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
#include "UiState.hpp"
#include "AstGUI/UiTimePoint.hpp"
#include "AstGUI/UiQuantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class StateKeplerian;
class Frame;

class AST_GUI_API UiStateKeplerian : public UiState
{
    Q_OBJECT
public:
    explicit UiStateKeplerian(Object* object, QWidget *parent = nullptr);
    explicit UiStateKeplerian(QWidget *parent = nullptr);
    ~UiStateKeplerian() = default;

    StateKeplerian* getStateKeplerian() const;
    void setStateKeplerian(StateKeplerian* state);
    void refreshUi();
    // void apply();
    // void applyTo(StateKeplerian* state);

private slots:
    void onSelectFrame();

protected:
    void onSizeTypeChanged();
    void onSizeParamChanged();
    void onShapeTypeChanged();
    void onShapeParamChanged();
    void onOrientationTypeChanged();
    void onOrientationParamChanged();
    void onPositionTypeChanged();
    void onPositionParamChanged();
    void onEpochChanged();
    void onFrameChanged();
    void onIncChanged();
    void onArgPeriChanged();
protected:
    void refreshSizeType();
    void refreshSizeParam();
    void refreshShapeType();
    void refreshShapeParam();
    void refreshOrientationType();
    void refreshOrientationParam();
    void refreshPositionType();
    void refreshPositionParam();
    void refreshEpoch();
    void refreshFrame();
    void refreshInc();
    void refreshArgPeri();

signals:
    void stateKeplerianChanged(StateKeplerian* state);
private:
    // 轨道历元
    UiTimePoint* epochEdit_{nullptr};
    // 坐标系
    QComboBox* frameCombo_{nullptr};
    QPushButton* frameSelectBtn_{nullptr};
    // 轨道大小
    QHBoxLayout* sizeLayout_{nullptr};
    QLabel* sizeLabel_{nullptr};
    QComboBox* sizeTypeCombo_{nullptr};
    UiQuantity* sizeEdit_{nullptr};
    // 轨道形状
    QHBoxLayout* shapeLayout_{nullptr};
    QLabel* shapeLabel_{nullptr};
    QComboBox* shapeTypeCombo_{nullptr};
    UiQuantity* shapeEdit_{nullptr};
    // 倾角
    QHBoxLayout* incLayout_{nullptr};
    QLabel* incLabel_{nullptr};
    UiQuantity* incEdit_{nullptr};
    // 轨道面方向
    QHBoxLayout* orientationLayout_{nullptr};
    QLabel* orientationLabel_{nullptr};
    QComboBox* orientationTypeCombo_{nullptr};
    UiQuantity* orientationEdit_{nullptr};
    // 近地点幅角
    QHBoxLayout* argPeriLayout_{nullptr};
    QLabel* argPeriLabel_{nullptr};
    UiQuantity* argPeriEdit_{nullptr};
    // 轨道位置
    QHBoxLayout* positionLayout_{nullptr};
    QLabel* positionLabel_{nullptr};
    QComboBox* positionTypeCombo_{nullptr};
    UiQuantity* positionEdit_{nullptr};
};

/*! @} */

AST_NAMESPACE_END