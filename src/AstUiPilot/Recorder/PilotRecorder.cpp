///
/// @file      PilotRecorder.cpp
/// @brief     界面操作录制器实现
/// @author    axel
/// @date      2026-06-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "PilotRecorder.hpp"
#include "../PilotAgent.hpp"
#include "../PilotSession.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/IO.hpp"
#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QComboBox>
#include <QLineEdit>
#include <QHash>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QAbstractSpinBox>
#include <QDialog>
#include <QAbstractButton>
#include <QMenu>
#include <QElapsedTimer>
#include <QTimer>
#include <sstream>
#include <chrono>

AST_NAMESPACE_BEGIN

// ============================================================
//  构造 / 析构
// ============================================================

PilotRecorder::PilotRecorder(PilotAgent* agent, QObject* parent)
    : QObject(parent)
    , agent_(agent)
{
}

PilotRecorder::~PilotRecorder()
{
    if (recording_) stop();
}

// ============================================================
//  start / stop
// ============================================================

static void connectCombo(QComboBox* combo, PilotRecorder* recorder)
{
    QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        recorder, [recorder, combo](int) { recorder->handleComboChange(combo); });
}

static void connectDialog(QDialog* dlg, PilotRecorder* recorder)
{
    QObject::connect(dlg, &QDialog::finished,
        recorder, [recorder, dlg](int result) { recorder->handleDialogFinished(dlg, result); });
}

static void scanAndConnect(QObject* root, PilotRecorder* recorder)
{
    // 连接 QComboBox
    auto combos = root->findChildren<QComboBox*>();
    for (auto* c : combos) connectCombo(c, recorder);
    // 连接 QDialog
    auto dialogs = root->findChildren<QDialog*>();
    for (auto* d : dialogs) connectDialog(d, recorder);
}

void PilotRecorder::start()
{
    if (recording_) return;

    steps_.clear();
    recording_ = true;
    startTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();

    qApp->installEventFilter(this);

    // 扫描已有控件并连接信号
    for (auto* top : QApplication::topLevelWidgets())
        scanAndConnect(top, this);

    emit recordingStarted();
}

void PilotRecorder::stop()
{
    if (!recording_) return;

    flushPendingEdit();
    recording_ = false;

    qApp->removeEventFilter(this);

    emit recordingStopped();
}

// ============================================================
//  eventFilter 入口
// ============================================================

bool PilotRecorder::eventFilter(QObject* obj, QEvent* event)
{
    if (!recording_) return QObject::eventFilter(obj, event);

    QWidget* w = qobject_cast<QWidget*>(obj);

    switch (event->type())
    {
    case QEvent::MouseButtonPress:
        if (w) handleMousePress(w, static_cast<QMouseEvent*>(event));
        break;

    case QEvent::MouseButtonRelease:
        if (w) handleMouseRelease(w, static_cast<QMouseEvent*>(event));
        break;

    case QEvent::KeyRelease:
        if (w) handleKeyRelease(w, static_cast<QKeyEvent*>(event));
        break;

    case QEvent::FocusIn:
        if (w) handleFocusIn(w, static_cast<QFocusEvent*>(event));
        break;

    case QEvent::FocusOut:
        if (w) handleFocusOut(w, static_cast<QFocusEvent*>(event));
        break;

    default:
        break;
    }

    return QObject::eventFilter(obj, event);
}

// ============================================================
//  handleMousePress
// ============================================================

void PilotRecorder::handleMousePress(QWidget* w, QMouseEvent* e)
{
    // 拖拽：记录起始控件
    if (e->button() == Qt::LeftButton)
    {
        dragSourceWidget_ = w;
        dragSourceText_ = w->property("text").toString();
    }
}

// ============================================================
//  handleMouseRelease
// ============================================================

void PilotRecorder::handleMouseRelease(QWidget* w, QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    // 检查拖拽
    if (dragSourceWidget_ && dragSourceWidget_ != w)
    {
        RecordStep step;
        step.action = ERecordAction::eDrag;
        populateWidgetInfo(step, dragSourceWidget_);
        step.value = w->property("text").toString().toStdString();
        addStep(step);

        dragSourceWidget_ = nullptr;
        return;
    }
    dragSourceWidget_ = nullptr;

    // 跳过已知的编辑控件（由 focusIn/Out 处理）
    if (qobject_cast<QLineEdit*>(w)
        || qobject_cast<QTextEdit*>(w)
        || qobject_cast<QPlainTextEdit*>(w)
        || qobject_cast<QAbstractSpinBox*>(w))
        return;

    // 跳过 QComboBox（由 handleComboChange 处理）
    if (qobject_cast<QComboBox*>(w))
        return;

    // 双击检测
    if (w == lastClickedWidget_ && clickTimer_.elapsed() < 500)
    {
        clickCount_++;
        if (clickCount_ == 2)
        {
            // 移除上一步 click，替换为 dblclick
            if (!steps_.isEmpty() && steps_.last().action == ERecordAction::eClick)
            {
                steps_.removeLast();
            }
            RecordStep step;
            step.action = ERecordAction::eDblClick;
            populateWidgetInfo(step, w);
            addStep(step);
            clickCount_ = 0;
            lastClickedWidget_ = nullptr;
            return;
        }
    }
    else
    {
        clickCount_ = 1;
    }

    lastClickedWidget_ = w;
    clickTimer_.restart();

    // 普通点击
    RecordStep step;
    step.action = ERecordAction::eClick;
    populateWidgetInfo(step, w);
    addStep(step);
}

// ============================================================
//  handleKeyRelease
// ============================================================

void PilotRecorder::handleKeyRelease(QWidget* w, QKeyEvent* e)
{
    // 输入框内不记录键盘（由 focusIn/Out 处理 fill）
    if (qobject_cast<QLineEdit*>(w)
        || qobject_cast<QTextEdit*>(w)
        || qobject_cast<QPlainTextEdit*>(w)
        || qobject_cast<QAbstractSpinBox*>(w))
        return;

    int key = e->key();
    auto mods = e->modifiers();

    // 只记录有意义的按键
    if (key == Qt::Key_Control || key == Qt::Key_Shift
        || key == Qt::Key_Alt || key == Qt::Key_Meta)
        return;

    RecordStep step;

    if (mods != Qt::NoModifier)
    {
        step.action = ERecordAction::eShortcut;
        QString combo;
        if (mods & Qt::ControlModifier) combo += "Ctrl+";
        if (mods & Qt::ShiftModifier)   combo += "Shift+";
        if (mods & Qt::AltModifier)     combo += "Alt+";

        if (key >= Qt::Key_F1 && key <= Qt::Key_F12)
            combo += "F" + QString::number(key - Qt::Key_F1 + 1);
        else if (key >= Qt::Key_A && key <= Qt::Key_Z)
            combo += QChar(key);
        else
            combo += QKeySequence(key).toString();

        step.value = combo.toStdString();
    }
    else
    {
        step.action = ERecordAction::ePressKey;

        if (key == Qt::Key_Return || key == Qt::Key_Enter)
            step.value = "Enter";
        else if (key == Qt::Key_Tab)
            step.value = "Tab";
        else if (key == Qt::Key_Escape)
            step.value = "Escape";
        else if (key == Qt::Key_Backspace)
            step.value = "Backspace";
        else if (key == Qt::Key_Delete)
            step.value = "Delete";
        else if (key >= Qt::Key_F1 && key <= Qt::Key_F12)
            step.value = ("F" + std::to_string(key - Qt::Key_F1 + 1));
        else
            step.value = QKeySequence(key).toString().toStdString();
    }

    addStep(step);
}

// ============================================================
//  handleFocusIn / handleFocusOut
// ============================================================

void PilotRecorder::handleFocusIn(QWidget* w, QFocusEvent* /*e*/)
{
    if (qobject_cast<QLineEdit*>(w)
        || qobject_cast<QTextEdit*>(w)
        || qobject_cast<QPlainTextEdit*>(w)
        || qobject_cast<QAbstractSpinBox*>(w))
    {
        editWidget_ = w;
        editBeforeText_ = w->property("text").toString();
        if (editBeforeText_.isEmpty())
            editBeforeText_ = w->property("plainText").toString();
    }
}

void PilotRecorder::handleFocusOut(QWidget* w, QFocusEvent* /*e*/)
{
    if (w != editWidget_) return;

    QString currentText = w->property("text").toString();
    if (currentText.isEmpty())
        currentText = w->property("plainText").toString();

    // 只有内容变化了才记录
    if (currentText != editBeforeText_)
    {
        RecordStep step;
        step.action = ERecordAction::eFill;
        populateWidgetInfo(step, w);
        step.value = currentText.toUtf8().toStdString();
        addStep(step);
    }

    editWidget_ = nullptr;
}

// ============================================================
//  handleComboChange — 通过信号连接
// ============================================================

void PilotRecorder::handleComboChange(QComboBox* combo)
{
    RecordStep step;
    step.action = ERecordAction::eSelect;
    populateWidgetInfo(step, combo);
    step.value = combo->currentText().toUtf8().toStdString();
    addStep(step);
}

// ============================================================
//  handleDialogFinished — 通过信号连接
// ============================================================

void PilotRecorder::handleDialogFinished(QDialog* dlg, int result)
{
    RecordStep step;
    step.action = (result == QDialog::Accepted)
        ? ERecordAction::eDialogAccept
        : ERecordAction::eDialogDismiss;
    step.widgetText = dlg->windowTitle().toUtf8().toStdString();
    addStep(step);
}

// ============================================================
//  addStep / populateWidgetInfo
// ============================================================

void PilotRecorder::addStep(const RecordStep& step)
{
    RecordStep s = step;

    s.timestampMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count() - startTime_;

    steps_.append(s);

    int index = steps_.size() - 1;
    emit stepRecorded(index, s.toTemplate());
}

void PilotRecorder::populateWidgetInfo(RecordStep& step, QWidget* w)
{
    if (!w) return;

    step.widgetClass = w->metaObject()->className();

    QString text = w->property("text").toString();
    if (!text.isEmpty())
        step.widgetText = text.toUtf8().toStdString();

    if (!w->objectName().isEmpty())
        step.widgetObjName = w->objectName().toUtf8().toStdString();

    if (!w->accessibleName().isEmpty())
        step.widgetAccName = w->accessibleName().toUtf8().toStdString();

    // 记录同级同类控件序号
    QWidget* parent = w->parentWidget();
    if (parent)
    {
        auto siblings = parent->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
        int idx = 0;
        for (auto* sib : siblings)
        {
            if (sib->metaObject()->className() == step.widgetClass)
            {
                if (sib == w) { step.siblingIndex = idx; break; }
                idx++;
            }
        }
    }
}

// ============================================================
//  flushPendingEdit
// ============================================================

void PilotRecorder::flushPendingEdit()
{
    if (editWidget_)
    {
        // 模拟失焦处理
        QString current = editWidget_->property("text").toString();
        if (current != editBeforeText_ && !current.isEmpty())
        {
            RecordStep step;
            step.action = ERecordAction::eFill;
            populateWidgetInfo(step, editWidget_);
            step.value = current.toUtf8().toStdString();
            addStep(step);
        }
        editWidget_ = nullptr;
    }
}

// ============================================================
//  stepAt
// ============================================================

const RecordStep& PilotRecorder::stepAt(int index) const
{
    return steps_.at(index);
}

// ============================================================
//  toJson
// ============================================================

std::string PilotRecorder::toJson() const
{
    JsonValue root;
    root["format"] = "ast-uipilot-script-v1";

    if (qApp)
    {
        JsonValue appInfo;
        appInfo["name"] = qApp->applicationName().toUtf8().toStdString();
        appInfo["qtVersion"] = qVersion();
        root["appInfo"] = appInfo;
    }

    JsonValue stepsArr;
    int id = 1;
    for (const auto& s : steps_)
    {
        JsonValue obj;
        obj["id"] = id++;
        obj["action"] = recordActionToString(s.action);
        if (!s.widgetClass.empty())   obj["widgetClass"] = s.widgetClass;
        if (!s.widgetText.empty())    obj["widgetText"] = s.widgetText;
        if (!s.widgetObjName.empty()) obj["widgetObjName"] = s.widgetObjName;
        if (!s.widgetAccName.empty()) obj["widgetAccName"] = s.widgetAccName;
        if (!s.parentInfo.empty())    obj["parentInfo"] = s.parentInfo;
        if (!s.value.empty())         obj["value"] = s.value;
        if (!s.naturalLanguage.empty()) obj["natural"] = s.naturalLanguage;
        obj["template"] = s.toTemplate();
        obj["timestampMs"] = static_cast<double>(s.timestampMs);
        stepsArr.append(obj);
    }
    root["steps"] = stepsArr;

    return root.toJsonString(2);
}

// ============================================================
//  saveToFile
// ============================================================

bool PilotRecorder::saveToFile(const std::string& path) const
{
    std::string json = toJson();
    FILE* f = ast_fopen(path.c_str(), "wb");
    if (!f) return false;
    fwrite(json.data(), 1, json.size(), f);
    fclose(f);
    return true;
}

// ============================================================
//  polish — LLM 润色
// ============================================================

std::string PilotRecorder::polish(PilotSession* session)
{
    if (steps_.isEmpty()) return "";

    // 收集模板步骤
    std::ostringstream prompt;
    prompt << u8"将以下操作步骤的描述润色为自然流畅的中文，每行以\"→ \"开头：\n\n";
    for (int i = 0; i < steps_.size(); i++)
    {
        prompt << (i + 1) << ". " << steps_[i].toTemplate() << "\n";
    }
    prompt << u8"\n要求：删除控件类名，保留人类可读的描述，不要合并或遗漏步骤。";

    // 调用 LLM
    std::string result = session->execute(prompt.str());

    // 解析结果，回填到步骤中
    std::istringstream iss(result);
    std::string line;
    int idx = 0;
    while (std::getline(iss, line))
    {
        // 跳过非步骤行
        if (line.find("→") == std::string::npos) continue;
        size_t pos = line.find("→");
        std::string nl = line.substr(pos + 1);
        // trim
        while (!nl.empty() && nl.front() == ' ') nl.erase(0, 1);

        if (idx < steps_.size())
        {
            steps_[idx].naturalLanguage = nl;
            idx++;
        }
    }

    return toJson();
}

AST_NAMESPACE_END
