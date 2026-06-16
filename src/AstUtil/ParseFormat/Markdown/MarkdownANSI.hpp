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
private:
    // ---- ANSI 辅助 ----
    void emitActiveStyles();
    void emitStyleTransition(int oldStyles, int newStyles);
    void emitBlockPrefix();
    void outputCodeBlockGutter(int lineNum);

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
        Type type = QUOTE;
    };

    // ---- 输出缓冲 ----
    std::string output_;

    // ---- 行内状态 ----
    int activeStyles_ = STYLE_NONE;

    // ---- 块级状态 ----
    std::vector<BlockFrame> blockStack_;
    int  listItemNumber_ = 0;
    bool listOrdered_    = false;
    bool inCodeBlock_    = false;
    int  codeLineNumber_ = 0;

    // ---- 链接状态 ----
    std::string linkUrl_;
};

/*! @} */

AST_NAMESPACE_END
