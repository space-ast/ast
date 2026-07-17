///
/// @file      UiChatMessageItem.hpp
/// @brief     单条聊天消息气泡部件
/// @details   根据消息角色（用户/助手/系统/工具）使用不同的样式和对齐方式。
///            助手消息支持 Markdown 渲染。
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QString>

AST_NAMESPACE_BEGIN

enum class EChatRole;

/*!
    @addtogroup UiAI
    @{
*/

/// @brief 单条聊天消息气泡部件
/// @details 用户消息右对齐（蓝色气泡）、助手消息左对齐（灰色气泡）、
///          系统消息居中（浅色）、工具消息左对齐（小字体）。
class AST_UIAI_API UiChatMessageItem : public QWidget
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param role 消息角色
    /// @param content 消息内容
    /// @param parent 父控件
    explicit UiChatMessageItem(EChatRole role,
                               const QString& content = QString(),
                               QWidget* parent = nullptr);

    ~UiChatMessageItem() override;
    UiChatMessageItem(const UiChatMessageItem&) = delete;
    UiChatMessageItem& operator=(const UiChatMessageItem&) = delete;

    /// @brief 设置消息内容（替换全部）
    void setContent(const QString& content);

    /// @brief 追加 HTML 片段（用于流式更新、思考、错误等已格式化的内容）
    void appendHtml(const QString& text);

    /// @brief 获取消息角色
    EChatRole role() const { return role_; }

    /// @brief 获取当前内容
    QString content() const;

    /// @brief 根据角色获取角色名称
    static QString roleDisplayName(EChatRole role);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private Q_SLOTS:
    void updateContentHeight();

private:
    void setupUi();
    void applyRoleStyle();

    EChatRole   role_;
    QLabel*     roleLabel_ = nullptr;     // 角色标签（"你" / "助手"）
    QTextEdit*  contentEdit_ = nullptr;   // 消息内容（支持富文本）
    QString     accumulatedHtml_{};         // 累积的 HTML（用于流式渲染 setHtml）
    int         cachedDocHeight_ = 0;     // 上次计算的文档高度，避免循环
};

/*! @} */

AST_NAMESPACE_END
