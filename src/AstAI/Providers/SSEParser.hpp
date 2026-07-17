///
/// @file      SSEParser.hpp
/// @brief     SSE (Server-Sent Events) 解析器 — 内部实现，供测试使用
/// @details   实现 NetworkStreamReceiver，将原始字节流解析为 SSE 事件。
///           非公开 API，仅 OpenAI 提供者和测试代码应包含此头。
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
#include "AstUtil/NetworkStreamReceiver.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstAI/ChatEventHandler.hpp"
#include <map>
#include <string>

AST_NAMESPACE_BEGIN


/// @brief SSE 增量解析器（内部组件）
/// @details 累积原始字节 → 按 \n\n 分割 SSE 事件 → 解析 data: 行 → 分派到 ChatEventHandler
///          流式完成后调用 buildResult() 重建非流式格式的完整 JSON 响应
class AST_AI_API SSEParser : public NetworkStreamReceiver
{
public:
    explicit SSEParser(ChatEventHandler& handler);

    // ── NetworkStreamReceiver 接口 ──

    void onHeaders(int statusCode, const std::map<std::string, std::string>& headers) override;
    errc_t onData(const char* data, size_t size) override;
    void onComplete() override;
    void onError(errc_t error) override;

    // ── 状态 ──

    /// @brief 是否发生了错误（HTTP 非 200 或网络错误）
    bool hasError() const { return !error_.empty(); }

    /// @brief 获取错误描述
    const std::string& errorMessage() const { return error_; }

    // ── 累积结果 ──

    /// @brief 将累积的 SSE deltas 重建为非流式 /chat/completions 格式的 JSON 响应
    JsonValue buildResult() const;

private:
    struct AccumulatedToolCall
    {
        std::string id{};
        std::string type{};
        std::string functionName{};
        std::string functionArguments{};
    };

    void processBuffer();
    void processEvent(const std::string& event);
    void processToolCallDeltas(const JsonValue& toolCalls);

    ChatEventHandler& handler_;
    bool thoughtCompleted_{false};   // 是否已完成推理/思考内容
    std::string       buffer_{};
    std::string       error_{};

    std::string id_{};
    std::string model_{};
    std::string accumulatedContent_{};
    std::string accumulatedReasoning_{};
    std::string finishReason_{};
    std::map<int, AccumulatedToolCall> toolCallsByIndex_{};
};


AST_NAMESPACE_END
