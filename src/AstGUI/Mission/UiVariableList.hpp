///
/// @file      UiVariableList.hpp
/// @brief     脚本变量列表编辑控件
/// @details   用于编辑 VariableList 中的 Variable 对象，以表格形式展示
/// @author    axel
/// @date      2026-05-24
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
#include "AstCore/VariableList.hpp"
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

AST_NAMESPACE_BEGIN

/// @brief 变量列表编辑控件，以表格形式管理脚本变量的增删改
class AST_GUI_API UiVariableList : public QWidget
{
    Q_OBJECT
public:
    explicit UiVariableList(QWidget* parent = nullptr);

    /// @brief 设置要编辑的变量列表（裸指针，不持有所有权）
    void setVariableList(VariableList* variableList);

    /// @brief 刷新表格显示
    void refreshUi();

    /// @brief 获取当前选中的变量
    Variable* selectedVariable() const;

signals:
    /// @brief 选中变量变化
    void variableSelected(Variable* variable);

    /// @brief 变量列表已修改（增/删/编辑）
    void variableListChanged();

private slots:
    void onSelectionChanged();
    void onAddVariable();
    void onRemoveVariable();
    void onCellDoubleClicked(int row, int column);

private:
    void setupUi();

    QVBoxLayout*   mainLayout_;
    QTableWidget*  tableWidget_;
    QHBoxLayout*   buttonLayout_;
    QPushButton*   addButton_;
    QPushButton*   removeButton_;
    VariableList*  variableList_ = nullptr;
};

AST_NAMESPACE_END
