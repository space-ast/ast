#include "UiODEVarStepIntegrator.hpp"
#include "AstMath/ODEVarStepIntegrator.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

AST_NAMESPACE_BEGIN

UiODEVarStepIntegrator::UiODEVarStepIntegrator(Object* object, QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiODEVarStepIntegrator");
    setupUi();
    if (object) {
        setObject(object);
        setODEVarStepIntegrator(dynamic_cast<ODEVarStepIntegrator*>(object));
    }
}

UiODEVarStepIntegrator::UiODEVarStepIntegrator(QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiODEVarStepIntegrator");
    setupUi();
}

void UiODEVarStepIntegrator::setupUi()
{
    // 主布局
    mainLayout_ = new QVBoxLayout(this);
    
    // 积分器选择
    integratorLayout_ = new QHBoxLayout();
    integratorLabel_ = new QLabel("数值积分器:", this);
    integratorCombo_ = new QComboBox(this);
    integratorCombo_->addItem("RK4th5th");
    integratorCombo_->addItem("RK5th6th");
    integratorCombo_->addItem("RK6th7th");
    integratorLayout_->addWidget(integratorLabel_);
    integratorLayout_->addWidget(integratorCombo_);
    mainLayout_->addLayout(integratorLayout_);
    
    // 初始步长
    initialStepLayout_ = new QHBoxLayout();
    initialStepLabel_ = new QLabel("初始步长:", this);
    initialStepEdit_ = new QDoubleSpinBox(this);
    initialStepEdit_->setRange(1e-6, 86400);
    initialStepEdit_->setValue(5);
    initialStepEdit_->setDecimals(6);
    initialStepUnit_ = new QLabel("秒", this);
    initialStepLayout_->addWidget(initialStepLabel_);
    initialStepLayout_->addWidget(initialStepEdit_);
    initialStepLayout_->addWidget(initialStepUnit_);
    mainLayout_->addLayout(initialStepLayout_);
    
    // 步长控制
    stepSizeGroup_ = new QGroupBox("步长控制", this);
    stepSizeLayout_ = new QVBoxLayout(stepSizeGroup_);
    
    // 固定步长
    fixedStepLayout_ = new QHBoxLayout();
    fixedStepCheck_ = new QCheckBox("使用固定步长", this);
    fixedStepLayout_->addWidget(fixedStepCheck_);
    stepSizeLayout_->addLayout(fixedStepLayout_);
    
    // 最大步长
    maxStepLayout_ = new QHBoxLayout();
    maxStepCheck_ = new QCheckBox(this);
    maxStepCheck_->setChecked(true);
    maxStepLabel_ = new QLabel("最大步长:", this);
    maxStepEdit_ = new QDoubleSpinBox(this);
    maxStepEdit_->setRange(1e-6, 86400);
    maxStepEdit_->setValue(86400);
    maxStepEdit_->setDecimals(6);
    maxStepUnit_ = new QLabel("秒", this);
    maxStepLayout_->addWidget(maxStepCheck_);
    maxStepLayout_->addWidget(maxStepLabel_);
    maxStepLayout_->addWidget(maxStepEdit_);
    maxStepLayout_->addWidget(maxStepUnit_);
    stepSizeLayout_->addLayout(maxStepLayout_);
    
    // 最小步长
    minStepLayout_ = new QHBoxLayout();
    minStepCheck_ = new QCheckBox(this);
    minStepCheck_->setChecked(true);
    minStepLabel_ = new QLabel("最小步长:", this);
    minStepEdit_ = new QDoubleSpinBox(this);
    minStepEdit_->setRange(1e-6, 86400);
    minStepEdit_->setValue(1);
    minStepEdit_->setDecimals(6);
    minStepUnit_ = new QLabel("秒", this);
    minStepLayout_->addWidget(minStepCheck_);
    minStepLayout_->addWidget(minStepLabel_);
    minStepLayout_->addWidget(minStepEdit_);
    minStepLayout_->addWidget(minStepUnit_);
    stepSizeLayout_->addLayout(minStepLayout_);
    
    mainLayout_->addWidget(stepSizeGroup_);
    
    // 误差控制
    errorControlLayout_ = new QHBoxLayout();
    errorControlLabel_ = new QLabel("误差控制:", this);
    errorControlCombo_ = new QComboBox(this);
    errorControlCombo_->addItem("按分量相对误差");
    errorControlCombo_->addItem("按分量绝对误差");
    errorControlLayout_->addWidget(errorControlLabel_);
    errorControlLayout_->addWidget(errorControlCombo_);
    mainLayout_->addLayout(errorControlLayout_);
    
    // 最大绝对误差
    maxAbsErrorLayout_ = new QHBoxLayout();
    maxAbsErrorLabel_ = new QLabel("最大绝对误差:", this);
    maxAbsErrorEdit_ = new QDoubleSpinBox(this);
    maxAbsErrorEdit_->setRange(1e-16, 1e-1);
    maxAbsErrorEdit_->setValue(1e-10);
    maxAbsErrorEdit_->setDecimals(16);
    maxAbsErrorEdit_->setMinimumWidth(150);
    maxAbsErrorLayout_->addWidget(maxAbsErrorLabel_);
    maxAbsErrorLayout_->addWidget(maxAbsErrorEdit_);
    mainLayout_->addLayout(maxAbsErrorLayout_);
    
    // 最大相对误差
    maxRelErrorLayout_ = new QHBoxLayout();
    maxRelErrorLabel_ = new QLabel("最大相对误差:", this);
    maxRelErrorEdit_ = new QDoubleSpinBox(this);
    maxRelErrorEdit_->setRange(1e-16, 1e-1);
    maxRelErrorEdit_->setValue(1e-13);
    maxRelErrorEdit_->setDecimals(16);
    maxRelErrorEdit_->setMinimumWidth(150);
    maxRelErrorLayout_->addWidget(maxRelErrorLabel_);
    maxRelErrorLayout_->addWidget(maxRelErrorEdit_);
    mainLayout_->addLayout(maxRelErrorLayout_);
    
    // 最大迭代次数
    maxIterationsLayout_ = new QHBoxLayout();
    maxIterationsLabel_ = new QLabel("最大迭代次数:", this);
    maxIterationsEdit_ = new QSpinBox(this);
    maxIterationsEdit_->setRange(1, 1000);
    maxIterationsEdit_->setValue(100);
    maxIterationsLayout_->addWidget(maxIterationsLabel_);
    maxIterationsLayout_->addWidget(maxIterationsEdit_);
    mainLayout_->addLayout(maxIterationsLayout_);
    
    // 高安全系数
    highSafetyLayout_ = new QHBoxLayout();
    highSafetyLabel_ = new QLabel("高安全系数:", this);
    highSafetyEdit_ = new QDoubleSpinBox(this);
    highSafetyEdit_->setRange(0.1, 2.0);
    highSafetyEdit_->setValue(0.9);
    highSafetyEdit_->setDecimals(2);
    highSafetyLayout_->addWidget(highSafetyLabel_);
    highSafetyLayout_->addWidget(highSafetyEdit_);
    mainLayout_->addLayout(highSafetyLayout_);
    
    // 低安全系数
    lowSafetyLayout_ = new QHBoxLayout();
    lowSafetyLabel_ = new QLabel("低安全系数:", this);
    lowSafetyEdit_ = new QDoubleSpinBox(this);
    lowSafetyEdit_->setRange(0.1, 2.0);
    lowSafetyEdit_->setValue(0.9);
    lowSafetyEdit_->setDecimals(2);
    lowSafetyLayout_->addWidget(lowSafetyLabel_);
    lowSafetyLayout_->addWidget(lowSafetyEdit_);
    mainLayout_->addLayout(lowSafetyLayout_);
    
    // 连接信号槽
    connect(fixedStepCheck_, &QCheckBox::toggled, this, &UiODEVarStepIntegrator::onFixedStepChanged);
    
    // 初始化
    refreshUi();
}

ODEVarStepIntegrator* UiODEVarStepIntegrator::getODEVarStepIntegrator() const
{
    return dynamic_cast<ODEVarStepIntegrator*>(getObject());
}

void UiODEVarStepIntegrator::setODEVarStepIntegrator(ODEVarStepIntegrator* integrator)
{
    if (integrator) {
        setObject(integrator);
        refreshUi();
    }
}

void UiODEVarStepIntegrator::refreshUi()
{
    auto integrator = getODEVarStepIntegrator();
    if (!integrator) return;
    
    // 刷新积分器选择
    // @todo: 根据实际的积分器类型设置
    
    // 刷新初始步长
    // initialStepEdit_->setValue(integrator->initialStep());
    
    // 刷新步长控制
    // fixedStepCheck_->setChecked(integrator->useFixedStep());
    // maxStepCheck_->setChecked(integrator->useMaxStep());
    // maxStepEdit_->setValue(integrator->maxStep());
    // minStepCheck_->setChecked(integrator->useMinStep());
    // minStepEdit_->setValue(integrator->minStep());
    
    // 刷新误差控制
    // errorControlCombo_->setCurrentIndex(static_cast<int>(integrator->errorControl()));
    // maxAbsErrorEdit_->setValue(integrator->maxAbsError());
    // maxRelErrorEdit_->setValue(integrator->maxRelError());
    // maxIterationsEdit_->setValue(integrator->maxIterations());
    
    // 刷新安全系数
    // highSafetyEdit_->setValue(integrator->highSafetyCoefficient());
    // lowSafetyEdit_->setValue(integrator->lowSafetyCoefficient());
    
    // 更新UI状态
    onFixedStepChanged(fixedStepCheck_->isChecked());
}

void UiODEVarStepIntegrator::apply()
{
    auto integrator = getODEVarStepIntegrator();
    if (integrator) {
        applyTo(integrator);
        emit odeVarStepIntegratorChanged(integrator);
    }
}

void UiODEVarStepIntegrator::applyTo(ODEVarStepIntegrator* integrator)
{
    if (integrator) {
        // 应用积分器选择
        // @todo: 根据选择的积分器类型设置
        
        // 应用初始步长
        // integrator->setInitialStep(initialStepEdit_->value());
        
        // 应用步长控制
        // integrator->setUseFixedStep(fixedStepCheck_->isChecked());
        // integrator->setUseMaxStep(maxStepCheck_->isChecked());
        // integrator->setMaxStep(maxStepEdit_->value());
        // integrator->setUseMinStep(minStepCheck_->isChecked());
        // integrator->setMinStep(minStepEdit_->value());
        
        // 应用误差控制
        // integrator->setErrorControl(static_cast<ErrorControlType>(errorControlCombo_->currentIndex()));
        // integrator->setMaxAbsError(maxAbsErrorEdit_->value());
        // integrator->setMaxRelError(maxRelErrorEdit_->value());
        // integrator->setMaxIterations(maxIterationsEdit_->value());
        
        // 应用安全系数
        // integrator->setHighSafetyCoefficient(highSafetyEdit_->value());
        // integrator->setLowSafetyCoefficient(lowSafetyEdit_->value());
    }
}

void UiODEVarStepIntegrator::onFixedStepChanged(bool checked)
{
    // 当使用固定步长时，禁用最大步长和最小步长
    maxStepCheck_->setEnabled(!checked);
    maxStepEdit_->setEnabled(!checked && maxStepCheck_->isChecked());
    minStepCheck_->setEnabled(!checked);
    minStepEdit_->setEnabled(!checked && minStepCheck_->isChecked());
}

AST_NAMESPACE_END