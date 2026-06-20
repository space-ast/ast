///
/// @file      UiChatMessageList.cpp
/// @brief     聊天消息列表实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiChatMessageList.hpp"
#include "UiChatMessageItem.hpp"
#include "AstAI/ChatMessage.hpp"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>

AST_NAMESPACE_BEGIN

UiChatMessageList::UiChatMessageList(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

UiChatMessageList::~UiChatMessageList() = default;

void UiChatMessageList::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setStyleSheet(
        "QScrollArea {"
        "  background-color: #FAFAFA;"
        "}"
    );

    contentWidget_ = new QWidget(scrollArea_);
    contentWidget_->setStyleSheet("background-color: #FAFAFA;");
    contentLayout_ = new QVBoxLayout(contentWidget_);
    contentLayout_->setContentsMargins(0, 8, 0, 8);
    contentLayout_->setSpacing(2);
    contentLayout_->addStretch();

    scrollArea_->setWidget(contentWidget_);
    mainLayout->addWidget(scrollArea_);

    // 监听用户滚动行为：滚到底部 → 开启自动跟随；滚离底部 → 关闭
    connect(scrollArea_->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &UiChatMessageList::onScrollBarValueChanged);
}

UiChatMessageItem* UiChatMessageList::appendMessage(EChatRole role, const QString& content)
{
    auto* item = new UiChatMessageItem(role, content, contentWidget_);

    // 插入到 stretch 之前
    int stretchIndex = contentLayout_->count() - 1; // 最后一个 item 是 stretch
    contentLayout_->insertWidget(stretchIndex, item);

    // 延迟滚动（等布局更新后）
    QTimer::singleShot(0, this, [this]() { scrollToBottom(); });

    return item;
}

UiChatMessageItem* UiChatMessageList::lastMessage() const
{
    // 倒序查找第一个 UiChatMessageItem（跳过 stretch）
    for (int i = contentLayout_->count() - 1; i >= 0; --i)
    {
        QLayoutItem* layoutItem = contentLayout_->itemAt(i);
        if (layoutItem && layoutItem->widget())
        {
            if (auto* msg = qobject_cast<UiChatMessageItem*>(layoutItem->widget()))
                return msg;
        }
    }
    return nullptr;
}

void UiChatMessageList::appendWidget(QWidget* widget)
{
    if (!widget) return;

    widget->setParent(contentWidget_);

    // 插入到 stretch 之前
    int stretchIndex = contentLayout_->count() - 1;
    contentLayout_->insertWidget(stretchIndex, widget);

    // 延迟滚动
    QTimer::singleShot(0, this, [this]() { scrollToBottom(); });
}

void UiChatMessageList::clear()
{
    // 移除所有控件（保留 stretch）
    while (contentLayout_->count() > 1)
    {
        QLayoutItem* item = contentLayout_->takeAt(0);
        if (item->widget())
            delete item->widget();
        delete item;
    }
}

void UiChatMessageList::scrollToBottom()
{
    QScrollBar* sb = scrollArea_->verticalScrollBar();
    if (sb)
        sb->setValue(sb->maximum());
}

void UiChatMessageList::scrollToBottomIfEnabled()
{
    if (!autoScrollEnabled_)
        return;

    QScrollBar* sb = scrollArea_->verticalScrollBar();
    if (sb)
        sb->setValue(sb->maximum());
}

void UiChatMessageList::onScrollBarValueChanged(int value)
{
    QScrollBar* sb = scrollArea_->verticalScrollBar();
    if (!sb) return;

    // 用户滚到底部附近（5px 容忍）→ 开启自动跟随
    // 用户滚离底部 → 关闭自动跟随，查看历史消息
    autoScrollEnabled_ = (value >= sb->maximum() - 5);
}

int UiChatMessageList::messageCount() const
{
    // count - 1 是因为最后一个始终是 stretch
    return qMax(0, contentLayout_->count() - 1);
}

AST_NAMESPACE_END
