///
/// @file      ReportLine.hpp
/// @brief     报表数据行定义
/// @details   定义报表中的数据行（ReportLine）类型，
///            对应 ReportStyle 文件中 Section 内的 Line 块。
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
#include "ReportElement.hpp"

#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstReport
    @{
*/

/// @brief 报表数据行
/// @details 对应 ReportStyle 文件中 Section 内的 Line 块。
///          每个 Line 包含多个 Element，在表格报告中对应一行数据列定义，
///          在图形样式中对应一条数据曲线。
class AST_REPORT_API ReportLine
{
public:
    std::string name_{};   ///< 行名称（如 "Line 1"）
    std::string title_{};  ///< 图形样式的 Y 轴/序列标题（表格样式不适用）

    std::vector<ReportElement> elements_{};
};

/*! @} */

AST_NAMESPACE_END
