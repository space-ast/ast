///
/// @file      ReportSection.hpp
/// @brief     报表数据段定义
/// @details   定义报表中的数据段（ReportSection）类型，
///            对应 ReportStyle 文件中的 Section 块。
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
#include "ReportLine.hpp"

#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstReport
    @{
*/

/// @brief 报表数据段
/// @details 对应 ReportStyle 文件中的 Section 块。
///          一个 Section 包含多个 Line，每个 Line 包含多个 Element。
class AST_REPORT_API ReportSection
{
public:
    std::string name_;             ///< 段名称（如 "Section 1"）
    std::string className_;        ///< 对象类型名（如 "Satellite", "Facility"）
    bool        nameInTitle_ = true;
    EExpandMethod expandMethod_ = EExpandMethod::eNone; ///< 展开方式
    int         propMask_     = 2;
    bool        showIntervals_ = false;
    int         numIntervals_  = 0;

    std::vector<ReportLine> lines_;
};

/*! @} */

AST_NAMESPACE_END
