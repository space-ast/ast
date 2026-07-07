///
/// @file      interactive_chat.cxx
/// @brief     AsAI 交互式多轮对话
/// @details   你手动输入问题，AI 回答，支持连续多轮对话。
///            输入 quit / exit / q 退出。
///
///            对应Python示例：
///              - AssistantAgent        → AssistantAgent
///              - UserProxyAgent        → UserProxyAgent
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
#include "AstAI/AssistantAgent.hpp"
#include "AstAI/UserProxyAgent.hpp"
#include <iostream>
#include <cstdlib>

using namespace ast;

int main()
{
    std::setlocale(LC_ALL, ".UTF-8");

    // ── 1. 检查环境变量 ─────────────────────────────────────────
    const char* apiKey = std::getenv("AST_AI_API_KEY");
    const char* baseUrl = std::getenv("AST_AI_BASE_URL");

    if (!apiKey || !*apiKey)
    {
        std::cerr << "错误: 请设置环境变量 AST_AI_API_KEY" << std::endl;
        return 1;
    }

    // ── 2. 创建 AI 助手 ─────────────────────────────────────────
    AssistantAgent assistant;
    assistant.setName("Assistant");
    assistant.setSystemPrompt(
        "你是一个有用且友好的 AI 助手。请用中文回答问题。"
        "回答要简洁清晰，控制在 5-10 句话内。"
    );
    assistant.config().setModel("deepseek-chat");

    // ── 3. 创建用户代理（读取终端输入） ───────────────────────────
    UserProxyAgent user;
    user.setName("User");
    user.setPrompt("你: ");
    user.setExitKeywords({"quit", "exit", "q"});

    // ── 4. 创建终止条件 ─────────────────────────────────────────
    // 当用户输入 quit/exit/q 时，UserProxyAgent 会向消息中添加 "EXIT"
    // TextMentionTermination 检测到 "EXIT" 后自动结束对话
    std::unique_ptr<TerminationCondition> termination(new TextMentionTermination("EXIT"));

    // ── 5. 创建 Round-Robin 团队 ─────────────────────────────────
    RoundRobinGroupChat team;
    team.addAgent(&user);
    team.addAgent(&assistant);
    team.setTerminationCondition(std::move(termination));
    team.setMaxRounds(200);  // 支持最多200轮对话

    // ── 6. 打印欢迎信息 ─────────────────────────────────────────
    std::cout << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << "  AsAI 交互式多轮对话" << std::endl;
    std::cout << "  输入问题 → AI 回答 → 继续问 → ..." << std::endl;
    std::cout << "  输入 quit / exit / q 退出" << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << std::endl;

    // ── 7. 启动多轮对话 ─────────────────────────────────────────
    ChatMessage task = ChatMessage::User(
        "用户会逐个输入问题，请认真回答每一个问题。"
    );

    ChatMessage response;
    errc_t rc = team.run(task, response);

    // ── 8. 输出结果 ─────────────────────────────────────────────
    if (rc != 0)
    {
        std::cerr << "\n❌ 对话执行失败，错误码: " << rc << std::endl;
        return rc;
    }

    std::cout << std::endl;
    std::cout << "对话结束，再见！" << std::endl;

    return 0;
}
