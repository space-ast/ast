///
/// @file      testStreaming.cxx
/// @brief     SSE 解析器单元测试
/// @details   测试 SSEParser 的核心解析逻辑（纯数据测试，不依赖网络）
/// @author    axel
/// @date      2026-06-14
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "ast/SSEParser.hpp"
#include "AstUtil/JsonValue.hpp"
#include <gtest/gtest.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace ast;


// ── 测试用事件处理器：记录所有回调 ─────────────────────────────────

class MockEventHandler : public ChatEventHandler
{
public:
    void onContentChunk(const std::string& text) override
    {
        textChunks_.push_back(text);
    }

    void onReasoningChunk(const std::string& thought) override
    {
        thoughtChunks_.push_back(thought);
    }

    void onToolCallRequest(const std::string& /*toolCallId*/,
                           const std::string& /*functionName*/,
                           const std::string& /*arguments*/) override
    {
        toolCallRequestCount_++;
    }

    void onToolCallResult(const std::string& /*toolCallId*/,
                          const std::string& /*functionName*/,
                          const std::string& /*result*/) override
    {
        toolCallResultCount_++;
    }

    void onComplete() override { completeCount_++; }
    void onError(const std::string& error) override { lastError_ = error; }

    // 累计结果
    const std::vector<std::string>& textChunks() const { return textChunks_; }
    const std::vector<std::string>& thoughtChunks() const { return thoughtChunks_; }
    int toolCallRequestCount() const { return toolCallRequestCount_; }
    int toolCallResultCount() const { return toolCallResultCount_; }
    int completeCount() const { return completeCount_; }
    const std::string& lastError() const { return lastError_; }

    std::string cumulativeContent() const
    {
        std::string result;
        for (const auto& chunk : textChunks_)
            result += chunk;
        return result;
    }

private:
    std::vector<std::string> textChunks_;
    std::vector<std::string> thoughtChunks_;
    int toolCallRequestCount_ = 0;
    int toolCallResultCount_  = 0;
    int completeCount_        = 0;
    std::string lastError_;
};


// ── 测试 1: 纯文本内容解析 ─────────────────────────────────────────

TEST(SSEParserTest, TextContentDelta)
{
    MockEventHandler handler;
    SSEParser parser(handler);

    // 模拟 SSE 数据流
    const char* sse1 = "data: {\"id\":\"chatcmpl-001\",\"object\":\"chat.completion.chunk\","
                        "\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"}}]}\n\n";
    const char* sse2 = "data: {\"choices\":[{\"index\":0,\"delta\":{\"content\":\" World\"}}]}\n\n";
    const char* sse3 = "data: [DONE]\n\n";

    parser.onHeaders(200, {});
    parser.onData(sse1, std::strlen(sse1));
    parser.onData(sse2, std::strlen(sse2));
    parser.onData(sse3, std::strlen(sse3));
    parser.onComplete();

    EXPECT_EQ(handler.textChunks().size(), 2u);
    EXPECT_EQ(handler.textChunks()[0], "Hello");
    EXPECT_EQ(handler.textChunks()[1], " World");
    EXPECT_EQ(handler.cumulativeContent(), "Hello World");
    EXPECT_TRUE(handler.lastError().empty());

    // 验证 buildResult
    JsonValue result = parser.buildResult();
    EXPECT_EQ(result["id"].toString(), "chatcmpl-001");
    EXPECT_EQ(result["object"].toString(), "chat.completion");
    EXPECT_EQ(result["choices"][0]["message"]["content"].toString(), "Hello World");
}


// ── 测试 2: 推理内容解析 ────────────────────────────────────────────

TEST(SSEParserTest, ReasoningDelta)
{
    MockEventHandler handler;
    SSEParser parser(handler);

    const char* sse = "data: {\"choices\":[{\"index\":0,\"delta\":"
                       "{\"reasoning_content\":\"Let me think...\"}}]}\n\n";

    parser.onHeaders(200, {});
    parser.onData(sse, std::strlen(sse));
    parser.onComplete();

    EXPECT_EQ(handler.thoughtChunks().size(), 1u);
    EXPECT_EQ(handler.thoughtChunks()[0], "Let me think...");

    JsonValue result = parser.buildResult();
    EXPECT_EQ(result["choices"][0]["message"]["reasoning_content"].toString(),
              "Let me think...");
}


// ── 测试 3: 工具调用累积 ────────────────────────────────────────────

TEST(SSEParserTest, ToolCallAccumulation)
{
    MockEventHandler handler;
    SSEParser parser(handler);

    // 第一块：工具名称
    const char* sse1 = "data: {\"choices\":[{\"index\":0,\"delta\":"
                        "{\"tool_calls\":[{\"index\":0,\"id\":\"call_001\","
                        "\"type\":\"function\",\"function\":{\"name\":\"get_weather\"}}]}}]}\n\n";

    // 第二块：参数（完整的一整块）
    const char* sse2 = "data: {\"choices\":[{\"index\":0,\"delta\":"
                        "{\"tool_calls\":[{\"index\":0,\"function\":"
                        "{\"arguments\":\"{\\\"city\\\":\\\"Beijing\\\"}\"}}]}}]}\n\n";

    parser.onHeaders(200, {});
    parser.onData(sse1, std::strlen(sse1));
    parser.onData(sse2, std::strlen(sse2));
    parser.onComplete();

    // buildResult 应包含完整的工具调用
    JsonValue result = parser.buildResult();
    auto& toolCalls = result["choices"][0]["message"]["tool_calls"];
    EXPECT_TRUE(toolCalls.isArray());
    EXPECT_EQ(toolCalls.size(), 1u);

    auto& tc = toolCalls[0];
    EXPECT_EQ(tc["id"].toString(), "call_001");
    EXPECT_EQ(tc["type"].toString(), "function");
    EXPECT_EQ(tc["function"]["name"].toString(), "get_weather");
    EXPECT_EQ(tc["function"]["arguments"].toString(), "{\"city\":\"Beijing\"}");
}


// ── 测试 4: HTTP 错误状态 ───────────────────────────────────────────

TEST(SSEParserTest, HttpErrorStatus)
{
    MockEventHandler handler;
    SSEParser parser(handler);

    parser.onHeaders(500, {});
    parser.onComplete();

    EXPECT_FALSE(handler.lastError().empty());
    EXPECT_TRUE(handler.lastError().find("500") != std::string::npos);
}


// ── 测试 5: 跨数据块拼接 ────────────────────────────────────────────

TEST(SSEParserTest, SplitAcrossChunks)
{
    // 测试 SSE 数据被切分在多个 onData() 调用中的情况
    MockEventHandler handler;
    SSEParser parser(handler);

    // 一个完整的 SSE 事件被切成两块
    const char* part1 = "data: {\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hel";
    const char* part2 = "lo\"}}]}\n\n";

    parser.onHeaders(200, {});
    parser.onData(part1, std::strlen(part1));
    parser.onData(part2, std::strlen(part2));
    parser.onComplete();

    EXPECT_EQ(handler.cumulativeContent(), "Hello");
}


// ── 测试 6: 处理 [DONE] 标记 ─────────────────────────────────────────

TEST(SSEParserTest, HandlesDoneSentinel)
{
    MockEventHandler handler;
    SSEParser parser(handler);

    const char* sse = "data: [DONE]\n\n";

    parser.onHeaders(200, {});
    parser.onData(sse, std::strlen(sse));
    parser.onComplete();

    // [DONE] 不应产生任何文本
    EXPECT_EQ(handler.textChunks().size(), 0u);
    EXPECT_TRUE(handler.lastError().empty());
}


// ── 测试 7: 空 data 行 ──────────────────────────────────────────────

TEST(SSEParserTest, IgnoresEmptyDataLines)
{
    MockEventHandler handler;
    SSEParser parser(handler);

    const char* sse = "\n\n";  // 只有一个空行

    parser.onHeaders(200, {});
    parser.onData(sse, std::strlen(sse));
    parser.onComplete();

    EXPECT_EQ(handler.textChunks().size(), 0u);
}


int main(int argc, char **argv) {
    printf("Running main() from %s\n", __FILE__);
    testing::GTEST_FLAG(catch_exceptions) = false;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
