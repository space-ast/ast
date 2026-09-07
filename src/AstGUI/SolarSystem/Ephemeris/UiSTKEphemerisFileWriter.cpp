///
/// @file      UiSTKEphemerisFileWriter.cpp
/// @brief     STK 星历文件导出界面实现
/// @details   ~
/// @author    axel
/// @date      2026-09-07
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

#include "UiSTKEphemerisFileWriter.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/SolarSystem.hpp"
#include "AstCore/RunTimeSolarSystem.hpp"
#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstGUI/UiTimePoint.hpp"
#include "AstGUI/UiQuantity.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>

AST_NAMESPACE_BEGIN

using namespace units;

namespace{

/// @brief 从星历参考系推断坐标系统（尽量与写入器一致）
/// @param frame 星历参考系
/// @return 坐标系统枚举；无法推断时返回 eAuto
STKEphemerisFileWriter::ECoordinate coordinateFromFrame(Frame* frame)
{
    if(!frame)
        return STKEphemerisFileWriter::eAuto;
    if(frame->getAxes() == aAxesICRF())
        return STKEphemerisFileWriter::eICRF;
    if(frame->getAxes() == aAxesJ2000())
        return STKEphemerisFileWriter::eJ2000;
    return STKEphemerisFileWriter::eInertial;
}

/// @brief 判断天体是否为质心（不应作为中心天体）
bool isBarycenter(const CelestialBody* body)
{
    return body && body->getName().find("Barycenter") != std::string::npos;
}

}

// ============================================================================
// 构造 / 析构
// ============================================================================

UiSTKEphemerisFileWriter::UiSTKEphemerisFileWriter(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
    populateBodies();
}

UiSTKEphemerisFileWriter::~UiSTKEphemerisFileWriter() = default;

// ============================================================================
// 界面构建
// ============================================================================

void UiSTKEphemerisFileWriter::setupUi()
{
    auto* rootLayout = new QVBoxLayout(this);

    // ---------- 上半部分：左详情 + 右时段/步长 ----------
    auto* topLayout = new QHBoxLayout();
    rootLayout->addLayout(topLayout);

    // ---- 左列：参考系 ----
    auto* frameGroup = new QGroupBox(tr("参考系"), this);
    auto* frameLayout = new QGridLayout(frameGroup);
    frameLayout->setColumnStretch(1, 1);

    // 中心天体
    frameLayout->addWidget(new QLabel(tr("中心天体"), frameGroup), 0, 0);
    centralBodyCombo_ = new QComboBox(frameGroup);
    frameLayout->addWidget(centralBodyCombo_, 0, 1);

    // 坐标系统
    frameLayout->addWidget(new QLabel(tr("坐标系"), frameGroup), 1, 0);
    coordinateCombo_ = new QComboBox(frameGroup);
    coordinateCombo_->addItem(tr("自动"),     static_cast<int>(STKEphemerisFileWriter::eAuto));
    coordinateCombo_->addItem(tr("ICRF"),     static_cast<int>(STKEphemerisFileWriter::eICRF));
    coordinateCombo_->addItem(tr("J2000"),    static_cast<int>(STKEphemerisFileWriter::eJ2000));
    coordinateCombo_->addItem(tr("天体惯性系"),     static_cast<int>(STKEphemerisFileWriter::eInertial));
    coordinateCombo_->addItem(tr("天体固连系"),     static_cast<int>(STKEphemerisFileWriter::eFixed));
    coordinateCombo_->setCurrentIndex(1);   // 默认 ICRF
    frameLayout->addWidget(coordinateCombo_, 1, 1);

    frameLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 0);

    topLayout->addWidget(frameGroup, 3);

    // ---- 右列：Time Period + Steps ----
    auto* rightLayout = new QVBoxLayout();
    topLayout->addLayout(rightLayout, 4);

    // Time Period
    auto* timeGroup = new QGroupBox(tr("时间区间"), this);
    auto* timeLayout = new QVBoxLayout(timeGroup);

    entireRadi_ = new QRadioButton(tr("使用整个星历"), timeGroup);
    entireRadi_->setChecked(true);
    specifyRadi_ = new QRadioButton(tr("指定时间区间"), timeGroup);
    auto* timeBtnGroup = new QButtonGroup(timeGroup);
    timeBtnGroup->addButton(entireRadi_);
    timeBtnGroup->addButton(specifyRadi_);
    timeLayout->addWidget(entireRadi_);
    timeLayout->addWidget(specifyRadi_);

    auto* startRow = new QHBoxLayout();
    startRow->addSpacing(20);
    startRow->addWidget(new QLabel(tr("开始:"), timeGroup));
    startEdit_ = new UiTimePoint(timeGroup);
    startRow->addWidget(startEdit_, 1);
    timeLayout->addLayout(startRow);

    auto* stopRow = new QHBoxLayout();
    stopRow->addSpacing(20);
    stopRow->addWidget(new QLabel(tr("停止:"), timeGroup));
    stopEdit_ = new UiTimePoint(timeGroup);
    stopRow->addWidget(stopEdit_, 1);
    timeLayout->addLayout(stopRow);

    rightLayout->addWidget(timeGroup);

    // Steps
    auto* stepGroup = new QGroupBox(tr("步长"), this);
    auto* stepLayout = new QVBoxLayout(stepGroup);

    ephStepsRadi_ = new QRadioButton(tr("使用星历步长"), stepGroup);
    ephStepsRadi_->setChecked(true);
    stepSizeRadi_ = new QRadioButton(tr("使用步长:"), stepGroup);
    auto* stepBtnGroup = new QButtonGroup(stepGroup);
    stepBtnGroup->addButton(ephStepsRadi_);
    stepBtnGroup->addButton(stepSizeRadi_);
    stepLayout->addWidget(ephStepsRadi_);

    auto* stepSizeRow = new QHBoxLayout();
    stepSizeRow->addWidget(stepSizeRadi_);
    stepSizeEdit_ = new UiQuantity(stepGroup);
    stepSizeEdit_->setQuantity(Quantity(60, s));
    stepSizeEdit_->setEnabled(false);
    stepSizeRow->addWidget(stepSizeEdit_, 1);
    stepLayout->addLayout(stepSizeRow);

    rightLayout->addWidget(stepGroup);
    rightLayout->addStretch();

    // ---------- 连接 ----------
    connect(specifyRadi_, &QRadioButton::toggled, this, &UiSTKEphemerisFileWriter::onTimeModeChanged);
    connect(stepSizeRadi_, &QRadioButton::toggled, this, &UiSTKEphemerisFileWriter::onStepModeChanged);

    updateTimeControls();
    updateStepControls();
}

// ============================================================================
// 天体列表
// ============================================================================

void UiSTKEphemerisFileWriter::populateBodies()
{
    centralBodyCombo_->clear();

    auto* solarSystem = aGetSolarSystem();
    if(!solarSystem)
        return;

    auto children = aFindChildren(solarSystem, CelestialBody::StaticType());
    for(auto* obj : children)
    {
        auto* body = static_cast<CelestialBody*>(obj);
        if(isBarycenter(body))
            continue;
        centralBodyCombo_->addItem(QString::fromStdString(body->displayName()),
                                   reinterpret_cast<qulonglong>(body));
    }

    // 回退：未注册子对象时通过 getter 收集
    if(centralBodyCombo_->count() == 0)
    {
        static CelestialBody* (*const kFallbackGetters[])() = {
            aGetEarth, aGetSun, aGetMoon,
            aGetMercury, aGetVenus, aGetMars,
            aGetJupiter, aGetSaturn, aGetUranus, aGetNeptune, aGetPluto,
        };
        for(auto getter : kFallbackGetters)
        {
            auto* body = getter();
            if(!body)
                continue;
            centralBodyCombo_->addItem(QString::fromStdString(body->getName()),
                                       reinterpret_cast<qulonglong>(body));
        }
    }
}

// ============================================================================
// 公共接口
// ============================================================================

void UiSTKEphemerisFileWriter::setEphemeris(Point* ephemeris)
{
    ephemeris_ = ephemeris;
    refreshFromEphemeris();
}

Point* UiSTKEphemerisFileWriter::getEphemeris() const
{
    return ephemeris_;
}

STKEphemerisFileWriter::ECoordinate UiSTKEphemerisFileWriter::coordinate() const
{
    return static_cast<STKEphemerisFileWriter::ECoordinate>(
        coordinateCombo_->currentData().toInt());
}

Body* UiSTKEphemerisFileWriter::centralBody() const
{
    int idx = centralBodyCombo_->currentIndex();
    if(idx < 0)
        return nullptr;
    return reinterpret_cast<Body*>(centralBodyCombo_->itemData(idx).value<qulonglong>());
}

TimeInterval UiSTKEphemerisFileWriter::interval() const
{
    if(specifyRadi_->isChecked())
        return TimeInterval(startEdit_->getTimePoint(), stopEdit_->getTimePoint());
    return TimeInterval::Whole();
}

double UiSTKEphemerisFileWriter::step() const
{
    if(stepSizeRadi_->isChecked())
        return stepSizeEdit_->getValueSI();
    // 使用星历自带离散点 → 非正步长表示取原生时间点
    return 0.0;
}

errc_t UiSTKEphemerisFileWriter::exportTo(const QString& filepath)
{
    if(!ephemeris_)
    {
        aWarning("no ephemeris is bound for STK export");
        return eErrorInvalidParam;
    }
    if(filepath.isEmpty())
    {
        aWarning("output file path is empty");
        return eErrorInvalidParam;
    }

    STKEphemerisFileWriter writer;
    configureWriter(writer);
    errc_t rc = writer.write(filepath.toStdString(), *ephemeris_);

    emit ephemerisExported(filepath, rc);
    return rc;
}

// ============================================================================
// 构建写入器
// ============================================================================

void UiSTKEphemerisFileWriter::configureWriter(STKEphemerisFileWriter& writer) const
{
    writer.setInterval(interval());
    writer.setStep(step());
    writer.setCoordinate(coordinate());
    // 中心天体：未在下拉框选择时传空，写入器自动取星历参考系对应天体
    writer.setBody(centralBody());
}

// ============================================================================
// 槽
// ============================================================================

void UiSTKEphemerisFileWriter::onTimeModeChanged()
{
    updateTimeControls();
}

void UiSTKEphemerisFileWriter::onStepModeChanged()
{
    updateStepControls();
}

void UiSTKEphemerisFileWriter::updateTimeControls()
{
    const bool specify = specifyRadi_->isChecked();
    startEdit_->setEnabled(specify);
    stopEdit_->setEnabled(specify);
}

void UiSTKEphemerisFileWriter::updateStepControls()
{
    stepSizeEdit_->setEnabled(stepSizeRadi_->isChecked());
}

void UiSTKEphemerisFileWriter::refreshFromEphemeris()
{
    if(!ephemeris_)
        return;

    // 时间区间预填
    TimeInterval ephInterval;
    if(ephemeris_->getInterval(ephInterval) == eNoError
        && ephInterval.isValid() && !ephInterval.isInf())
    {
        startEdit_->setTimePoint(ephInterval.getStart());
        stopEdit_->setTimePoint(ephInterval.getStop());
    }

    // 坐标系统预选（依据星历参考系）
    if(Frame* frame = ephemeris_->getFrame())
        coordinateCombo_->setCurrentIndex(coordinateCombo_->findData((int)coordinateFromFrame(frame)));

    // 中心天体预选（星历参考系的天体）
    if(Frame* frame = ephemeris_->getFrame())
    {
        if(Body* body = frame->getBody())
        {
            for(int i = 0; i < centralBodyCombo_->count(); ++i)
            {
                if(reinterpret_cast<Body*>(centralBodyCombo_->itemData(i).value<qulonglong>()) == body)
                {
                    centralBodyCombo_->blockSignals(true);
                    centralBodyCombo_->setCurrentIndex(i);
                    centralBodyCombo_->blockSignals(false);
                    break;
                }
            }
        }
    }
}

// ============================================================================
// 便捷函数
// ============================================================================

errc_t aUiExportSTKEphemeris(Point* ephemeris, QWidget* parent)
{
    if(!ephemeris)
        return eErrorInvalidParam;

    QDialog dlg(parent);
    dlg.setWindowTitle(QObject::tr("导出 STK 星历文件"));

    auto* layout = new QVBoxLayout(&dlg);
    auto* form = new UiSTKEphemerisFileWriter(&dlg);
    form->setEphemeris(ephemeris);
    layout->addWidget(form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    layout->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if(dlg.exec() == QDialog::Accepted)
    {
        const QString filepath = QFileDialog::getSaveFileName(
            parent, QObject::tr("导出 STK 星历文件"),
            QStringLiteral("Satellite1.e"),
            QObject::tr("STK 星历文件 (*.e);;所有文件 (*.*)"));
        if(filepath.isEmpty())
            return eErrorInvalidParam;   // 用户取消保存
        return form->exportTo(filepath);
    }

    return eErrorInvalidParam;   // 用户取消
}

AST_NAMESPACE_END
