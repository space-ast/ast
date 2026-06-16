///
/// @file      MarkdownANSI.cpp
/// @brief     Markdown ANSI 渲染器 — SAX 事件驱动，输出 ANSI 终端转义序列
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

#include "MarkdownANSI.hpp"
#include "AstUtil/Escape.hpp"

AST_NAMESPACE_BEGIN

using namespace escape;

// ============================================================================
// Additional ANSI sequences (not in escape::)
// ============================================================================


// 代码块 256 色样式（与 Markdown.cc 保持一致）
static const char kCodeBg[]     = "\033[48;5;236m";  // #303030 背景
static const char kCodeGutter[] = "\033[38;5;240m";  // #585858 gutter │
static const char kCodeLineNo[] = "\033[38;5;243m";  // #767676 行号
static const char kCodeText[]   = "\033[38;5;252m";  // #d0d0d0 代码文本
static const char kCodeFence[]  = "\033[38;5;240m";  // #585858 围栏标记
static const char kCodeLang[]   = "\033[38;5;228m";  // #ffff87 语言标签

// UTF-8 符号
static const char kBullet[] = "\xe2\x80\xa2";  // "•"
static const char kBar[]    = "\xe2\x94\x82";  // "│"
static const char kHr[]     = "\xe2\x94\x80";  // "─"

// ============================================================================
// 标题 ANSI 样式（按级别递减权重）
// ============================================================================
static const char* headingAnsiOpen(int level)
{
    switch (level)
    {
    case 1: return "\033[1m\033[97m";   // 粗体+亮白 — 最突出
    case 2: return "\033[1m\033[96m";   // 粗体+亮青
    case 3: return "\033[1m\033[36m";   // 粗体+青
    case 4: return "\033[1m\033[34m";   // 粗体+蓝
    case 5: return "\033[34m";          // 蓝
    case 6: return "\033[2m\033[34m";   // 暗+蓝 — 最弱
    default: return "";
    }
}

// ============================================================================
// 内部辅助
// ============================================================================

void MarkdownANSI::emitActiveStyles()
{
    if (activeStyles_ & STYLE_BOLD)   output_ += kBold;
    if (activeStyles_ & STYLE_ITALIC) output_ += kItalic;
    if (activeStyles_ & STYLE_DELETE) output_ += kStrike;
    if (activeStyles_ & STYLE_LINK)   { output_ += kUnderline; output_ += kBlue; }
}

void MarkdownANSI::emitStyleTransition(int oldStyles, int newStyles)
{
    if (oldStyles == newStyles) return;

    if (newStyles == STYLE_NONE) { output_ += kReset; return; }
    if (oldStyles != STYLE_NONE)  output_ += kReset;

    if (newStyles & STYLE_BOLD)   output_ += kBold;
    if (newStyles & STYLE_ITALIC) output_ += kItalic;
    if (newStyles & STYLE_DELETE) output_ += kStrike;
    if (newStyles & STYLE_LINK)   { output_ += kUnderline; output_ += kBlue; }
}

void MarkdownANSI::emitBlockPrefix()
{
    // 遍历块栈，为每层块引用输出前缀
    for (size_t i = 0; i < blockStack_.size(); ++i)
    {
        output_ += kDim;
        output_ += kBar;
        output_ += " ";
        output_ += kReset;
        emitActiveStyles();
    }
}

void MarkdownANSI::outputCodeBlockGutter(int lineNum)
{
    output_ += kCodeBg;       // 深灰背景
    output_ += kCodeGutter;   // gutter 颜色
    output_ += kBar;          // │
    output_ += " ";
    output_ += kCodeLineNo;   // 行号颜色
    // 右对齐到 3 位
    if (lineNum < 10)       output_ += "  ";
    else if (lineNum < 100) output_ += " ";
    output_ += std::to_string(lineNum);
    output_ += " ";
    output_ += kCodeText;     // 代码文本颜色
}

// ============================================================================
// 文档生命周期
// ============================================================================

void MarkdownANSI::startDocument()
{
    output_.clear();
    activeStyles_  = STYLE_NONE;
    blockStack_.clear();
    listItemNumber_ = 0;
    listOrdered_    = false;
    inCodeBlock_    = false;
    codeLineNumber_ = 0;
    linkUrl_.clear();
}

void MarkdownANSI::endDocument()
{
    // 关闭所有活跃行内样式
    if (activeStyles_ != STYLE_NONE)
    {
        output_ += kReset;
        activeStyles_ = STYLE_NONE;
    }

    // 确保文档以换行结尾
    if (!output_.empty() && output_.back() != '\n')
        output_ += '\n';
}

// ============================================================================
// 块级元素 — 标题
// ============================================================================

void MarkdownANSI::startHeading(int level)
{
    output_ += headingAnsiOpen(level);
}

void MarkdownANSI::endHeading(int level)
{
    (void)level;
    output_ += kReset;
    output_ += "\n\n";
}

// ============================================================================
// 块级元素 — 段落
// ============================================================================

void MarkdownANSI::startParagraph()
{
    // 输出块引用前缀（如果在引用中）
    emitBlockPrefix();
}

void MarkdownANSI::endParagraph()
{
    output_ += "\n\n";
}

// ============================================================================
// 块级元素 — 代码块
// ============================================================================

void MarkdownANSI::startCodeBlock(StringView language)
{
    inCodeBlock_    = true;
    codeLineNumber_ = 1;

    // 围栏标记（暗灰）
    output_ += kCodeFence;
    output_ += "```";
    if (!language.empty())
    {
        output_ += " ";
        output_ += kCodeLang;
        output_.append(language.data(), language.size());
    }
    output_ += kReset;
    output_ += '\n';
}

void MarkdownANSI::codeLine(StringView line)
{
    if (!inCodeBlock_) return;

    outputCodeBlockGutter(codeLineNumber_);
    output_.append(line.data(), line.size());
    // EL (Erase in Line) — 清除至行尾，终端用当前背景色填充，
    // 解决中文字符宽度不一致导致的锯齿问题
    output_ += "\033[K";
    output_ += kReset;
    output_ += '\n';
    codeLineNumber_++;
}

void MarkdownANSI::endCodeBlock()
{
    if (!inCodeBlock_) return;

    // 闭合围栏
    output_ += kCodeFence;
    output_ += "```";
    output_ += kReset;
    output_ += "\n\n";

    inCodeBlock_    = false;
    codeLineNumber_ = 0;
}

// ============================================================================
// 块级元素 — 列表
// ============================================================================

void MarkdownANSI::startList(bool ordered)
{
    // 列表前确保换行（与前一个块分隔）
    if (!output_.empty() && output_.back() != '\n')
        output_ += '\n';

    listOrdered_    = ordered;
    listItemNumber_ = 0;
}

void MarkdownANSI::startListItem()
{
    listItemNumber_++;

    // 输出块引用前缀（如果在引用中）
    emitBlockPrefix();

    output_ += kYellow;
    if (listOrdered_)
    {
        output_ += std::to_string(listItemNumber_);
        output_ += ". ";
    }
    else
    {
        output_ += kBullet;
        output_ += " ";
    }
    output_ += kReset;
    emitActiveStyles();
}

void MarkdownANSI::endListItem()
{
    output_ += '\n';
}

void MarkdownANSI::endList()
{
    // 列表后空行分隔
    output_ += '\n';
    listItemNumber_ = 0;
}

// ============================================================================
// 块级元素 — 引用
// ============================================================================

void MarkdownANSI::startBlockquote()
{
    BlockFrame bf = { BlockFrame::QUOTE };
    blockStack_.push_back(bf);
}

void MarkdownANSI::endBlockquote()
{
    if (!blockStack_.empty())
        blockStack_.pop_back();
}

// ============================================================================
// 块级元素 — 分割线
// ============================================================================

void MarkdownANSI::horizontalRule()
{
    // 分割线前确保换行
    if (!output_.empty() && output_.back() != '\n')
        output_ += '\n';

    output_ += kDim;
    for (int i = 0; i < 60; ++i) output_ += kHr;
    output_ += kReset;
    output_ += "\n\n";
}

// ============================================================================
// 行内元素 — 文本
// ============================================================================

void MarkdownANSI::text(StringView txt)
{
    output_.append(txt.data(), txt.size());
}

// ============================================================================
// 行内元素 — 斜体
// ============================================================================

void MarkdownANSI::startEmphasis()
{
    int old = activeStyles_;
    activeStyles_ |= STYLE_ITALIC;
    emitStyleTransition(old, activeStyles_);
}

void MarkdownANSI::endEmphasis()
{
    int old = activeStyles_;
    activeStyles_ &= ~STYLE_ITALIC;
    emitStyleTransition(old, activeStyles_);
}

// ============================================================================
// 行内元素 — 粗体
// ============================================================================

void MarkdownANSI::startStrong()
{
    int old = activeStyles_;
    activeStyles_ |= STYLE_BOLD;
    emitStyleTransition(old, activeStyles_);
}

void MarkdownANSI::endStrong()
{
    int old = activeStyles_;
    activeStyles_ &= ~STYLE_BOLD;
    emitStyleTransition(old, activeStyles_);
}

// ============================================================================
// 行内元素 — 删除线
// ============================================================================

void MarkdownANSI::startDelete()
{
    int old = activeStyles_;
    activeStyles_ |= STYLE_DELETE;
    emitStyleTransition(old, activeStyles_);
}

void MarkdownANSI::endDelete()
{
    int old = activeStyles_;
    activeStyles_ &= ~STYLE_DELETE;
    emitStyleTransition(old, activeStyles_);
}

// ============================================================================
// 行内元素 — 行内代码
// ============================================================================

void MarkdownANSI::codeSpan(StringView code)
{
    output_ += kCyan;
    output_.append(code.data(), code.size());
    output_ += kReset;
    emitActiveStyles();
}

// ============================================================================
// 行内元素 — 链接
// ============================================================================

void MarkdownANSI::startLink(StringView url)
{
    linkUrl_.assign(url.data(), url.size());

    int old = activeStyles_;
    activeStyles_ |= STYLE_LINK;
    emitStyleTransition(old, activeStyles_);
}

void MarkdownANSI::endLink()
{
    int old = activeStyles_;
    activeStyles_ &= ~STYLE_LINK;
    emitStyleTransition(old, activeStyles_);

    // 输出 URL
    output_ += " (";
    output_ += kBlue;
    output_ += linkUrl_;
    output_ += kReset;
    emitActiveStyles();
    output_ += ")";

    linkUrl_.clear();
}

// ============================================================================
// 行内元素 — 图片
// ============================================================================

void MarkdownANSI::image(StringView alt, StringView url)
{
    output_ += kDim;
    output_ += "[Image: ";
    output_.append(alt.data(), alt.size());
    if (!url.empty())
    {
        output_ += " (";
        output_.append(url.data(), url.size());
        output_ += ")";
    }
    output_ += "]";
    output_ += kReset;
    emitActiveStyles();
}

AST_NAMESPACE_END
