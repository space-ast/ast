///
/// @file      UiChatDockWidget.hpp
/// @brief     聊天停靠面板 — 将聊天控件包装为 QDockWidget
/// @details   适配器：将 UiChatMainWidget 嵌入 QDockWidget，
///            方便集成到 UiMainWindow 中。
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include <QDockWidget>

AST_NAMESPACE_BEGIN

class ChatSession;
class UiChatMainWidget;

/*!
    @addtogroup UiAI
    @{
*/

/// @brief 聊天停靠面板
/// @details 将聊天主控件包装为 QDockWidget，
///          提供标准的停靠/浮动/关闭行为。
class AST_UIAI_API UiChatDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param session 聊天会话（非拥有）
    /// @param parent 父控件
    explicit UiChatDockWidget(ChatSession* session, QWidget* parent = nullptr);

    ~UiChatDockWidget() override;

    /// @brief 获取内部的聊天主控件
    UiChatMainWidget* chatWidget() const { return chatWidget_; }

    /// @brief 查找主窗口并 dock 到其右侧
    /// @param mainWindow 目标主窗口，为 nullptr 时自动查找
    /// @return 是否成功 dock
    bool dockInto(QMainWindow* mainWindow = nullptr);

    /// @brief 自动查找并附加到第一个可用的 QMainWindow
    /// @return 是否成功
    bool autoDock();

Q_SIGNALS:
    /// @brief dock 面板关闭
    void dockClosed();

private:
    void setupUi();

    UiChatMainWidget* chatWidget_ = nullptr;
};

/*! @} */

AST_NAMESPACE_END
