///
/// @file      TextMentionTermination.hpp
/// @brief     文本提及终止条件
/// @details   当最后一条消息内容中包含指定文本时终止
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

#pragma once

#include "AstGlobal.h"
#include "TerminationCondition.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Chat
    @{
*/

/// @brief 文本提及终止条件
/// @details 当最后一条消息内容中包含指定文本时，判定应终止。
///          常用于让Agent在达成一致时输出特定关键词（如 "APPROVED"）来结束对话。
class AST_AI_API TextMentionTermination : public TerminationCondition
{
public:
    /// @brief 构造函数
    /// @param text 触发终止的文本
    explicit TextMentionTermination(StringView text);

    /// @brief 检查是否应终止
    /// @param messages 消息历史
    /// @return 若最后一条消息内容包含指定文本则返回true
    bool shouldTerminate(const ChatMessages& messages) const override;

    /// @brief 设置触发终止的文本
    void setText(StringView text);

    /// @brief 获取触发终止的文本
    const std::string& text() const;

private:
    std::string text_;
};

/*! @} */

AST_NAMESPACE_END
