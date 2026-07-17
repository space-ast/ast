///
/// @file      MarkdownBlockParser.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-17
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
#include "MarkdownInlineParser.hpp"
#include "MarkdownTableParser.hpp"
#include <vector>
#include <memory>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class MarkdownSax;

/// @brief  Markdown 块级解析器（字符级流式解析，SAX 事件驱动）
/// @details 逐字符判定行类型（标题/代码围栏/引用/列表/分割线/段落），
///          在确定的块上下文中将行内文本委托给 MarkdownBlockParser
class AST_UTIL_API MarkdownBlockParser
{
public:
    MarkdownBlockParser(MarkdownSax& sax);
    ~MarkdownBlockParser();

    /// @brief  流式输入数据块
    void feed(StringView data);

    /// @brief  通知输入结束，处理剩余内容
    void finish();

    /// @brief  重置解析器状态
    void reset();

private:
    // ---- 块级状态 ----
    enum class EState
    {
        eLineStart,          ///< 行首 — 跳过空白，待首个非空白字符判定
        eHeadingHashes,      ///< 正在累积 # 字符
        eHeadingContent,     ///< 标题内容（行内文本流式解析）
        eCodeFenceOpen,      ///< 正在识别代码围栏开界（计数 + 语言标识）
        eCodeBlockContent,   ///< 代码块内部
        eBlockquoteContent,  ///< 引用内容（行内文本流式解析）
        eListMarker,         ///< 正在解析列表标记（- / * / + / N.）
        eListItemContent,    ///< 列表项内容（行内文本流式解析）
        eParagraph,          ///< 段落文本（行内文本流式解析）
        eTable,              ///< 表格（委托给 MarkdownTableParser）
    };

    // ---- 块栈帧 ----
    enum class EBlockType
    {
        Paragraph,
        Heading,
        CodeBlock,
        List,
        ListItem,
        Blockquote,
    };
    struct BlockFrame
    {
        EBlockType type;
        int  level;    ///< 标题级别(1-6)，列表有序标记
        bool ordered;  ///< 列表是否有序
    };

    // ---- 核心方法 ----
    void feedChar(char c);

    /// @brief  将 contentBuf_ 中的内容喂入行内状态机并清空
    void flushInlineContent();
    /// @param force 为 true 时强制 flush（即使末尾为反引号），用于行末/文档结束
    void flushInlineContent(bool force);

    // ---- 行首分类 ----
    void classifyFirstChar(char c);

    // ---- 各状态字符处理 ----
    void handleHeadingHashes(char c);
    void handleHeadingChar(char c);
    void handleCodeFenceOpen(char c);
    void handleCodeBlockChar(char c);
    void handleBlockquoteChar(char c);
    void handleListMarker(char c);
    void handleParagraphChar(char c);
    void handleTableChar(char c);

    // ---- 块栈管理 ----
    void openBlock(EBlockType type, int level = 0, bool ordered = false);
    void closeTop();
    void closeToType(EBlockType type);
    void closeAllBlocks();
    bool hasBlock(EBlockType type) const;

    // ---- 段落辅助 ----
    void ensureParagraph();
    void endParagraph();
    void closeParagraph();       ///< 关闭段落（不刷新行内缓冲，由调用方负责）
    void ensureListBlock(bool ordered, int indent = 0); ///< 确保列表块已打开（含缩进嵌套）

    // ---- 文档辅助 ----
    void ensureDocStarted();

    // ============================================================
    // 状态成员
    // ============================================================
    MarkdownSax& sax_;
    MarkdownInlineParser inlineSM_;
    std::unique_ptr<MarkdownTableParser> tableSM_;

    EState state_ = EState::eLineStart;
    std::vector<BlockFrame> blockStack_{};

    // 分类缓冲（行首判定用，最多缓冲几个字符）
    std::string classBuf_{};
    // 行内内容缓冲（分类确认后，累积待喂入 inlineSM 的内容）
    std::string contentBuf_{};
    // 代码块行缓冲（仅代码块内逐行检测关界围栏）
    std::string codeLineBuf_{};

    // ---- 标题状态 ----
    int headingLevel_ = 0;

    // ---- 代码围栏状态 ----
    char codeFenceChar_ = 0;
    int  codeFenceCount_ = 0;
    std::string codeFenceLang_{};

    // ---- 列表状态 ----
    bool inList_           = false;
    bool listOrdered_      = false;
    int  currentLineIndent_ = 0;      ///< 当前行前导空白数
    bool listItemAfterNL_  = false;   ///< \n 后延迟关闭列表项
    bool lineFresh_         = true;   ///< 刚进入 eLineStart，需重置缩进计数

    /// @brief 获取当前最内层列表的缩进量（无列表时返回 -1）
    int currentListIndent() const;
    /// @brief 安全关闭已标记为延迟的列表项
    void closePendingListItem();

    // ---- 段落跨行追踪 ----
    bool paraAfterNL_ = false;   ///< 段落中刚看到 \n，等待下一字符判定软换行/段落结束
    bool paraHadContent_ = false;///< 当前段落是否已有内容（用于空行检测）

    // ---- 文档状态 ----
    bool docStarted_ = false;
};



/*! @} */

AST_NAMESPACE_END
