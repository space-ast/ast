///
/// @file      UiChatMainWidget.hpp
/// @brief     聊天主控件 — 会话列表 + 聊天面板的组合
/// @details   右侧为聊天面板，左侧为会话列表（暂为占位，后续迭代实现）。
///            作为独立控件可直接嵌入 QMainWindow 或 QDockWidget。
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

AST_NAMESPACE_BEGIN

class ChatSession;
class UiChatPanel;

/*!
    @addtogroup UiAI
    @{
*/

/// @brief 聊天主控件
/// @details 组合会话列表（左侧）和聊天面板（右侧）。
///          MVP 阶段会话列表为占位，后续迭代实现多会话管理。
class AST_UIAI_API UiChatMainWidget : public QWidget
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param session 聊天会话（非拥有）
    /// @param parent 父控件
    explicit UiChatMainWidget(ChatSession* session, QWidget* parent = nullptr);

    ~UiChatMainWidget() override;

    /// @brief 获取聊天面板
    UiChatPanel* chatPanel() const { return chatPanel_; }

    /// @brief 获取关联的聊天会话
    ChatSession* session() const;

    /// @brief 设置聊天会话
    void setSession(ChatSession* session);

private:
    void setupUi();

    UiChatPanel* chatPanel_ = nullptr;
};

/*! @} */

AST_NAMESPACE_END
