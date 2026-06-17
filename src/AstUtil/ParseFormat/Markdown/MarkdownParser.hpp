///
/// @file      MarkdownParser.hpp
/// @brief     Markdown 流式解析器 — SAX 事件驱动
/// @details   块级状态机 + 行内状态机，均为逐字符流式解析。
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
#include <memory>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class MarkdownSax;
class MarkdownTableStateMachine;


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

    /// @brief  将 result 中的待提交文本刷新到 SAX
    void flushPending(std::string& result);

    /// @brief  结束当前内联上下文（用于表格 cell 边界），刷新待提交文本、关闭格式
    void flushCell();

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


/// @brief  Markdown 块级状态机（字符级流式解析，SAX 事件驱动）
/// @details 逐字符判定行类型（标题/代码围栏/引用/列表/分割线/段落），
///          在确定的块上下文中将行内文本委托给 MarkdownInlineStateMachine。
class AST_UTIL_API MarkdownBlockStateMachine
{
public:
    MarkdownBlockStateMachine(MarkdownSax& sax);
    ~MarkdownBlockStateMachine();

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
        eTable,              ///< 表格（委托给 MarkdownTableStateMachine）
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
    void ensureListBlock(bool ordered); ///< 确保列表块已打开

    // ---- 文档辅助 ----
    void ensureDocStarted();

    // ============================================================
    // 状态成员
    // ============================================================
    MarkdownSax& sax_;
    MarkdownInlineStateMachine inlineSM_;
    std::unique_ptr<MarkdownTableStateMachine> tableSM_;

    EState state_ = EState::eLineStart;
    std::vector<BlockFrame> blockStack_;

    // 分类缓冲（行首判定用，最多缓冲几个字符）
    std::string classBuf_;
    // 行内内容缓冲（分类确认后，累积待喂入 inlineSM 的内容）
    std::string contentBuf_;
    // 代码块行缓冲（仅代码块内逐行检测关界围栏）
    std::string codeLineBuf_;

    // ---- 标题状态 ----
    int headingLevel_ = 0;

    // ---- 代码围栏状态 ----
    char codeFenceChar_ = 0;
    int  codeFenceCount_ = 0;
    std::string codeFenceLang_;

    // ---- 列表状态 ----
    bool inList_     = false;
    bool listOrdered_ = false;

    // ---- 段落跨行追踪 ----
    bool paraAfterNL_ = false;   ///< 段落中刚看到 \n，等待下一字符判定软换行/段落结束
    bool paraHadContent_ = false;///< 当前段落是否已有内容（用于空行检测）

    // ---- 文档状态 ----
    bool docStarted_ = false;
};


/// @brief  Markdown解析器（SAX事件驱动，流式输入）
/// @details 薄封装，组合 MarkdownBlockStateMachine 实现字符级流式解析。
class AST_UTIL_API MarkdownParser {
public:
    MarkdownParser(MarkdownSax& sax)
        : blockSM_(sax)
    {}
    ~MarkdownParser() = default;

    /// @brief  流式输入数据块
    void feed(StringView data) { blockSM_.feed(data); }

    /// @brief  通知输入结束，处理剩余内容
    void finish() { blockSM_.finish(); }

    /// @brief  重置解析器状态
    void reset() { blockSM_.reset(); }

private:
    MarkdownBlockStateMachine blockSM_;
};

/*! @} */

AST_NAMESPACE_END
