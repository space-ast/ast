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


/// @brief  Markdown内联文本状态机（流式输入输出SAX事件驱动）
class AST_UTIL_API MarkdownInlineStateMachine
{
public:
    MarkdownInlineStateMachine(MarkdownSax& sax)
        : sax_(sax)
    {}
    void feed(StringView chunk);
    void feed(char c, std::string& result);
    void finish();
    std::string& result() { return result_; }
public:
    /// @brief  内联格式状态标志位
    enum class EStateFlags: int
    {
        ePlain          = 0x00,                         ///< 普通状态
        eEscape         = 0x02,                         ///< 转义状态
        eEmphasis       = 0x04,                         ///< 斜体状态
        eStrong         = 0x08,                         ///< 加粗状态
        eStrongEmphasis = eEmphasis | eStrong,          ///< 加粗斜体状态
        eDelete         = 0x10,                         ///< 删除线状态
    };

    /// @brief  解析模式 —— 控制当前在解析哪种高层结构
    enum class EParseMode
    {
        eNormal,       ///< 普通内联文本
        eCodeSpan,     ///< 行内代码 `...`
        eLinkText,     ///< 链接文本 [...]
        eLinkTextEnd,  ///< 链接文本刚看到 ]，等待 ( 确认
        eLinkURL,      ///< 链接 URL (...)
        eImageAlt,     ///< 图片 Alt ![...
        eImageAltEnd,  ///< 图片 Alt 刚看到 ]，等待 ( 确认
        eImageURL,     ///< 图片 URL (...)
    };
private:
    void toggleState(EStateFlags state);
    EStateFlags state() const { return state_; }

    /// @brief  刷新待提交字符和已缓冲文本到 result
    void flushPending(std::string& result);

    /// @brief  关闭所有活跃格式（finish / replay 共用）
    void closeAllFormats(std::string& result);

    /// @brief  以完整格式支持重播链接/图片文本
    void replayBufferedText(const std::string& text, std::string& result);

    // ---- 定界符游程解析 ----
    /// @brief  解析待提交的定界符游程 —— 决定打开或关闭格式
    /// @param next 紧跟游程之后的字符（0 = 输入末尾）
    void resolveDelimRun(char next, std::string& result);

    /// @brief  判断字符是否为 Unicode 空白符（含 ASCII 空格、制表符等）
    static bool isWhitespace(char c);
    /// @brief  判断字符是否为 Unicode 单词字符（字母、数字、下划线）
    static bool isWordChar(char c);
    /// @brief  判断字符是否为 ASCII 标点
    static bool isPunctuation(char c);

    // ---- 格式栈操作（封装深度计数器与 state_ 同步） ----
    void openEmph(char delim);
    void closeEmph();
    void openStrong();
    void closeStrong();
    /// @brief  若 pendingState_ 中有待定删除线则立即触发
    void triggerPendingDelete(std::string& result);

    /// @brief  提交 result 中的文本（不触及 tildePending_，供 ~ 处理器使用）
    void flushText(std::string& result);

    /// @brief  剥离行内代码首尾空格
    static std::string stripCodeSpan(const std::string& raw);
private:
    MarkdownSax& sax_;

    EStateFlags state_ = EStateFlags::ePlain;           ///< 格式状态位掩码
    int  emphDepth_   = 0;                             ///< 斜体嵌套深度（替代位标记的 eEmphasis）
    int  strongDepth_ = 0;                             ///< 粗体嵌套深度（替代位标记的 eStrong）
    char emphDelim_   = 0;                             ///< 打开当前斜体的定界符类型（* 或 _）
    EStateFlags pendingState_ = EStateFlags::ePlain;    ///< 待处理格式位掩码
    EParseMode  mode_ = EParseMode::eNormal;            ///< 当前解析模式
    std::string result_;                                ///< feed(StringView) 的输出缓冲
    std::string linkText_;                              ///< 链接文本 / 图片 Alt 缓冲
    std::string linkUrl_;                               ///< 链接 URL / 图片 URL 缓冲
    std::string codeBuf_;                               ///< 行内代码内容缓冲
    char   delimRunChar_ = 0;                           ///< 当前待提交定界符游程的类型（* 或 _, 0 = 无）
    int    delimRunLen_ = 0;                            ///< 当前待提交定界符游程的长度
    char   runPrevChar_ = 0;                            ///< 紧邻定界符游程之前的字符
    bool   tildePending_ = false;                       ///< 已看到单个 ~，等待第二个 ~
    bool   textEmitted_ = false;                        ///< 是否已输出过文本
    bool   emphasisOpenedFirst_ = false;                ///< 当 emph+strong 同时打开时，emph 是否先于 strong
    bool   pendingBang_ = false;                        ///< 刚看到 !，等待 [ 确认图片语法
    int    codeSpanBackticks_ = 0;                      ///< 行内代码段开界反引号数量
    int    codeSpanTickRun_ = 0;                        ///< 行内代码段内连续反引号计数
};

A_ENUM_CLASS_FLAGS(MarkdownInlineStateMachine::EStateFlags);


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
    void finish();

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
