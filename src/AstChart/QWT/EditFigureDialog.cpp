///
/// @file      EditFigureDialog.cpp
/// @brief     Figure 元素属性编辑对话框实现
/// @author    axel
/// @date      2026-06-06
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#include "EditFigureDialog.hpp"
#include "PropertyPages.hpp"
A_SUPPRESS_WARNINGS_BEGIN
#include <matplot/core/figure_type.h>
#include <matplot/core/axes_type.h>
#include <matplot/axes_objects/line.h>
#include <matplot/axes_objects/surface.h>
A_SUPPRESS_WARNINGS_END
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QHeaderView>
#include <QCoreApplication>
#include <QDebug>

AST_NAMESPACE_BEGIN

EditFigureDialog::EditFigureDialog(matplot::figure_type* figure, QWidget* parent)
    : QDialog(parent)
    , figure_(figure)
{
    setupUi();
    buildTree();
    connect(applyBtn_, &QPushButton::clicked, this, &EditFigureDialog::onApply);
    connect(tree_, &QTreeWidget::currentItemChanged,
            this, &EditFigureDialog::onTreeSelectionChanged);
}

void EditFigureDialog::setupUi()
{
    setWindowTitle(tr("图形属性编辑器"));
    resize(680, 500);

    auto* mainLayout = new QVBoxLayout(this);

    // 水平分割器：树 | 属性页
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    tree_ = new QTreeWidget(this);
    tree_->setHeaderLabel(tr("元素"));
    tree_->setMinimumWidth(140);
    tree_->setMaximumWidth(200);

    stack_ = new QStackedWidget(this);
    axesPage_ = new AxesPropertyPage(this);
    linePage_ = new LinePropertyPage(this);
    surfacePage_ = new SurfacePropertyPage(this);
    placeholder_ = new QWidget(this);
    auto* placeholderLayout = new QVBoxLayout(placeholder_);
    auto* label = new QLabel(tr("请从左侧列表选择要编辑的元素"), placeholder_);
    label->setAlignment(Qt::AlignCenter);
    placeholderLayout->addWidget(label);

    stack_->addWidget(placeholder_);   // index 0
    stack_->addWidget(axesPage_);      // index 1
    stack_->addWidget(linePage_);      // index 2
    stack_->addWidget(surfacePage_);   // index 3

    splitter->addWidget(tree_);
    splitter->addWidget(stack_);
    mainLayout->addWidget(splitter);

    // 底部按钮
    applyBtn_ = new QPushButton(tr("应用"), this);
    auto* okBtn = new QPushButton(tr("确定"), this);
    auto* cancelBtn = new QPushButton(tr("取消"), this);

    auto* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(applyBtn_);
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, [this]() { onApply(); accept(); });
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void EditFigureDialog::buildTree()
{
    tree_->clear();
    if (!figure_) return;

    auto* rootItem = new QTreeWidgetItem(tree_);
    rootItem->setText(0, tr("Figure"));
    rootItem->setIcon(0, loadIcon("ChartFigure"));
    rootItem->setData(0, DataRole::TypeRole, TypeFigure);
    rootItem->setData(0, DataRole::AxesIndexRole, -1);
    rootItem->setData(0, DataRole::ItemIndexRole, -1);
    rootItem->setExpanded(true);

    QIcon axesIcon = loadIcon("Axes");
    QIcon lineIcon = loadIcon("ChartLine");
    QIcon surfaceIcon = loadIcon("ChartSurface");
    QIcon elementIcon = loadIcon("ChartElement");

    auto& axesChildren = figure_->children();
    for (size_t i = 0; i < axesChildren.size(); ++i) {
        auto& axes = axesChildren[i];
        auto* axesItem = new QTreeWidgetItem(rootItem);
        QString title = QString::fromStdString(axes->title());
        if (title.isEmpty())
            title = tr("坐标轴 %1").arg(static_cast<int>(i + 1));
        axesItem->setText(0, title);
        axesItem->setIcon(0, axesIcon);
        axesItem->setData(0, DataRole::TypeRole, TypeAxes);
        axesItem->setData(0, DataRole::AxesIndexRole, static_cast<int>(i));
        axesItem->setData(0, DataRole::ItemIndexRole, -1);
        axesItem->setExpanded(true);

        auto& items = axes->children();
        for (size_t j = 0; j < items.size(); ++j) {
            auto& obj = items[j];
            auto* itemNode = new QTreeWidgetItem(axesItem);
            QString name = QString::fromStdString(obj->display_name());
            if (name.isEmpty())
                name = tr("元素 %1").arg(static_cast<int>(j + 1));
            itemNode->setText(0, name);
            // 根据对象类型设置图标
            if (dynamic_cast<class matplot::line*>(obj.get()))
                itemNode->setIcon(0, lineIcon);
            else if (dynamic_cast<class matplot::surface*>(obj.get()))
                itemNode->setIcon(0, surfaceIcon);
            else
                itemNode->setIcon(0, elementIcon);
            itemNode->setData(0, DataRole::TypeRole, TypePlotItem);
            itemNode->setData(0, DataRole::AxesIndexRole, static_cast<int>(i));
            itemNode->setData(0, DataRole::ItemIndexRole, static_cast<int>(j));
        }
    }
}

void EditFigureDialog::selectPlotItem(int axesIndex, int itemIndex)
{
    buildTree();  // 重建树确保同步

    // 遍历找到对应节点并选中
    for (int i = 0; i < tree_->topLevelItemCount(); ++i) {
        auto* figItem = tree_->topLevelItem(i);
        for (int ai = 0; ai < figItem->childCount(); ++ai) {
            auto* axesItem = figItem->child(ai);
            int adAxesIndex = axesItem->data(0, DataRole::AxesIndexRole).toInt();
            if (adAxesIndex != axesIndex) continue;

            if (itemIndex < 0) {
                tree_->setCurrentItem(axesItem);
                return;
            }

            for (int ii = 0; ii < axesItem->childCount(); ++ii) {
                auto* itemNode = axesItem->child(ii);
                int idItemIndex = itemNode->data(0, DataRole::ItemIndexRole).toInt();
                if (idItemIndex == itemIndex) {
                    tree_->setCurrentItem(itemNode);
                    return;
                }
            }
        }
    }
}

void EditFigureDialog::onTreeSelectionChanged()
{
    auto* item = tree_->currentItem();
    if (!item) {
        stack_->setCurrentIndex(0);
        return;
    }

    int ndType = item->data(0, DataRole::TypeRole).toInt();
    int ndAxesIndex = item->data(0, DataRole::AxesIndexRole).toInt();
    int ndItemIndex = item->data(0, DataRole::ItemIndexRole).toInt();

    if (!figure_) {
        stack_->setCurrentIndex(0);
        return;
    }

    switch (static_cast<NodeType>(ndType)) {
    case TypeFigure:
        stack_->setCurrentIndex(0);
        break;
    case TypeAxes: {
        auto& axesChildren = figure_->children();
        if (ndAxesIndex >= 0 && ndAxesIndex < static_cast<int>(axesChildren.size())) {
            axesPage_->load(axesChildren[ndAxesIndex].get(), ndAxesIndex);
            stack_->setCurrentIndex(1);
        }
        break;
    }
    case TypePlotItem: {
        auto& axesChildren = figure_->children();
        if (ndAxesIndex >= 0 && ndAxesIndex < static_cast<int>(axesChildren.size())) {
            auto& axes = axesChildren[ndAxesIndex];
            auto& items = axes->children();
            if (ndItemIndex >= 0 && ndItemIndex < static_cast<int>(items.size())) {
                auto& obj = items[ndItemIndex];
                if (auto* line = dynamic_cast<class matplot::line*>(obj.get())) {
                    linePage_->load(line, ndItemIndex);
                    stack_->setCurrentIndex(2);
                } else if (auto* surf = dynamic_cast<class matplot::surface*>(obj.get())) {
                    surfacePage_->load(surf, ndItemIndex);
                    stack_->setCurrentIndex(3);
                } else {
                    stack_->setCurrentIndex(0);
                }
            }
        }
        break;
    }
    }
}

void EditFigureDialog::onApply()
{
    if (!figure_) return;

    auto* item = tree_->currentItem();
    if (!item) return;

    int ndType = item->data(0, DataRole::TypeRole).toInt();
    int ndAxesIndex = item->data(0, DataRole::AxesIndexRole).toInt();
    int ndItemIndex = item->data(0, DataRole::ItemIndexRole).toInt();

    auto& axesChildren = figure_->children();

    switch (static_cast<NodeType>(ndType)) {
    case TypeAxes: {
        if (ndAxesIndex >= 0 && ndAxesIndex < static_cast<int>(axesChildren.size())) {
            axesPage_->apply(axesChildren[ndAxesIndex].get());
        }
        break;
    }
    case TypePlotItem: {
        if (ndAxesIndex >= 0 && ndAxesIndex < static_cast<int>(axesChildren.size())) {
            auto& axes = axesChildren[ndAxesIndex];
            auto& items = axes->children();
            if (ndItemIndex >= 0 && ndItemIndex < static_cast<int>(items.size())) {
                auto& obj = items[ndItemIndex];
                if (auto* line = dynamic_cast<class matplot::line*>(obj.get()))
                    linePage_->apply(line);
                else if (auto* surf = dynamic_cast<class matplot::surface*>(obj.get()))
                    surfacePage_->apply(surf);
            }
        }
        break;
    }
    default:
        break;
    }

    // 触发重绘
    figure_->draw();
}

QIcon EditFigureDialog::loadIcon(const QString& name) const
{
    QString path = QCoreApplication::applicationDirPath()
                   + "/data/icons/" + name + ".svg";
    return QIcon(path);
}

AST_NAMESPACE_END
