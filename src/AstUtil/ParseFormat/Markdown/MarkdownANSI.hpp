///
/// @file      MarkdownANSI.hpp
/// @brief     Markdown ANSI 渲染器 — SAX 事件驱动，输出 ANSI 终端转义序列
/// @details   将 MarkdownSax 事件转为 ANSI 转义序列输出。
///            支持：标题（6级）、粗体、斜体、行内代码、代码块（行号+gutter）、
///            无序/有序列表、引用、分割线、链接、图片。
/// @author    axel
/// @date      2026-06-16
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
#include "MarkdownSax.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief Markdown SAX → ANSI 终端转义序列渲染器
/// @details 实现 MarkdownSax 接口，将 SAX 事件转换为带 ANSI 颜色的终端输出。
///          与 MarkdownParser 配合使用：
///          @code
///          MarkdownANSI renderer;
///          MarkdownParser parser(renderer);
///          parser.feed(markdownText);
///          parser.finish();
///          std::string result = renderer.output();
///          @endcode
class AST_UTIL_API MarkdownANSI final : public MarkdownSax
{
public:
    MarkdownANSI() = default;
    ~MarkdownANSI() override = default;

    // 文档生命周期
    void startDocument() override;
    void endDocument() override;

    // 块级元素
    void startHeading(int level) override;
    void endHeading(int level) override;

    void startParagraph() override;
    void endParagraph() override;

    void startCodeBlock(StringView language) override;
    void codeLine(StringView line) override;  // 代码行
    void endCodeBlock() override;

    void startList(bool ordered) override;   // true: 有序, false: 无序
    void startListItem() override;
    void endListItem() override;
    void endList() override;

    void startBlockquote() override;
    void endBlockquote() override;

    void horizontalRule() override;

    // void newline(bool force = false) override;

    // 表格
    void startTable() override;
    void startTableHead() override;
    void endTableHead() override;
    void startTableBody() override;
    void endTableBody() override;
    void startTableRow() override;
    void endTableRow() override;
    void startTableCell(ETableAlign align) override;
    void endTableCell() override;
    void endTable() override;

    // 行内元素
    void text(StringView txt) override;
    void startEmphasis() override;     // 斜体
    void endEmphasis() override;
    void startStrong() override;       // 粗体
    void endStrong() override;
    void startDelete() override;         // 删除线
    void endDelete() override;
    void codeSpan(StringView code) override;
    void startLink(StringView url) override;
    void endLink() override;
    void image(StringView alt, StringView url) override;

public:
    /// @brief 获取输出 ANSI 字符串
    std::string& output() { return output_; }
    const std::string& output() const { return output_; }
    void clearOutput() { output_.clear(); }

    /// @brief 设置是否截断超出列宽的表格单元格内容（默认开启）
    void setTableTruncate(bool enable) { truncateCells_ = enable; }
    bool isTableTruncate() const { return truncateCells_; }

private:
    // ---- ANSI 辅助 ----
    void emitActiveStyles();
    void emitStyleTransition(int oldStyles, int newStyles);
    void emitBlockPrefix();
    void outputCodeBlockGutter(int lineNum);

    /// @brief 获取当前活跃输出缓冲（table cell 内为 cellContent_，否则为 output_）
    std::string& curBuf() { return inCell_ ? cellContent_ : output_; }

    A_DISABLE_COPY(MarkdownANSI);

    // ---- 行内样式位掩码 ----
    enum InlineStyle : int
    {
        STYLE_NONE   = 0,
        STYLE_BOLD   = 1 << 0,  ///< **...**
        STYLE_ITALIC = 1 << 1,  ///< *...* 或 _..._
        STYLE_LINK   = 1 << 2,  ///< [text](url) — 下划线+蓝色
        STYLE_DELETE = 1 << 3,  ///< ~~...~~ — 删除线
    };

    // ---- 块级元素帧（仅用于块引用嵌套跟踪） ----
    struct BlockFrame
    {
        enum Type { QUOTE };
        BlockFrame() = default;
        BlockFrame(Type type) : type(type) {}
        Type type = QUOTE;
    };

    // ---- 输出缓冲 ----
    std::string output_;

    // ---- 行内状态 ----
    int activeStyles_ = STYLE_NONE;

    // ---- 块级状态 ----
    std::vector<BlockFrame> blockStack_;

    /// @brief 列表栈帧（支持嵌套列表的逐级编号与缩进）
    struct ListFrame
    {
        bool ordered;       ///< true=有序, false=无序
        int  itemNumber;    ///< 当前级已输出的列表项序号（0=尚未开始第一项）
    };
    std::vector<ListFrame> listStack_;

    bool inCodeBlock_    = false;
    int  codeLineNumber_ = 0;

    // ---- 表格缓冲 ----
    struct CellData
    {
        std::string rendered;   ///< ANSI 渲染后的内容（含转义序列）
        int         width = 0;  ///< 净显示宽度（剔除转义序列后）
        ETableAlign align = ETableAlign::eDefault;
    };
    struct RowData
    {
        std::vector<CellData> cells;
        bool isHeader = false;
    };

    bool   inTable_       = false;   ///< 当前是否在表格内
    bool   inTableHead_   = false;   ///< 当前是否在表头区域
    bool   inCell_        = false;   ///< 当前是否在 cell 内（inline 事件重定向到 cellContent_）
    ETableAlign cellAlign_ = ETableAlign::eDefault;  ///< 当前 cell 对齐方式
    std::string cellContent_;        ///< cell 内容缓冲（独立于 output_，跨 chunk 不丢失）
    bool   truncateCells_ = true;     ///< 是否截断超出列宽的单元格（默认开启）
    RowData currentRow_;             ///< 当前行缓冲
    std::vector<RowData> tableRows_; ///< 表格所有行（含表头）

    // ---- 链接状态 ----
    std::string linkUrl_;
};

/*! @} */

AST_NAMESPACE_END
