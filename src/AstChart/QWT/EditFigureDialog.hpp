///
/// @file      EditFigureDialog.hpp
/// @brief     Figure 元素属性编辑对话框
/// @author    axel
/// @date      2026-06-06
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
#pragma once

#include "AstGlobal.h"
#include <QDialog>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QIcon>

namespace matplot {
    class figure_type;
    class axes_type;
    class axes_object;
}

AST_NAMESPACE_BEGIN

class AxesPropertyPage;
class LinePropertyPage;
class SurfacePropertyPage;

class AST_CHART_API EditFigureDialog : public QDialog {
    Q_OBJECT
public:
    explicit EditFigureDialog(matplot::figure_type* figure, QWidget* parent = nullptr);

    void selectPlotItem(int axesIndex, int itemIndex);

private slots:
    void onTreeSelectionChanged();
    void onApply();

private:
    void setupUi();
    void buildTree();
    QIcon loadIcon(const QString& name) const;

    enum NodeType { TypeFigure, TypeAxes, TypePlotItem };
    enum DataRole { TypeRole = Qt::UserRole, AxesIndexRole = Qt::UserRole + 1, ItemIndexRole = Qt::UserRole + 2 };

    matplot::figure_type* figure_;
    QTreeWidget* tree_;
    QStackedWidget* stack_;
    AxesPropertyPage* axesPage_;
    LinePropertyPage* linePage_;
    SurfacePropertyPage* surfacePage_;
    QWidget* placeholder_;
    QPushButton* applyBtn_;
};

AST_NAMESPACE_END
