///
/// @file      UiChatWorker.hpp
/// @brief     后台 LLM 对话工作线程
/// @details   将 ChatSession::chatStream() 封装在 QThread 中运行，
///            通过 UiChatEventHandler 将流式事件投递到 UI 线程。
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include <QThread>
#include <QString>
#include <QPointer>
#include <memory>

AST_NAMESPACE_BEGIN

class ChatSession;
class UiChatEventHandler;

/*!
    @addtogroup UiAI
    @{
*/

/// @brief 后台 LLM 对话工作线程
/// @details 使用方式：
///   ```cpp
///   auto* worker = new UiChatWorker(&session, "用户消息", handler);
///   connect(worker, &UiChatWorker::finished, worker, &QObject::deleteLater);
///   worker->start();
///   ```
///   handler 在工作线程中通过 emit 投递事件到 UI 线程。
class AST_UIAI_API UiChatWorker : public QThread
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param session 聊天会话（非拥有，生命周期由调用者保证）
    /// @param userMessage 用户消息
    /// @param handler 事件处理器（非拥有，一般与 UiChatPanel 共享同一 handler）
    /// @param maxToolIterations 最大工具调用轮数
    /// @param parent 父对象
    explicit UiChatWorker(ChatSession* session,
                          const QString& userMessage,
                          UiChatEventHandler* handler,
                          int maxToolIterations = 100,
                          QObject* parent = nullptr);

    ~UiChatWorker() override;

    UiChatWorker(const UiChatWorker&) = delete;
    UiChatWorker& operator=(const UiChatWorker&) = delete;

    /// @brief 获取本轮对话的最终响应
    const std::string& response() const { return response_; }

    /// @brief 获取错误码
    int errorCode() const { return errorCode_; }

Q_SIGNALS:
    /// @brief 工作完成
    /// @param errorCode 错误码，0 表示成功
    void workFinished(int errorCode);

protected:
    void run() override;

private:
    ChatSession*        session_;
    QString             userMessage_;
    QPointer<UiChatEventHandler> handler_;
    int                 maxToolIterations_;
    std::string         response_;
    int                 errorCode_{0};
};

/*! @} */

AST_NAMESPACE_END
