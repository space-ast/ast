///
/// @file      DataReport.hpp
/// @brief     报表样式定义
/// @details   定义报表样式（DataReport）类型及相关枚举，
///            对应 ReportStyle 文件的完整结构。
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
#include "DataSection.hpp"
#include "AstUtil/Color.hpp"

#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstReport
    @{
*/

// 枚举定义见 DataElement.hpp

/// @brief 报表样式
/// @details 对应 ReportStyle 文件的完整结构，包含文件头属性、图形显示参数
///          以及按 Section → Line → Element 组织的报表数据列定义。
class AST_REPORT_API DataReport
{
public:
    // ---- 标识 ----
    std::string name_;       ///< 报表名称（如 "Classical Orbit Elements"）
    std::string className_;  ///< 对象类型名（来自 ClassId 块的 Class 字段）

    // ---- 文件头属性 ----
    EStyleType  styleType_ = EStyleType::eTabular;
    std::string title_;          ///< 报表标题
    bool        showDate_      = true;
    bool        showName_      = true;
    bool        showDescShort_ = false;
    bool        showDescLong_  = false;

    // ---- 坐标轴/比例 ----
    bool yLog10_  = false;
    bool y2Log10_ = false;
    bool verticalGridLines_   = false;
    bool horizontalGridLines_ = false;
    int  numAnnotations_       = 3;
    int  annotationRotation_   = 1;

    // ---- 外观 ----
    Color  backgroundColor_  = Color(0xFFFFFFFF);   ///< 背景色（默认白色）
    double viewableDuration_ = 0.0;
    bool   realTimeMode_     = false;
    bool   readOnlyMode_     = false;
    int    dayLinesStatus_   = 1;
    int    legendStatus_     = 1;

    // ---- 数据段 ----
    std::vector<DataSection> sections_;
};

/*! @} */

AST_NAMESPACE_END
