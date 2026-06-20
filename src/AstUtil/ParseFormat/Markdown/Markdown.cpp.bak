///
/// @file      Markdown.cpp
/// @brief     Markdown 渲染器 — 流式状态机实现
/// @author    axel
/// @date      2026-06-15
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "Markdown.hpp"
#include "MarkdownTable.hpp"
#include "AstUtil/IO.hpp"
#include <algorithm>
#include <cstring>

AST_NAMESPACE_BEGIN

// ============================================================================
// ANSI 转义序列
// ============================================================================
static const char kReset[]     = "\033[0m";
static const char kBold[]      = "\033[1m";
static const char kItalic[]    = "\033[3m";
static const char kUnderline[] = "\033[4m";
static const char kStrike[]    = "\033[9m";
static const char kDim[]       = "\033[2m";
static const char kBlue[]      = "\033[34m";
static const char kCyan[]      = "\033[36m";
static const char kYellow[]      = "\033[33m";
static const char kBrightWhite[] = "\033[97m";
static const char kBrightCyan[]  = "\033[96m";

// 代码块 256 色样式（深灰背景 + 层次化前景）
static const char kCodeBg[]       = "\033[48;5;236m";  // #303030 背景
static const char kCodeGutter[]   = "\033[38;5;240m";  // #585858 gutter │
static const char kCodeLineNo[]   = "\033[38;5;243m";  // #767676 行号
static const char kCodeText[]     = "\033[38;5;252m";  // #d0d0d0 代码文本
static const char kCodeFence[]    = "\033[38;5;240m";  // #585858 围栏标记
static const char kCodeLang[]     = "\033[38;5;228m";  // #ffff87 语言标签

// UTF-8 符号
static const char kBullet[]    = "\xe2\x80\xa2";  // "•"
static const char kBar[]       = "\xe2\x94\x82";  // "│"
static const char kHr[]        = "\xe2\x94\x80";  // "─" (横线)

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
// 公开接口
// ============================================================================

const char* Markdown::operator()(StringView chunk)
{
	result_.clear();
	for (auto c : chunk)
		feed(c, result_);
	return result_.c_str();
}

void Markdown::print(StringView chunk)
{
	ast_printf("%s", this->operator()(chunk));
}

void Markdown::reset()
{
	result_.clear();
	pending_.clear();
	leadingSpaces_.clear();
	blockStack_.clear();
	linkText_.clear();
	codeBlockLang_.clear();

	activeStyles_          = STYLE_NONE;
	codeFenceTicks_        = 0;
	headingLevel_          = 0;
	scanMode_              = SCAN_NONE;
	closeChar_             = 0;
	closeLen_              = 0;
	italicMarker_          = 0;

	atLineStart_           = true;
	inCodeBlock_           = false;
	escapeNext_            = false;
	readingHeadingPrefix_  = false;
	codeBlockBuffering_    = false;
	codeLineNumber_        = 0;

	// ---- 表格缓冲 ----
	tableState_ = TABLE_NONE;
	tableRaw_.clear();
	tableLine_.clear();
}

std::string Markdown::renderInline(const std::string& text)
{
	// 用临时实例避免状态污染；追加 \n 以触发样式闭合
	Markdown temp;
	const char* result = temp(text + "\n");
	std::string rendered(result);
	// 去掉尾部 \n
	if (!rendered.empty() && rendered.back() == '\n')
		rendered.pop_back();
	return rendered;
}

// ============================================================================
// ANSI 辅助
// ============================================================================

const char* Markdown::ansiOpen(int style)
{
	switch (style)
	{
	case STYLE_BOLD:   return kBold;
	case STYLE_ITALIC: return kItalic;
	case STYLE_CODE:   return kCyan;
	case STYLE_STRIKE: return kStrike;
	default:           return "";
	}
}

const char* Markdown::ansiReset() { return kReset; }

void Markdown::emitActiveStyles(std::string& buffer)
{
	if (activeStyles_ & STYLE_BOLD)   buffer += kBold;
	if (activeStyles_ & STYLE_ITALIC) buffer += kItalic;
	if (activeStyles_ & STYLE_CODE)   buffer += kCyan;
	if (activeStyles_ & STYLE_STRIKE) buffer += kStrike;
}

void Markdown::emitStyleTransition(std::string& buffer, int oldStyles, int newStyles)
{
	if (oldStyles == newStyles) return;

	if (newStyles == STYLE_NONE) { buffer += kReset; return; }
	if (oldStyles != STYLE_NONE)  buffer += kReset;

	if (newStyles & STYLE_BOLD)   buffer += kBold;
	if (newStyles & STYLE_ITALIC) buffer += kItalic;
	if (newStyles & STYLE_CODE)   buffer += kCyan;
	if (newStyles & STYLE_STRIKE) buffer += kStrike;
}

void Markdown::closeAllInline(std::string& buffer)
{
	if (activeStyles_ != STYLE_NONE)
	{
		buffer += kReset;
		activeStyles_ = STYLE_NONE;
	}
	italicMarker_ = 0;
}

bool Markdown::isInlineMarker(char c)
{
	return c == '*' || c == '_' || c == '`' || c == '[' || c == '~';
}

void Markdown::flushPendingAsLiteral(std::string& buffer)
{
	buffer += pending_;
	pending_.clear();
}

// ============================================================================
// 闭合标记查找（用于递归行内解析）
// ============================================================================
static size_t findClosing(const std::string& s, size_t start,
                          const char* marker, int markerLen)
{
	char m0 = marker[0];
	char m1 = (markerLen > 1) ? marker[1] : 0;

	for (size_t i = start; i < s.size(); ++i)
	{
		// 跳过转义
		if (s[i] == '\\' && i + 1 < s.size()) { ++i; continue; }

		if (s[i] != m0) continue;

		if (markerLen == 1)
		{
			// 单字符标记：确保不是双字符的一部分
			if (m0 == '*' && i + 1 < s.size() && s[i + 1] == '*') continue;
			if (m0 == '_' && i + 1 < s.size() && s[i + 1] == '_') continue;
			if (m0 == '~' && i + 1 < s.size() && s[i + 1] == '~') continue;
			return i;
		}
		else
		{
			if (i + 1 < s.size() && s[i + 1] == m1) return i;
		}
	}
	return std::string::npos;
}

// ============================================================================
// 递归行内内容解析
// ============================================================================
void Markdown::resolveInlineContent(const std::string& text, std::string& buffer)
{
	size_t pos = 0;
	while (pos < text.size())
	{
		char c = text[pos];

		// ---- 转义 ----
		if (c == '\\' && pos + 1 < text.size())
		{
			buffer += text[pos + 1];
			pos += 2;
			continue;
		}

		// ---- 粗体 **...** ----
		if (c == '*' && pos + 1 < text.size() && text[pos + 1] == '*')
		{
			size_t end = findClosing(text, pos + 2, "**", 2);
			if (end != std::string::npos)
			{
				std::string inner = text.substr(pos + 2, end - pos - 2);
				int old = activeStyles_;
				activeStyles_ |= STYLE_BOLD;
				emitStyleTransition(buffer, old, activeStyles_);
				resolveInlineContent(inner, buffer);
				activeStyles_ = old;
				emitStyleTransition(buffer, activeStyles_ | STYLE_BOLD, activeStyles_);
				pos = end + 2;
				continue;
			}
		}

		// ---- 斜体 *...*（不匹配 **） ----
		if (c == '*' && !(activeStyles_ & STYLE_BOLD))
		{
			size_t end = findClosing(text, pos + 1, "*", 1);
			if (end != std::string::npos)
			{
				std::string inner = text.substr(pos + 1, end - pos - 1);
				int old = activeStyles_;
				activeStyles_ |= STYLE_ITALIC;
				italicMarker_ = '*';
				emitStyleTransition(buffer, old, activeStyles_);
				resolveInlineContent(inner, buffer);
				activeStyles_ = old;
				italicMarker_ = 0;
				emitStyleTransition(buffer, activeStyles_ | STYLE_ITALIC, activeStyles_);
				pos = end + 1;
				continue;
			}
		}

		// ---- 斜体 _..._ ----
		if (c == '_')
		{
			size_t end = findClosing(text, pos + 1, "_", 1);
			if (end != std::string::npos)
			{
				std::string inner = text.substr(pos + 1, end - pos - 1);
				int old = activeStyles_;
				activeStyles_ |= STYLE_ITALIC;
				italicMarker_ = '_';
				emitStyleTransition(buffer, old, activeStyles_);
				resolveInlineContent(inner, buffer);
				activeStyles_ = old;
				italicMarker_ = 0;
				emitStyleTransition(buffer, activeStyles_ | STYLE_ITALIC, activeStyles_);
				pos = end + 1;
				continue;
			}
		}

		// ---- 行内代码 `...` ----
		if (c == '`')
		{
			size_t end = text.find('`', pos + 1);
			if (end != std::string::npos)
			{
				std::string code = text.substr(pos + 1, end - pos - 1);
				int old = activeStyles_;
				activeStyles_ |= STYLE_CODE;
				emitStyleTransition(buffer, old, activeStyles_);
				buffer += code; // 代码内容不做递归解析
				activeStyles_ = old;
				emitStyleTransition(buffer, activeStyles_ | STYLE_CODE, activeStyles_);
				pos = end + 1;
				continue;
			}
		}

		// ---- 删除线 ~~...~~ ----
		if (c == '~' && pos + 1 < text.size() && text[pos + 1] == '~')
		{
			size_t end = findClosing(text, pos + 2, "~~", 2);
			if (end != std::string::npos)
			{
				std::string inner = text.substr(pos + 2, end - pos - 2);
				int old = activeStyles_;
				activeStyles_ |= STYLE_STRIKE;
				emitStyleTransition(buffer, old, activeStyles_);
				resolveInlineContent(inner, buffer);
				activeStyles_ = old;
				emitStyleTransition(buffer, activeStyles_ | STYLE_STRIKE, activeStyles_);
				pos = end + 2;
				continue;
			}
		}

		// ---- 链接 [text](url) ----
		if (c == '[')
		{
			size_t cb = text.find(']', pos + 1);
			if (cb != std::string::npos && cb + 1 < text.size() && text[cb + 1] == '(')
			{
				size_t ce = text.find(')', cb + 2);
				if (ce != std::string::npos)
				{
					std::string lt  = text.substr(pos + 1, cb - pos - 1);
					std::string url = text.substr(cb + 2, ce - cb - 2);

					buffer += kUnderline;
					buffer += kBlue;
					resolveInlineContent(lt, buffer);
					buffer += kReset;
					emitActiveStyles(buffer);
					buffer += " (";
					buffer += kBlue;
					buffer += url;
					buffer += kReset;
					emitActiveStyles(buffer);
					buffer += ")";

					pos = ce + 1;
					continue;
				}
			}
		}

		// 普通字符
		buffer += c;
		++pos;
	}
}

// ============================================================================
// 行内扫描：从 SCAN_MARKER 判定标记类型 startInlineScan / continueScan
// ============================================================================
void Markdown::startInlineScan(std::string& buffer)
{
	if (pending_.empty()) return;

	char c0 = pending_[0];

	// ---- '*'：需第 2 字符区分粗体/斜体 ----
	if (c0 == '*')
	{
		if (pending_.size() >= 2 && pending_[1] == '*')
		{
			pending_.clear();
			scanMode_  = SCAN_BOLD;
			closeChar_ = '*';
			closeLen_  = 2;
			int old = activeStyles_;
			activeStyles_ |= STYLE_BOLD;
			emitStyleTransition(buffer, old, activeStyles_);
			return;
		}
		if (pending_.size() >= 2)
		{
			// * + 非* → 斜体
			pending_.erase(0, 1);
			scanMode_  = SCAN_ITALIC;
			closeChar_ = '*';
			closeLen_  = 1;
			italicMarker_ = '*';
			int old = activeStyles_;
			activeStyles_ |= STYLE_ITALIC;
			emitStyleTransition(buffer, old, activeStyles_);
			return;
		}
		return; // 等待更多
	}

	// ---- '_'：单字符判斜体 ----
	if (c0 == '_')
	{
		if (pending_.size() >= 2 && pending_[1] == '_')
		{
			flushPendingAsLiteral(buffer);
			scanMode_ = SCAN_NONE;
			return;
		}
		pending_.erase(0, 1);
		scanMode_  = SCAN_ITALIC;
		closeChar_ = '_';
		closeLen_  = 1;
		italicMarker_ = '_';
		int old = activeStyles_;
		activeStyles_ |= STYLE_ITALIC;
		emitStyleTransition(buffer, old, activeStyles_);
		return;
	}

	// ---- '~'：需第 2 字符 ----
	if (c0 == '~')
	{
		if (pending_.size() >= 2 && pending_[1] == '~')
		{
			pending_.clear();
			scanMode_  = SCAN_STRIKE;
			closeChar_ = '~';
			closeLen_  = 2;
			int old = activeStyles_;
			activeStyles_ |= STYLE_STRIKE;
			emitStyleTransition(buffer, old, activeStyles_);
			return;
		}
		if (pending_.size() >= 2)
		{
			flushPendingAsLiteral(buffer);
			scanMode_ = SCAN_NONE;
			return;
		}
		return; // 等待更多
	}

	// ---- '`'：行内代码 ----
	if (c0 == '`')
	{
		pending_.erase(0, 1);
		scanMode_  = SCAN_CODE;
		closeChar_ = '`';
		closeLen_  = 1;
		int old = activeStyles_;
		activeStyles_ |= STYLE_CODE;
		emitStyleTransition(buffer, old, activeStyles_);
		return;
	}

	// ---- '['：链接 ----
	if (c0 == '[')
	{
		pending_.erase(0, 1);
		scanMode_ = SCAN_LINK_TEXT;
		return;
	}

	flushPendingAsLiteral(buffer);
	scanMode_ = SCAN_NONE;
}

// ============================================================================
// 行内扫描：检查最新字符是否闭合当前标记
// ============================================================================
void Markdown::continueScan(std::string& buffer)
{
	switch (scanMode_)
	{
	case SCAN_ITALIC:
		if (pending_.size() >= 1 && pending_.back() == closeChar_)
		{
			bool esc = (pending_.size() >= 2 && pending_[pending_.size() - 2] == '\\');
			if (!esc)
			{
				std::string inner = pending_.substr(0, pending_.size() - 1);
				pending_.clear();
				scanMode_ = SCAN_NONE;
				int old = activeStyles_;
				activeStyles_ &= ~STYLE_ITALIC;
				italicMarker_ = 0;
				resolveInlineContent(inner, buffer);
				emitStyleTransition(buffer, old, activeStyles_);
			}
		}
		break;

	case SCAN_BOLD:
		if (pending_.size() >= 2 &&
		    pending_[pending_.size() - 2] == '*' &&
		    pending_[pending_.size() - 1] == '*')
		{
			bool esc = (pending_.size() >= 3 && pending_[pending_.size() - 3] == '\\');
			if (!esc)
			{
				std::string inner = pending_.substr(0, pending_.size() - 2);
				pending_.clear();
				scanMode_ = SCAN_NONE;
				int old = activeStyles_;
				activeStyles_ &= ~STYLE_BOLD;
				resolveInlineContent(inner, buffer);
				emitStyleTransition(buffer, old, activeStyles_);
			}
		}
		break;

	case SCAN_CODE:
		if (pending_.size() >= 1 && pending_.back() == '`')
		{
			bool esc = (pending_.size() >= 2 && pending_[pending_.size() - 2] == '\\');
			if (!esc)
			{
				std::string code = pending_.substr(0, pending_.size() - 1);
				pending_.clear();
				scanMode_ = SCAN_NONE;
				int old = activeStyles_;
				activeStyles_ &= ~STYLE_CODE;
				buffer += code;
				emitStyleTransition(buffer, old, activeStyles_);
			}
		}
		break;

	case SCAN_STRIKE:
		if (pending_.size() >= 2 &&
		    pending_[pending_.size() - 2] == '~' &&
		    pending_[pending_.size() - 1] == '~')
		{
			bool esc = (pending_.size() >= 3 && pending_[pending_.size() - 3] == '\\');
			if (!esc)
			{
				std::string inner = pending_.substr(0, pending_.size() - 2);
				pending_.clear();
				scanMode_ = SCAN_NONE;
				int old = activeStyles_;
				activeStyles_ &= ~STYLE_STRIKE;
				resolveInlineContent(inner, buffer);
				emitStyleTransition(buffer, old, activeStyles_);
			}
		}
		break;

	case SCAN_LINK_TEXT:
		if (pending_.size() >= 1 && pending_.back() == ']')
		{
			linkText_ = pending_.substr(0, pending_.size() - 1);
			pending_.clear();
			scanMode_ = SCAN_LINK_EXPECT_PAREN;
		}
		break;

	case SCAN_LINK_EXPECT_PAREN:
		if (pending_.size() >= 1 && pending_[0] == '(')
		{
			pending_.erase(0, 1);
			scanMode_ = SCAN_LINK_URL;
		}
		else if (pending_.size() >= 1)
		{
			// 不是链接，回退
			buffer += '[';
			resolveInlineContent(linkText_, buffer);
			buffer += ']';
			linkText_.clear();
			buffer += pending_;
			pending_.clear();
			scanMode_ = SCAN_NONE;
		}
		break;

	case SCAN_LINK_URL:
		if (pending_.size() >= 1 && pending_.back() == ')')
		{
			std::string url = pending_.substr(0, pending_.size() - 1);
			pending_.clear();
			scanMode_ = SCAN_NONE;

			buffer += kUnderline;
			buffer += kBlue;
			resolveInlineContent(linkText_, buffer);
			buffer += kReset;
			emitActiveStyles(buffer);
			buffer += " (";
			buffer += kBlue;
			buffer += url;
			buffer += kReset;
			emitActiveStyles(buffer);
			buffer += ")";
			linkText_.clear();
		}
		break;

	default:
		break;
	}
}

// ============================================================================
// 换行 / 转义
// ============================================================================
void Markdown::handleNewline(std::string& buffer)
{
	// 1. 处理扫描中的未闭合状态
	if (scanMode_ == SCAN_MARKER)
	{
		flushPendingAsLiteral(buffer);
		scanMode_ = SCAN_NONE;
	}
	else if (scanMode_ == SCAN_LINK_EXPECT_PAREN)
	{
		buffer += '[';
		resolveInlineContent(linkText_, buffer);
		buffer += ']';
		linkText_.clear();
		buffer += pending_;
		pending_.clear();
		scanMode_ = SCAN_NONE;
	}
	else if (scanMode_ != SCAN_NONE)
	{
		// 未闭合的行内标记 — 强制关闭样式，内容当字面量
		closeAllInline(buffer);
		buffer += pending_;
		pending_.clear();
		scanMode_ = SCAN_NONE;
	}

	// 2. 行首 pending_（块标记判定用）
	if (!pending_.empty())
	{
		finalizeLineStart(buffer);
	}

	// 2.5 关闭流式标题样式
	if (headingLevel_ > 0)
		buffer += kReset;

	// 3. 关闭所有活跃行内样式
	closeAllInline(buffer);

	// 4. 输出换行
	buffer += '\n';

	// 5. 重置行级状态
	atLineStart_ = true;
	headingLevel_ = 0;
	readingHeadingPrefix_ = false;
	leadingSpaces_.clear();
	pending_.clear();
	scanMode_ = SCAN_NONE;
}

// ============================================================================
// 转义处理
// ============================================================================
void Markdown::handleEscape(char c, std::string& buffer)
{
	escapeNext_ = false;

	if (c == '\n')
	{
		// 行尾反斜杠 → 硬换行（Markdown 中的 \<newline> = <br>）
		handleNewline(buffer);
		return;
	}

	// 输出转义字符的字面值
	buffer += c;

	// 如果正在行内扫描，保留 \ 以便递归解析时正确处理
	if (scanMode_ != SCAN_NONE)
	{
		pending_ += '\\';
		pending_ += c;
	}
}

// ============================================================================
// 行首块标记判定
// ============================================================================
void Markdown::finalizeLineStart(std::string& buffer)
{
	if (pending_.empty()) return;

	// 保留前导空白（缩进）
	buffer += leadingSpaces_;
	leadingSpaces_.clear();

	char   c0  = pending_[0];
	size_t len = pending_.size();

	// ---- 标题 # ----
	if (c0 == '#')
	{
		int lv = 0;
		while ((size_t)lv < len && pending_[lv] == '#') ++lv;
		if (lv >= 1 && lv <= 6)
		{
			buffer += headingAnsiOpen(lv);

			size_t contentStart = lv;
			if (contentStart < len && pending_[contentStart] == ' ') ++contentStart;
			if (contentStart < len)
				resolveInlineContent(pending_.substr(contentStart), buffer);
			buffer += kReset;
			pending_.clear();
			headingLevel_ = 0; // 标题已闭合，避免 handleNewline 重复 reset
			return;
		}
	}

	// ---- 代码围栏 ``` 或 ~~~ ----
	if ((c0 == '`' || c0 == '~') && len >= 3)
	{
		size_t cnt = 0;
		while (cnt < len && pending_[cnt] == c0) ++cnt;
		if (cnt >= 3)
		{
			openCodeFence(pending_, buffer);
			pending_.clear();
			return;
		}
	}

	// ---- 分割线 --- / *** / ___ ----
	if ((c0 == '-' || c0 == '*' || c0 == '_') && len >= 3 &&
	    pending_.find_first_not_of(std::string(1, c0) + " ") == std::string::npos)
	{
		size_t cnt = 0;
		for (size_t i = 0; i < len; ++i) if (pending_[i] == c0) ++cnt;
		if (cnt >= 3)
		{
			buffer += kDim;
			for (int i = 0; i < 60; ++i) buffer += kHr;
			buffer += kReset;
			pending_.clear();
			return;
		}
	}

	// ---- 引用 > ----
	if (c0 == '>')
	{
		buffer += kDim;
		buffer += kBar;
		buffer += " ";
		buffer += kReset;
		emitActiveStyles(buffer);

		size_t start = 1;
		if (start < len && pending_[start] == ' ') ++start;
		if (start < len)
			resolveInlineContent(pending_.substr(start), buffer);
		pending_.clear();
		return;
	}

	// ---- 无序列表 "- " / "* " / "+ " ----
	if ((c0 == '-' || c0 == '*' || c0 == '+') && len >= 2 && pending_[1] == ' ')
	{
		buffer += kYellow;
		buffer += kBullet;
		buffer += " ";
		buffer += kReset;
		emitActiveStyles(buffer);

		if (len > 2)
			resolveInlineContent(pending_.substr(2), buffer);
		pending_.clear();
		return;
	}

	// ---- 有序列表 "N. " ----
	if (c0 >= '0' && c0 <= '9')
	{
		size_t dot = pending_.find(". ");
		if (dot != std::string::npos && dot > 0)
		{
			bool ok = true;
			for (size_t i = 0; i < dot; ++i)
				if (pending_[i] < '0' || pending_[i] > '9') { ok = false; break; }
			if (ok)
			{
				buffer += kYellow;
				buffer += pending_.substr(0, dot + 2); // "N. "
				buffer += kReset;
				emitActiveStyles(buffer);

				if (len > dot + 2)
					resolveInlineContent(pending_.substr(dot + 2), buffer);
				pending_.clear();
				return;
			}
		}
	}

	// ---- 未识别 → 普通段落 ----
	emitBlockPrefix(buffer);
	resolveInlineContent(pending_, buffer);
	pending_.clear();
}

// ============================================================================
// 代码块
// ============================================================================
void Markdown::openCodeFence(const std::string& fenceLine, std::string& buffer)
{
	char f = fenceLine[0];
	int  cnt = 0;
	while (cnt < (int)fenceLine.size() && fenceLine[cnt] == f) ++cnt;

	inCodeBlock_     = true;
	codeFenceTicks_  = cnt;
	codeBlockLang_   = "";
	codeLineNumber_  = 1;

	if ((size_t)cnt < fenceLine.size())
	{
		size_t start = cnt;
		if (fenceLine[start] == ' ') ++start;
		codeBlockLang_ = fenceLine.substr(start);
	}

	// 围栏标记（暗灰） + 语言标签（亮黄）
	buffer += kCodeFence;
	buffer += std::string(cnt, f);
	if (!codeBlockLang_.empty())
	{
		buffer += " ";
		buffer += kCodeLang;
		buffer += codeBlockLang_;
	}
	buffer += kReset;
}

void Markdown::handleCodeBlockChar(char c, std::string& buffer)
{
	if (c == '\n')
	{
		if (codeBlockBuffering_)
		{
			// 检查缓冲内容是否为闭合围栏
			char f = pending_[0];
			bool isClose = false;
			if ((f == '`' || f == '~') && (int)pending_.size() >= codeFenceTicks_)
			{
				size_t cnt = 0;
				while (cnt < pending_.size() && pending_[cnt] == f) ++cnt;
				bool ok = true;
				for (size_t i = cnt; i < pending_.size(); ++i)
					if (pending_[i] != ' ' && pending_[i] != '\t') { ok = false; break; }
				if ((int)cnt >= codeFenceTicks_ && ok)
				{
					// 闭合围栏 — 不输出 pending_ 中的代码文本
					pending_.clear();
					codeBlockBuffering_ = false;
					buffer += kReset;
					buffer += '\n';
					buffer += kCodeFence;
					buffer += std::string(cnt, f);
					buffer += kReset;
					buffer += '\n';
					inCodeBlock_ = false;
					codeFenceTicks_ = 0;
					codeBlockLang_.clear();
					codeLineNumber_ = 0;
					atLineStart_ = true;
					return;
				}
			}
			// 非闭合围栏 — 刷新缓冲
			codeBlockBuffering_ = false;
			outputCodeBlockGutter(buffer, codeLineNumber_);
			buffer += pending_;
		}
		else if (atLineStart_)
		{
			// 空行 — 仍输出 gutter 以保持背景连续
			outputCodeBlockGutter(buffer, codeLineNumber_);
		}

		// EL (Erase in Line) — 清除至行尾，终端用当前背景色填充
		// 解决中文字符宽度不一致导致的锯齿问题
		buffer += "\033[K";

		buffer += kReset;
		buffer += '\n';
		pending_.clear();
		atLineStart_ = true;
		codeLineNumber_++;
		return;
	}

	if (atLineStart_)
	{
		atLineStart_ = false;
		if (c == '`' || c == '~')
		{
			// 可能为闭合围栏 — 缓冲，暂不输出
			codeBlockBuffering_ = true;
			pending_ += c;
			return;
		}
		// 普通代码行 — 输出 gutter 前缀
		outputCodeBlockGutter(buffer, codeLineNumber_);
		pending_ += c;
		buffer += c;
		return;
	}

	if (codeBlockBuffering_)
	{
		// 仍可能为围栏（同字符或空白）
		if (c == pending_[0] || c == ' ' || c == '\t')
		{
			pending_ += c;
			return;
		}
		// 非围栏字符 → 刷新全部缓冲
		codeBlockBuffering_ = false;
		pending_ += c;
		outputCodeBlockGutter(buffer, codeLineNumber_);
		buffer += pending_;
		return;
	}

	pending_ += c;
	buffer += c;
}

void Markdown::outputCodeBlockGutter(std::string& buffer, int lineNum)
{
	buffer += kCodeBg;       // 深灰背景
	buffer += kCodeGutter;   // gutter 颜色
	buffer += kBar;          // │
	buffer += " ";
	buffer += kCodeLineNo;   // 行号颜色
	// 右对齐到 3 位
	if (lineNum < 10)       buffer += "  ";
	else if (lineNum < 100) buffer += " ";
	buffer += std::to_string(lineNum);
	buffer += " ";
	buffer += kCodeText;     // 代码文本颜色
}

// ============================================================================
// 块前缀
// ============================================================================
void Markdown::emitBlockPrefix(std::string& buffer)
{
	for (size_t i = 0; i < blockStack_.size(); ++i)
	{
		const auto& bf = blockStack_[i];
		switch (bf.type)
		{
		case BlockFrame::QUOTE:
			buffer += kDim;
			buffer += kBar;
			buffer += " ";
			buffer += kReset;
			emitActiveStyles(buffer);
			break;
		case BlockFrame::LIST_U:
			buffer += kYellow;
			buffer += (i + 1 == blockStack_.size()) ? kBullet : " ";
			buffer += (i + 1 == blockStack_.size()) ? " " : "  ";
			buffer += kReset;
			emitActiveStyles(buffer);
			break;
		case BlockFrame::LIST_O:
			buffer += kYellow;
			if (i + 1 == blockStack_.size())
			{ buffer += std::to_string(bf.number); buffer += ". "; }
			else
				buffer += "   ";
			buffer += kReset;
			emitActiveStyles(buffer);
			break;
		default:
			break;
		}
	}
}

// ============================================================================
// 表格检测 & 缓冲
// ============================================================================

bool Markdown::isTableSeparatorLine(const char* line)
{
	if (!line || !*line) return false;
	bool hasDash = false;
	for (const char* p = line; *p; ++p)
	{
		if (*p == '|' || *p == ':' || *p == ' ' || *p == '\t') continue;
		if (*p == '-') { hasDash = true; continue; }
		return false;
	}
	return hasDash;
}

void Markdown::flushTable(std::string& buffer)
{
	if (tableState_ == TABLE_NONE) return;

	std::string rendered = aMarkdownTable(tableRaw_);
	if (!rendered.empty())
		buffer += rendered;
	else
		buffer += tableRaw_;  // 解析失败：原样输出

	tableState_ = TABLE_NONE;
	tableRaw_.clear();
	tableLine_.clear();
	atLineStart_ = true;  // 下一字符在新行首
}

void Markdown::processTableNewline(std::string& buffer)
{
	// 去掉尾部 \n
	std::string line = tableLine_;
	if (!line.empty() && line.back() == '\n') line.pop_back();
	if (!line.empty() && line.back() == '\r') line.pop_back();

	// 定位到非空白字符
	const char* p = line.c_str();
	while (*p == ' ' || *p == '\t') ++p;
	bool startsWithPipe = (*p == '|');
	bool isEmpty        = (*p == '\0');

	if (tableState_ == TABLE_HEADER)
	{
		if (tableRaw_.empty())
		{
			// 第一条 | 行：暂存为候选表头，等待下一行判定
			tableRaw_ += line + '\n';
			return;
		}
		// 已有候选表头，检查当前行是否为分隔行
		if (isTableSeparatorLine(p))
		{
			// 确认表格：header + separator
			tableRaw_ += line + '\n';
			tableState_ = TABLE_BODY;
			return;
		}
		// 第二行不是分隔行 → 不是表格，原样输出缓冲的所有行
		tableRaw_ += line + '\n';
		buffer += tableRaw_;
		tableState_ = TABLE_NONE;
		tableRaw_.clear();
		atLineStart_ = true;
		return;
	}

	// TABLE_BODY
	if (isEmpty)
	{
		// 空行结束表格
		flushTable(buffer);
		buffer += '\n';  // 保留空行
		return;
	}

	if (startsWithPipe)
	{
		// 数据行
		tableRaw_ += line + '\n';
		return;
	}

	// 非 | 非空行 → 表格结束，原样输出当前行
	flushTable(buffer);
	buffer += line + '\n';
}

// ============================================================================
// 核心分发器 feed()
// ============================================================================
void Markdown::feed(char c, std::string& buffer)
{
	// ---- 1. 代码块模式 ----
	if (inCodeBlock_)
	{
		handleCodeBlockChar(c, buffer);
		return;
	}

	// ---- 2. 表格缓冲模式 ----
	if (tableState_ != TABLE_NONE)
	{
		tableLine_ += c;
		if (c == '\n')
		{
			processTableNewline(buffer);
			tableLine_.clear();
		}
		return;
	}

	// ---- 3. 转义 ----
	if (escapeNext_)
	{
		handleEscape(c, buffer);
		return;
	}

	if (c == '\\')
	{
		escapeNext_ = true;
		return;
	}

	// ---- 4. 换行 ----
	if (c == '\n')
	{
		handleNewline(buffer);
		return;
	}

	// ---- 5. 标题前缀收集 ----
	if (readingHeadingPrefix_)
	{
		if (c == '#' && headingLevel_ < 6)
		{
			++headingLevel_;
			pending_ += c;
			return;
		}

		// 标题级别确定
		readingHeadingPrefix_ = false;

		// # 后必须有空格才是标题，否则 (#tag) 当作普通文本
		if (c == ' ')
		{
			buffer += headingAnsiOpen(headingLevel_);
			pending_.clear();
			return;
		}

		// #tag — 不是标题，回退 # 为字面量
		buffer += pending_;   // pending_ 中存的是 # 号序列
		pending_.clear();
		headingLevel_ = 0;
		buffer += c;           // 当前字符也输出
		return;
	}

	// ---- 6. 行内扫描模式 ----
	if (scanMode_ != SCAN_NONE)
	{
		if (scanMode_ == SCAN_MARKER)
		{
			pending_ += c;
			startInlineScan(buffer);
			return;
		}

		pending_ += c;
		continueScan(buffer);
		return;
	}

	// ---- 7. 行首块级 ----
	if (atLineStart_)
	{
		if (c == ' ' || c == '\t')
		{
			leadingSpaces_ += c;
			return;
		}

		atLineStart_ = false;

		// ---- 表格候选：行首 | ----
		if (c == '|')
		{
			tableState_ = TABLE_HEADER;
			tableRaw_.clear();
			tableLine_ = std::move(leadingSpaces_);
			tableLine_ += c;
			leadingSpaces_.clear();
			return;
		}

		// 标题
		if (c == '#')
		{
			headingLevel_ = 1;
			readingHeadingPrefix_ = true;
			pending_ += c;
			return;
		}

		// 候选块标记
		if (c == '-' || c == '*' || c == '+' || c == '>' ||
		    c == '`' || c == '~' || c == '_' ||
		    (c >= '0' && c <= '9'))
		{
			pending_ += c;
			return;
		}

		// 普通段落行
		emitBlockPrefix(buffer);
		buffer += leadingSpaces_;
		leadingSpaces_.clear();

		if (isInlineMarker(c))
		{
			pending_ += c;
			scanMode_ = SCAN_MARKER;
			startInlineScan(buffer);
		}
		else
		{
			buffer += c;
		}
		return;
	}

	// ---- 8. pending_ 中有行首候选，遇空格判定块类型 ----
	//      但 ` ~ _ 可能是行内标记（代码/删除线/斜体），不因空格而判定
	if (!pending_.empty())
	{
		if (c == ' ')
		{
			char first = pending_[0];
			if (first != '`' && first != '~' && first != '_')
			{
				pending_ += c;
				finalizeLineStart(buffer);
				return;
			}
		}
		// 继续累积（如 ``` 的后续反引号、有序列表数字）
		pending_ += c;
		return;
	}

	// ---- 9. 行内标记起始 ----
	if (isInlineMarker(c))
	{
		pending_ += c;
		scanMode_ = SCAN_MARKER;
		startInlineScan(buffer);
		return;
	}

	// ---- 10. 普通字符 ----
	buffer += c;
}

AST_NAMESPACE_END
