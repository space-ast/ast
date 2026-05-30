///
/// @file      testQTreeWidget.cpp
/// @brief     QTreeWidget 基础用法示例
/// @author    Aist
/// @date      2026-05-28
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char* argv[])
{
    printf("testQTreeWidget.cpp\n");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    qDebug() << QStyleFactory::keys();
    app.setStyle(QStyleFactory::create("Windows"));
    //app.setStyle(QStyleFactory::create("windowsvista"));
    //app.setStyle(QStyleFactory::create("Fusion"));

    QMainWindow window;
    QWidget* centralWidget = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    // ---- 创建 QTreeWidget ----
    QTreeWidget* tree = new QTreeWidget(centralWidget);
    tree->setHeaderLabels({"名称", "类型", "描述"});
    tree->setColumnCount(3);
    tree->header()->setStretchLastSection(true);
    tree->setAlternatingRowColors(true);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    // ---- 添加顶层项 ----
    QTreeWidgetItem* group1 = new QTreeWidgetItem(tree);
    group1->setText(0, "航天器");
    group1->setText(1, "组");
    group1->setText(2, "所有航天器对象");
    group1->setFlags(group1->flags() | Qt::ItemIsEditable);

    QTreeWidgetItem* group2 = new QTreeWidgetItem(tree);
    group2->setText(0, "地面设施");
    group2->setText(1, "组");
    group2->setText(2, "所有地面站对象");

    // ---- 添加子项到 group1 ----
    QTreeWidgetItem* sat1 = new QTreeWidgetItem(group1);
    sat1->setText(0, "Sat-001");
    sat1->setText(1, "卫星");
    sat1->setText(2, "低轨观测卫星");
    sat1->setCheckState(0, Qt::Checked);

    QTreeWidgetItem* sat2 = new QTreeWidgetItem(group1);
    sat2->setText(0, "Sat-002");
    sat2->setText(1, "卫星");
    sat2->setText(2, "地球同步轨道通信卫星");
    sat2->setCheckState(0, Qt::Unchecked);

    QTreeWidgetItem* probe = new QTreeWidgetItem(group1);
    probe->setText(0, "Probe-A");
    probe->setText(1, "探测器");
    probe->setText(2, "深空探测器");

    // ---- 添加子项到 group2 ----
    QTreeWidgetItem* station1 = new QTreeWidgetItem(group2);
    station1->setText(0, "北京站");
    station1->setText(1, "测控站");
    station1->setText(2, "位于北京的测控地面站");

    QTreeWidgetItem* station2 = new QTreeWidgetItem(group2);
    station2->setText(0, "喀什站");
    station2->setText(1, "测控站");
    station2->setText(2, "位于喀什的测控地面站");

    // ---- 全部展开 ----
    tree->expandAll();

    // ---- 信号连接 ----
    QObject::connect(tree, &QTreeWidget::itemClicked,
        [](QTreeWidgetItem* item, int column) {
            qDebug() << "点击:" << item->text(0)
                     << "列:" << column
                     << "文本:" << item->text(column);
        });

    QObject::connect(tree, &QTreeWidget::itemChanged,
        [](QTreeWidgetItem* item, int column) {
            qDebug() << "修改:" << item->text(0)
                     << "列:" << column;
        });

    QObject::connect(tree, &QTreeWidget::customContextMenuRequested,
        [tree](const QPoint& pos) {
            QTreeWidgetItem* item = tree->itemAt(pos);
            if (!item) return;

            QMenu menu;
            QAction* expandAct = menu.addAction("展开");
            QAction* collapseAct = menu.addAction("折叠");
            menu.addSeparator();
            QAction* infoAct = menu.addAction("查看信息");

            QAction* chosen = menu.exec(tree->viewport()->mapToGlobal(pos));
            if (chosen == expandAct) {
                tree->expandItem(item);
            } else if (chosen == collapseAct) {
                tree->collapseItem(item);
            } else if (chosen == infoAct) {
                qDebug() << "项信息:"
                         << item->text(0)
                         << item->text(1)
                         << item->text(2)
                         << "子项数:" << item->childCount();
            }
        });

    // ---- 按钮 ----
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* expandBtn = new QPushButton("全部展开", centralWidget);
    QPushButton* collapseBtn = new QPushButton("全部折叠", centralWidget);
    QPushButton* printBtn = new QPushButton("打印选中项", centralWidget);
    QPushButton* addBtn = new QPushButton("添加子项", centralWidget);

    buttonLayout->addWidget(expandBtn);
    buttonLayout->addWidget(collapseBtn);
    buttonLayout->addWidget(printBtn);
    buttonLayout->addWidget(addBtn);
    buttonLayout->addStretch();

    QObject::connect(expandBtn, &QPushButton::clicked, [tree]() {
        tree->expandAll();
    });

    QObject::connect(collapseBtn, &QPushButton::clicked, [tree]() {
        tree->collapseAll();
    });

    QObject::connect(printBtn, &QPushButton::clicked, [tree]() {
        QTreeWidgetItem* sel = tree->currentItem();
        if (sel) {
            qDebug() << "选中项:" << sel->text(0)
                     << sel->text(1) << sel->text(2);
            if (sel->parent()) {
                qDebug() << "  父项:" << sel->parent()->text(0);
            }
        } else {
            qDebug() << "未选中任何项";
        }
    });

    QObject::connect(addBtn, &QPushButton::clicked, [tree]() {
        QTreeWidgetItem* sel = tree->currentItem();
        if (sel) {
            QTreeWidgetItem* child = new QTreeWidgetItem();
            child->setText(0, "新项");
            child->setText(1, "未分类");
            child->setText(2, "新添加的项");
            sel->addChild(child);
            tree->expandItem(sel);
            qDebug() << "已添加子项到:" << sel->text(0);
        } else {
            // 没有选中项时，添加到根
            QTreeWidgetItem* item = new QTreeWidgetItem(tree);
            item->setText(0, "新顶层项");
            item->setText(1, "未分类");
            item->setText(2, "新添加的顶层项");
            qDebug() << "已添加顶层项";
        }
    });

    // ---- 标签 ----
    QLabel* hintLabel = new QLabel("提示: 双击可编辑项文本 | 右键可弹出菜单", centralWidget);
    layout->addWidget(hintLabel);
    layout->addWidget(tree);
    layout->addLayout(buttonLayout);

    window.setCentralWidget(centralWidget);
    window.setWindowTitle("QTreeWidget 基础示例");
    window.resize(600, 500);
    window.show();

    return app.exec();
}
