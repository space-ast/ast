#include "UiODEVarStepIntegrator.hpp"
#include "AstMath/ODEVarStepIntegrator.hpp"
#include "AstGUI/UiDouble.hpp"
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
    
    // 初始步长
    initialStepLayout_ = new QHBoxLayout();
    initialStepLabel_ = new QLabel(tr("初始步长:"), this);
    initialStepEdit_ = new QDoubleSpinBox(this);
    initialStepEdit_->setRange(1e-6, 86400);
    initialStepEdit_->setValue(5);
    initialStepEdit_->setDecimals(6);
    initialStepUnit_ = new QLabel(tr("秒"), this);
    initialStepLayout_->addWidget(initialStepLabel_);
    initialStepLayout_->addWidget(initialStepEdit_);
    initialStepLayout_->addWidget(initialStepUnit_);
    mainLayout_->addLayout(initialStepLayout_);
    
    // 步长控制
    stepSizeGroup_ = new QGroupBox(tr("步长控制"), this);
    stepSizeLayout_ = new QVBoxLayout(stepSizeGroup_);
    
    // 最大步长
    maxStepLayout_ = new QHBoxLayout();
    maxStepCheck_ = new QCheckBox(this);
    maxStepCheck_->setChecked(true);
    maxStepLabel_ = new QLabel(tr("最大步长:"), this);
    maxStepEdit_ = new QDoubleSpinBox(this);
    maxStepEdit_->setRange(1e-6, 86400);
    maxStepEdit_->setValue(86400);
    maxStepEdit_->setDecimals(6);
    maxStepUnit_ = new QLabel(tr("秒"), this);
    maxStepLayout_->addWidget(maxStepCheck_);
    maxStepLayout_->addWidget(maxStepLabel_);
    maxStepLayout_->addWidget(maxStepEdit_);
    maxStepLayout_->addWidget(maxStepUnit_);
    stepSizeLayout_->addLayout(maxStepLayout_);
    
    // 最小步长
    minStepLayout_ = new QHBoxLayout();
    minStepCheck_ = new QCheckBox(this);
    minStepCheck_->setChecked(true);
    minStepLabel_ = new QLabel(tr("最小步长:"), this);
    minStepEdit_ = new QDoubleSpinBox(this);
    minStepEdit_->setRange(1e-6, 86400);
    minStepEdit_->setValue(1);
    minStepEdit_->setDecimals(6);
    minStepUnit_ = new QLabel(tr("秒"), this);
    minStepLayout_->addWidget(minStepCheck_);
    minStepLayout_->addWidget(minStepLabel_);
    minStepLayout_->addWidget(minStepEdit_);
    minStepLayout_->addWidget(minStepUnit_);
    stepSizeLayout_->addLayout(minStepLayout_);
    
    mainLayout_->addWidget(stepSizeGroup_);
    
    // 误差控制
    errorControlLayout_ = new QHBoxLayout();
    errorControlLabel_ = new QLabel(tr("误差控制:"), this);
    errorControlCombo_ = new QComboBox(this);
    errorControlCombo_->addItem(tr("按分量相对误差"));
    errorControlCombo_->addItem(tr("按分量绝对误差"));
    errorControlLayout_->addWidget(errorControlLabel_);
    errorControlLayout_->addWidget(errorControlCombo_);
    mainLayout_->addLayout(errorControlLayout_);
    
    // 最大绝对误差
    maxAbsErrorLayout_ = new QHBoxLayout();
    maxAbsErrorLabel_ = new QLabel(tr("最大绝对误差:"), this);
    maxAbsErrorEdit_ = new UiDouble(this);
    maxAbsErrorEdit_->setValue(1e-10);
    maxAbsErrorLayout_->addWidget(maxAbsErrorLabel_);
    maxAbsErrorLayout_->addWidget(maxAbsErrorEdit_);
    mainLayout_->addLayout(maxAbsErrorLayout_);

    // 最大相对误差
    maxRelErrorLayout_ = new QHBoxLayout();
    maxRelErrorLabel_ = new QLabel(tr("最大相对误差:"), this);
    maxRelErrorEdit_ = new UiDouble(this);
    maxRelErrorEdit_->setValue(1e-13);
    maxRelErrorLayout_->addWidget(maxRelErrorLabel_);
    maxRelErrorLayout_->addWidget(maxRelErrorEdit_);
    mainLayout_->addLayout(maxRelErrorLayout_);
    
    // 最大迭代次数
    maxIterationsLayout_ = new QHBoxLayout();
    maxIterationsLabel_ = new QLabel(tr("最大迭代次数:"), this);
    maxIterationsEdit_ = new QSpinBox(this);
    maxIterationsEdit_->setRange(1, 1000);
    maxIterationsEdit_->setValue(100);
    maxIterationsLayout_->addWidget(maxIterationsLabel_);
    maxIterationsLayout_->addWidget(maxIterationsEdit_);
    mainLayout_->addLayout(maxIterationsLayout_);
    
    // 高安全系数
    highSafetyLayout_ = new QHBoxLayout();
    highSafetyLabel_ = new QLabel(tr("高安全系数:"), this);
    highSafetyEdit_ = new QDoubleSpinBox(this);
    highSafetyEdit_->setRange(0.1, 2.0);
    highSafetyEdit_->setValue(0.9);
    highSafetyEdit_->setSingleStep(0.1);
    highSafetyEdit_->setDecimals(2);
    highSafetyLayout_->addWidget(highSafetyLabel_);
    highSafetyLayout_->addWidget(highSafetyEdit_);
    mainLayout_->addLayout(highSafetyLayout_);
    
    // 低安全系数
    lowSafetyLayout_ = new QHBoxLayout();
    lowSafetyLabel_ = new QLabel(tr("低安全系数:"), this);
    lowSafetyEdit_ = new QDoubleSpinBox(this);
    lowSafetyEdit_->setRange(0.1, 2.0);
    lowSafetyEdit_->setValue(0.9);
    lowSafetyEdit_->setSingleStep(0.1);
    lowSafetyEdit_->setDecimals(2);
    lowSafetyLayout_->addWidget(lowSafetyLabel_);
    lowSafetyLayout_->addWidget(lowSafetyEdit_);
    mainLayout_->addLayout(lowSafetyLayout_);
    
    // 连接信号槽
    connect(maxStepCheck_, &QCheckBox::toggled, maxStepEdit_, &QWidget::setEnabled);
    connect(minStepCheck_, &QCheckBox::toggled, minStepEdit_, &QWidget::setEnabled);

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

    initialStepEdit_->setValue(integrator->getStepSize());

    maxStepCheck_->setChecked(integrator->getUseMaxStep());
    maxStepEdit_->setValue(integrator->getMaxStepSize());
    maxStepEdit_->setEnabled(integrator->getUseMaxStep());

    minStepCheck_->setChecked(integrator->getUseMinStep());
    minStepEdit_->setValue(integrator->getMinStepSize());
    minStepEdit_->setEnabled(integrator->getUseMinStep());

    maxAbsErrorEdit_->setValue(integrator->maxAbsErr_);

    maxRelErrorEdit_->setValue(integrator->maxRelErr_);

    maxIterationsEdit_->setValue(integrator->getMaxIterations());

    highSafetyEdit_->setValue(integrator->getSafetyCoeffHigh());

    lowSafetyEdit_->setValue(integrator->getSafetyCoeffLow());
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
    if (!integrator)
        return;

    integrator->setInitialStepSize(initialStepEdit_->value());
    integrator->setUseMaxStep(maxStepCheck_->isChecked());
    integrator->setMaxStepSize(maxStepEdit_->value());
    integrator->setUseMinStep(minStepCheck_->isChecked());
    integrator->setMinStepSize(minStepEdit_->value());
    integrator->setMaxAbsErr(maxAbsErrorEdit_->value());
    integrator->setMaxRelErr(maxRelErrorEdit_->value());
    integrator->setMaxIterations(maxIterationsEdit_->value());
    integrator->setSafetyCoeffHigh(highSafetyEdit_->value());
    integrator->setSafetyCoeffLow(lowSafetyEdit_->value());
}

AST_NAMESPACE_END