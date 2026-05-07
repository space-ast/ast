///
/// @file      UiDragForce.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-22
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

#include "UiDragForce.hpp"
#include "AstCore/DragForce.hpp"
#include <QGroupBox>

AST_NAMESPACE_BEGIN

UiDragForce::UiDragForce(Object* object, QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiDragForce");
    setupUi();
    if (object) {
        setObject(object);
        setDragForce(dynamic_cast<DragForce*>(object));
    }
}

UiDragForce::UiDragForce(QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiDragForce");
    setupUi();
}

void UiDragForce::setupUi()
{
    // 主布局
    mainLayout_ = new QVBoxLayout(this);
    
    // 模型配置
    modelGroup_ = new QGroupBox("模型", this);
    modelLayout_ = new QGridLayout(modelGroup_);
    
    typeLayout_ = new QHBoxLayout();
    typeLabel_ = new QLabel("类型:", this);
    typeCombo_ = new QComboBox(this);
    typeCombo_->addItem("球形");
    typeLayout_->addWidget(typeLabel_);
    typeLayout_->addWidget(typeCombo_);
    modelLayout_->addLayout(typeLayout_, 0, 0, 1, 2);
    
    cdLayout_ = new QHBoxLayout();
    cdLabel_ = new QLabel("Cd:", this);
    cdEdit_ = new UiDouble(this);
    cdEdit_->setValue(2.2);
    cdLayout_->addWidget(cdLabel_);
    cdLayout_->addWidget(cdEdit_);
    modelLayout_->addLayout(cdLayout_, 1, 0);
    
    areaMassLayout_ = new QHBoxLayout();
    areaMassLabel_ = new QLabel("面积/质量比:", this);
    areaMassEdit_ = new UiQuantity(this);
    areaMassEdit_->setValueInUnit(0.02, Unit("m^2/kg"));
    areaMassLayout_->addWidget(areaMassLabel_);
    areaMassLayout_->addWidget(areaMassEdit_);
    modelLayout_->addLayout(areaMassLayout_, 1, 1);
    
    // 大气密度模型
    atmDensityGroup_ = new QGroupBox("大气密度模型", this);
    atmDensityLayout_ = new QGridLayout(atmDensityGroup_);
    
    mainAtmLayout_ = new QHBoxLayout();
    mainAtmLabel_ = new QLabel("主模型:", this);
    mainAtmCombo_ = new QComboBox(this);
    mainAtmLayout_->addWidget(mainAtmLabel_);
    mainAtmLayout_->addWidget(mainAtmCombo_);
    atmDensityLayout_->addLayout(mainAtmLayout_, 0, 0, 1, 2);
    
    lowAltAtmLayout_ = new QHBoxLayout();
    lowAltAtmLabel_ = new QLabel("低高度模型:", this);
    lowAltAtmCombo_ = new QComboBox(this);
    lowAltAtmLayout_->addWidget(lowAltAtmLabel_);
    lowAltAtmLayout_->addWidget(lowAltAtmCombo_);
    atmDensityLayout_->addLayout(lowAltAtmLayout_, 1, 0, 1, 2);
    
    blendingRangeLayout_ = new QHBoxLayout();
    blendingRangeLabel_ = new QLabel("混合过渡范围:", this);
    blendingRangeEdit_ = new UiQuantity(this);
    blendingRangeEdit_->setValueInUnit(30.0, Unit("km"));
    blendingRangeLayout_->addWidget(blendingRangeLabel_);
    blendingRangeLayout_->addWidget(blendingRangeEdit_);
    atmDensityLayout_->addLayout(blendingRangeLayout_, 2, 0, 1, 2);
    
    // 太阳通量/地磁指数
    solarGeoMagGroup_ = new QGroupBox("太阳通量/地磁指数", this);
    solarGeoMagLayout_ = new QGridLayout(solarGeoMagGroup_);
    
    fluxSourceLayout_ = new QHBoxLayout();
    fluxSourceLabel_ = new QLabel("数据来源:", this);
    fluxSourceCombo_ = new QComboBox(this);
    fluxSourceCombo_->addItem("手动输入");
    fluxSourceCombo_->addItem("文件输入");
    fluxSourceLayout_->addWidget(fluxSourceLabel_);
    fluxSourceLayout_->addWidget(fluxSourceCombo_);
    solarGeoMagLayout_->addLayout(fluxSourceLayout_, 0, 0, 1, 2);
    
    // 手动输入参数
    f10p7DailyLayout_ = new QHBoxLayout();
    f10p7DailyLabel_ = new QLabel("F10.7日值:", this);
    f10p7DailyEdit_ = new UiDouble(this);
    f10p7DailyEdit_->setValue(150.0);
    f10p7DailyLayout_->addWidget(f10p7DailyLabel_);
    f10p7DailyLayout_->addWidget(f10p7DailyEdit_);
    solarGeoMagLayout_->addLayout(f10p7DailyLayout_, 1, 0);
    
    f10p7AverageLayout_ = new QHBoxLayout();
    f10p7AverageLabel_ = new QLabel("F10.7平均值:", this);
    f10p7AverageEdit_ = new UiDouble(this);
    f10p7AverageEdit_->setValue(150.0);
    f10p7AverageLayout_->addWidget(f10p7AverageLabel_);
    f10p7AverageLayout_->addWidget(f10p7AverageEdit_);
    solarGeoMagLayout_->addLayout(f10p7AverageLayout_, 1, 1);
    
    kpLayout_ = new QHBoxLayout();
    kpLabel_ = new QLabel("地磁指数(Kp):", this);
    kpEdit_ = new UiDouble(this);
    kpEdit_->setValue(3.0);
    kpLayout_->addWidget(kpLabel_);
    kpLayout_->addWidget(kpEdit_);
    solarGeoMagLayout_->addLayout(kpLayout_, 2, 0, 1, 2);
    
    // 文件输入参数
    fluxApFileLayout_ = new QHBoxLayout();
    fluxApFileLabel_ = new QLabel("空间天气文件:", this);
    fluxApFileEdit_ = new UiFilePath(this);
    // fluxApFileEdit_->setFileMode(UiFilePath::FileMode::OpenFile);
    fluxApFileEdit_->setFilter("Text files (*.txt);;All files (*.*)");
    fluxApFileLayout_->addWidget(fluxApFileLabel_);
    fluxApFileLayout_->addWidget(fluxApFileEdit_);
    solarGeoMagLayout_->addLayout(fluxApFileLayout_, 3, 0, 1, 2);
    
    geoMagFluxUpdateRateLayout_ = new QHBoxLayout();
    geoMagFluxUpdateRateLabel_ = new QLabel("更新频率:", this);
    geoMagFluxUpdateRateCombo_ = new QComboBox(this);
    geoMagFluxUpdateRateLayout_->addWidget(geoMagFluxUpdateRateLabel_);
    geoMagFluxUpdateRateLayout_->addWidget(geoMagFluxUpdateRateCombo_);
    solarGeoMagLayout_->addLayout(geoMagFluxUpdateRateLayout_, 4, 0);
    
    geoMagFluxSourceLayout_ = new QHBoxLayout();
    geoMagFluxSourceLabel_ = new QLabel("地磁指数类型:", this);
    geoMagFluxSourceCombo_ = new QComboBox(this);
    geoMagFluxSourceLayout_->addWidget(geoMagFluxSourceLabel_);
    geoMagFluxSourceLayout_->addWidget(geoMagFluxSourceCombo_);
    solarGeoMagLayout_->addLayout(geoMagFluxSourceLayout_, 4, 1);
    
    // 计算选项
    calcOptionsGroup_ = new QGroupBox("计算选项", this);
    calcOptionsLayout_ = new QVBoxLayout(calcOptionsGroup_);
    
    useApproxAltLayout_ = new QHBoxLayout();
    useApproxAltCheck_ = new QCheckBox("使用近似高度", this);
    useApproxAltCheck_->setChecked(true);
    useApproxAltLayout_->addWidget(useApproxAltCheck_);
    calcOptionsLayout_->addLayout(useApproxAltLayout_);
    
    sunPositionLayout_ = new QHBoxLayout();
    sunPositionLabel_ = new QLabel("太阳位置计算方法:", this);
    sunPositionCombo_ = new QComboBox(this);
    sunPositionLayout_->addWidget(sunPositionLabel_);
    sunPositionLayout_->addWidget(sunPositionCombo_);
    calcOptionsLayout_->addLayout(sunPositionLayout_);
    
    // 添加到主布局
    mainLayout_->addWidget(modelGroup_);
    mainLayout_->addWidget(atmDensityGroup_);
    mainLayout_->addWidget(solarGeoMagGroup_);
    mainLayout_->addWidget(calcOptionsGroup_);
    mainLayout_->addStretch();
    
    // 连接信号槽
    connect(fluxSourceCombo_, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
        bool useFile = (index == 1);
        f10p7DailyEdit_->setVisible(!useFile);
        f10p7DailyLabel_->setVisible(!useFile);
        f10p7AverageEdit_->setVisible(!useFile);
        f10p7AverageLabel_->setVisible(!useFile);
        kpEdit_->setVisible(!useFile);
        kpLabel_->setVisible(!useFile);
        fluxApFileEdit_->setVisible(useFile);
        fluxApFileLabel_->setVisible(useFile);
        geoMagFluxUpdateRateCombo_->setVisible(useFile);
        geoMagFluxUpdateRateLabel_->setVisible(useFile);
        geoMagFluxSourceCombo_->setVisible(useFile);
        geoMagFluxSourceLabel_->setVisible(useFile);
    });
    
    // 刷新UI
    refreshAtmDensityModel();
    refreshGeoMagFluxSource();
    refreshGeoMagFluxUpdateRate();
    refreshSunPosition();
    
    // 初始化文件输入参数为不可见
    fluxApFileEdit_->setVisible(false);
    fluxApFileLabel_->setVisible(false);
    geoMagFluxUpdateRateCombo_->setVisible(false);
    geoMagFluxUpdateRateLabel_->setVisible(false);
    geoMagFluxSourceCombo_->setVisible(false);
    geoMagFluxSourceLabel_->setVisible(false);
}

DragForce* UiDragForce::getDragForce() const
{
    return dynamic_cast<DragForce*>(getObject());
}

void UiDragForce::setDragForce(DragForce* drag)
{
    if (drag) {
        setObject(drag);
        refreshUi();
    }
}

void UiDragForce::refreshUi()
{
    auto drag = getDragForce();
    if (!drag) return;
    
    // 大气密度模型
    mainAtmCombo_->setCurrentIndex(static_cast<int>(drag->atmDensityModel_));
    lowAltAtmCombo_->setCurrentIndex(static_cast<int>(drag->lowAltAtmDensityModel_));
    blendingRangeEdit_->setValue(drag->atmBlendingRange_);
    
    // 太阳通量/地磁指数
    fluxSourceCombo_->setCurrentIndex(drag->useFluxApFile_ ? 1 : 0);
    f10p7AverageEdit_->setValue(drag->f10p7Average_);
    f10p7DailyEdit_->setValue(drag->f10p7Daily_);
    kpEdit_->setValue(drag->kp_);
    fluxApFileEdit_->setPath(drag->fluxApFile_);
    geoMagFluxUpdateRateCombo_->setCurrentIndex(static_cast<int>(drag->geoMagFluxUpdateRate_));
    geoMagFluxSourceCombo_->setCurrentIndex(static_cast<int>(drag->geoMagFluxSource_));
    
    // 计算选项
    useApproxAltCheck_->setChecked(drag->useApproxAltForDrag_);
    sunPositionCombo_->setCurrentIndex(static_cast<int>(drag->sunPosition_));
}

void UiDragForce::apply()
{
    auto drag = getDragForce();
    if (drag) {
        applyTo(drag);
    }
}

void UiDragForce::applyTo(DragForce* drag)
{
    if (!drag) return;
    
    // 大气密度模型
    drag->atmDensityModel_ = static_cast<EAtmDensityModel>(mainAtmCombo_->currentIndex());
    drag->lowAltAtmDensityModel_ = static_cast<EAtmDensityModel>(lowAltAtmCombo_->currentIndex());
    drag->atmBlendingRange_ = blendingRangeEdit_->value();
    
    // 太阳通量/地磁指数
    drag->useFluxApFile_ = (fluxSourceCombo_->currentIndex() == 1);
    drag->f10p7Average_ = f10p7AverageEdit_->value();
    drag->f10p7Daily_ = f10p7DailyEdit_->value();
    drag->kp_ = kpEdit_->value();
    drag->fluxApFile_ = fluxApFileEdit_->path().toUtf8().data();
    drag->geoMagFluxUpdateRate_ = static_cast<EGeoMagFluxUpdateRate>(geoMagFluxUpdateRateCombo_->currentIndex());
    drag->geoMagFluxSource_ = static_cast<EGeoMagFluxSource>(geoMagFluxSourceCombo_->currentIndex());
    
    // 计算选项
    drag->useApproxAltForDrag_ = useApproxAltCheck_->isChecked();
    drag->sunPosition_ = static_cast<ESunPosition>(sunPositionCombo_->currentIndex());
}

void UiDragForce::refreshAtmDensityModel()
{
    mainAtmCombo_->clear();
    lowAltAtmCombo_->clear();
    
    QStringList models = {
        "无",
        "1976标准",
        "Harris-Priester",
        "Jacchia 60",
        "Jacchia 70",
        "Jacchia 71",
        "Jacchia-Roberts",
        "MSIS 1986",
        "MSISE 1990",
        "NRLMSISE 2000",
        "CIRA 72",
        "DTM 2012"
    };
    
    mainAtmCombo_->addItems(models);
    lowAltAtmCombo_->addItems(models);
    
    // 默认值
    mainAtmCombo_->setCurrentIndex(static_cast<int>(EAtmDensityModel::e1976Standard));
    lowAltAtmCombo_->setCurrentIndex(static_cast<int>(EAtmDensityModel::eNone));
}

void UiDragForce::refreshGeoMagFluxSource()
{
    geoMagFluxSourceCombo_->clear();
    geoMagFluxSourceCombo_->addItem("Kp");
    geoMagFluxSourceCombo_->addItem("Ap");
}

void UiDragForce::refreshGeoMagFluxUpdateRate()
{
    geoMagFluxUpdateRateCombo_->clear();
    geoMagFluxUpdateRateCombo_->addItem("每日");
    geoMagFluxUpdateRateCombo_->addItem("每3小时");
    geoMagFluxUpdateRateCombo_->addItem("每3小时插值");
    geoMagFluxUpdateRateCombo_->addItem("每3小时三次样条");
}

void UiDragForce::refreshSunPosition()
{
    sunPositionCombo_->clear();
    sunPositionCombo_->addItem("真实太阳位置");
    sunPositionCombo_->addItem("视太阳到真实中心天体");
    sunPositionCombo_->addItem("视太阳位置");
}

void UiDragForce::onFluxApFileChanged(bool checked)
{
    // 处理空间天气文件输入变化
}

AST_NAMESPACE_END