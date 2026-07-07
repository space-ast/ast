///
/// @file      InlineTextRenderer.hpp
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
#include "BaseRenderer.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 行内文本渲染器
/// @details 将流式 Markdown 行内格式（粗体、斜体、代码、删除线、链接）
///          转为 ANSI 终端输出。不处理块级元素。
class AST_UTIL_API InlineTextRenderer final: public BaseRenderer
{
public:
    InlineTextRenderer() = default;
    ~InlineTextRenderer() override = default;
public:
    void feed(StringView chunk, std::string& accumulated) override;
    void end(std::string& remaining) override;
    void reset() override;
private:
    A_DISABLE_COPY(InlineTextRenderer);

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

    // ---- 核心方法 ----
    void feedChar(char c, std::string& buffer);

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

    // ---- 换行 / 转义 ----
    void handleNewline(std::string& buffer);
    void handleEscape(char c, std::string& buffer);

    // ============================================================
    // 持久状态（跨 feed 延续）
    // ============================================================
    std::string pending_;       ///< 前景缓冲（行内扫描内容）
    std::string linkText_;      ///< [链接文本] 暂存

    int      activeStyles_ = STYLE_NONE;
    ScanMode scanMode_     = SCAN_NONE;
    char     closeChar_    = 0;
    int      closeLen_     = 0;
    char     italicMarker_ = 0;

    bool atLineStart_ = true;
    bool escapeNext_  = false;
};



/*! @} */

AST_NAMESPACE_END
