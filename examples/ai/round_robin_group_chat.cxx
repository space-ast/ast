///
/// @file      round_robin_group_chat.cpp
/// @brief     AutoGen 风格多轮对话示例
/// @details   模拟一个AI团队讨论"如何设计高并发短链接服务"
///            参与者：Architect（架构师）、Reviewer（评审者）、PM（产品经理）
///            使用 RoundRobinGroupChat 实现 Agent 轮流发言
///
///            对应Python示例：
///              - AssistantAgent        → ChatAgent
///              - RoundRobinGroupChat   → RoundRobinGroupChat
///              - TextMentionTermination → TextMentionTermination
///
///            环境变量：
///              - AST_AI_API_KEY: API Key
///              - AST_AI_BASE_URL: API Base URL (DeepSeek)
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

#include "AstAI/RoundRobinGroupChat.hpp"
#include "AstAI/ChatAgent.hpp"
#include "AstAI/TextMentionTermination.hpp"
#include "AstAI/DeepSeek.hpp"
#include "AstAI/LLMConfig.hpp"
#include "AstAI/ChatMessage.hpp"
#include "AstAI/ChatMessages.hpp"
#include <iostream>
#include <cstdlib>

using namespace ast;

// ── 辅助函数：打印分隔线 ──────────────────────────────────────────
static void printSeparator(const char* title = nullptr)
{
    if (title)
    {
        std::cout << "\n━━━ " << title << " ━━━\n";
    }
    else
    {
        std::cout << "────────────────────────────────────────────────────────────\n";
    }
}

// ── 辅助函数：打印单条消息 ────────────────────────────────────────
static void printMessage(const ChatMessage& msg)
{
    // 获取显示名称：优先使用Agent名称，其次使用角色名
    std::string displayName;
    if (!msg.name().empty())
    {
        displayName = msg.name();
    }
    else
    {
        displayName = toString(msg.role());
    }

    std::cout << "\n[" << displayName << "]:\n";
    std::cout << msg.content() << std::endl;
}

int main()
{
    std::setlocale(LC_ALL, ".UTF-8");
    // ── 1. 检查环境变量 ─────────────────────────────────────────
    printSeparator("环境检查");

    const char* apiKey = std::getenv("AST_AI_API_KEY");
    const char* baseUrl = std::getenv("AST_AI_BASE_URL");

    if (!apiKey || !*apiKey)
    {
        std::cerr << "错误: 请设置环境变量 AST_AI_API_KEY\n";
        return 1;
    }
    std::cout << "AST_AI_API_KEY: " << std::string(apiKey).substr(0, 8) << "...\n";

    if (baseUrl && *baseUrl)
    {
        std::cout << "AST_AI_BASE_URL: " << baseUrl << "\n";
    }
    else
    {
        std::cout << "AST_AI_BASE_URL: (未设置，将使用DeepSeek默认端点)\n";
    }

    // ── 2. 创建三个 Agent ───────────────────────────────────────
    printSeparator("创建 Agent");

    // Architect - 架构师
    ChatAgent architect;
    architect.setName("Architect");
    architect.setSystemPrompt(
        "你是一位资深系统架构师，擅长高并发分布式系统设计。\n"
        "你的职责是：\n"
        "1. 提出清晰、可落地的系统架构方案\n"
        "2. 说明技术选型理由\n"
        "3. 对评审意见做出回应和改进\n\n"
        "每次发言请聚焦一个主题，不要太长。\n"
        "当你认为方案已经完善且无需修改时，请在末尾说: APPROVED"
    );
    architect.config().setModel("deepseek-chat");
    std::cout << "✓ Architect (架构师) 已创建\n";

    // Reviewer - 评审者
    ChatAgent reviewer;
    reviewer.setName("Reviewer");
    reviewer.setSystemPrompt(
        "你是一位严格的技术评审，关注以下维度：\n"
        "- 高可用: 单点故障？容灾方案？\n"
        "- 高性能: 瓶颈在哪？缓存策略？\n"
        "- 安全性: 恶意短链、DDoS 防护？\n"
        "- 可运维: 监控、告警、灰度发布？\n\n"
        "每次发言只点评1-2个关键问题，不要罗列清单。\n"
        "当你认为方案已经完善时，请在末尾说: APPROVED"
    );
    reviewer.config().setModel("deepseek-chat");
    std::cout << "✓ Reviewer (评审者) 已创建\n";

    // PM - 产品经理
    ChatAgent pm;
    pm.setName("PM");
    pm.setSystemPrompt(
        "你是产品经理，关注用户体验和业务需求：\n"
        "- 短链长度 & 有效期\n"
        "- 用户自定义短链 & 统计分析\n"
        "- API 限流 & 商业化定价\n\n"
        "你的职责是在架构师和评审达成一致后做最终确认。\n"
        "当你确认需求全部满足时，请在末尾说: APPROVED"
    );
    pm.config().setModel("deepseek-chat");
    std::cout << "✓ PM (产品经理) 已创建\n";

    // ── 3. 创建终止条件 ─────────────────────────────────────────
    // 当任意Agent说出 "APPROVED" 时自动终止
    std::unique_ptr<TerminationCondition> termination(new TextMentionTermination("APPROVED"));
    std::cout << "\n✓ 终止条件: 检测到 \"APPROVED\" 时自动结束\n";

    // ── 4. 创建 Round-Robin 团队 ─────────────────────────────────
    printSeparator("开始讨论");

    RoundRobinGroupChat team;
    team.addAgent(&architect);
    team.addAgent(&reviewer);
    team.addAgent(&pm);
    team.setTerminationCondition(std::move(termination));
    team.setMaxRounds(10);   // 安全上限，防止无限循环
    team.setMaxStepsPerAgent(15);

    std::cout << "团队配置: Architect → Reviewer → PM (轮询)\n";
    std::cout << "最大轮次: 10 | 每Agent最大步数: 15\n\n";

    // ── 5. 启动多轮对话 ─────────────────────────────────────────
    std::cout << "============================================================\n";
    std::cout << "  C++ RoundRobinGroupChat 多轮对话演示\n";
    std::cout << "  短链接服务设计讨论 (类 AutoGen)\n";
    std::cout << "============================================================\n";

    ChatMessage task = ChatMessage::User(
        "请Architect先提出一个高并发短链接服务（类似 t.cn / bit.ly）的系统设计方案。"
        "要求：日活用户100万，每天生成1000万条短链，QPS峰值5000。"
        "然后Reviewer评审，PM确认需求。大家讨论直到达成一致。"
    );

    ChatMessage response;
    errc_t rc = team.run(task, response);

    // ── 6. 输出结果 ─────────────────────────────────────────────
    printSeparator("讨论结果");

    if (rc != 0)
    {
        std::cerr << "\n❌ 对话执行失败，错误码: " << rc << std::endl;
        return rc;
    }

    std::cout << "\n✓ 对话已结束 (检测到 APPROVED 或达到最大轮次)\n";
    std::cout << "共产生 " << team.messages().size() << " 条消息\n";

    // 逐条打印完整对话历史
    printSeparator("完整对话记录");
    for (const auto& msg : team.messages())
    {
        printMessage(msg);
    }

    printSeparator();
    std::cout << "\n对话结束。\n" << std::endl;

    return 0;
}
