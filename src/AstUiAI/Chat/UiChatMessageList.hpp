///
/// @file      UiChatMessageList.hpp
/// @brief     聊天消息列表（可滚动区域）
/// @details   使用 QScrollArea + 垂直布局容纳多条 UiChatMessageItem。
///            支持追加消息、流式更新末尾消息、清空等操作。
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include <QWidget>

class QScrollArea;
class QVBoxLayout;
class UiChatMessageItem;

AST_NAMESPACE_BEGIN

enum class EChatRole;

/*!
    @addtogroup UiAI
    @{
*/

/// @brief 聊天消息列表（可滚动区域）
class AST_UIAI_API UiChatMessageList : public QWidget
{
    Q_OBJECT

public:
    explicit UiChatMessageList(QWidget* parent = nullptr);
    ~UiChatMessageList() override;

    /// @brief 追加一条消息
    /// @return 新创建的消息部件指针
    UiChatMessageItem* appendMessage(EChatRole role, const QString& content = QString());

    /// @brief 获取最后一条消息（用于流式追加文本）
    /// @return 最后一条消息，列表为空返回 nullptr
    UiChatMessageItem* lastMessage() const;

    /// @brief 在消息列表中插入一个自定义部件（如工具调用时间线）
    void appendWidget(QWidget* widget);

    /// @brief 清空所有消息
    void clear();

    /// @brief 滚动到底部
    void scrollToBottom();

    /// @brief 若自动跟随已启用则滚动到底部（用于流式更新）
    void scrollToBottomIfEnabled();

    /// @brief 获取消息数量
    int messageCount() const;

private Q_SLOTS:
    /// @brief 根据滚动条位置更新自动跟随标志
    void onScrollBarValueChanged(int value);

private:
    void setupUi();

    QScrollArea*    scrollArea_ = nullptr;
    QWidget*        contentWidget_ = nullptr;
    QVBoxLayout*    contentLayout_ = nullptr;

    bool autoScrollEnabled_ = true;  // 用户是否开启了自动跟随（滚到底部=开启，滚离=关闭）
};

/*! @} */

AST_NAMESPACE_END
