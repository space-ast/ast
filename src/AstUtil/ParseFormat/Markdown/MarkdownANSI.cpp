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
#include "AstUtil/Environment.hpp"

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

// Unicode 表格框线字符 (UTF-8)
namespace {
const char kTblTopLeft[]  = "\xe2\x94\x8c";  // ┌
const char kTblTopT[]     = "\xe2\x94\xac";  // ┬
const char kTblTopRight[] = "\xe2\x94\x90";  // ┐
const char kTblMidLeft[]  = "\xe2\x94\x9c";  // ├
const char kTblMidCross[] = "\xe2\x94\xbc";  // ┼
const char kTblMidRight[] = "\xe2\x94\xa4";  // ┤
const char kTblBotLeft[]  = "\xe2\x94\x94";  // └
const char kTblBotT[]     = "\xe2\x94\xb4";  // ┴
const char kTblBotRight[] = "\xe2\x94\x98";  // ┘
const char kTblHLine[]    = "\xe2\x94\x80";  // ─
const char kTblVLine[]    = "\xe2\x94\x82";  // │

// ---- UTF-8 显示宽度辅助 ----

/// @brief 解码一个 UTF-8 码点并返回码点值；*it 前进到下一码点起始
uint32_t tblUtf8Decode(const char*& it, const char* end)
{
    if (it >= end) return 0;
    unsigned char c = static_cast<unsigned char>(*it);
    if (c < 0x80) { ++it; return c; }

    uint32_t cp = 0;
    int len = 0;
    if      ((c & 0xE0) == 0xC0) { cp = c & 0x1F; len = 1; }
    else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; len = 2; }
    else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; len = 3; }
    else { ++it; return 0xFFFD; }

    ++it;
    for (int i = 0; i < len && it < end; ++i, ++it)
    {
        c = static_cast<unsigned char>(*it);
        if ((c & 0xC0) != 0x80) { cp = 0xFFFD; break; }
        cp = (cp << 6) | (c & 0x3F);
    }
    return cp;
}

/// @brief 判断码点是否为宽字符（终端显示占 2 列）
bool tblIsWideChar(uint32_t cp)
{
    // CJK / 全角字符
    if ((cp >= 0x1100 && cp <= 0x115F)  ||  // Hangul Jamo
        (cp >= 0x2E80 && cp <= 0xA4CF)  ||  // CJK
        (cp >= 0xAC00 && cp <= 0xD7A3)  ||  // Hangul Syllables
        (cp >= 0xF900 && cp <= 0xFAFF)  ||  // CJK Compatibility
        (cp >= 0xFE10 && cp <= 0xFE19)  ||  // Vertical forms
        (cp >= 0xFE30 && cp <= 0xFE6F)  ||  // CJK Compatibility Forms
        (cp >= 0xFF01 && cp <= 0xFF60)  ||  // Fullwidth Forms
        (cp >= 0xFFE0 && cp <= 0xFFE6))     // Fullwidth Signs
        return true;

    // Emoji / 符号（多数现代终端中占 2 列）
    // 注意：该列表持续维护，未覆盖的块逐步补充
    if ((cp >= 0x231A && cp <= 0x231B)  ||  // ⌚⌛ — East Asian Wide
        (cp >= 0x23E9 && cp <= 0x23F3)  ||  // ⏩…⏳ emoji media controls
        (cp >= 0x23F4 && cp <= 0x23FA)  ||  // ⏴…⏺ emoji controls
        (cp >= 0x2600 && cp <= 0x27BF)  ||  // Misc Symbols + Dingbats
        (cp >= 0x2B50 && cp <= 0x2B55)  ||  // ⭐ ⭕ ⬛ … 常用符号
        // 补充多文种平面 (SMP) emoji 块
        (cp >= 0x1F000 && cp <= 0x1F02F) ||  // Mahjong Tiles
        (cp >= 0x1F0A0 && cp <= 0x1F0FF) ||  // Playing Cards
        (cp >= 0x1F100 && cp <= 0x1F1FF) ||  // Enclosed Alphanumeric Supplement
        (cp >= 0x1F200 && cp <= 0x1F2FF) ||  // Enclosed Ideographic Supplement
        (cp >= 0x1F300 && cp <= 0x1F5FF) ||  // Misc Symbols & Pictographs
        (cp >= 0x1F600 && cp <= 0x1F64F) ||  // Emoticons
        (cp >= 0x1F680 && cp <= 0x1F6FF) ||  // Transport & Map Symbols
        (cp >= 0x1F780 && cp <= 0x1F7FF) ||  // Geometric Shapes Extended (🟠🟡🟢…)
        (cp >= 0x1F800 && cp <= 0x1F8FF) ||  // Supplemental Arrows-C
        (cp >= 0x1F900 && cp <= 0x1F9FF) ||  // Supplemental Symbols & Pictographs
        (cp >= 0x1FA00 && cp <= 0x1FAFF))    // Symbols & Pictographs Extended-A
        return true;

    return false;
}

/// @brief 判断码点是否为零宽字符（ZWJ、变体选择器等）
bool tblIsZeroWidth(uint32_t cp)
{
    return cp == 0x200D                          // ZWJ (Zero Width Joiner)
        || (cp >= 0xFE00 && cp <= 0xFE0F)       // Variation Selectors (含 VS16 U+FE0F)
        || (cp >= 0xE0100 && cp <= 0xE01EF);    // Variation Selectors Supplement
}

/// @brief 计算 UTF-8 字符串的终端显示宽度（CJK/Emoji 占 2 列，零宽字符 0）
int tblStrWidth(const std::string& s)
{
    int w = 0;
    const char* p = s.c_str();
    const char* end = p + s.size();
    while (p < end)
    {
        uint32_t cp = tblUtf8Decode(p, end);
        if (tblIsZeroWidth(cp)) continue;        // ZWJ / 变体选择器不计宽
        w += tblIsWideChar(cp) ? 2 : 1;
    }
    return w;
}

/// @brief 剔除 ANSI 转义序列，返回纯文本
std::string tblStripAnsi(const std::string& s)
{
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] == '\033' && i + 1 < s.size() && s[i + 1] == '[')
        {
            i += 2;
            while (i < s.size() && s[i] != 'm') ++i;
            continue;
        }
        out += s[i];
    }
    return out;
}

/// @brief 计算 ANSI 字符串的净显示宽度（剔除转义序列后）
int tblRenderedWidth(const std::string& rendered)
{
    return tblStrWidth(tblStripAnsi(rendered));
}

/// @brief 重复字符串 n 次
std::string tblRepeat(const char* s, int n)
{
    std::string r;
    r.reserve(strlen(s) * n);
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

/// @brief 空格填充
std::string tblSpaces(int n) { return std::string(n > 0 ? n : 0, ' '); }

/// @brief 将内容截断到指定显示宽度（末尾加 …）
std::string tblTruncate(const std::string& rendered, int maxWidth)
{
    if (maxWidth <= 0) return "";
    int rw = tblRenderedWidth(rendered);
    if (rw <= maxWidth) return rendered;

    std::string clean = tblStripAnsi(rendered);
    std::string out;
    int w = 0;
    const char* p = clean.c_str();
    const char* end = p + clean.size();
    const char* lastGood = p;  // 安全截断点（不在 ZWJ 序列中间）
    while (p < end && w < maxWidth - 1)  // -1 留给 …
    {
        const char* prev = p;
        uint32_t cp = tblUtf8Decode(p, end);
        if (tblIsZeroWidth(cp)) continue;          // ZWJ / VS 不占宽度
        int cw = tblIsWideChar(cp) ? 2 : 1;
        if (w + cw > maxWidth - 1) { p = prev; break; }
        w += cw;
        lastGood = p;  // 完整 codepoint 通过，更新安全点
    }
    size_t byteLen = lastGood - clean.c_str();
    out = rendered.substr(0, byteLen);
    out += "\xe2\x80\xa6";  // …
    return out;
}

/// @brief 按对齐方式填充单元格内容至目标宽度
std::string tblPadCell(const std::string& rendered, int width, ETableAlign align)
{
    int rw = tblRenderedWidth(rendered);
    int diff = width - rw;
    if (diff <= 0) return rendered;

    int padL = 0, padR = 0;
    switch (align)
    {
    case ETableAlign::eLeft:   padL = 0; padR = diff; break;
    case ETableAlign::eRight:  padL = diff; padR = 0; break;
    case ETableAlign::eCenter: padL = diff / 2; padR = diff - padL; break;
    case ETableAlign::eDefault: padL = 0; padR = diff; break;
    }
    return tblSpaces(padL) + rendered + tblSpaces(padR);
}

/// @brief 画水平框线
std::string tblDrawBorder(const std::vector<int>& colWidths,
                          const char* left, const char* cross, const char* right)
{
    std::string out;
    out += left;
    for (size_t j = 0; j < colWidths.size(); ++j)
    {
        if (j > 0) out += cross;
        out += tblRepeat(kTblHLine, colWidths[j]);
    }
    out += right;
    out += '\n';
    return out;
}

}  // namespace

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
    auto& buf = curBuf();
    if (activeStyles_ & STYLE_BOLD)   buf += kBold;
    if (activeStyles_ & STYLE_ITALIC) buf += kItalic;
    if (activeStyles_ & STYLE_DELETE) buf += kStrike;
    if (activeStyles_ & STYLE_LINK)   { buf += kUnderline; buf += kBlue; }
}

void MarkdownANSI::emitStyleTransition(int oldStyles, int newStyles)
{
    if (oldStyles == newStyles) return;

    auto& buf = curBuf();
    if (newStyles == STYLE_NONE) { buf += kReset; return; }
    if (oldStyles != STYLE_NONE)  buf += kReset;

    if (newStyles & STYLE_BOLD)   buf += kBold;
    if (newStyles & STYLE_ITALIC) buf += kItalic;
    if (newStyles & STYLE_DELETE) buf += kStrike;
    if (newStyles & STYLE_LINK)   { buf += kUnderline; buf += kBlue; }
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
    listStack_.clear();
    inCodeBlock_    = false;
    codeLineNumber_ = 0;
    inTable_        = false;
    inTableHead_    = false;
    inCell_         = false;
    cellContent_.clear();
    tableRows_.clear();
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

    listStack_.push_back({ordered, 0});
}

void MarkdownANSI::startListItem()
{
    auto& frame = listStack_.back();
    frame.itemNumber++;

    // 输出块引用前缀（如果在引用中）
    emitBlockPrefix();

    // 嵌套列表缩进：每嵌套一级缩进 2 格（最外层不缩进）
    size_t depth = listStack_.size();
    for (size_t i = 1; i < depth; ++i)
        output_ += "  ";

    output_ += kYellow;
    if (frame.ordered)
    {
        output_ += std::to_string(frame.itemNumber);
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
    if (!listStack_.empty())
        listStack_.pop_back();
    // 列表后空行分隔（仅最外层列表结束时空行）
    if (listStack_.empty())
        output_ += '\n';
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
// 块级元素 — 表格
// ============================================================================

void MarkdownANSI::startTable()
{
    inTable_ = true;
    inTableHead_ = false;
    tableRows_.clear();
    currentRow_ = RowData();
}

void MarkdownANSI::startTableHead()
{
    inTableHead_ = true;
}

void MarkdownANSI::endTableHead()
{
    inTableHead_ = false;
}

void MarkdownANSI::startTableBody()
{
    // 表体开始，无需额外操作
}

void MarkdownANSI::endTableBody()
{
    // 表体结束，无需额外操作
}

void MarkdownANSI::startTableRow()
{
    currentRow_ = RowData();
    currentRow_.isHeader = inTableHead_;
}

void MarkdownANSI::endTableRow()
{
    if (!currentRow_.cells.empty())
        tableRows_.push_back(std::move(currentRow_));
    currentRow_ = RowData();
}

void MarkdownANSI::startTableCell(ETableAlign align)
{
    // 重定向到独立 cell 缓冲（不受 feed() 间 clearOutput 影响）
    cellContent_.clear();
    inCell_ = true;
    cellAlign_ = align;
}

void MarkdownANSI::endTableCell()
{
    inCell_ = false;

    // 关闭残余的行内样式（cell 边界处格式独立）
    if (activeStyles_ != STYLE_NONE)
    {
        cellContent_ += kReset;
        activeStyles_ = STYLE_NONE;
    }

    int width = tblRenderedWidth(cellContent_);
    CellData cd;
    cd.rendered = std::move(cellContent_);
    cd.width    = width;
    cd.align    = cellAlign_;
    currentRow_.cells.push_back(std::move(cd));
    cellContent_.clear();
}

void MarkdownANSI::endTable()
{
    inTable_ = false;
    inCell_  = false;
    if (tableRows_.empty()) return;

    int nCols = 0;
    for (const auto& row : tableRows_)
    {
        int n = static_cast<int>(row.cells.size());
        if (n > nCols) nCols = n;
    }
    if (nCols == 0) { tableRows_.clear(); return; }

    // ---- 计算列宽 ----
    std::vector<int> colWidths(nCols, 4);  // 最小列宽 4（含左右各 1 空格）
    for (const auto& row : tableRows_)
    {
        for (int j = 0; j < nCols; ++j)
        {
            int cw = (j < static_cast<int>(row.cells.size())) ? row.cells[j].width : 0;
            if (cw + 2 > colWidths[j])
                colWidths[j] = cw + 2;  // +2 左右各一个空格
        }
    }

    // ---- 终端宽度约束 ----
    {
        int termWidth = aTerminalWidth();
        int total = 0;
        for (int w : colWidths) total += w;
        total += nCols + 1;  // 竖线 |
        if (total > termWidth && nCols > 1)
        {
            int excess = total - termWidth;
            for (int j = 0; j < nCols && excess > 0; ++j)
            {
                int reduce = std::min(excess, colWidths[j] - 4);
                colWidths[j] -= reduce;
                excess -= reduce;
            }
        }
    }

    // ---- 准备渲染用的单元格内容（可选截断） ----
    std::vector<std::vector<std::string>> renderedCells(tableRows_.size(),
                                                         std::vector<std::string>(nCols));
    for (size_t i = 0; i < tableRows_.size(); ++i)
    {
        for (int j = 0; j < nCols; ++j)
        {
            if (j < static_cast<int>(tableRows_[i].cells.size()))
            {
                if (truncateCells_)
                {
                    int innerW = colWidths[j] - 2;
                    renderedCells[i][j] = tblTruncate(tableRows_[i].cells[j].rendered, innerW);
                }
                else
                {
                    renderedCells[i][j] = tableRows_[i].cells[j].rendered;
                }
            }
        }
    }

    // ---- 组装输出 ----
    // 顶框
    output_ += tblDrawBorder(colWidths, kTblTopLeft, kTblTopT, kTblTopRight);

    size_t headerCount = 0;
    for (const auto& row : tableRows_)
        if (row.isHeader) ++headerCount;

    for (size_t i = 0; i < tableRows_.size(); ++i)
    {
        const auto& row = tableRows_[i];
        bool isHeader = row.isHeader;

        // 表头-数据分隔线
        if (i > 0 && isHeader && i == headerCount)
        {
            output_ += tblDrawBorder(colWidths, kTblMidLeft, kTblMidCross, kTblMidRight);
        }
        else if (i > 0 && !isHeader && tableRows_[i - 1].isHeader)
        {
            // 第一个数据行前画分隔线
            output_ += tblDrawBorder(colWidths, kTblMidLeft, kTblMidCross, kTblMidRight);
        }

        // 数据行
        output_ += kTblVLine;
        for (int j = 0; j < nCols; ++j)
        {
            if (j > 0) output_ += kTblVLine;

            ETableAlign align = (j < static_cast<int>(row.cells.size()))
                                ? row.cells[j].align : ETableAlign::eDefault;
            // 表头始终居中对齐
            if (isHeader) align = ETableAlign::eCenter;

            std::string cell = (j < nCols) ? renderedCells[i][j] : std::string();
            std::string padded = tblPadCell(" " + cell + " ", colWidths[j], align);
            output_ += padded;
        }
        output_ += kTblVLine;
        output_ += '\n';
    }

    // 底框
    output_ += tblDrawBorder(colWidths, kTblBotLeft, kTblBotT, kTblBotRight);
    output_ += '\n';

    // ---- 调试：输出每 cell 的原始宽度与净宽度（编译期宏控制） ----
#ifdef _AST_DEBUG_MARKDOWN_ANSI
    {
        output_ += kDim;
        output_ += "── TABLE DEBUG ──";
        output_ += kReset;
        output_ += '\n';
        for (size_t i = 0; i < tableRows_.size(); ++i)
        {
            const auto& row = tableRows_[i];
            output_ += row.isHeader ? "[HEAD] " : "[BODY] ";
            for (int j = 0; j < nCols; ++j)
            {
                if (j > 0) output_ += " | ";
                if (j < static_cast<int>(row.cells.size()))
                {
                    std::string stripped = tblStripAnsi(row.cells[j].rendered);
                    output_ += "[\"" + stripped + "\" w="
                            + std::to_string(row.cells[j].width) + "]";
                }
                else
                {
                    output_ += "(empty)";
                }
            }
            output_ += '\n';
        }
        output_ += "colWidths: ";
        for (int j = 0; j < nCols; ++j)
        {
            if (j > 0) output_ += ", ";
            output_ += "c" + std::to_string(j) + "=" + std::to_string(colWidths[j]);
        }
        output_ += '\n';
        output_ += kDim;
        output_ += "── END DEBUG ──";
        output_ += kReset;
        output_ += '\n';
    }
#endif

    tableRows_.clear();
}

// ============================================================================
// 行内元素 — 文本
// ============================================================================

void MarkdownANSI::text(StringView txt)
{
    curBuf().append(txt.data(), txt.size());
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
    auto& buf = curBuf();
    buf += kCyan;
    buf.append(code.data(), code.size());
    buf += kReset;
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
    auto& buf = curBuf();
    buf += " (";
    buf += kBlue;
    buf += linkUrl_;
    buf += kReset;
    emitActiveStyles();
    buf += ")";

    linkUrl_.clear();
}

// ============================================================================
// 行内元素 — 图片
// ============================================================================

void MarkdownANSI::image(StringView alt, StringView url)
{
    auto& buf = curBuf();
    buf += kDim;
    buf += "[Image: ";
    buf.append(alt.data(), alt.size());
    if (!url.empty())
    {
        buf += " (";
        buf.append(url.data(), url.size());
        buf += ")";
    }
    buf += "]";
    buf += kReset;
    emitActiveStyles();
}

AST_NAMESPACE_END
