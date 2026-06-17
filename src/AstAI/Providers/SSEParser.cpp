///
/// @file      SSEParser.cpp
/// @brief     SSE (Server-Sent Events) 解析器实现
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

#include "SSEParser.hpp"

AST_NAMESPACE_BEGIN

SSEParser::SSEParser(ChatEventHandler& handler)
    : handler_(handler)
{
}

// ── NetworkStreamReceiver 接口 ──

void SSEParser::onHeaders(int statusCode,
                           const std::map<std::string, std::string>& /*headers*/)
{
    if (statusCode != 200)
    {
        error_ = "HTTP " + std::to_string(statusCode);
        handler_.onError(error_);
    }
}

errc_t SSEParser::onData(const char* data, size_t size)
{
    if (handler_.isCancelled())
        return -1;
    buffer_.append(data, size);
    processBuffer();
    return error_.empty() ? 0 : -1;
}

void SSEParser::onComplete()
{
    processBuffer();  // 刷新缓冲区中残余的数据
    handler_.onContentComplete(accumulatedContent_);
}

void SSEParser::onError(errc_t /*error*/)
{
    error_ = "network error";
    handler_.onError(error_);
}

// ── 累积结果 ──

JsonValue SSEParser::buildResult() const
{
    JsonValue result;
    result["id"]     = id_;
    result["object"] = "chat.completion";
    if (!model_.empty())
        result["model"] = model_;

    JsonValue choice;
    choice["index"]         = 0;
    choice["finish_reason"] = finishReason_;

    JsonValue message;
    message["role"]    = "assistant";
    message["content"] = accumulatedContent_;

    if (!accumulatedReasoning_.empty())
        message["reasoning_content"] = accumulatedReasoning_;

    if (!toolCallsByIndex_.empty())
    {
        JsonValue toolCallsJson;
        for (const auto& kv : toolCallsByIndex_)
        {
            const auto& tc = kv.second;

            JsonValue tcJson;
            tcJson["id"]   = tc.id;
            tcJson["type"] = tc.type.empty() ? "function" : tc.type;

            JsonValue func;
            func["name"]      = tc.functionName;
            func["arguments"] = tc.functionArguments;
            tcJson["function"] = func;

            toolCallsJson.append(tcJson);
        }
        message["tool_calls"] = toolCallsJson;
    }

    choice["message"] = message;

    JsonValue choices;
    choices.append(choice);
    result["choices"] = choices;

    return result;
}

// ── SSE 缓冲与解析 ──

void SSEParser::processBuffer()
{
    for (;;)
    {
        // SSE 事件由 \n\n 或 \r\n\r\n 分隔
        size_t pos   = buffer_.find("\n\n");
        size_t sepLen = 2;
        if (pos == std::string::npos)
        {
            pos    = buffer_.find("\r\n\r\n");
            sepLen = 4;
        }
        if (pos == std::string::npos)
            break;

        if (handler_.isCancelled())
            break;

        std::string event = buffer_.substr(0, pos);
        buffer_.erase(0, pos + sepLen);

        processEvent(event);
    }
}

void SSEParser::processEvent(const std::string& event)
{
    // 必须从 "data: " 开始
    const char* prefix = "data: ";
    if (event.compare(0, 6, prefix) != 0)
        return;

    std::string data = event.substr(6);

    // 去除尾部空白（\r, \n）
    while (!data.empty()
           && (data.back() == '\r' || data.back() == '\n'))
    {
        data.pop_back();
    }

    if (data.empty())
        return;

    // [DONE] 标记
    if (data == "[DONE]")
        return;

    JsonValue json = JsonValue::FromString(data);
    if (json.isNull())
        return;

    // 提取顶层 id（通常只出现在第一个 chunk 中）
    auto& idVal = json["id"];
    if (!idVal.isNull())
        id_ = idVal.toString();

    // 提取 model（通常只出现在第一个 chunk 中）
    auto& modelVal = json["model"];
    if (!modelVal.isNull())
        model_ = modelVal.toString();

    // 提取 choices
    auto& choices = json["choices"];
    if (!choices.isArray() || choices.size() == 0)
        return;

    auto& firstChoice = choices[0];
    auto& delta       = firstChoice["delta"];
    if (delta.isNull())
        return;

    // ── 文本内容 ──
    auto& contentVal = delta["content"];
    if (!contentVal.isNull())
    {
        if(!thoughtCompleted_)
        {
            thoughtCompleted_ = true;
            if(!accumulatedReasoning_.empty())
            {
                handler_.onReasoningComplete(accumulatedReasoning_);
            }
        }
        std::string content = contentVal.toString();
        if (!content.empty())
        {
            accumulatedContent_ += content;
            handler_.onContentChunk(content);
        }
    }

    // ── 推理/思考内容 ──
    auto& reasoningVal = delta["reasoning_content"];
    if (!reasoningVal.isNull())
    {
        std::string reasoning = reasoningVal.toString();
        if (!reasoning.empty())
        {
            accumulatedReasoning_ += reasoning;
            handler_.onReasoningChunk(reasoning);
        }
    }

    // ── 工具调用 ──
    auto& toolCalls = delta["tool_calls"];
    if (toolCalls.isArray())
    {
        processToolCallDeltas(toolCalls);
    }

    // ── 完成原因 ──
    auto& finishVal = firstChoice["finish_reason"];
    if (!finishVal.isNull())
        finishReason_ = finishVal.toString();
}

void SSEParser::processToolCallDeltas(const JsonValue& toolCalls)
{
    for (const auto& tc : toolCalls.getArray())
    {
        // 跳过缺少 index 字段的 tool call delta（格式错误）
        auto& indexVal = tc["index"];
        if (indexVal.isNull())
            continue;
        int index = indexVal.toInt();
        auto& accum = toolCallsByIndex_[index];

        auto& idVal = tc["id"];
        if (!idVal.isNull())
            accum.id = idVal.toString();

        auto& typeVal = tc["type"];
        if (!typeVal.isNull())
            accum.type = typeVal.toString();

        auto& func = tc["function"];
        if (!func.isNull())
        {
            auto& nameVal = func["name"];
            if (!nameVal.isNull())
                accum.functionName = nameVal.toString();

            auto& argsVal = func["arguments"];
            if (!argsVal.isNull())
                accum.functionArguments += argsVal.toString();
        }
    }
}

AST_NAMESPACE_END
