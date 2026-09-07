///
/// @file      UiSTKEphemerisFileWriter.hpp
/// @brief     STK 星历文件导出界面对话框 — 仿 STK Ephemeris 导出
/// @details   用于将星历（Ephemeris/Point）以 STK (.e) 格式导出到文件。
///   界面分为「Details」「Time Period」「Steps」三组及底部「Output File / Export」，
///   布局与 STK 的星历文件导出对话框保持一致。配置项直接映射到
///   STKEphemerisFileWriter 的对应参数。
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/STKEphemerisFileWriter.hpp"
#include "AstCore/TimeInterval.hpp"
#include <QWidget>

class QComboBox;
class QRadioButton;

AST_NAMESPACE_BEGIN

class Point;
class UiQuantity;
class UiTimePoint;

/*!
    @addtogroup GUI
    @{
*/

/// @brief STK 星历文件导出界面
/// @details 配置 STK 星历的导出参数并以 .e 格式写入文件。
///          - Coordinate：输出参考轴系（Auto/ICRF/J2000/Inertial/Fixed）
///          - Central Body：输出中心天体（默认取星历参考系天体）
///          - Time Period：整段星历或指定起止时段
///          - Steps：使用星历自带离散点或固定步长
///          导出通过 exportTo(path) 或便捷函数 aUiExportSTKEphemeris 完成。
class AST_GUI_API UiSTKEphemerisFileWriter : public QWidget
{
    Q_OBJECT
public:
    explicit UiSTKEphemerisFileWriter(QWidget* parent = nullptr);
    ~UiSTKEphemerisFileWriter() override;
    UiSTKEphemerisFileWriter(const UiSTKEphemerisFileWriter&) = delete;
    UiSTKEphemerisFileWriter& operator=(const UiSTKEphemerisFileWriter&) = delete;

    /// @brief 绑定要导出的星历对象
    /// @param ephemeris 星历对象（Point 派生）
    void setEphemeris(Point* ephemeris);

    /// @brief 获取绑定的星历对象
    /// @return 星历对象指针
    Point* getEphemeris() const;

    /// @brief 获取当前选定的参考轴系
    /// @return 参考轴系枚举
    STKEphemerisFileWriter::ECoordinate coordinate() const;

    /// @brief 获取当前选定的中心天体
    /// @return 中心天体指针
    Body* centralBody() const;

    /// @brief 获取当前导出的时间区间
    /// @return 时间区间（整段时返回 TimeInterval::Whole()）
    TimeInterval interval() const;

    /// @brief 获取当前导出的时间步长（秒）
    /// @return 步长（秒）；非正值表示使用星历自身离散时间点
    double step() const;

    /// @brief 用当前界面配置导出星历到指定文件
    /// @param filepath 输出文件路径
    /// @return 错误码，eNoError 表示成功
    errc_t exportTo(const QString& filepath);

signals:
    /// @brief 导出的星历文件发生错误或完成后发出
    void ephemerisExported(const QString& filepath, errc_t err);

private slots:
    void onTimeModeChanged();
    void onStepModeChanged();

private:
    void setupUi();
    void populateBodies();
    void updateTimeControls();
    void updateStepControls();
    void refreshFromEphemeris();

    // —— 按界面当前状态配置底层写入器（就地配置，避免拷贝非拷贝类） ——
    void configureWriter(STKEphemerisFileWriter& writer) const;

    // ---- Details ----
    QComboBox* coordinateCombo_  = nullptr;
    QComboBox* centralBodyCombo_ = nullptr;

    // ---- Time Period ----
    QRadioButton* entireRadi_  = nullptr;
    QRadioButton* specifyRadi_ = nullptr;
    UiTimePoint* startEdit_    = nullptr;
    UiTimePoint* stopEdit_     = nullptr;

    // ---- Steps ----
    QRadioButton* ephStepsRadi_ = nullptr;
    QRadioButton* stepSizeRadi_ = nullptr;
    UiQuantity*   stepSizeEdit_ = nullptr;

    Point* ephemeris_ = nullptr;   ///< 绑定的星历对象
};

/*! @} */

/// @brief 弹出模态对话框导出 STK 星历文件
/// @param ephemeris 要导出的星历对象
/// @param parent 父窗口
/// @return eNoError 表示成功导出；其余为错误码（含用户取消情况由界面自行提示）
AST_GUI_CAPI errc_t aUiExportSTKEphemeris(Point* ephemeris, QWidget* parent = nullptr);

AST_NAMESPACE_END
