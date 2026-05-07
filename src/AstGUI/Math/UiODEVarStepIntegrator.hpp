///
/// @file      UiODEVarStepIntegrator.hpp
/// @brief     变步长ODE积分器配置界面
/// @author    axel
/// @date      2026-04-23
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

AST_NAMESPACE_BEGIN

class ODEVarStepIntegrator;

class AST_GUI_API UiODEVarStepIntegrator: public UiObject
{
    Q_OBJECT
public:
    UiODEVarStepIntegrator(Object* object, QWidget *parent = nullptr);
    UiODEVarStepIntegrator(QWidget *parent = nullptr);
    ~UiODEVarStepIntegrator() = default;

    ODEVarStepIntegrator* getODEVarStepIntegrator() const;
    void setODEVarStepIntegrator(ODEVarStepIntegrator* integrator);
    void refreshUi();
    void apply();
    void applyTo(ODEVarStepIntegrator* integrator);

signals:
    void odeVarStepIntegratorChanged(ODEVarStepIntegrator* integrator);
protected:
    void setupUi();
private slots:
    void onFixedStepChanged(bool checked);
private:
    // 布局
    QVBoxLayout* mainLayout_{nullptr};
    QHBoxLayout* integratorLayout_{nullptr};
    QLabel* integratorLabel_{nullptr};
    QComboBox* integratorCombo_{nullptr};
    
    // 初始步长
    QHBoxLayout* initialStepLayout_{nullptr};
    QLabel* initialStepLabel_{nullptr};
    QDoubleSpinBox* initialStepEdit_{nullptr};
    QLabel* initialStepUnit_{nullptr};
    
    // 步长控制
    QGroupBox* stepSizeGroup_{nullptr};
    QVBoxLayout* stepSizeLayout_{nullptr};
    QHBoxLayout* fixedStepLayout_{nullptr};
    QCheckBox* fixedStepCheck_{nullptr};
    QLabel* fixedStepLabel_{nullptr};
    
    QHBoxLayout* maxStepLayout_{nullptr};
    QCheckBox* maxStepCheck_{nullptr};
    QLabel* maxStepLabel_{nullptr};
    QDoubleSpinBox* maxStepEdit_{nullptr};
    QLabel* maxStepUnit_{nullptr};
    
    QHBoxLayout* minStepLayout_{nullptr};
    QCheckBox* minStepCheck_{nullptr};
    QLabel* minStepLabel_{nullptr};
    QDoubleSpinBox* minStepEdit_{nullptr};
    QLabel* minStepUnit_{nullptr};
    
    // 误差控制
    QHBoxLayout* errorControlLayout_{nullptr};
    QLabel* errorControlLabel_{nullptr};
    QComboBox* errorControlCombo_{nullptr};
    
    QHBoxLayout* maxAbsErrorLayout_{nullptr};
    QLabel* maxAbsErrorLabel_{nullptr};
    QDoubleSpinBox* maxAbsErrorEdit_{nullptr};
    
    QHBoxLayout* maxRelErrorLayout_{nullptr};
    QLabel* maxRelErrorLabel_{nullptr};
    QDoubleSpinBox* maxRelErrorEdit_{nullptr};
    
    QHBoxLayout* maxIterationsLayout_{nullptr};
    QLabel* maxIterationsLabel_{nullptr};
    QSpinBox* maxIterationsEdit_{nullptr};
    
    // 安全系数
    QHBoxLayout* highSafetyLayout_{nullptr};
    QLabel* highSafetyLabel_{nullptr};
    QDoubleSpinBox* highSafetyEdit_{nullptr};
    
    QHBoxLayout* lowSafetyLayout_{nullptr};
    QLabel* lowSafetyLabel_{nullptr};
    QDoubleSpinBox* lowSafetyEdit_{nullptr};
    
    
};

AST_NAMESPACE_END