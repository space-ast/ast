///
/// @file      UiExpressionBrowser.hpp
/// @brief     Expr 表达式选择对话框
/// @details   用于选择对象属性表达式和对象计算量表达式
/// @author    axel
/// @date      2026-05-25
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
#include <QDialog>
#include <QString>

class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;

AST_NAMESPACE_BEGIN

class Object;
class UiObjectTree;

class AST_GUI_API UiExpressionBrowser : public QDialog
{
    Q_OBJECT
public:
    explicit UiExpressionBrowser(QWidget* parent = nullptr);

    QString selectedExpression() const { return selectedExpression_; }
    static QString getExpression(QWidget* parent = nullptr);

private slots:
    void onObjectSelected(Object* object);
    void onPropertySelectionChanged();
    void onCalculationSelectionChanged();
    void onPropertyAccepted();
    void onCalculationAccepted();
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    void setupUi();
    void refreshPropertyTree();
    void refreshCalculationTree();
    void acceptExpression(QTreeWidgetItem* item);
    QTreeWidgetItem* addExpressionItem(QTreeWidget* tree, const QString& name, const QString& expression);

    UiObjectTree* objectTree_ = nullptr;
    QTreeWidget* propertyTree_ = nullptr;
    QTreeWidget* calculationTree_ = nullptr;
    QPushButton* propertySelectButton_ = nullptr;
    QPushButton* calculationSelectButton_ = nullptr;
    Object* currentObject_ = nullptr;
    QString selectedExpression_;
};

AST_NAMESPACE_END
