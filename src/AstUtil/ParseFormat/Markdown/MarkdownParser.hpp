///
/// @file      MarkdownParser.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-06-15
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
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class MarkdownSax;

/// @brief  Markdown解析器（SAX事件驱动，流式输入）
class AST_UTIL_API MarkdownParser {
public:
    MarkdownParser(MarkdownSax& sax)
        : sax_(sax)
    {}
    ~MarkdownParser() = default;

    /// @brief  流式输入数据块
    void feed(StringView data);

    /// @brief  通知输入结束，处理剩余内容
    void flush();

    /// @brief  重置解析器状态
    void reset();
private:
    /// @brief  块级元素类型
    enum EBlockType
    {
        eBlockNone,         ///< 无块级元素
        eBlockParagraph,    ///< 段落
        eBlockHeading,      ///< 标题
        eBlockCode,         ///< 代码块
        eBlockList,         ///< 列表
        eBlockListItem,     ///< 列表项
        eBlockQuote,        ///< 引用
    };

    /// @brief  行内元素类型
    enum EInlineType
    {
        eInlineNone,       ///< 无行内元素
        eInlineLink,       ///< 链接
        eInlineEmphasis,   ///< 斜体
        eInlineStrong,     ///< 加粗
        eInlineCode,       ///< 代码
    };

    /// @brief  块级元素栈帧
    struct BlockFrame
    {
        EBlockType type;
        int       level;    // 标题级别(1-6) 或 0
        bool      ordered;  // 列表是否有序
    };

    // ============================================================
    // 行分类
    // ============================================================
    bool isHeadingLine(StringView line, int& level, StringView& content);
    bool isCodeFenceLine(StringView line, char& fenceChar, int& count, StringView& lang);
    bool isHorizRuleLine(StringView line);
    bool isBlockquoteLine(StringView line, StringView& content);
    bool isUnorderedListItem(StringView line, StringView& content);
    bool isOrderedListItem(StringView line, StringView& content, int& number);
    bool isClosingFence(StringView line);

    // ============================================================
    // 块栈管理
    // ============================================================
    void closeTop();
    void closeToBlock(EBlockType type);
    void closeAllBlocks();
    bool hasBlock(EBlockType type) const;
    void flushParagraph();
    void ensureParagraph();
    void ensureDocStarted();

    // ============================================================
    // 行处理
    // ============================================================
    void processLine(StringView line);
    void processBlockLine(StringView line);

    // ============================================================
    // 行内解析
    // ============================================================
    void parseInline(StringView text);
    static size_t findClosingMarker(StringView text, StringView marker);

    // ============================================================
    // 状态
    // ============================================================
    MarkdownSax& sax_;

    std::vector<BlockFrame> blockStack_;
    std::string lineBuf_;
    std::string paraBuf_;

    bool inCodeBlock_   = false;
    char codeFenceChar_ = 0;
    int  codeFenceCnt_  = 0;
    std::string codeFenceLang_;

    bool docStarted_ = false;
    bool docEnded_   = false;
    bool inList_     = false;
    bool listOrdered_ = false;
};

/*! @} */

AST_NAMESPACE_END
