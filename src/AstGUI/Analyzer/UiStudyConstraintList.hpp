///
/// @file      UiStudyConstraintList.hpp
/// @brief     响应列表面板，管理输出响应/约束列表
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstAnalyzer/StudyConstraint.hpp"
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <vector>

AST_NAMESPACE_BEGIN

/// @brief 响应列表面板，显示所有响应/约束，支持添加和选中编辑
class AST_GUI_API UiStudyConstraintList : public QWidget
{
    Q_OBJECT
public:
    explicit UiStudyConstraintList(QWidget* parent = nullptr);

    /// @brief 用响应列表刷新显示
    void setResponses(const std::vector<SharedPtr<StudyConstraint>>& responses);

    /// @brief 获取当前选中的响应
    StudyConstraint* selectedResponse() const;

signals:
    /// @brief 选中响应变化
    void responseSelected(StudyConstraint* response);

    /// @brief 请求添加新响应
    void addResponseRequested();

private slots:
    void onSelectionChanged();

private:
    void setupUi();

    QVBoxLayout*  layout_;
    QListWidget*  listWidget_;
    QPushButton*  addButton_;
    std::vector<SharedPtr<StudyConstraint>> responses_;
};

AST_NAMESPACE_END
