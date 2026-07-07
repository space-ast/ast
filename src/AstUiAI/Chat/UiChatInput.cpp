///
/// @file      UiChatInput.cpp
/// @brief     聊天输入区域部件实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiChatInput.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QKeyEvent>

AST_NAMESPACE_BEGIN

UiChatInput::UiChatInput(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

UiChatInput::~UiChatInput() = default;

void UiChatInput::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    // 输入框
    textEdit_ = new QTextEdit(this);
    textEdit_->setPlaceholderText(QString::fromUtf8("请输入您的需求，我来设计场景..."));
    textEdit_->setMaximumHeight(120);
    textEdit_->setMinimumHeight(48);
    textEdit_->setStyleSheet(
        "QTextEdit {"
        "  border: 1px solid #BDBDBD;"
        "  border-radius: 8px;"
        "  padding: 8px 10px;"
        "  font-size: 13px;"
        "  background-color: #FFFFFF;"
        "}"
        "QTextEdit:focus {"
        "  border-color: #1976D2;"
        "}"
    );
    // 安装事件过滤器以处理 Enter/Shift+Enter
    textEdit_->installEventFilter(this);
    layout->addWidget(textEdit_);

    // 按钮行
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);

    stopButton_ = new QPushButton(QString::fromUtf8("停止"), this);
    stopButton_->setStyleSheet(
        "QPushButton {"
        "  background-color: #EF5350;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 6px 18px;"
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #E53935;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #FFCDD2;"
        "}"
    );
    stopButton_->setVisible(false);
    btnLayout->addWidget(stopButton_);

    btnLayout->addStretch();

    sendButton_ = new QPushButton(QString::fromUtf8("发送"), this);
    sendButton_->setStyleSheet(
        "QPushButton {"
        "  background-color: #1976D2;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 6px 24px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #1565C0;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #BBDEFB;"
        "}"
    );
    btnLayout->addWidget(sendButton_);

    layout->addLayout(btnLayout);

    // 连接信号
    connect(sendButton_, &QPushButton::clicked,
            this, &UiChatInput::onSendClicked);
    connect(stopButton_, &QPushButton::clicked,
            this, &UiChatInput::stopRequested);

    connect(textEdit_, &QTextEdit::textChanged,
            this, &UiChatInput::onTextChanged);
}

bool UiChatInput::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == textEdit_ && event->type() == QEvent::KeyPress)
    {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            if (!(keyEvent->modifiers() & Qt::ShiftModifier))
            {
                onSendClicked();
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void UiChatInput::onSendClicked()
{
    if (!textEdit_) return;
    QString text = textEdit_->toPlainText().trimmed();
    if (text.isEmpty()) return;

    emit sendMessage(text);
}

void UiChatInput::onTextChanged()
{
    if (!textEdit_) return;
    QString text = textEdit_->toPlainText().trimmed();
    sendButton_->setEnabled(!text.isEmpty());
}

void UiChatInput::setSendEnabled(bool enabled)
{
    sendButton_->setVisible(enabled);
    stopButton_->setVisible(!enabled);
    textEdit_->setReadOnly(!enabled);

    if (enabled)
        textEdit_->setFocus();
}

void UiChatInput::clear()
{
    textEdit_->clear();
}

void UiChatInput::setInputFocus()
{
    textEdit_->setFocus();
}

void UiChatInput::setPlaceholderText(const QString& text)
{
    textEdit_->setPlaceholderText(text);
}

AST_NAMESPACE_END
