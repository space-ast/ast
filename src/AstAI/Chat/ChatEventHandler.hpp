///
/// @file      ChatEventHandler.hpp
/// @brief     流式聊天事件处理器 — 纯虚接口
/// @details   用户通过继承此类并重写相关方法，实现对 LLM 流式输出的实时处理。
///            对应 Python AutoGen 的事件类型体系：
///              onTextChunk       → ModelClientStreamingChunkEvent
///              onToolCallRequest → ToolCallRequestEvent
///              onToolCallResult  → ToolCallExecutionEvent
///              onThought         → ThoughtEvent
///              onComplete        → Response
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
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Chat
    @{
*/

/// @brief 流式聊天事件处理器
/// @details 纯虚接口，所有方法均有空默认体。子类按需重写。
class AST_AI_API ChatEventHandler
{
public:
    ChatEventHandler() = default;
    virtual ~ChatEventHandler() = default;

    // ── 流式文本事件（在 LLM 生成过程中实时触发） ──

    /// @brief 收到一块文本内容（delta）
    /// @param text 文本增量（可能是一个或几个 token）
    virtual void onTextChunk(const std::string& text) {}

    /// @brief 收到推理/思考内容（DeepSeek R1 / Claude thinking）
    /// @param thought 推理内容增量
    virtual void onThought(const std::string& thought) {}

    // ── 工具调用事件（在流式响应完成后触发） ──

    /// @brief LLM 请求调用工具（含完整函数名和参数）
    /// @param toolCallId  工具调用 ID
    /// @param functionName 函数名称
    /// @param arguments    参数字符串（JSON 格式）
    virtual void onToolCallRequest(const std::string& toolCallId,
                                   const std::string& functionName,
                                   const std::string& arguments) {}

    /// @brief 工具执行完毕，返回结果
    /// @param toolCallId  工具调用 ID
    /// @param functionName 函数名称
    /// @param result       执行结果字符串
    virtual void onToolCallResult(const std::string& toolCallId,
                                  const std::string& functionName,
                                  const std::string& result) {}

    // ── 生命周期事件 ──

    /// @brief 本轮对话完成
    virtual void onComplete() {}

    /// @brief 发生错误
    /// @param error 错误描述
    virtual void onError(const std::string& error) {}

    /// @brief 检查是否被取消
    /// @return true 表示取消当前操作
    virtual bool isCancelled() const { return false; }
};

/*! @} */

AST_NAMESPACE_END
