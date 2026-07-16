///
/// @file      UiToolCallTimeline.hpp
/// @brief     工具调用时间线部件（Timeline 列表 + 默认折叠）
/// @details   将一个对话回合中的所有工具调用聚合为一条紧凑的时间线。
///            默认折叠，仅显示摘要（"N 个工具调用" + 函数链）。
///            展开后显示垂直时间线条目，每条可单独展开查看参数/结果。
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include <QWidget>
#include <QVector>

class QVBoxLayout;
class QPushButton;

AST_NAMESPACE_BEGIN

enum class EToolCallState;

/*!
    @addtogroup UiAI
    @{
*/

/// @brief 工具调用时间线部件
/// @details 聚合一个对话回合中所有工具调用，以时间线形式紧凑展示。
///
///   Collapsed:  ▶ 🔧 3 个工具调用 (New → SetState → Propagate)
///   Expanded:   垂直时间线，每条一个圆点 + 函数名 + 状态，可展开详情
class AST_UIAI_API UiToolCallTimeline : public QWidget
{
    Q_OBJECT

public:
    explicit UiToolCallTimeline(QWidget* parent = nullptr);
    ~UiToolCallTimeline() override;
    UiToolCallTimeline(const UiToolCallTimeline&) = delete;
    UiToolCallTimeline& operator=(const UiToolCallTimeline&) = delete;

    /// @brief 添加一条工具调用条目
    /// @param toolCallId 工具调用 ID
    /// @param functionName 函数名称
    /// @param arguments 参数（JSON 字符串）
    void addEntry(const QString& toolCallId,
                  const QString& functionName,
                  const QString& arguments);

    /// @brief 设置条目结果
    /// @param toolCallId 工具调用 ID
    /// @param result 执行结果
    /// @param success 是否成功
    void setEntryResult(const QString& toolCallId,
                        const QString& result,
                        bool success = true);

    /// @brief 获取条目数量
    int entryCount() const { return entries_.size(); }

    /// @brief 是否已折叠
    bool isCollapsed() const { return collapsed_; }

private Q_SLOTS:
    void toggleCollapsed();
    void onEntryClicked(int index);

private:
    void setupUi();
    void rebuildBody();
    void updateHeaderText();

    struct Entry
    {
        QString toolCallId;
        QString functionName;
        QString arguments;
        QString result;
        EToolCallState state{};
        bool    detailExpanded = false;
    };

    QVector<Entry>  entries_;
    bool            collapsed_ = true;

    // UI 部件
    QPushButton*    headerButton_ = nullptr;
    QWidget*        bodyWidget_ = nullptr;
    QVBoxLayout*    bodyLayout_ = nullptr;
};

/*! @} */

AST_NAMESPACE_END
