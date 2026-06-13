///
/// @file      PilotPolisher.cpp
/// @brief     独立 LLM 润色器实现
/// @author    axel
/// @date      2026-06-13

#include "PilotPolisher.hpp"
#include "Recorder/RecordStep.hpp"
#include "AstAI/ChatSession.hpp"
#include <sstream>

AST_NAMESPACE_BEGIN

// ============================================================
//  系统提示词 — 纯文本转换，不涉及 UI 操控
// ============================================================

const char* PilotPolisher::systemPrompt()
{
    return u8R"(
你是一个文本润色工具。你的唯一任务是将录制的 UI 操作步骤改写为自然流畅的中文。

规则：
1. 每行以 "→ " 开头
2. 删除控件类名（如 QPushButton、QLineEdit），只保留人类可读的描述
3. 保持原意不变，不要添加、删除或合并步骤
4. 语言简洁流畅，适合作为操作文档阅读

示例输入：
1. 点击 QPushButton "新建"
2. 在输入框填入 "600"
3. 在下拉框中选择 "MEO"

示例输出：
→ 点击"新建"按钮
→ 在输入框中填入"600"
→ 在下拉框中选择"MEO"
)";
}

// ============================================================
//  构造 / 析构
// ============================================================

PilotPolisher::PilotPolisher()
{
    chatSession_ = new ChatSession();
    chatSession_->setSystemPrompt(systemPrompt());
    // 注意：不注册任何工具 — 润色是纯文本任务
}

PilotPolisher::~PilotPolisher()
{
    delete chatSession_;
}

// ============================================================
//  polish
// ============================================================

std::string PilotPolisher::polish(std::vector<RecordStep>& steps)
{
    if (steps.empty()) return "";

    // 构建 prompt
    std::ostringstream prompt;
    prompt << u8"请润色以下操作步骤：\n\n";
    for (size_t i = 0; i < steps.size(); i++)
    {
        prompt << (i + 1) << ". " << steps[i].toTemplate() << "\n";
    }
    prompt << "\n";

    // 调用 LLM（同步）
    std::string result = chatSession_->chat(prompt.str());

    // 解析结果，回填 naturalLanguage
    std::istringstream iss(result);
    std::string line;
    int idx = 0;
    while (std::getline(iss, line))
    {
        // 跳过非步骤行
        if (line.find("→") == std::string::npos) continue;

        size_t pos = line.find("→");
        std::string nl = line.substr(pos + 1);

        // trim
        while (!nl.empty() && (nl.front() == ' ' || nl.front() == '\t'))
            nl.erase(0, 1);
        while (!nl.empty() && (nl.back() == '\r' || nl.back() == '\n'))
            nl.pop_back();

        if (idx < static_cast<int>(steps.size()))
        {
            steps[idx].naturalLanguage = nl;
            idx++;
        }
    }

    return ""; // caller is responsible for serializing to JSON
}

AST_NAMESPACE_END
