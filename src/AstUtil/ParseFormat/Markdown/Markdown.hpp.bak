///
/// @file      Markdown.hpp
/// @brief     Markdown 渲染器 — 将 LLM 流式 Markdown 文本转为 ANSI 终端输出
/// @details   支持格式：标题、加粗、斜体、行内代码、代码块、
///                      无序/有序列表、引用、分割线、链接、转义。
///            无行缓冲，即到即出。支持跨 chunk 的流式渲染。
/// @author    axel
/// @date      2026-06-15
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#pragma once

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*! @addtogroup Platform @{ */

class AST_UTIL_API Markdown
{
public:
	Markdown() = default;
	~Markdown() = default;

	/// @brief 渲染一段文本，返回本次 chunk 新产生的增量输出
	const char* operator()(StringView chunk);

	/// @brief 打印渲染结果（不返回）
	/// @note  直接在标准输出（cout）打印，不返回任何值
	/// @param chunk 要渲染的文本 chunk
	void print(StringView chunk);

	/// @brief 渲染行内 Markdown 文本（不含块级元素），返回 ANSI 字符串
	/// @note  线程安全（每次调用使用独立状态）
	static std::string renderInline(const std::string& text);

	/// @brief 重置所有内部状态（新一轮对话前调用）
	void reset();

private:
	// ============================================================
	// 行内样式位掩码（可组合，如粗斜体 = STYLE_BOLD | STYLE_ITALIC）
	// ============================================================
	enum InlineStyle : int
	{
		STYLE_NONE   = 0,
		STYLE_BOLD   = 1 << 0,  ///< **...**
		STYLE_ITALIC = 1 << 1,  ///< *...* 或 _..._
		STYLE_CODE   = 1 << 2,  ///< `...`
		STYLE_STRIKE = 1 << 3,  ///< ~~...~~
	};

	// ============================================================
	// 行内扫描模式
	// ============================================================
	enum ScanMode
	{
		SCAN_NONE,
		SCAN_MARKER,            ///< 见到标记起始符，等待更多字符以判定
		SCAN_BOLD,              ///< 在 **...** 内部
		SCAN_ITALIC,            ///< 在 *...* 或 _..._ 内部
		SCAN_CODE,              ///< 在 `...` 内部
		SCAN_STRIKE,            ///< 在 ~~...~~ 内部
		SCAN_LINK_TEXT,         ///< 收集 [text]
		SCAN_LINK_EXPECT_PAREN, ///< 见到 ]，等待 (
		SCAN_LINK_URL,          ///< 收集 (url)
	};

	// ============================================================
	// 块级元素帧
	// ============================================================
	struct BlockFrame
	{
		enum Type { NONE, HEADING, LIST_U, LIST_O, QUOTE };
		Type type   = NONE;
		int  indent = 0;
		int  number = 0;
	};

	// ============================================================
	// 核心方法
	// ============================================================
	void feed(char c, std::string& buffer);

	// ---- 行内 ----
	static bool isInlineMarker(char c);
	void startInlineScan(std::string& buffer);
	void continueScan(std::string& buffer);
	void resolveInlineContent(const std::string& text, std::string& buffer);
	void flushPendingAsLiteral(std::string& buffer);
	void closeAllInline(std::string& buffer);
	void emitActiveStyles(std::string& buffer);
	void emitStyleTransition(std::string& buffer, int oldStyles, int newStyles);
	static const char* ansiOpen(int style);
	static const char* ansiReset();

	// ---- 块级 ----
	void finalizeLineStart(std::string& buffer);
	void emitBlockPrefix(std::string& buffer);
	void openCodeFence(const std::string& fenceLine, std::string& buffer);
	void handleCodeBlockChar(char c, std::string& buffer);
	void handleNewline(std::string& buffer);
	void handleEscape(char c, std::string& buffer);
	void outputCodeBlockGutter(std::string& buffer, int lineNum);

	// ---- 表格 ----
	void processTableNewline(std::string& buffer);
	void flushTable(std::string& buffer);
	static bool isTableSeparatorLine(const char* line);

private:
	A_DISABLE_COPY(Markdown);

	// ============================================================
	// 持久状态（跨 chunk / feed 延续）
	// ============================================================
	std::string result_;
	std::string pending_;              ///< 前景缓冲（行首块标记 / 行内扫描内容）
	std::string leadingSpaces_;        ///< 当前行前导空白
	std::string linkText_;             ///< [链接文本] 暂存
	std::string codeBlockLang_;        ///< 代码块语言标识
	std::vector<BlockFrame> blockStack_;

	int      activeStyles_          = STYLE_NONE;
	int      codeFenceTicks_        = 0;
	int      headingLevel_          = 0;
	ScanMode scanMode_              = SCAN_NONE;
	char     closeChar_             = 0;
	int      closeLen_              = 0;
	char     italicMarker_          = 0;

	bool atLineStart_           = true;
	bool inCodeBlock_           = false;
	bool escapeNext_            = false;
	bool readingHeadingPrefix_  = false;
	bool codeBlockBuffering_    = false;
	int  codeLineNumber_        = 0;

	// ---- 表格缓冲 ----
	enum TableState { TABLE_NONE, TABLE_HEADER, TABLE_BODY };
	TableState  tableState_ = TABLE_NONE;
	std::string tableRaw_;     ///< 已确认表格的原始行（含 \n）
	std::string tableLine_;    ///< 当前行缓冲
};

/*! @} */

AST_NAMESPACE_END
