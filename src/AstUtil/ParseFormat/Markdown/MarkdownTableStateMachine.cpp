///
/// @file      MarkdownTableStateMachine.cpp
/// @brief     Markdown 表格流式解析状态机实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "MarkdownTableStateMachine.hpp"
#include <cctype>

AST_NAMESPACE_BEGIN

// ============================================================================
// 构造 / 析构
// ============================================================================

MarkdownTableStateMachine::MarkdownTableStateMachine(MarkdownSax& sax)
    : sax_(sax)
    , inlineSM_(sax)
{
}

// ============================================================================
// 公共接口
// ============================================================================


void MarkdownTableStateMachine::feed(StringView chunk)
{
    for (char c : chunk)
    {
        feedChar(c);
    }
}


void MarkdownTableStateMachine::feedChar(char c)
{
    charConsumed_ = true;

    // ---- 行首检测（rowBuf_ 在上一个 \n 后被清空） ----
    if (rowBuf_.empty())
    {
        // 初始状态：从 BlockSM classifyFirstChar 收到 |
        if (state_ == EState::eIdle)
        {
            if (c == '|')
            {
                state_ = EState::eHeaderRow;
                rowBuf_ += c;
                return;
            }
            // 非 | 字符：不应该到达这里（BlockSM 仅在看到 | 时进入表格模式）
            state_ = EState::eIdle;
            charConsumed_ = false;
            return;
        }

        // 跳过行首空白
        if (c == ' ' || c == '\t')
            return;

        // 空行（\n 紧跟 \n）
        if (c == '\n')
        {
            if (state_ == EState::eBodyRow)
            {
                emitTableEnd();
            }
            else
            {
                abortAsParagraph();
            }
            state_ = EState::eIdle;
            return;
        }

        // 非 | 字符 → 表格结束
        if (c != '|')
        {
            if (state_ == EState::eBodyRow)
            {
                emitTableEnd();
            }
            else
            {
                abortAsParagraph();
            }
            state_ = EState::eIdle;
            charConsumed_ = false;  // 当前字符未消费，由 BlockSM 重新分类
            return;
        }
    }

    // ---- 行内字符累积 ----
    if (c == '\n')
    {
        processRow(rowBuf_);
        rowBuf_.clear();
        return;
    }

    rowBuf_ += c;
}

void MarkdownTableStateMachine::finish()
{
    // 处理未完成的行（最后一行可能不以 \n 结尾）
    if (!rowBuf_.empty())
    {
        if (state_ == EState::eBodyRow)
        {
            // 最后一行作为数据行处理
            processBodyRow(rowBuf_);
        }
        else if (state_ == EState::eHeaderRow || state_ == EState::eSeparator)
        {
            // 未确认的表格 → 回退为段落
            abortBuf_ += rowBuf_;
            abortAsParagraph();
        }
        rowBuf_.clear();
    }

    // 关闭表格（若仍打开）
    if (state_ == EState::eBodyRow)
    {
        emitTableEnd();
    }

    state_ = EState::eIdle;
    charConsumed_ = true;
}

void MarkdownTableStateMachine::reset()
{
    state_ = EState::eIdle;
    charConsumed_ = true;
    rowBuf_.clear();
    headerCells_.clear();
    colAligns_.clear();
    abortBuf_.clear();

    // 重建 inlineSM（引用成员不可赋值）
    inlineSM_.~MarkdownInlineStateMachine();
    new (&inlineSM_) MarkdownInlineStateMachine(sax_);
}

// ============================================================================
// 行处理
// ============================================================================

void MarkdownTableStateMachine::processRow(const std::string& rowContent)
{
    switch (state_)
    {
    case EState::eHeaderRow:
        processHeaderRow(rowContent);
        break;

    case EState::eSeparator:
        processSeparatorRow(rowContent);
        break;

    case EState::eBodyRow:
        processBodyRow(rowContent);
        break;

    case EState::eIdle:
        break;
    }
}

// ============================================================================
// 表头行处理
// ============================================================================

void MarkdownTableStateMachine::processHeaderRow(const std::string& row)
{
    // 检查行首是否为 |
    if (!isTableRow(row))
    {
        abortBuf_ += row;
        abortAsParagraph();
        state_ = EState::eIdle;
        return;
    }

    // 分割 cell
    headerCells_ = splitCells(row);
    if (headerCells_.empty())
    {
        abortBuf_ += row;
        abortAsParagraph();
        state_ = EState::eIdle;
        return;
    }

    // 缓冲原始行文本（用于回退）+ 进入分隔行状态
    abortBuf_ += row;
    abortBuf_ += '\n';
    state_ = EState::eSeparator;
}

// ============================================================================
// 分隔行处理
// ============================================================================

void MarkdownTableStateMachine::processSeparatorRow(const std::string& row)
{
    if (!isSeparatorRow(row))
    {
        // 不是合法分隔行 → 回退（abortBuf_ 已包含表头行文本 + \n）
        abortBuf_ += row;
        abortAsParagraph();
        state_ = EState::eIdle;
        return;
    }

    // 提取列对齐
    colAligns_ = parseAlignments(row);

    // 补齐对齐数量（分隔行列数可能少于表头列数）
    while (colAligns_.size() < headerCells_.size())
        colAligns_.push_back(ETableAlign::eDefault);

    // 确认是表格！清空回退缓冲，发射表格开始 + 表头
    abortBuf_.clear();
    emitTableStart();

    state_ = EState::eBodyRow;
}

// ============================================================================
// 数据行处理
// ============================================================================

void MarkdownTableStateMachine::processBodyRow(const std::string& row)
{
    if (!isTableRow(row))
    {
        // 非表格行 → 结束表格，将本行作为段落输出
        emitTableEnd();
        if (!row.empty())
        {
            sax_.startParagraph();
            sax_.text(StringView(row.data(), row.size()));
            sax_.endParagraph();
        }
        state_ = EState::eIdle;
        return;
    }

    auto cells = splitCells(row);

    // 发射行开始
    sax_.startTableRow();

    size_t nCols = colAligns_.size();
    for (size_t i = 0; i < nCols; ++i)
    {
        ETableAlign align = (i < colAligns_.size()) ? colAligns_[i] : ETableAlign::eDefault;
        std::string cellText = (i < cells.size()) ? cells[i] : std::string();

        sax_.startTableCell(align);
        if (!cellText.empty())
        {
            inlineSM_.feed(StringView(cellText.data(), cellText.size()));
            inlineSM_.flushCell();
        }
        sax_.endTableCell();
    }

    // 发射行结束
    sax_.endTableRow();
}

// ============================================================================
// 表格结构发射
// ============================================================================

void MarkdownTableStateMachine::emitTableStart()
{
    sax_.startTable();
    sax_.startTableHead();
    sax_.startTableRow();

    for (size_t i = 0; i < headerCells_.size(); ++i)
    {
        ETableAlign align = i < colAligns_.size() ? colAligns_[i] : ETableAlign::eDefault;
        sax_.startTableCell(align);
        if (!headerCells_[i].empty())
        {
            inlineSM_.feed(StringView(headerCells_[i].data(), headerCells_[i].size()));
            inlineSM_.flushCell();
        }
        sax_.endTableCell();
    }

    sax_.endTableRow();
    sax_.endTableHead();
    sax_.startTableBody();
}

void MarkdownTableStateMachine::emitTableEnd()
{
    sax_.endTableBody();
    sax_.endTable();
}

// ============================================================================
// 回退
// ============================================================================

void MarkdownTableStateMachine::abortAsParagraph()
{
    if (abortBuf_.empty()) return;

    sax_.startParagraph();
    // 将回退文本作为普通文本输出（保留 | 字面量）
    sax_.text(StringView(abortBuf_.data(), abortBuf_.size()));
    sax_.endParagraph();

    abortBuf_.clear();
}

// ============================================================================
// 辅助方法
// ============================================================================

std::vector<std::string> MarkdownTableStateMachine::splitCells(const std::string& row)
{
    std::vector<std::string> cells;
    size_t start = 0;

    // 跳过行首 |
    if (!row.empty() && row[0] == '|')
        start = 1;

    while (start <= row.size())
    {
        size_t end = row.find('|', start);
        std::string cell;
        if (end == std::string::npos)
        {
            cell = trim(row.substr(start));
            if (!cell.empty() || !cells.empty())
                cells.push_back(cell);
            break;
        }
        cell = trim(row.substr(start, end - start));
        cells.push_back(cell);
        start = end + 1;
    }

    // 去掉尾部空 cell（行尾 | 产生的）
    while (!cells.empty() && cells.back().empty() && cells.size() > 1)
        cells.pop_back();

    return cells;
}

bool MarkdownTableStateMachine::isTableRow(const std::string& row)
{
    // 表行以 | 开头（允许前导空白）
    for (char c : row)
    {
        if (c == ' ' || c == '\t') continue;
        return c == '|';
    }
    return false;
}

bool MarkdownTableStateMachine::isSeparatorRow(const std::string& row)
{
    bool hasPipe = false;
    bool hasDash = false;

    for (char c : row)
    {
        if (c == '|' || c == '-' || c == ':' || c == ' ' || c == '\t')
        {
            if (c == '|') hasPipe = true;
            if (c == '-') hasDash = true;
            continue;
        }
        return false;  // 非法字符
    }

    return hasPipe && hasDash;
}

std::vector<ETableAlign> MarkdownTableStateMachine::parseAlignments(const std::string& sepRow)
{
    std::vector<std::string> parts = splitCells(sepRow);
    std::vector<ETableAlign> aligns;

    for (const auto& part : parts)
    {
        bool left  = (!part.empty() && part.front() == ':');
        bool right = (!part.empty() && part.back()  == ':');

        if (left && right)
            aligns.push_back(ETableAlign::eCenter);
        else if (right)
            aligns.push_back(ETableAlign::eRight);
        else if (left)
            aligns.push_back(ETableAlign::eLeft);
        else
            aligns.push_back(ETableAlign::eDefault);
    }

    return aligns;
}

std::string MarkdownTableStateMachine::trim(const std::string& s)
{
    size_t b = 0;
    while (b < s.size() && (s[b] == ' ' || s[b] == '\t')) ++b;
    size_t e = s.size();
    while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t')) --e;
    return s.substr(b, e - b);
}

AST_NAMESPACE_END
