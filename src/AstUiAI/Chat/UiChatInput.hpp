///
/// @file      UiChatInput.hpp
/// @brief     聊天输入区域部件
/// @details   多行文本输入框 + 发送按钮 + 停止按钮。
///            支持 Enter 发送、Shift+Enter 换行。
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include <QWidget>

class QTextEdit;
class QPushButton;

AST_NAMESPACE_BEGIN

/*!
    @addtogroup UiAI
    @{
*/

/// @brief 聊天输入区域部件
class AST_UIAI_API UiChatInput : public QWidget
{
    Q_OBJECT

public:
    explicit UiChatInput(QWidget* parent = nullptr);
    ~UiChatInput() override;

    UiChatInput(const UiChatInput&) = delete;
    UiChatInput& operator=(const UiChatInput&) = delete;

    /// @brief 设置发送按钮是否可用
    void setSendEnabled(bool enabled);

    /// @brief 清空输入框
    void clear();

    /// @brief 设置输入框焦点
    void setInputFocus();

    /// @brief 设置占位提示文本
    void setPlaceholderText(const QString& text);

Q_SIGNALS:
    /// @brief 用户点击发送
    void sendMessage(const QString& message);

    /// @brief 用户点击停止
    void stopRequested();

private Q_SLOTS:
    void onSendClicked();
    void onTextChanged();

private:
    void setupUi();
    bool eventFilter(QObject* obj, QEvent* event) override;

    QTextEdit*   textEdit_ = nullptr;
    QPushButton* sendButton_ = nullptr;
    QPushButton* stopButton_ = nullptr;
};

/*! @} */

AST_NAMESPACE_END
