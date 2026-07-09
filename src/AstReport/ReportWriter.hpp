///
/// @file      ReportWriter.hpp
/// @brief     数据报表生成
/// @details   根据 ReportStyle 模板，从对象中提取数据并写出报表。
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
#include "ReportStyle.hpp"
#include "AstCore/TimeInterval.hpp"

#include <cstdio>

AST_NAMESPACE_BEGIN

// 前置声明
class Object;

/*!
    @addtogroup AstReport
    @{
*/

class ReportGenerateOptions
{
public:
    bool useObjectInterval_{true};
    TimeInterval interval_{};
};

/// @brief 根据报表模板生成对象的数据报告
/// @param file   输出流
/// @param report 报表模板定义（Section → Line → Element 结构）
/// @param object 数据源对象
/// @return 错误码
AST_REPORT_API errc_t aWriteReport(const ReportStyle& report, const Object* object, FILE* file, const ReportGenerateOptions& options = ReportGenerateOptions{});

/*! @} */

AST_NAMESPACE_END
