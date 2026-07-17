///
/// @file      MarkdownTableParser.hpp
/// @brief     Markdown 表格流式解析状态机 — SAX 事件驱动
/// @details   逐字符解析 GFM 表格语法（| 分隔的 header + separator + body），
///            通过 MarkdownSax 表格事件输出，单元格内委托 MarkdownInlineParser
///            处理行内格式。
/// @author    axel
/// @date      2026-06-17
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
#include "MarkdownInlineParser.hpp"
#include "MarkdownSax.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


class MarkdownSax;

/// @brief  Markdown 表格流式解析状态机
/// @details 与 MarkdownBlockParser 配合使用：
///          1. BlockSM 在行首检测到 | 时调用 feedChar('|')
///          2. 后续逐字符 feedChar(c)，直到 isIdle() 为 true
///          3. 内部自动完成 header/separator/body 行判定与 SAX 事件发射
///
///          表格语法（GFM）：
///          | Header 1 | Header 2 |
///          |:---------|:--------:|
///          | Cell 1   | Cell 2   |
///
///          解析流程：
///          - eHeaderRow: 缓冲第一行 cell 原始文本
///          - eSeparator: 解析分隔行，提取列对齐；确认后发射 startTable + 表头
///          - eBodyRow:   逐行解析 cell，委托 inlineSM 处理后发射 SAX 事件
///
///          若表头行后未跟随有效分隔行，自动回退为段落文本输出。
class AST_UTIL_API MarkdownTableParser
{
public:
    explicit MarkdownTableParser(MarkdownSax& sax);
    ~MarkdownTableParser() = default;

    void feed(StringView chunk);
    
    /// @brief  逐字符输入
    /// @param c 当前字符
    /// @note   当表格结束时（遇到非表格行），当前字符可能未被消费；
    ///         调用方应在 isIdle() 为 true 后检查 isCharConsumed()。
    void feedChar(char c);

    /// @brief  输入结束，关闭所有打开的表格结构
    void finish();

    /// @brief  重置状态机
    void reset();

    /// @brief  是否处于空闲状态（未在解析表格）
    bool isIdle() const { return state_ == EState::eIdle; }

    /// @brief  feedChar 返回后，最近一次输入的字符是否已被消费
    /// @details 若为 false，调用方需用同一字符重新进行行首分类
    bool isCharConsumed() const { return charConsumed_; }

private:
    // ---- 内部状态 ----
    enum class EState
    {
        eIdle,        ///< 空闲，不在表格内
        eHeaderRow,   ///< 正在累积表头行
        eSeparator,   ///< 正在解析分隔行
        eBodyRow,     ///< 正在解析数据行
    };

    // ---- 行处理 ----
    /// @brief  处理完整的一行（在 \n 时调用）
    void processRow(const std::string& rowContent);

    /// @brief  处理表头行（缓冲 cell）
    void processHeaderRow(const std::string& row);
    /// @brief  处理分隔行（提取对齐，确认表格）
    void processSeparatorRow(const std::string& row);
    /// @brief  处理数据行（发射 SAX 事件）
    void processBodyRow(const std::string& row);

    // ---- 表格结构发射 ----
    /// @brief  分隔行确认后：发射 startTable + 表头 + startTableBody
    void emitTableStart();
    /// @brief  结束当前表格：发射 endTableBody + endTable
    void emitTableEnd();

    // ---- 回退 ----
    /// @brief  回退：将已缓冲的行作为段落文本输出
    void abortAsParagraph();

    // ---- 辅助 ----
    /// @brief  将行按 | 分割为单元格（首尾 | 可选），对每个 cell 做 trim
    static std::vector<std::string> splitCells(const std::string& row);
    /// @brief  判断是否为合法表格行（以 | 开头）
    static bool isTableRow(const std::string& row);
    /// @brief  判断是否为合法分隔行（仅含 | - : 空格，且至少一个 -）
    static bool isSeparatorRow(const std::string& row);
    /// @brief  从分隔行提取列对齐
    static std::vector<ETableAlign> parseAlignments(const std::string& sepRow);
    /// @brief  trim 字符串首尾空格与制表符
    static std::string trim(const std::string& s);

    // ---- 状态成员 ----
    MarkdownSax& sax_;
    MarkdownInlineParser inlineSM_;

    EState state_ = EState::eIdle;
    bool charConsumed_ = true;

    // 行缓冲（逐字符累积，\n 时处理并清空）
    std::string rowBuf_{};

    // 表头缓冲（分隔行确认前暂存）
    std::vector<std::string> headerCells_{};

    // 列对齐（分隔行解析后确定）
    std::vector<ETableAlign> colAligns_{};

    // 回退文本（表格未确认时，累积的原始行文本，用于回退为段落）
    std::string abortBuf_{};
};


/*! @} */

AST_NAMESPACE_END
