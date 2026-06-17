///
/// @file      UiChatEventHandler.cpp
/// @brief     流式聊天事件的 Qt 信号桥接器实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiChatEventHandler.hpp"

AST_NAMESPACE_BEGIN

UiChatEventHandler::UiChatEventHandler(QObject* parent)
    : QObject(parent)
{
}

UiChatEventHandler::~UiChatEventHandler() = default;

void UiChatEventHandler::onContentChunk(const std::string& text)
{
    // 发射原始 Markdown 文本
    emit contentChunk(QString::fromStdString(text));

    // 流式 Markdown→HTML 转换，发射增量 HTML
    std::string htmlDelta;
    mdRenderer_.feed(text, htmlDelta);
    if (!htmlDelta.empty())
        emit htmlChunk(QString::fromUtf8(htmlDelta.c_str(), static_cast<int>(htmlDelta.size())));
}

void UiChatEventHandler::onContentComplete(const std::string& text)
{
    emit contentComplete(QString::fromStdString(text));
}

void UiChatEventHandler::onReasoningChunk(const std::string& reasoning)
{
    emit reasoningChunk(QString::fromStdString(reasoning));
}

void UiChatEventHandler::onReasoningComplete(const std::string& reasoning)
{
    emit reasoningComplete(QString::fromStdString(reasoning));
}

void UiChatEventHandler::onToolCallRequest(const std::string& toolCallId,
                                           const std::string& functionName,
                                           const std::string& arguments)
{
    emit toolCallRequest(
        QString::fromStdString(toolCallId),
        QString::fromStdString(functionName),
        QString::fromStdString(arguments));
}

void UiChatEventHandler::onToolCallResult(const std::string& toolCallId,
                                          const std::string& functionName,
                                          const std::string& result)
{
    emit toolCallResult(
        QString::fromStdString(toolCallId),
        QString::fromStdString(functionName),
        QString::fromStdString(result));
}

void UiChatEventHandler::onComplete()
{
    mdRenderer_.reset();
    emit completed();
}

void UiChatEventHandler::onError(const std::string& error)
{
    emit errorOccurred(QString::fromStdString(error));
}

bool UiChatEventHandler::isCancelled() const
{
    return cancelled_.load(std::memory_order_acquire);
}

void UiChatEventHandler::cancel()
{
    cancelled_.store(true, std::memory_order_release);
    mdRenderer_.reset();
}

void UiChatEventHandler::reset()
{
    cancelled_.store(false, std::memory_order_release);
    mdRenderer_.reset();
}

AST_NAMESPACE_END
