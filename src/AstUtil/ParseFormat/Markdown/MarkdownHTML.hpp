///
/// @file      MarkdownHTML.hpp
/// @brief     Markdown HTML 渲染器 — SAX 事件驱动，输出 HTML 标签
/// @details   将 MarkdownSax 事件转为 HTML 标签输出。
///            支持：标题（h1-h6）、粗体、斜体、行内代码、代码块、
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
#include "MarkdownSax.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


/// @brief Markdown SAX → HTML 渲染器
/// @details 实现 MarkdownSax 接口，将 SAX 事件转换为 HTML 标签输出。
///          与 MarkdownParser 配合使用：
///          @code
///          MarkdownHTML renderer;
///          MarkdownParser parser(renderer);
///          parser.feed(markdownText);
///          parser.flush();
///          std::string html = renderer.output();
///          @endcode
class AST_UTIL_API MarkdownHTML: public MarkdownSax
{
public:
    MarkdownHTML() = default;
    ~MarkdownHTML() = default;

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
    void codeSpan(StringView code) override;
    void startLink(StringView url) override;
    void endLink() override;
    void image(StringView alt, StringView url) override;

public:
    /// @brief 获取输出的 HTML 字符串
    std::string& output() { return output_; }
    const std::string& output() const { return output_; }
    void clearOutput() { output_.clear(); }

private:
    /// @brief 将文本中的 HTML 特殊字符转义后追加到输出
    static void appendEscaped(std::string& out, StringView text);

    // ---- 输出缓冲 ----
    std::string output_;

    // ---- 块级状态 ----
    std::vector<bool> listStack_;   ///< 列表类型栈: true=有序, false=无序
    int  blockquoteDepth_ = 0;      ///< 块引用嵌套深度

    // ---- 链接状态 ----
    std::string linkUrl_;
};


/*! @} */

AST_NAMESPACE_END
