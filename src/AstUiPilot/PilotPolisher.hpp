///
/// @file      PilotPolisher.hpp
/// @brief     独立的 LLM 润色器 — 不混用 PilotSession 的对话上下文
/// @author    axel
/// @date      2026-06-13
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#pragma once

#include "AstGlobal.h"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

struct RecordStep;
class ChatSession;

/// @brief 录制脚本润色器
/// @details 持有独立的 ChatSession，不注册任何工具，使用纯文本系统提示词。
///          与 PilotSession 完全隔离，避免润色对话污染 UI 操控上下文。
class AST_UIPILOT_API PilotPolisher
{
public:
    PilotPolisher();
    ~PilotPolisher();

    /// @brief 对步骤列表进行 LLM 润色
    /// @param steps 录制步骤列表（in/out：naturalLanguage 字段被回填）
    /// @return 润色后的 JSON 字符串
    std::string polish(std::vector<RecordStep>& steps);

    /// @brief 获取系统提示词
    static const char* systemPrompt();

private:
    ChatSession* chatSession_;
};

AST_NAMESPACE_END
