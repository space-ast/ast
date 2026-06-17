///
/// @file      MarkdownTable.cpp
/// @brief     Markdown 表格渲染 — 解析 + Unicode 单线框 + ANSI 输出
/// @author    axel
/// @date      2026-06-15
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "MarkdownTable.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

// ============================================================================
// ANSI 转义序列（匿名命名空间避免 unity build 符号冲突）
// ============================================================================
namespace {
const char kMT_Reset[]     = "\033[0m";
const char kMT_Bold[]      = "\033[1m";
const char kMT_Italic[]    = "\033[3m";
const char kMT_Underline[] = "\033[4m";
const char kMT_Strike[]    = "\033[9m";
const char kMT_Cyan[]      = "\033[36m";
const char kMT_Blue[]      = "\033[34m";
}  // namespace

// ============================================================================
// Unicode 框线字符 (UTF-8)
// ============================================================================
static const char kTopLeft[]      = "\xe2\x94\x8c";  // ┌
static const char kTopT[]         = "\xe2\x94\xac";  // ┬
static const char kTopRight[]     = "\xe2\x94\x90";  // ┐
static const char kMidLeft[]      = "\xe2\x94\x9c";  // ├
static const char kMidCross[]     = "\xe2\x94\xbc";  // ┼
static const char kMidRight[]     = "\xe2\x94\xa4";  // ┤
static const char kBotLeft[]      = "\xe2\x94\x94";  // └
static const char kBotT[]         = "\xe2\x94\xb4";  // ┴
static const char kBotRight[]     = "\xe2\x94\x98";  // ┘
static const char kHLine[]        = "\xe2\x94\x80";  // ─
static const char kVLine[]        = "\xe2\x94\x82";  // │

// ============================================================================
// 表格数据结构
// ============================================================================
enum CellAlign { ALIGN_LEFT = -1, ALIGN_CENTER = 0, ALIGN_RIGHT = 1 };

struct TableData
{
	std::vector<std::string> headers;
	std::vector<CellAlign>   aligns;
	std::vector<std::vector<std::string>> rows;
};

// ============================================================================
// UTF-8 辅助
// ============================================================================

/// @brief 解码一个 UTF-8 码点，返回码点值；*it 前进到下一码点起始
static uint32_t utf8Decode(const char*& it, const char* end)
{
	if (it >= end) return 0;
	unsigned char c = static_cast<unsigned char>(*it);

	if (c < 0x80) { ++it; return c; }

	uint32_t cp = 0;
	int len = 0;

	if ((c & 0xE0) == 0xC0)      { cp = c & 0x1F; len = 1; }
	else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; len = 2; }
	else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; len = 3; }
	else { ++it; return 0xFFFD; } // 非法字节，跳过

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
static bool isWideChar(uint32_t cp)
{
	// 东亚宽字符范围（覆盖中文、日文、韩文、全角等）
	return (cp >= 0x1100  && cp <= 0x115F)  ||  // Hangul Jamo
	       (cp >= 0x2E80  && cp <= 0xA4CF)  ||  // CJK Radicals … Yi
	       (cp >= 0xAC00  && cp <= 0xD7A3)  ||  // Hangul Syllables
	       (cp >= 0xF900  && cp <= 0xFAFF)  ||  // CJK Compatibility
	       (cp >= 0xFE10  && cp <= 0xFE19)  ||  // Vertical forms
	       (cp >= 0xFE30  && cp <= 0xFE6F)  ||  // CJK Compatibility Forms
	       (cp >= 0xFF01  && cp <= 0xFF60)  ||  // Fullwidth Forms
	       (cp >= 0xFFE0  && cp <= 0xFFE6);      // Fullwidth Signs
}

/// @brief 计算 UTF-8 字符串的终端显示宽度（CJK 字符占 2 列）
static int strDisplayWidth(const std::string& s)
{
	int w = 0;
	const char* p = s.c_str();
	const char* end = p + s.size();
	while (p < end)
	{
		uint32_t cp = utf8Decode(p, end);
		w += isWideChar(cp) ? 2 : 1;
	}
	return w;
}

/// @brief 剔除 ANSI 转义序列，返回纯文本（用于宽度计算）
static std::string stripAnsi(const std::string& s)
{
	std::string out;
	out.reserve(s.size());
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] == '\033' && i + 1 < s.size() && s[i + 1] == '[')
		{
			// 跳过 \033[...m
			i += 2;
			while (i < s.size() && s[i] != 'm') ++i;
			continue;
		}
		out += s[i];
	}
	return out;
}

/// @brief 计算已渲染 ANSI 字符串的"净显示宽度"（剔除转义序列后）
static int renderedWidth(const std::string& rendered)
{
	return strDisplayWidth(stripAnsi(rendered));
}

// ============================================================================
// 字符串辅助
// ============================================================================

static std::string trim(const std::string& s)
{
	size_t b = 0;
	while (b < s.size() && (s[b] == ' ' || s[b] == '\t')) ++b;
	size_t e = s.size();
	while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t')) --e;
	return s.substr(b, e - b);
}

/// @brief 重复字符串 n 次
static std::string repeat(const char* s, int n)
{
	std::string r;
	r.reserve(std::strlen(s) * n);
	for (int i = 0; i < n; ++i) r += s;
	return r;
}

/// @brief 按字节宽度填充空格
static std::string padSpaces(int n)
{
	return std::string(n > 0 ? n : 0, ' ');
}

// ============================================================================
// 行内 Markdown → ANSI（简化版，覆盖表格单元格常见格式）
// ============================================================================
static void renderInline(const std::string& text, std::string& out)
{
	// 简化的状态机：处理 **bold**, *italic*, _italic_, `code`, ~~strike~~, [text](url)
	int mode = 0;  // 0=NORM, 1=BOLD, 2=ITALIC, 3=CODE, 4=STRIKE
	enum { NORM, BOLD, ITALIC, CODE, STRIKE };
	char italicChar = 0;

	auto emitStyle = [&](int targetMode)
	{
		if (mode == targetMode) return;
		// 关闭当前
		switch (mode)
		{
		case BOLD:   case ITALIC: case CODE: case STRIKE: out += kMT_Reset; break;
		default: break;
		}
		mode = targetMode;
		switch (mode)
		{
		case BOLD:   out += kMT_Bold;   break;
		case ITALIC: out += kMT_Italic; break;
		case CODE:   out += kMT_Cyan;   break;
		case STRIKE: out += kMT_Strike; break;
		default: break;
		}
	};

	size_t i = 0;
	while (i < text.size())
	{
		char c = text[i];

		// 转义
		if (c == '\\' && i + 1 < text.size())
		{
			out += text[i + 1];
			i += 2;
			continue;
		}

		// **bold**
		if (c == '*' && i + 1 < text.size() && text[i + 1] == '*')
		{
			i += 2;
			// 查找闭合 **
			size_t end = text.find("**", i);
			if (end != std::string::npos)
			{
				emitStyle(BOLD);
				renderInline(text.substr(i, end - i), out);
				emitStyle(NORM);
				i = end + 2;
				continue;
			}
			// 未闭合，回退为字面量
			out += "**";
			continue;
		}

		// ~~strike~~
		if (c == '~' && i + 1 < text.size() && text[i + 1] == '~')
		{
			i += 2;
			size_t end = text.find("~~", i);
			if (end != std::string::npos)
			{
				emitStyle(STRIKE);
				renderInline(text.substr(i, end - i), out);
				emitStyle(NORM);
				i = end + 2;
				continue;
			}
			out += "~~";
			continue;
		}

		// `code`
		if (c == '`')
		{
			size_t end = text.find('`', i + 1);
			if (end != std::string::npos)
			{
				std::string code = text.substr(i + 1, end - i - 1);
				emitStyle(CODE);
				out += code;
				emitStyle(NORM);
				i = end + 1;
				continue;
			}
			out += '`';
			++i;
			continue;
		}

		// *italic* or _italic_
		if ((c == '*' || c == '_') && (mode != ITALIC || c == italicChar))
		{
			char marker = c;
			size_t end = text.find(marker, i + 1);
			// 排除 ** 和 __
			if (end != std::string::npos &&
			    !(end + 1 < text.size() && text[end + 1] == marker))
			{
				italicChar = marker;
				emitStyle(ITALIC);
				renderInline(text.substr(i + 1, end - i - 1), out);
				emitStyle(NORM);
				italicChar = 0;
				i = end + 1;
				continue;
			}
		}

		// [text](url) → 仅显示 text（表格空间有限）
		if (c == '[')
		{
			size_t cb = text.find(']', i + 1);
			if (cb != std::string::npos && cb + 1 < text.size() && text[cb + 1] == '(')
			{
				size_t ce = text.find(')', cb + 2);
				if (ce != std::string::npos)
				{
					out += kMT_Underline;
					out += kMT_Blue;
					renderInline(text.substr(i + 1, cb - i - 1), out);
					out += kMT_Reset;
					// 恢复当前样式
					switch (mode)
					{
					case BOLD: out += kMT_Bold; break;
					case ITALIC: out += kMT_Italic; break;
					case CODE: out += kMT_Cyan; break;
					case STRIKE: out += kMT_Strike; break;
					default: break;
					}
					i = ce + 1;
					continue;
				}
			}
		}

		out += c;
		++i;
	}

	emitStyle(NORM);  // 确保样式关闭
}

/// @brief 渲染单元格内容（行内 Markdown → ANSI），并返回净显示宽度
static std::string renderCell(const std::string& raw, int& outWidth)
{
	std::string rendered;
	renderInline(raw, rendered);
	outWidth = renderedWidth(rendered);
	return rendered;
}

// ============================================================================
// 表格解析
// ============================================================================

/// @brief 将一行按 | 分割为单元格，首尾 | 可选
static std::vector<std::string> splitRow(const std::string& line)
{
	std::vector<std::string> cells;
	size_t start = 0;

	// 跳过行首 |
	if (!line.empty() && line[0] == '|') start = 1;

	while (start <= line.size())
	{
		size_t end = line.find('|', start);
		if (end == std::string::npos)
		{
			// 最后一个单元格
			std::string cell = trim(line.substr(start));
			if (!cell.empty() || !cells.empty())
				cells.push_back(cell);
			break;
		}
		cells.push_back(trim(line.substr(start, end - start)));
		start = end + 1;
	}

	// 去掉尾部的空单元格（行尾 | 产生的）
	while (!cells.empty() && cells.back().empty() && cells.size() > 1)
		cells.pop_back();

	return cells;
}

/// @brief 判断一行是否为表格分隔行（如 |---|:---:|---:|）
static bool isSeparatorLine(const std::string& line)
{
	for (char c : line)
	{
		if (c == '|' || c == '-' || c == ':' || c == ' ' || c == '\t') continue;
		return false;
	}
	// 必须至少含有一个 '-'
	return line.find('-') != std::string::npos;
}

/// @brief 从分隔行解析对齐方式
static std::vector<CellAlign> parseAlignments(const std::string& sepLine)
{
	std::vector<std::string> parts = splitRow(sepLine);
	std::vector<CellAlign> aligns;
	for (const auto& p : parts)
	{
		bool left  = (!p.empty() && p.front() == ':');
		bool right = (!p.empty() && p.back()  == ':');
		if (left && right)      aligns.push_back(ALIGN_CENTER);
		else if (right)         aligns.push_back(ALIGN_RIGHT);
		else                    aligns.push_back(ALIGN_LEFT);
	}
	return aligns;
}

/// @brief 解析完整 Markdown 表格字符串
static bool parseTable(const std::string& markdown, TableData& table)
{
	// 按行分割
	std::vector<std::string> lines;
	{
		size_t pos = 0;
		while (pos < markdown.size())
		{
			size_t nl = markdown.find('\n', pos);
			std::string line = markdown.substr(pos, nl - pos);
			// 去掉 \r
			if (!line.empty() && line.back() == '\r') line.pop_back();
			lines.push_back(line);
			if (nl == std::string::npos) break;
			pos = nl + 1;
		}
	}

	// 找到表头行（第一个含 | 的行）
	size_t i = 0;
	while (i < lines.size())
	{
		std::string trimmed = trim(lines[i]);
		if (trimmed.empty()) { ++i; continue; }
		if (!trimmed.empty() && trimmed[0] == '|')
		{
			table.headers = splitRow(lines[i]);
			++i;
			break;
		}
		++i;
	}

	if (table.headers.empty()) return false;

	// 找分隔行
	if (i >= lines.size()) return false;
	if (!isSeparatorLine(lines[i])) return false;

	table.aligns = parseAlignments(lines[i]);

	// 对齐数量不足时补默认值
	while (table.aligns.size() < table.headers.size())
		table.aligns.push_back(ALIGN_LEFT);

	++i;

	// 收集数据行
	while (i < lines.size())
	{
		std::string trimmed = trim(lines[i]);
		if (trimmed.empty()) break;          // 空行结束表格
		if (trimmed[0] != '|') break;        // 非 | 行结束表格

		auto cells = splitRow(lines[i]);
		if (cells.empty()) break;

		// 补齐列数
		while (cells.size() < table.headers.size())
			cells.emplace_back("");
		table.rows.push_back(std::move(cells));
		++i;
	}

	return true;
}

// ============================================================================
// 表格渲染
// ============================================================================

/// @brief 用填充使字符串达到目标显示宽度
/// @param rendered  已渲染的 ANSI 字符串
/// @param width      目标显示宽度（不含 ANSI）
/// @param align      对齐方式
static std::string padCell(const std::string& rendered, int width, CellAlign align)
{
	int rw = renderedWidth(rendered);
	int diff = width - rw;
	if (diff <= 0) return rendered;  // 内容已满，不裁剪（由上层控制）

	int padL = 0, padR = 0;
	switch (align)
	{
	case ALIGN_LEFT:   padL = 0; padR = diff; break;
	case ALIGN_RIGHT:  padL = diff; padR = 0; break;
	case ALIGN_CENTER: padL = diff / 2; padR = diff - padL; break;
	}

	std::string out;
	out.reserve(rendered.size() + padL + padR + 2);
	if (padL) out += padSpaces(padL);
	out += rendered;
	if (padR) out += padSpaces(padR);
	return out;
}

/// @brief 将内容截断到指定显示宽度（末尾加 …）
static std::string truncateToWidth(const std::string& rendered, int maxWidth)
{
	if (maxWidth <= 0) return "";
	int rw = renderedWidth(rendered);
	if (rw <= maxWidth) return rendered;

	// 逐码点截断
	std::string out;
	int w = 0;
	const std::string clean = stripAnsi(rendered);
	const char* p = clean.c_str();
	const char* end = p + clean.size();
	const char* last = p;
	while (p < end && w < maxWidth - 1)  // -1 留给 …
	{
		last = p;
		uint32_t cp = utf8Decode(p, end);
		int cw = isWideChar(cp) ? 2 : 1;
		if (w + cw > maxWidth - 1) break;
		w += cw;
	}
	// 从原串中取对应字节数
	size_t byteLen = last - clean.c_str();
	out = rendered.substr(0, byteLen);
	out += "\xe2\x80\xa6";  // …
	return out;
}

/// @brief 画水平框线
static std::string drawBorder(const std::vector<int>& colWidths,
                              const char* left, const char* cross, const char* right)
{
	std::string out;
	out += left;
	for (size_t j = 0; j < colWidths.size(); ++j)
	{
		if (j > 0) out += cross;
		out += repeat(kHLine, colWidths[j]);
	}
	out += right;
	out += '\n';
	return out;
}

AST_UTIL_API std::string aMarkdownTable(StringView input)
{
	// ---- 1. 解析 ----
	TableData table;
	if (!parseTable(std::string(input.data(), input.size()), table))
		return {};

	int nCols = (int)table.headers.size();
	if (nCols < 1) return {};

	// ---- 2. 渲染各单元格并计算净宽度 ----
	struct Col { std::string rendered; int width = 0; };
	std::vector<Col> headerCols(nCols);
	std::vector<std::vector<Col>> rowCols(table.rows.size(),
	                                      std::vector<Col>(nCols));

	for (int j = 0; j < nCols; ++j)
		headerCols[j].rendered = renderCell(table.headers[j], headerCols[j].width);

	for (size_t i = 0; i < table.rows.size(); ++i)
		for (int j = 0; j < nCols; ++j)
			rowCols[i][j].rendered = renderCell(table.rows[i][j], rowCols[i][j].width);

	// ---- 3. 计算列宽 ----
	std::vector<int> colWidths(nCols, 4);  // 最小列宽 4（含左右各 1 空格）

	for (int j = 0; j < nCols; ++j)
	{
		int maxW = headerCols[j].width;
		for (size_t i = 0; i < table.rows.size(); ++i)
			if (rowCols[i][j].width > maxW)
				maxW = rowCols[i][j].width;
		colWidths[j] = std::max(colWidths[j], maxW + 2);  // +2 左右各一个空格
	}

	// ---- 4. 终端宽度约束（默认 100 列） ----
	const int kMaxTermWidth = 100;
	{
		int total = 0;
		for (int w : colWidths) total += w;
		total += nCols + 1;  // 分隔线 |

		if (total > kMaxTermWidth && nCols > 1)
		{
			// 超宽：按比例缩减各列
			int excess = total - kMaxTermWidth;
			for (int j = 0; j < nCols && excess > 0; ++j)
			{
				int reduce = std::min(excess, colWidths[j] - 4);  // 不低于 4
				colWidths[j] -= reduce;
				excess -= reduce;
			}
		}
	}

	// ---- 5. 截断超出列宽的内容 ----
	for (int j = 0; j < nCols; ++j)
	{
		int innerW = colWidths[j] - 2;  // 内容区宽度（不含左右空格）
		headerCols[j].rendered = truncateToWidth(headerCols[j].rendered, innerW);
		for (size_t i = 0; i < table.rows.size(); ++i)
			rowCols[i][j].rendered = truncateToWidth(rowCols[i][j].rendered, innerW);
	}

	// ---- 6. 组装输出 ----
	std::string out;
	out.reserve(1024);

	// 顶框
	out += drawBorder(colWidths, kTopLeft, kTopT, kTopRight);

	// 表头
	out += kVLine;
	for (int j = 0; j < nCols; ++j)
	{
		if (j > 0) out += kVLine;
		// 表头居中对齐
		std::string padded = padCell(" " + headerCols[j].rendered + " ",
		                             colWidths[j], ALIGN_LEFT);
		out += padded;
	}
	out += kVLine;
	out += '\n';

	// 表头-数据分隔
	out += drawBorder(colWidths, kMidLeft, kMidCross, kMidRight);

	// 数据行
	for (size_t i = 0; i < table.rows.size(); ++i)
	{
		out += kVLine;
		for (int j = 0; j < nCols; ++j)
		{
			if (j > 0) out += kVLine;
			CellAlign align = (j < (int)table.aligns.size())
			                  ? table.aligns[j] : ALIGN_LEFT;
			std::string padded = padCell(" " + rowCols[i][j].rendered + " ",
			                             colWidths[j], align);
			out += padded;
		}
		out += kVLine;
		out += '\n';
	}

	// 底框
	out += drawBorder(colWidths, kBotLeft, kBotT, kBotRight);

	return out;
}

AST_NAMESPACE_END
