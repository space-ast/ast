///
/// @file      RoundRobinGroupChat.cpp
/// @brief     轮询式群聊实现
/// @details   每个Agent按顺序处理消息，直到达到所设定的停止条件。
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

#include "RoundRobinGroupChat.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

// —— Agent管理 ——

void RoundRobinGroupChat::addAgent(ChatAgent* agent)
{
    if (agent)
    {
        agents_.push_back(agent);
    }
}

// —— 终止条件 ——

void RoundRobinGroupChat::setTerminationCondition(std::unique_ptr<TerminationCondition> condition)
{
    terminationCondition_ = std::move(condition);
}

// —— 执行 ——

errc_t RoundRobinGroupChat::run(const ChatMessage& message, ChatMessage& response)
{
    // 1. 将用户输入消息追加到共享消息历史
    messages_.addMessage(message);

    if (agents_.empty())
    {
        aError("RoundRobinGroupChat::run: no agents registered");
        return -1;
    }

    // 2. 轮询执行
    for (int round = 0; round < maxRounds_; ++round)
    {
        for (size_t i = 0; i < agents_.size(); ++i)
        {
            ChatAgent* agent = agents_[i];

            // 让当前Agent执行推理（内部自动处理工具调用循环）
            errc_t rc = agent->run(messages_);
            if (rc != 0)
            {
                aError("RoundRobinGroupChat::run: agent[%zu] failed", i);
                return rc;
            }

            // 检查终止条件
            if (terminationCondition_
                && terminationCondition_->shouldTerminate(messages_))
            {
                response = messages_.back();
                return 0;
            }
        }
    }

    // 3. 达到最大轮次，返回最后一条消息
    if (!messages_.empty())
    {
        response = messages_.back();
    }

    return 0;
}

AST_NAMESPACE_END
