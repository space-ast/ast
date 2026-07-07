///
/// @file      TextMentionTermination.cpp
/// @brief     文本提及终止条件实现
/// @details   检查最后一条消息内容中是否包含指定文本
/// @author    axel
/// @date      2026-06-14
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

#include "TextMentionTermination.hpp"
#include "AstAI/ChatMessages.hpp"
#include "AstAI/ChatMessage.hpp"

AST_NAMESPACE_BEGIN

TextMentionTermination::TextMentionTermination(StringView text)
    : text_(text)
{
}

bool TextMentionTermination::shouldTerminate(const ChatMessages& messages) const
{
    if (messages.empty() || text_.empty())
        return false;

    const auto& last = messages.back();
    return last.content().find(text_) != std::string::npos;
}

void TextMentionTermination::setText(StringView text)
{
    text_ = static_cast<std::string>(text);
}

const std::string& TextMentionTermination::text() const
{
    return text_;
}

AST_NAMESPACE_END
