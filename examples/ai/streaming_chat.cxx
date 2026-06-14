///
/// @file      streaming_chat.cxx
/// @brief     AsAI 流式输出对话示例
/// @details   逐字/逐块实时输出 LLM 回复，同时展示工具调用的实时通知。
///            输入 quit / exit / q 退出。
///
///            事件对应关系：
///              onTextChunk       → ModelClientStreamingChunkEvent
///              onToolCallRequest → ToolCallRequestEvent
///              onToolCallResult  → ToolCallExecutionEvent
///              onThought         → ThoughtEvent
///              onComplete        → Response
///
///            环境变量：
///              - AST_AI_API_KEY: API Key
///              - AST_AI_BASE_URL: API Base URL
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

#include "AstAI/AssistantAgent.hpp"
#include "AstAI/ChatEventHandler.hpp"
#include "AstAI/ChatMessage.hpp"
#include "AstAI/ChatMessages.hpp"
#include "AstAI/DeepSeek.hpp"
#include "AstAI/LLMConfig.hpp"
#include <iostream>
#include <cstdlib>

using namespace ast;


// ── 事件处理器：打印到终端 ──────────────────────────────────────────

class ConsoleEventHandler : public ChatEventHandler
{
public:
    void onTextChunk(const std::string& text) override
    {
        std::cout << text << std::flush;
    }

    void onThought(const std::string& thought) override
    {
        std::cout << thought << std::flush;
    }

    void onToolCallRequest(const std::string& /*toolCallId*/,
                           const std::string& functionName,
                           const std::string& arguments) override
    {
        std::cout << "\n  📎 [调用工具: " << functionName
                  << "(" << arguments << ")]\n";
    }

    void onToolCallResult(const std::string& /*toolCallId*/,
                          const std::string& functionName,
                          const std::string& result) override
    {
        std::cout << "  ✅ [工具结果: " << functionName
                  << " → " << result << "]\n";
        std::cout << "AI: " << std::flush;  // 继续等待文本输出
    }

    void onComplete() override
    {
        std::cout << std::endl;
    }

    void onError(const std::string& error) override
    {
        std::cerr << "\n  ❌ [错误: " << error << "]" << std::endl;
    }
};


// ── 主函数 ──────────────────────────────────────────────────────────

int main()
{
    std::setlocale(LC_ALL, ".UTF-8");

    // ── 1. 检查环境变量 ─────────────────────────────────────────
    const char* apiKey  = std::getenv("AST_AI_API_KEY");
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
        "回答要简洁清晰。"
    );
    assistant.config().setModel("deepseek-reasoner");
    assistant.config().setExtraBody({});

    // ── 3. 创建事件处理器 ─────────────────────────────────────
    ConsoleEventHandler handler;

    // ── 4. 消息历史 ────────────────────────────────────────────
    ChatMessages messages;
    messages.setSystemPrompt(
        "你是一个有用且友好的 AI 助手。请用中文回答。"
    );

    // ── 5. 打印欢迎信息 ─────────────────────────────────────────
    std::cout << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << "  AsAI 流式输出示例" << std::endl;
    std::cout << "  逐字实时输出 LLM 回复" << std::endl;
    std::cout << "  输入 quit / exit / q 退出" << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << std::endl;

    // ── 6. 主循环：逐轮流式对话 ────────────────────────────────
    while (true)
    {
        // 读取用户输入
        std::cout << "你: " << std::flush;
        std::string input;
        if (!std::getline(std::cin, input))
            break;  // EOF

        if (input.empty())
            continue;

        // 检查退出关键词（大小写不敏感）
        {
            std::string lower = input;
            for (auto& c : lower)
                c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
            if (lower == "quit" || lower == "exit" || lower == "q")
                break;
        }

        // 将用户输入加入消息历史
        messages.addUserMessage(input);

        // 流式调用 LLM
        std::cout << "AI: " << std::flush;
        errc_t rc = assistant.runStream(messages, handler);

        if (rc != 0)
        {
            std::cerr << "\n对话执行失败，错误码: " << rc << std::endl;
            return rc;
        }
    }

    std::cout << std::endl;
    std::cout << "对话结束，再见！" << std::endl;

    return 0;
}
