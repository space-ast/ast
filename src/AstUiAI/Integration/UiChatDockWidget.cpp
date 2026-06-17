///
/// @file      UiChatDockWidget.cpp
/// @brief     聊天停靠面板实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiChatDockWidget.hpp"
#include "AstUiAI/UiChatMainWidget.hpp"
#include "AstAI/ChatSession.hpp"
#include <QMainWindow>
#include <QApplication>

AST_NAMESPACE_BEGIN

UiChatDockWidget::UiChatDockWidget(ChatSession* session, QWidget* parent)
    : QDockWidget(parent)
{
    setupUi();
    if (session && chatWidget_)
        chatWidget_->setSession(session);
}

UiChatDockWidget::~UiChatDockWidget() = default;

void UiChatDockWidget::setupUi()
{
    setWindowTitle(QString::fromUtf8("AI 助手"));
    setObjectName(QStringLiteral("ChatDock"));
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    setMinimumWidth(320);
    setFeatures(QDockWidget::DockWidgetMovable |
                QDockWidget::DockWidgetClosable |
                QDockWidget::DockWidgetFloatable);

    chatWidget_ = new UiChatMainWidget(nullptr, this);
    setWidget(chatWidget_);
}

bool UiChatDockWidget::dockInto(QMainWindow* mainWindow)
{
    if (!mainWindow)
        return false;

    mainWindow->addDockWidget(Qt::RightDockWidgetArea, this);

    // 关闭时通知
    connect(this, &QDockWidget::visibilityChanged,
            this, [this](bool visible) {
        if (!visible)
            emit dockClosed();
    });

    return true;
}

bool UiChatDockWidget::autoDock()
{
    const auto tops = QApplication::topLevelWidgets();
    for (auto* top : tops)
    {
        if (auto* mw = qobject_cast<QMainWindow*>(top))
        {
            if (mw->isVisible())
                return dockInto(mw);
        }
    }
    return false;
}

AST_NAMESPACE_END
