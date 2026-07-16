///
/// @file      UiOrbitWizard.hpp
/// @brief     轨道向导对话框
/// @author    axel
/// @date      2026-05-08
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QFrame>

AST_NAMESPACE_BEGIN

class AST_GUI_API UiOrbitWizard : public QDialog
{
    Q_OBJECT

public:
    explicit UiOrbitWizard(QWidget *parent = nullptr);
    ~UiOrbitWizard();
    UiOrbitWizard(const UiOrbitWizard&) = delete;
    UiOrbitWizard& operator=(const UiOrbitWizard&) = delete;

private:
    void setupUi();
    void setupConnections();

private slots:
    void onInclinationRadioClicked();
    void onAltitudeRadioClicked();
    void onAscendingNodeRadioClicked();
    void onDescendingNodeRadioClicked();
    void onInclinationSliderChanged(int value);
    void onAltitudeSliderChanged(int value);
    void onAscendingNodeSliderChanged(int value);
    void onDescendingNodeSliderChanged(int value);
    void onColorButtonClicked();
    void onModelBrowseClicked();
    void onApplyClicked();
    void onOkClicked();

private:
    QComboBox* typeComboBox{nullptr};
    QLineEdit* satelliteNameEdit{nullptr};
    
    QGroupBox* geometryGroup{nullptr};
    QRadioButton* inclinationRadio{nullptr};
    QLineEdit* inclinationEdit{nullptr};
    QSlider* inclinationSlider{nullptr};
    
    QRadioButton* altitudeRadio{nullptr};
    QLineEdit* altitudeEdit{nullptr};
    QSlider* altitudeSlider{nullptr};
    
    QGroupBox* nodeGroup{nullptr};
    QRadioButton* ascendingNodeRadio{nullptr};
    QLineEdit* ascendingNodeEdit{nullptr};
    QSlider* ascendingNodeSlider{nullptr};
    
    QRadioButton* descendingNodeRadio{nullptr};
    QLineEdit* descendingNodeEdit{nullptr};
    QSlider* descendingNodeSlider{nullptr};
    
    QCheckBox* defaultObjectCheckBox{nullptr};
    
    QGroupBox* timePeriodGroup{nullptr};
    QComboBox* intervalComboBox{nullptr};
    
    QGroupBox* graphicsGroup{nullptr};
    QCheckBox* showAllObjectsCheckBox{nullptr};
    QPushButton* colorButton{nullptr};
    QLineEdit* modelEdit{nullptr};
    QPushButton* modelBrowseButton{nullptr};
    
    QFrame* mapFrame{nullptr};
    QLabel* latLonLabel{nullptr};
    QLabel* displayingLabel{nullptr};
    
    QPushButton* okButton{nullptr};
    QPushButton* cancelButton{nullptr};
    QPushButton* applyButton{nullptr};
    QPushButton* helpButton{nullptr};
    
    QColor currentColor;
};

AST_NAMESPACE_END
