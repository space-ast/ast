///
/// @file      UiChatMainWidget.cpp
/// @brief     聊天主控件实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiChatMainWidget.hpp"
#include "UiChatPanel.hpp"
#include "AstAI/ChatSession.hpp"
#include <QHBoxLayout>

AST_NAMESPACE_BEGIN

UiChatMainWidget::UiChatMainWidget(ChatSession* session, QWidget* parent)
    : QWidget(parent)
{
    setupUi();
    if (session)
        setSession(session);
}

UiChatMainWidget::~UiChatMainWidget() = default;

void UiChatMainWidget::setupUi()
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 聊天面板
    chatPanel_ = new UiChatPanel(nullptr, this);
    layout->addWidget(chatPanel_, 1);
}

ChatSession* UiChatMainWidget::session() const
{
    return chatPanel_ ? chatPanel_->session() : nullptr;
}

void UiChatMainWidget::setSession(ChatSession* session)
{
    if (chatPanel_)
        chatPanel_->setSession(session);
}

AST_NAMESPACE_END
