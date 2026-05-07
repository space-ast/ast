///
/// @file      ChatMessages.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-25
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

#include "ChatMessages.hpp"
#include "AstUtil/JsonValue.hpp"

AST_NAMESPACE_BEGIN

JsonValue ChatMessages::toJson() const
{
    JsonValue json;
    for (const auto& message : *this) {
        json.append(message.toJson());
    }
    return json;
}

void ChatMessages::setSystemPrompt(StringView systemPrompt)
{
    // 如果第一个消息是系统提示，直接更新内容
    if(!this->empty() && this->front().role() == EChatRole::eSystem)
    {
        this->front().setContent(systemPrompt);
    }
    // 否则，在消息开头添加系统提示
    else
    {
        this->insert(this->begin(), ChatMessage::System(systemPrompt));
    }
}


AST_NAMESPACE_END
