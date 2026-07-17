///
/// @file      UiToolCallCard.hpp
/// @brief     工具调用卡片部件（可折叠）
/// @details   显示 LLM 工具调用的名称、参数和结果。
///            支持三种状态：运行中、成功、失败。
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include <QWidget>

class QLabel;
class QTextEdit;
class QPushButton;

AST_NAMESPACE_BEGIN

/*!
    @addtogroup UiAI
    @{
*/

/// @brief 工具调用状态
enum class EToolCallState
{
    eRunning,   ///< 执行中
    eSuccess,   ///< 成功
    eError      ///< 失败
};

/// @brief 工具调用卡片部件
class AST_UIAI_API UiToolCallCard : public QWidget
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param toolCallId 工具调用 ID
    /// @param functionName 函数名称
    /// @param arguments 参数（JSON 字符串）
    /// @param parent 父控件
    explicit UiToolCallCard(const QString& toolCallId,
                            const QString& functionName,
                            const QString& arguments = QString(),
                            QWidget* parent = nullptr);

    ~UiToolCallCard() override;
    UiToolCallCard(const UiToolCallCard&) = delete;
    UiToolCallCard& operator=(const UiToolCallCard&) = delete;

    /// @brief 设置工具调用状态
    void setState(EToolCallState state);

    /// @brief 设置执行结果
    void setResult(const QString& result);

    /// @brief 获取工具调用 ID
    QString toolCallId() const { return toolCallId_; }

    /// @brief 获取函数名称
    QString functionName() const { return functionName_; }

private Q_SLOTS:
    void toggleExpanded();

private:
    void setupUi();
    void updateAppearance();

    QString         toolCallId_;
    QString         functionName_;
    QString         arguments_;
    QString         result_{};
    EToolCallState  state_{EToolCallState::eRunning};
    bool            expanded_{false};

    QPushButton*    headerButton_ = nullptr;  // 标题栏按钮
    QWidget*        detailWidget_ = nullptr;  // 详情区域
    QTextEdit*      argsView_ = nullptr;      // 参数显示
    QTextEdit*      resultView_ = nullptr;    // 结果显示
    QLabel*         statusLabel_ = nullptr;   // 状态标签
};

/*! @} */

AST_NAMESPACE_END
