///
/// @file      MarkdownParser.hpp
/// @brief     Markdown 流式解析器 — SAX 事件驱动
/// @details   块级状态机 + 行内状态机，均为逐字符流式解析。
/// @author    axel
/// @date      2026-06-15
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
#include "AstUtil/StringView.hpp"
#include "MarkdownBlockParser.hpp"
#include <string>
#include <vector>
#include <memory>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class MarkdownSax;


AST_UTIL_API std::string aMarkdownANSI(StringView markdown);


/// @brief  Markdown解析器（SAX事件驱动，流式输入）
/// @details 薄封装，组合 MarkdownBlockParser 实现字符级流式解析。
class AST_UTIL_API MarkdownParser {
public:
    MarkdownParser(MarkdownSax& sax)
        : blockSM_(sax)
    {}
    ~MarkdownParser() = default;

    /// @brief  流式输入数据块
    void feed(StringView data) { blockSM_.feed(data); }

    /// @brief  通知输入结束，处理剩余内容
    void finish() { blockSM_.finish(); }

    /// @brief  重置解析器状态
    void reset() { blockSM_.reset(); }

private:
    MarkdownBlockParser blockSM_;
};

/*! @} */

AST_NAMESPACE_END
