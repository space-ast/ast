///
/// @file      ReportElement.hpp
/// @brief     报表数据元素定义
/// @details   定义报表中的数据元素（ReportElement）和单位（ReportUnits）类型，
///            对应 ReportStyle 文件中的 Element 和 Units 块。
/// @author    axel
/// @date      2026-07-02
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
#include "AstUtil/Color.hpp"
#include "AstReportGlobal.hpp"

#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstReport
    @{
*/


/// @brief 报表数据元素中的单位定义
/// @details 对应 ReportStyle 文件中 Element 内嵌的 Units 块。
///          所有字段均为可选，空字符串表示使用场景默认单位。
struct AST_REPORT_API ReportUnits
{
    std::string distanceUnit_;
    std::string timeUnit_;
    std::string dateFormat_;
    std::string angleUnit_;
    std::string massUnit_;
    std::string powerUnit_;
    std::string frequencyUnit_;
    std::string smallDistanceUnit_;
    std::string latitudeUnit_;
    std::string longitudeUnit_;
    std::string durationUnit_;
    std::string temperature_;
    std::string smallTimeUnit_;
    std::string ratioUnit_;
    std::string rcsUnit_;
    std::string dopplerVelocityUnit_;
    std::string sarTimeResProdUnit_;
    std::string forceUnit_;
    std::string pressureUnit_;
    std::string specificImpulseUnit_;
    std::string prfUnit_;
    std::string bandwidthUnit_;
    std::string smallVelocityUnit_;
    std::string dataRateUnit_;
    std::string percent_;
    std::string unitTemperature_;
    std::string missionModelerDistanceUnit_;
    std::string missionModelerTimeUnit_;
    std::string missionModelerAltitudeUnit_;
    std::string missionModelerFuelQuantityUnit_;
    std::string missionModelerRunwayLengthUnit_;
    std::string missionModelerBearingAngleUnit_;
    std::string missionModelerAngleOfAttackUnit_;
    std::string missionModelerAttitudeAngleUnit_;
    std::string missionModelerGUnit_;
    std::string solidAngle_;
    std::string radiationDoseUnit_;
    std::string radiationShieldThicknessUnit_;
    std::string magneticFieldUnit_;
    std::string powerFluxDensityUnit_;
    std::string spectralDensityUnit_;
};

/// @brief 报表数据元素
/// @details 对应 ReportStyle 文件中 Section/Line 内的 Element 块，
///          描述单个数据列（表格报告）或数据序列（图形样式）的属性和数据来源。
class AST_REPORT_API ReportElement
{
public:
    // ---- 标识 ----
    std::string name_;           ///< 元素名称（限定名，如 "Classical Elements-J2000-Semi-major Axis"）
    std::string title_;          ///< 显示标题
    bool        nameInTitle_ = true;

    // ---- 自变量 ----
    bool        isIndepVar_ = false;   ///< 是否为自变量（通常是时间）
    std::string indepVarName_;          ///< 自变量名称（如 "Time"）

    // ---- 数据来源 ----
    std::string service_;   ///< 数据提供者服务名（如 "ModOrbElem", "LLAState", "InviewData"）
    std::string type_;      ///< 数据类型/参考系（如 "J2000", "Fixed", "Sunlight"）
    std::string element_;   ///< 要检索的数据元素名（如 "Semi-major Axis", "Time"）

    // ---- 格式化 ----
    std::string format_;    ///< 输出格式字符串（如 "%.3f", "%.6f", "%d"）

    // ---- 聚合 ----
    int  sumAllowedMask_ = 0;
    bool summaryOnly_    = false;
    int  sumRequestMask_ = 0;

    // ---- 数据类型 ----
    EDataType  dataType_ = EDataType::eFloat; ///< 数据类型
    int        unitType_ = 0;                  ///< 单位类型代码

    // ---- 图形属性（仅图形样式使用） ----
    ELineStyle   lineStyle_  = ELineStyle::eSolid;
    int          lineWidth_  = 0;
    Color        lineColor_  = Color(0xFF000000);
    EPointStyle  pointStyle_ = EPointStyle::eNone;
    int          pointSize_  = 0;
    Color        pointColor_ = Color(0xFF000000);
    EFillPattern fillPattern_ = EFillPattern::eNone;
    Color        fillColor_  = Color(0xFF000000);

    // ---- 其他 ----
    int  propMask_     = 0;
    bool useScenUnits_ = true;   ///< 是否使用场景单位（No 时启用自定义 units_）
    ReportUnits units_;            ///< 自定义单位（仅当 useScenUnits_ == false 时有效）
};

/*! @} */

AST_NAMESPACE_END
