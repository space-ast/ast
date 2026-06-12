///
/// @file      PilotAgent.cpp
/// @brief     Qt控件生命周期管理、ref映射、快照生成实现
/// @author    axel
/// @date      2026-06-12
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

#include "PilotAgent.hpp"
#include "PilotUtil.hpp"
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QAbstractButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QAbstractSlider>
#include <QAbstractSpinBox>
#include <QTabWidget>
#include <QTabBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QGroupBox>
#include <QLabel>
#include <QAbstractItemView>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QHeaderView>
#include <QRubberBand>
#include <QEvent>
#include <QElapsedTimer>
#include <QTimer>
#include <QtTest/qtest.h>
#include <sstream>
#include <iomanip>
#include <ctime>

AST_NAMESPACE_BEGIN

// #define _AST_DEBUG_UIPILLOT_AGENT

// ============================================================
//  单例 & 构造/析构
// ============================================================

PilotAgent* PilotAgent::s_instance = nullptr;

PilotAgent* PilotAgent::instance() { return s_instance; }

PilotAgent::PilotAgent(QObject* parent)
    : QObject(parent)
{
    s_instance = this;
}

PilotAgent::~PilotAgent()
{
    if (s_instance == this) s_instance = nullptr;
}

// ============================================================
//  ref 管理
// ============================================================

int PilotAgent::ref(QWidget* widget)
{
    if (!widget) return -1;

    auto it = widgetToRef_.find(widget);
    if (it != widgetToRef_.end()) return it.value();

    int id = nextRef_++;
    widgetToRef_[widget] = id;
    refToWidget_[id] = widget;

    connect(widget, &QObject::destroyed, this, [this, widget]() {
        int removedId = widgetToRef_.take(widget);
        refToWidget_.remove(removedId);
    });

    return id;
}

int PilotAgent::ref(QAction* action)
{
    if (!action) return -1;

    auto it = actionToRef_.find(action);
    if (it != actionToRef_.end()) return it.value();

    int id = nextRef_++;
    actionToRef_[action] = id;
    refToAction_[id] = action;

    connect(action, &QObject::destroyed, this, [this, action]() {
        int removedId = actionToRef_.take(action);
        refToAction_.remove(removedId);
    });

    return id;
}

QWidget* PilotAgent::widget(int refId) const
{
    return refToWidget_.value(refId, nullptr);
}

QAction* PilotAgent::action(int refId) const
{
    return refToAction_.value(refId, nullptr);
}

QObject* PilotAgent::object(int refId) const
{
    if (auto* w = widget(refId)) return w;
    if (auto* a = action(refId)) return a;
    return nullptr;
}

std::string PilotAgent::objectType(int refId) const
{
    if (auto* w = widget(refId))
        return w->metaObject()->className();
    if (auto* a = action(refId))
        return "QAction";
    return "unknown";
}

bool PilotAgent::isValidRef(int refId) const
{
    return refToWidget_.contains(refId) || refToAction_.contains(refId);
}

void PilotAgent::cleanupRef(int refId)
{
    if (auto* w = refToWidget_.take(refId))
        widgetToRef_.remove(w);
    if (auto* a = refToAction_.take(refId))
        actionToRef_.remove(a);
}

// ============================================================
//  智能剪枝
// ============================================================

bool PilotAgent::isInteractive(QWidget* w)
{
    if (!w) return false;
    return qobject_cast<QAbstractButton*>(w)
        || qobject_cast<QLineEdit*>(w)
        || qobject_cast<QTextEdit*>(w)
        || qobject_cast<QPlainTextEdit*>(w)
        || qobject_cast<QComboBox*>(w)
        || qobject_cast<QAbstractSlider*>(w)
        || qobject_cast<QAbstractSpinBox*>(w)
        || qobject_cast<QTabWidget*>(w)
        || qobject_cast<QAbstractItemView*>(w)
        || qobject_cast<QMenu*>(w)
        || qobject_cast<QMenuBar*>(w);
}

bool PilotAgent::isWorthReporting(QWidget* w)
{
    if (!w || !w->isVisible()) return false;
    if (w->width() <= 0 || w->height() <= 0) return false;

    // objectName / toolTip / accessibleName → 有语义信息
    if (!w->objectName().isEmpty())       return true;
    if (!w->toolTip().isEmpty())          return true;
    if (!w->accessibleName().isEmpty())   return true;
    if (!w->accessibleDescription().isEmpty()) return true;

    // QLabel 有文本
    if (auto* lbl = qobject_cast<QLabel*>(w))
        return !lbl->text().isEmpty();

    // 有 title 的容器
    if (!w->property("title").toString().isEmpty()) return true;

    // 标准结构控件
    if (qobject_cast<QToolBar*>(w))   return true;
    if (qobject_cast<QStatusBar*>(w)) return true;
    if (qobject_cast<QDialog*>(w))    return true;

    // 可交互控件
    if (isInteractive(w)) return true;

    // QGroupBox
    if (qobject_cast<QGroupBox*>(w)) return true;

    // QTabBar（标签栏）
    if (qobject_cast<QTabBar*>(w)) return true;

    return false;
}

// ============================================================
//  statusTags / friendlyName
// ============================================================

QString PilotAgent::statusTags(QWidget* w)
{
    QStringList tags;

    if (w->hasFocus())                                   tags << "focused";
    if (!w->isEnabled())                                 tags << "disabled";

    if (auto* cb = qobject_cast<QCheckBox*>(w))
        tags << (cb->isChecked() ? "checked" : "unchecked");
    else if (auto* rb = qobject_cast<QRadioButton*>(w))
        tags << (rb->isChecked() ? "selected" : "unselected");

    // QTabWidget 当前选中页
    if (auto* tw = qobject_cast<QTabWidget*>(w))
    {
        // selected 标记在 tab 页的子控件上发出
    }
    if (qobject_cast<QTabBar*>(w))
    {
        // tab bar 本身
    }

    if (auto* combo = qobject_cast<QComboBox*>(w))
    {
        if (combo->isEditable()) tags << "editable";
    }

    if (auto* dlg = qobject_cast<QDialog*>(w))
    {
        if (dlg->isModal()) tags << "modal";
    }

    if (tags.isEmpty()) return QString();
    return QString(" [") + tags.join(", ") + QString("]");
}

QString PilotAgent::friendlyName(QWidget* w)
{
    QString text     = w->property("text").toString();
    QString title    = w->property("title").toString();
    QString winTitle = w->property("windowTitle").toString();
    QString objName  = w->objectName();
    QString accName  = w->accessibleName();

    QStringList parts;

    // 有文本/标题
    QString display;
    if (!text.isEmpty() && text != objName)       display = text;
    else if (!title.isEmpty())                    display = title;
    else if (!winTitle.isEmpty())                 display = winTitle;
    else if (!accName.isEmpty())                  display = accName;

    if (!display.isEmpty())
        parts << QString("\"%1\"").arg(display.left(80));

    // objectName
    if (!objName.isEmpty())
        parts << QString("#%1").arg(objName);

    if (parts.isEmpty()) return QString();
    return parts.join(" ");
}

// ============================================================
//  formatWidgetValue: 输出控件值信息
// ============================================================
void PilotAgent::formatWidgetValue(QWidget* w, std::ostringstream& out)
{
    if (auto* le = qobject_cast<QLineEdit*>(w))
    {
        QString t = le->text();
        if (!t.isEmpty())
            out << " [value=\"" << t.toUtf8().constData() << "\"]";
    }
    else if (auto* spin = qobject_cast<QAbstractSpinBox*>(w))
    {
        out << " [value=\"" << spin->text().toUtf8().constData() << "\"]";
    }
    else if (auto* combo = qobject_cast<QComboBox*>(w))
    {
        out << " [value=\"" << combo->currentText().toUtf8().constData() << "\"]";
        if (combo->count() > 0)
        {
            out << " {";
            int show = qMin(combo->count(), 10);
            for (int i = 0; i < show; i++)
            {
                if (i > 0) out << ", ";
                out << combo->itemText(i).toUtf8().constData();
            }
            if (combo->count() > show) out << ", ...+" << (combo->count() - show);
            out << "}";
        }
    }
    else if (auto* te = qobject_cast<QTextEdit*>(w))
    {
        QString txt = te->toPlainText().left(60);
        if (!txt.isEmpty())
        {
            out << " [value=\"" << txt.toUtf8().constData();
            if (te->toPlainText().length() > 60) out << "...";
            out << "\"]";
        }
    }
    else if (auto* slider = qobject_cast<QAbstractSlider*>(w))
    {
        out << " [value=" << slider->value()
            << ", min=" << slider->minimum()
            << ", max=" << slider->maximum() << "]";
    }
    else if (auto* tv = qobject_cast<QTableView*>(w))
    {
        auto* model = tv->model();
        if (model)
            out << " [" << model->rowCount() << "行×"
                << model->columnCount() << "列]";
    }
    else if (auto* list = qobject_cast<QListView*>(w))
    {
        auto* model = list->model();
        if (model)
            out << " [" << model->rowCount() << "项]";
    }
}

// ============================================================
//  formatWidget / formatAction
// ============================================================

void PilotAgent::formatWidget(QWidget* w, int depth, std::ostringstream& out)
{
    const char* className = w->metaObject()->className();

    // 缩进
    for (int i = 0; i < depth; i++) out << "  ";

    out << "- " << className;

    // 友好名称
    QString name = friendlyName(w);
    if (!name.isEmpty())
        out << " " << name.toUtf8().constData();

    // ref（仅可交互控件或 QMenuBar → 通过其 QMenus 间接交互）
    bool interactive = isInteractive(w);

    if (interactive)
    {
        int r = this->ref(w);
        out << " [ref=e" << r << "]";
    }

    // 控件值
    formatWidgetValue(w, out);

    // 状态标记
    QString tags = statusTags(w);
    if (!tags.isEmpty()) out << tags.toUtf8().constData();

    out << "\n";

    // QMenuBar → 展开 QMenu
    if (auto* mb = qobject_cast<QMenuBar*>(w))
    {
        for (auto* action : mb->actions())
        {
            if (auto* menu = action->menu())
            {
                formatWidget(menu, depth + 1, out);
            }
        }
    }

    // QToolBar → 展开子控件（按钮）
    if (qobject_cast<QToolBar*>(w))
    {
        walkWidgetTree(w, depth + 1, 1, out); // 只展开一层
    }

    // QMenu → 展开 QAction 子项（并分配ref）
    if (auto* menu = qobject_cast<QMenu*>(w))
    {
        for (auto* action : menu->actions())
        {
            formatAction(action, depth + 1, out);

            // 子菜单
            if (auto* subMenu = action->menu())
                formatWidget(subMenu, depth + 2, out);
        }
    }
}

void PilotAgent::formatAction(QAction* a, int depth, std::ostringstream& out)
{
    if (!a) return;
    if (a->isSeparator())
    {
        for (int i = 0; i < depth; i++) out << "  ";
        out << "- separator\n";
        return;
    }

    if (!a->isVisible()) return;

    for (int i = 0; i < depth; i++) out << "  ";

    out << "- QAction";

    if (!a->text().isEmpty())
        out << " \"" << a->text().toUtf8().constData() << "\"";

    int r = this->ref(a);
    out << " [ref=e" << r << "]";

    if (!a->shortcut().isEmpty())
        out << " [" << a->shortcut().toString().toUtf8().constData() << "]";

    if (!a->isEnabled())
        out << " [disabled]";

    if (a->isCheckable() && a->isChecked())
        out << " [checked]";

    out << "\n";
}

// ============================================================
//  walkWidgetTree: 递归遍历控件树
// ============================================================
void PilotAgent::walkWidgetTree(QWidget* root, int depth, int maxDepth,
                                   std::ostringstream& out)
{
    if (!root || depth > maxDepth) return;
    if (!root->isVisible()) return;
    if (root->width() <= 0 || root->height() <= 0) return;

    // QMenuBar / QToolBar / QMenu / QTabWidget 已在 formatWidget 中展开
    // 这里跳过它们，避免重复
    if (qobject_cast<QMenuBar*>(root)) return;
    if (qobject_cast<QMenu*>(root))    return;
    if (qobject_cast<QToolBar*>(root)) return;

    // QTabWidget: 展开每个 tab 页
    if (auto* tw = qobject_cast<QTabWidget*>(root))
    {
        for (int i = 0; i < tw->count(); i++)
        {
            QWidget* tab = tw->widget(i);
            if (!tab || !tab->isVisible()) continue;

            for (int j = 0; j < depth; j++) out << "  ";
            out << "- QWidget \"" << tw->tabText(i).toUtf8().constData() << "\"";
            if (i == tw->currentIndex()) out << " [selected]";
            out << "\n";

            walkWidgetTree(tab, depth + 1, maxDepth, out);
        }
        return; // 不继续遍历子控件
    }

    QList<QWidget*> children = root->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

    for (auto* child : children)
    {
        if (!child->isVisible()) continue;
        if (child->width() <= 0 || child->height() <= 0) continue;

        if (isWorthReporting(child))
        {
            formatWidget(child, depth, out);
            walkWidgetTree(child, depth + 1, maxDepth, out);
        }
        else
        {
            // 跳过纯容器，子控件提升一级
            walkWidgetTree(child, depth, maxDepth, out);
        }
    }
}

// ============================================================
//  snapshot / snapshotOf
// ============================================================

std::string PilotAgent::snapshot(int maxDepth)
{
    // 检测模态对话框——只返回对话框
    QWidget* modal = activeModalDialog();
    if (modal)
        return snapshotOf(modal, maxDepth);

    std::ostringstream out;
    auto tops = QApplication::topLevelWidgets();

    for (auto* top : tops)
    {
        if (!top->isVisible()) continue;

        formatWidget(top, 0, out);
        walkWidgetTree(top, 1, maxDepth, out);
    }

    return out.str();
}

std::string PilotAgent::snapshotOf(QWidget* root, int maxDepth)
{
    if (!root) return "";

    std::ostringstream out;
    formatWidget(root, 0, out);
    walkWidgetTree(root, 1, maxDepth, out);

    return out.str();
}

// ============================================================
//  模态检测、高亮、等待
// ============================================================

QWidget* PilotAgent::activeModalDialog() const
{
    auto tops = QApplication::topLevelWidgets();
    for (auto* w : tops)
    {
        if (w->isVisible() && w->isModal()) return w;
    }
    return nullptr;
}

void PilotAgent::highlight(QWidget* widget, int durationMs)
{
    if (!widget) return;

    auto* rubber = new QRubberBand(QRubberBand::Rectangle, widget);
    QPalette pal;
    pal.setBrush(QPalette::Highlight, QBrush(QColor(255, 0, 0, 127)));
    rubber->setPalette(pal);
    rubber->setGeometry(widget->rect());
    rubber->show();

    QTimer::singleShot(durationMs, rubber, &QObject::deleteLater);
}

void PilotAgent::waitForIdle(int timeoutMs)
{
    #ifdef _AST_DEBUG_UIPILLOT_AGENT
    clock_t start = clock();
    #endif

    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < timeoutMs)
    {
        QApplication::processEvents();
        if (!QApplication::hasPendingEvents()) break;
        QTest::qWait(50);
    }
    
    #ifdef _AST_DEBUG_UIPILLOT_AGENT
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    qDebug() << "waitForIdle: " << duration << "s";
    #endif
}

// ============================================================
//  QAction 操作
// ============================================================

bool PilotAgent::triggerAction(QAction* action)
{
    if (!action || !action->isEnabled()) return false;
    action->trigger();
    return true;
}

QList<QAction*> PilotAgent::expandMenu(QMenu* menu)
{
    if (!menu) return {};

    // 弹出菜单以便获取完整的 action 列表
    // 在实际使用中，hover 菜单后 LLM 会重新 snapshot
    // 这里只是记录子 action
    QList<QAction*> visibleActions;
    for (auto* a : menu->actions())
    {
        if (a->isVisible())
            visibleActions.append(a);
    }
    return visibleActions;
}

// ============================================================
//  eventFilter
// ============================================================

bool PilotAgent::eventFilter(QObject* obj, QEvent* event)
{
    return QObject::eventFilter(obj, event);
}

// ============================================================
//  topLevelWidgets
// ============================================================

QList<QWidget*> PilotAgent::topLevelWidgets() const
{
    return QApplication::topLevelWidgets();
}

AST_NAMESPACE_END
