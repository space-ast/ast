///
/// @file      ReportStyleLoader.hpp
/// @brief     报表样式加载器
/// @details   提供从 ReportStyle 文件（.rst/.rsg）加载报表样式数据的功能。
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
#include "AstReport/ReportStyle.hpp"

AST_NAMESPACE_BEGIN

// 前置声明
class BKVParser;

/*!
    @addtogroup AstLoader
    @{
*/

/// @brief 从文件加载报表样式
/// @param filepath 文件路径（.rst 或 .rsg 文件）
/// @param report   [out] 报表样式数据
/// @return 错误码
AST_LOADER_API errc_t aLoadReportStyle(StringView filepath, ReportStyle& report);

/// @brief 从已打开的 BKVParser 加载报表样式
/// @param parser  已定位到 ReportStyle 块内部的 BKVParser
/// @param report  [out] 报表样式数据
/// @return 错误码
AST_LOADER_API errc_t aLoadReportStyle(BKVParser& parser, ReportStyle& report);

/*! @} */

AST_NAMESPACE_END
