///
/// @file      MarkdownHTMLRenderer.cpp
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

#include "MarkdownHTMLRenderer.hpp"

AST_NAMESPACE_BEGIN


MarkdownHTMLRenderer::MarkdownHTMLRenderer()
    : htmlRenderer_()
    , parser_(htmlRenderer_)
{

}

void MarkdownHTMLRenderer::feed(StringView chunk, std::string &accumulated)
{
    parser_.feed(chunk);
    accumulated = std::move(htmlRenderer_.output());
    htmlRenderer_.clearOutput();
}

void MarkdownHTMLRenderer::end(std::string &remaining)
{
    parser_.finish();
    remaining = std::move(htmlRenderer_.output());
}

void MarkdownHTMLRenderer::reset()
{
    parser_.reset();
}


AST_NAMESPACE_END
