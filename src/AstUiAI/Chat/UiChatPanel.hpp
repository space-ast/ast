///
/// @file      UiChatPanel.hpp
/// @brief     聊天面板 — 消息列表 + 输入区域 + 工具栏
/// @details   聊天功能的主面板，组合 UiChatMessageList、UiChatInput 和
///            UiChatEventHandler，管理完整的对话交互流程。
///            使用 UiChatWorker 在后台线程中运行 LLM 调用。
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include <QWidget>
#include <memory>

class QLabel;
class QTimer;

AST_NAMESPACE_BEGIN

class ChatSession;
class UiChatMessageList;
class UiChatMessageItem;
class UiChatInput;
class UiChatEventHandler;
class UiChatWorker;
class UiToolCallTimeline;

/*!
    @addtogroup UiAI
    @{
*/

/// @brief 聊天面板
/// @details 聊天功能的主面板，管理对话的完整生命周期：
///          1. 用户输入 → 追加用户消息气泡
///          2. 启动 UiChatWorker 后台线程调用 LLM
///          3. 通过 UiChatEventHandler 实时更新助手消息气泡
///          4. 工具调用时插入 UiToolCallCard
///          5. 对话完成后恢复输入
class AST_UIAI_API UiChatPanel : public QWidget
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param session 聊天会话（非拥有）
    /// @param parent 父控件
    explicit UiChatPanel(ChatSession* session, QWidget* parent = nullptr);

    ~UiChatPanel() override;

    /// @brief 获取关联的聊天会话
    ChatSession* session() const { return session_; }

    /// @brief 设置聊天会话
    void setSession(ChatSession* session);

    /// @brief 发送消息
    void sendMessage(const QString& message);

    /// @brief 清空消息历史
    void clearMessages();

    /// @brief 是否正在等待回复
    bool isBusy() const { return busy_; }

Q_SIGNALS:
    /// @brief 对话开始（用户发送消息后）
    void chatStarted();

    /// @brief 对话完成
    void chatFinished();

private Q_SLOTS:
    void onSendMessage(const QString& message);
    void onStopRequested();

    // 事件处理器槽
    void onHtmlChunk(const QString& html);
    void flushHtml();
    void onContentComplete(const QString& text);
    void onReasoningChunk(const QString& reasoning);
    void onReasoningComplete(const QString& reasoning);
    void onToolCallRequest(const QString& toolCallId,
                           const QString& functionName,
                           const QString& arguments);
    void onToolCallResult(const QString& toolCallId,
                          const QString& functionName,
                          const QString& result);
    void onCompleted();
    void onError(const QString& error);
    void onWorkerFinished(int errorCode);

private:
    void setupUi();
    void setBusy(bool busy);

    ChatSession*            session_ = nullptr;
    UiChatEventHandler*     handler_ = nullptr;
    UiChatMessageList*      messageList_ = nullptr;
    UiChatInput*            chatInput_ = nullptr;
    QLabel*                 statusLabel_ = nullptr;

    // 流式状态
    UiChatMessageItem*      streamingItem_ = nullptr;   // 正在流式填充的助手消息
    UiToolCallTimeline*     toolTimeline_ = nullptr;    // 当前回合的工具调用时间线
    QTimer*                 throttleTimer_ = nullptr;   // 40ms 节流定时器
    QString                 pendingHtml_;               // 累积的 HTML（定时器触发后一次性渲染）
    std::string             accumulatedText_;            // 累积的流式文本
    bool                    busy_{false};
};

/*! @} */

AST_NAMESPACE_END
