///
/// @file      UiChatPanel.cpp
/// @brief     聊天面板实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiChatPanel.hpp"
#include "UiChatMessageList.hpp"
#include "UiChatMessageItem.hpp"
#include "UiChatInput.hpp"
#include "UiChatEventHandler.hpp"
#include "UiChatWorker.hpp"
#include "UiToolCallTimeline.hpp"
#include "AstAI/ChatSession.hpp"
#include "AstAI/ChatMessage.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>

AST_NAMESPACE_BEGIN

UiChatPanel::UiChatPanel(ChatSession* session, QWidget* parent)
    : QWidget(parent)
    , session_(session)
{
    setupUi();
}

UiChatPanel::~UiChatPanel()
{
    // 取消正在进行的操作
    if (handler_)
        handler_->cancel();
}

void UiChatPanel::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 状态栏
    statusLabel_ = new QLabel(QString::fromUtf8("就绪"), this);
    statusLabel_->setStyleSheet(
        "QLabel {"
        "  color: #9E9E9E;"
        "  font-size: 11px;"
        "  padding: 4px 12px;"
        "  background-color: #F5F5F5;"
        "  border-bottom: 1px solid #E0E0E0;"
        "}"
    );
    layout->addWidget(statusLabel_);

    // 消息列表
    messageList_ = new UiChatMessageList(this);
    layout->addWidget(messageList_, 1); // stretch=1: 占据剩余空间

    // 输入区域
    chatInput_ = new UiChatInput(this);
    chatInput_->setStyleSheet(
        "UiChatInput {"
        "  border-top: 1px solid #E0E0E0;"
        "  padding: 8px;"
        "  background-color: #FAFAFA;"
        "}"
    );
    layout->addWidget(chatInput_);

    // 创建事件处理器
    handler_ = new UiChatEventHandler(this);

    // 40ms 节流定时器：高频 htmlChunk 累积后一次性渲染，避免 UI 积压卡死
    throttleTimer_ = new QTimer(this);
    throttleTimer_->setSingleShot(true);
    throttleTimer_->setInterval(40);
    connect(throttleTimer_, &QTimer::timeout, this, &UiChatPanel::flushHtml);

    // 连接事件器信号到面板槽
    connect(handler_, &UiChatEventHandler::htmlChunk,
            this, &UiChatPanel::onHtmlChunk);
    connect(handler_, &UiChatEventHandler::contentComplete,
            this, &UiChatPanel::onContentComplete);
    connect(handler_, &UiChatEventHandler::reasoningChunk,
            this, &UiChatPanel::onReasoningChunk);
    connect(handler_, &UiChatEventHandler::reasoningComplete,
            this, &UiChatPanel::onReasoningComplete);
    connect(handler_, &UiChatEventHandler::toolCallRequest,
            this, &UiChatPanel::onToolCallRequest);
    connect(handler_, &UiChatEventHandler::toolCallResult,
            this, &UiChatPanel::onToolCallResult);
    connect(handler_, &UiChatEventHandler::completed,
            this, &UiChatPanel::onCompleted);
    connect(handler_, &UiChatEventHandler::errorOccurred,
            this, &UiChatPanel::onError);

    // 由于 handler 和 panel 在主线程，信号连接默认是 AutoConnection = DirectConnection
    // 但 handler 的回调在工作线程被调用，所以需要显式使用 QueuedConnection
    // 实际上 handler 的 emit 发生在工作线程（由 UiChatWorker::run 调用），
    // 由于 handler 的 thread affinity 是主线程，Qt 会自动使用 QueuedConnection。
    // 这里显式声明以确保安全：
    // （实际项目中需要确认 handler 所属线程；如果 handler moveToThread 到工作线程，
    //  则 emit 在工作线程，连接方式是 Auto 但接收者在主线程 → QueuedConnection）

    // 连接输入信号
    connect(chatInput_, &UiChatInput::sendMessage,
            this, &UiChatPanel::onSendMessage);
    connect(chatInput_, &UiChatInput::stopRequested,
            this, &UiChatPanel::onStopRequested);
}

void UiChatPanel::setSession(ChatSession* session)
{
    session_ = session;
}

void UiChatPanel::sendMessage(const QString& message)
{
    onSendMessage(message);
}

void UiChatPanel::clearMessages()
{
    messageList_->clear();
    streamingItem_ = nullptr;
    toolTimeline_ = nullptr;
    throttleTimer_->stop();
    pendingHtml_.clear();
    accumulatedText_.clear();
}

// ============================================================
//  发送消息 / 启动后台工作
// ============================================================

void UiChatPanel::onSendMessage(const QString& message)
{
    if (message.isEmpty() || busy_) return;
    if (!session_) return;

    // 添加用户消息气泡
    messageList_->appendMessage(EChatRole::eUser, message.toHtmlEscaped());

    // 创建助手消息占位
    streamingItem_ = messageList_->appendMessage(EChatRole::eAssistant);
    accumulatedText_.clear();

    setBusy(true);

    // 启动后台工作线程
    auto* worker = new UiChatWorker(session_, message, handler_, 100, this);
    connect(worker, &UiChatWorker::workFinished,
            this, &UiChatPanel::onWorkerFinished);
    connect(worker, &QThread::finished,
            worker, &QObject::deleteLater);
    worker->start();
}

void UiChatPanel::onStopRequested()
{
    if (handler_)
        handler_->cancel();
}

// ============================================================
//  事件处理器槽
// ============================================================

void UiChatPanel::onHtmlChunk(const QString& html)
{
    // 节流：累积 HTML，仅首次启动定时器（避免不断重置导致永远不渲染）
    pendingHtml_ += html;
    if (!throttleTimer_->isActive())
        throttleTimer_->start();
}

void UiChatPanel::flushHtml()
{
    if (streamingItem_ && !pendingHtml_.isEmpty())
    {
        streamingItem_->appendHtml(pendingHtml_);
        pendingHtml_.clear();
        messageList_->scrollToBottomIfEnabled();
    }
}

void UiChatPanel::onContentComplete(const QString &text)
{
    if(streamingItem_)
    {
        flushHtml();
        streamingItem_->appendHtml("<br>");
    }
}

void UiChatPanel::onReasoningChunk(const QString& reasoning)
{
    pendingHtml_ +=  QString("<span style='color:#9E9E9E; font-style:italic; font-size:11px;'>"
                    "%1</span>").arg(reasoning);
    if (!throttleTimer_->isActive())
        throttleTimer_->start();
}

void UiChatPanel::onReasoningComplete(const QString& /*reasoning*/)
{
    if(streamingItem_)
    {
        flushHtml();
        streamingItem_->appendHtml("<br>");
    }
}

void UiChatPanel::onToolCallRequest(const QString& toolCallId,
                                    const QString& functionName,
                                    const QString& arguments)
{
    // 该回合首个工具调用 → 创建时间线并插入消息列表
    if (!toolTimeline_)
    {
        toolTimeline_ = new UiToolCallTimeline(messageList_);
        messageList_->appendWidget(toolTimeline_);
    }

    toolTimeline_->addEntry(toolCallId, functionName, arguments);
}

void UiChatPanel::onToolCallResult(const QString& toolCallId,
                                   const QString& functionName,
                                   const QString& result)
{
    A_UNUSED(functionName);

    if (toolTimeline_)
    {
        toolTimeline_->setEntryResult(toolCallId, result);
    }

    // 重新创建助手消息占位（工具调用完成后 LLM 会继续生成）
    if (!streamingItem_)
        streamingItem_ = messageList_->appendMessage(EChatRole::eAssistant);
}

void UiChatPanel::onCompleted()
{
    // 完成流式消息
    streamingItem_ = nullptr;
    toolTimeline_ = nullptr;
}

void UiChatPanel::onError(const QString& error)
{
    if (streamingItem_)
    {
        streamingItem_->appendHtml(
            QString("<br><span style='color:#D32F2F;'>[错误] %1</span>").arg(error.toHtmlEscaped()));
        QTimer::singleShot(0, messageList_, &UiChatMessageList::scrollToBottomIfEnabled);
    }
}

void UiChatPanel::onWorkerFinished(int errorCode)
{
    A_UNUSED(errorCode);
    setBusy(false);
    emit chatFinished();
}

// ============================================================
//  辅助方法
// ============================================================

void UiChatPanel::setBusy(bool busy)
{
    busy_ = busy;
    chatInput_->setSendEnabled(!busy);
    statusLabel_->setText(busy ? QString::fromUtf8("助手思考中...")
                               : QString::fromUtf8("就绪"));
    statusLabel_->setStyleSheet(busy
        ? "QLabel { color: #1976D2; font-size: 11px; padding: 4px 12px;"
          "  background-color: #E3F2FD; border-bottom: 1px solid #90CAF9; }"
        : "QLabel { color: #9E9E9E; font-size: 11px; padding: 4px 12px;"
          "  background-color: #F5F5F5; border-bottom: 1px solid #E0E0E0; }");
}

AST_NAMESPACE_END
