///
/// @file      RoundRobinGroupChat.hpp
/// @brief     
/// @details   
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
#include "BaseGroupChat.hpp"
#include "AstAI/ChatAgent.hpp"
#include "AstAI/ChatMessages.hpp"
#include "AstAI/TerminationCondition.hpp"
#include <memory>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class TerminationCondition;

/// @brief 轮询式群聊
/// @details 该类实现了轮询式的群聊逻辑，每个Agent按顺序处理消息，直到达到所设定的停止条件。
class AST_AI_API RoundRobinGroupChat : public BaseGroupChat
{
public:
    RoundRobinGroupChat() = default;
    ~RoundRobinGroupChat() = default;

    // —— Agent管理 ——

    /// @brief 添加Agent到群聊（非拥有指针，调用者负责生命周期）
    void addAgent(ChatAgent* agent);

    /// @brief 获取所有Agent
    const std::vector<ChatAgent*>& agents() const { return agents_; }

    // —— 终止条件 ——

    /// @brief 设置终止条件
    void setTerminationCondition(std::unique_ptr<TerminationCondition> condition);

    /// @brief 获取终止条件
    TerminationCondition* terminationCondition() const { return terminationCondition_.get(); }

    // —— 消息历史 ——

    /// @brief 获取消息历史（可修改）
    ChatMessages& messages() { return messages_; }

    /// @brief 获取消息历史（只读）
    const ChatMessages& messages() const { return messages_; }

    // —— 配置 ——

    /// @brief 设置最大轮次数（默认100，防止无限循环）
    void setMaxRounds(int maxRounds) { maxRounds_ = maxRounds; }

    /// @brief 获取最大轮次数
    int maxRounds() const { return maxRounds_; }

    // —— 执行 ——

    errc_t run(const ChatMessage& message, ChatMessage& response) override;

private:
    std::vector<ChatAgent*> agents_;
    ChatMessages messages_;
    std::unique_ptr<TerminationCondition> terminationCondition_;
    int maxRounds_ = 100;
};

/*! @} */

AST_NAMESPACE_END
