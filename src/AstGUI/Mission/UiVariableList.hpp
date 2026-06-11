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
#include "AstCore/Object.hpp"
#include "AstScript/Expr.hpp"
#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QToolButton>

AST_NAMESPACE_BEGIN

/// @brief 变量列表编辑控件，以表格形式管理脚本变量的增删改
class AST_GUI_API UiVariableList : public QWidget
{
    Q_OBJECT
public:
    explicit UiVariableList(QWidget* parent = nullptr);

    /// @brief 设置要编辑的变量列表（裸指针，不持有所有权）
    /// @param variableList 要编辑的变量列表，被owner对象持有
    /// @param owner 所有者对象，用于判断变量列表的生命周期是否结束
    void setVariableList(VariableList* variableList, Object* owner);

    /// @brief 设置脚本解释器
    /// @param interpreter 脚本解释器，被owner对象持有
    /// @param owner 所有者对象，用于判断解释器的生命周期是否结束
    void setInterpreter(Interpreter* interpreter, Object* owner);

    /// @brief 设置工具栏可见性
    void setToolbarVisible(bool visible);

    /// @brief 刷新表格显示
    void refreshUi();

    /// @brief 获取当前选中的变量
    Variable* selectedVariable() const;

    /// @brief 获取当前编辑的变量列表
    VariableList* variableList() const;

    /// @brief 获取当前关联的脚本解释器
    Interpreter* interpreter() const;

    /// @brief 输入表达式，新建变量并追加到列表末尾
    /// @param expr 要添加的表达式
    void addExpression(Expr* expr, bool bind);

    /// @brief 输入表达式，新建与表达式双向绑定的变量并追加到列表末尾
    /// @param expr 要添加的表达式
    void addBindExpression(Expr* expr){return addExpression(expr, true);}


    void addExpression(Expr* expr){return addExpression(expr, false);}

signals:
    /// @brief 选中变量变化
    void variableSelected(Variable* variable);

    /// @brief 变量列表已修改（增/删/编辑/拖拽排序）
    void variableListChanged();

    /// @brief 用户选中了变量（用于通知外部联动控件）
    void variableFocused();

private slots:
    void onSelectionChanged();
    void onAddVariable();
    void onRemoveVariable();
    void onCellChanged(int row, int column);
    void onRefresh();

private:
    void setupUi();
    bool eventFilter(QObject* obj, QEvent* event) override;
    void syncOrderFromTable();

    QVBoxLayout*    mainLayout_;
    QTableWidget*   tableWidget_;
    QHBoxLayout*    buttonLayout_;
    QToolButton*    addButton_;
    QToolButton*    removeButton_;
    QToolButton*    refreshButton_;
private:
    VariableList*   variableList_ = nullptr;
    WeakPtr<Object> variableListOwner_;
    Interpreter*    interpreter_ = nullptr;
    WeakPtr<Object> interpreterOwner_;
};

AST_NAMESPACE_END
