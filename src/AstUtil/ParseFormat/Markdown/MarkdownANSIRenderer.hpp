///
/// @file      MarkdownRenderer.hpp
/// @brief     
/// @details   
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

#pragma once

#include "AstGlobal.h"
#include "BaseRenderer.hpp"
#include "MarkdownANSI.hpp"
#include "MarkdownParser.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief     ANSI 流式渲染器
/// @details   用于将 Markdown 文本渲染为 ANSI 格式
class AST_UTIL_API MarkdownANSIRenderer : public BaseRenderer
{
public:
    MarkdownANSIRenderer();
    ~MarkdownANSIRenderer() = default;

    /// @brief     渲染 Markdown 文本的下一个块
    void feed(StringView chunk, std::string& accumulated) override;
    /// @brief     渲染 Markdown 文本的最后一个块
    void end(std::string& remaining) override;
    void reset() override;
private:
    MarkdownANSI ansiRenderer_;   // 必须在 parser_ 之前声明（parser_ 引用 ansiRenderer_）
    MarkdownParser parser_;
};



/*! @} */

AST_NAMESPACE_END
