///
/// @file      UiChatWorker.cpp
/// @brief     后台 LLM 对话工作线程实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiChatWorker.hpp"
#include "UiChatEventHandler.hpp"
#include "AstAI/ChatSession.hpp"

AST_NAMESPACE_BEGIN

UiChatWorker::UiChatWorker(ChatSession* session,
                           const QString& userMessage,
                           UiChatEventHandler* handler,
                           int maxToolIterations,
                           QObject* parent)
    : QThread(parent)
    , session_(session)
    , userMessage_(userMessage)
    , handler_(handler)
    , maxToolIterations_(maxToolIterations)
{
}

UiChatWorker::~UiChatWorker()
{
    // 如果线程还在运行，请求取消并等待
    if (isRunning())
    {
        if (handler_)
            handler_->cancel();
        wait(3000); // 最多等待 3 秒
    }
}

void UiChatWorker::run()
{
    if (!session_ || !handler_)
    {
        errorCode_ = 1;
        if (handler_)
            handler_->onError("Session or handler is null");
        emit workFinished(errorCode_);
        return;
    }

    // 重置取消状态
    handler_->reset();

    // 在工作线程中执行流式对话
    std::string utf8Msg = userMessage_.toUtf8().toStdString();
    response_ = session_->chatStream(utf8Msg, *handler_, maxToolIterations_);

    emit workFinished(errorCode_);
}

AST_NAMESPACE_END
