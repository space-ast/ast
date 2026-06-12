///
/// @file      UiPilotSession.cpp
/// @brief     LLM对话管理，系统提示词构建，工具注册实现
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

#include "UiPilotSession.hpp"
#include "UiPilotAgent.hpp"
#include "AstAI/ChatSession.hpp"
#include "AstAI/ChatTools.hpp"
#include "AstAI/ChatTool.hpp"
#include "AstAI/ChatMessages.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/Logger.hpp"
#include "UiPilotUtil.hpp"
#include <QApplication>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QAbstractSlider>
#include <QTabWidget>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QAbstractSpinBox>
#include <QScrollArea>
#include <QAbstractScrollArea>
#include <QtTest/qtest.h>
#include <QImage>
#include <QBuffer>
#include <sstream>

AST_NAMESPACE_BEGIN

#define _AST_DEBUG_UIPILLOT_SESSION



// ============================================================
//  系统提示词
// ============================================================

const char* UiPilotSession::systemPrompt()
{
    return u8R"(
你是一个Qt应用程序操控助手。你可以操控运行中的Qt应用程序。

## 工作流程

1. 首先调用 snapshot() 获取当前界面快照
2. 快照中每个可交互元素都带有 [ref=eN] 标识
3. 使用 ref 来操作目标元素，如 click({ref: 'e10'})
4. ref 在控件生命周期内稳定不变——同一个控件永远是同一个 ref

## 关键原则

- 操作前获取 snapshot —— 界面可能已改变
- 直接使用 snapshot 中的 ref，不要自己猜测或构造定位
- 如果 ref 失效（控件已销毁），重新获取 snapshot
- 遇到模态对话框，使用 dialog_accept / dialog_dismiss
- fill 用于快速替换文本，type 用于需要触发实时响应的输入
- press_key 支持组合键格式：'Ctrl+S'、'Alt+F4'、'Ctrl+Shift+N'
- 使用 wait 等待动画或异步操作完成
- 自绘控件无法识别时，使用 screenshot 获取截图分析，但前提是你具有图片识别的能力
)";
}

std::string UiPilotSession::buildSystemPrompt() const
{
    std::ostringstream out;
    out << systemPrompt();

    // 追加应用信息
    if (qApp)
    {
        out << "\n\n## 当前应用\n";
        out << "应用名: " << qApp->applicationName().toUtf8().constData() << "\n";
        out << "Qt版本: " << qVersion() << "\n";
    }

    return out.str();
}

// ============================================================
//  构造 / 析构
// ============================================================

UiPilotSession::UiPilotSession(UiPilotAgent* agent, QObject* parent)
    : QObject(parent)
    , agent_(agent)
    , chatSession_(nullptr)
{
    chatSession_ = new ChatSession();
    chatSession_->setSystemPrompt(buildSystemPrompt());
    registerTools();
}

UiPilotSession::~UiPilotSession()
{
    delete chatSession_;
}

ChatSession& UiPilotSession::session() { return *chatSession_; }

std::string UiPilotSession::execute(const std::string& command)
{
    // 先获取快照注入上下文
    std::string snap = agent_->snapshot();
    std::ostringstream ctxMsg;
    ctxMsg << "[当前界面快照]\n" << snap << "\n\n[用户指令]\n" << command;

    return session().chat(ctxMsg.str());
}


static void chatCompletion(ChatSession& session, int depth)
{
    auto msg = session.makeChatCompletion();
    if(msg == nullptr)
        return;
    if(msg->hasToolCalls() && depth > 0)
    {
        session.handleToolCalls(*msg);
        if(depth > 0)
        {
            addQueued([&session, depth]() {
                chatCompletion(session, depth - 1);
            });
        }
    }
}

void UiPilotSession::chat(const std::string& message, int maxIterForToolCalls)
{
    auto& session = this->session();
    session.messages().addUserMessage(message);
    chatCompletion(session, maxIterForToolCalls);
}

// ============================================================
//  宏: 便捷注册工具
// ============================================================

namespace {



// 从 arguments 中提取 ref 并解析为 QObject*
static bool hasKey(const JsonValue& obj, const std::string& key)
{
    return !obj[key].isNull();
}

QObject* resolveRef(const JsonValue& args, UiPilotAgent* agent, std::string& err)
{
    if (!hasKey(args, "ref"))
    {
        err = "缺少必需参数 ref";
        return nullptr;
    }
    std::string refStr = args["ref"].toString();
    // 去除可能的 "e" 前缀
    if (!refStr.empty() && (refStr[0] == 'e' || refStr[0] == 'E'))
        refStr = refStr.substr(1);

    int refId = std::stoi(refStr);
    QObject* obj = agent->object(refId);
    if (!obj)
    {
        err = "ref无效: 控件可能已销毁";
        return nullptr;
    }
    return obj;
}

// 操作后自动获取 snapshot 并拼接返回
std::string appendSnapshot(UiPilotAgent* agent, const std::string& result)
{
    if (!agent->isAutoSnapshot())
        return result;

    std::string snap = agent->snapshot();
    return result + "\n" + snap;
}

} // anonymous

// ============================================================
//  Tool: snapshot
// ============================================================

static JsonValue makeSnapshotSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue maxDepth;
    maxDepth["type"] = "integer";
    maxDepth["description"] = "最大遍历深度，默认8。越小越省token";
    maxDepth["default"] = 8;
    props["maxDepth"] = maxDepth;
    s["properties"] = props;
    return s;
}

static std::string toolSnapshot(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    int maxDepth = 8;
    if (!args["maxDepth"].isNull())
        maxDepth = args["maxDepth"].toInt();

    return agent->snapshot(maxDepth);
}

// ============================================================
//  Tool: click
// ============================================================

static JsonValue makeClickSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "来自snapshot的元素引用，如'e10'";
    props["ref"] = ref;
    JsonValue button;
    button["type"] = "string";
    button["enum"] = JsonValue::FromString("[\"left\",\"right\",\"middle\"]");
    button["description"] = "鼠标按键。默认left";
    button["default"] = "left";
    props["button"] = button;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    s["required"] = required;
    return s;
}

static std::string toolClick(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    QWidget* w = qobject_cast<QWidget*>(obj);
    if (!w) return "ref对应的对象不是QWidget";

    // 确定鼠标按键
    Qt::MouseButton btn = Qt::LeftButton;
    if (!args["button"].isNull())
    {
        std::string b = args["button"].toString();
        if (b == "right")  btn = Qt::RightButton;
        if (b == "middle") btn = Qt::MiddleButton;
    }

    QPoint pt = w->rect().center();
    addQueued([w, btn, pt]() {
        QTest::mouseClick(w, btn, Qt::NoModifier, pt);
    });

    return appendSnapshot(agent,
        std::string("✓ clicked ") + w->metaObject()->className()
        + " \"" + w->property("text").toString().toUtf8().constData() + "\"");
}

// ============================================================
//  Tool: dblclick
// ============================================================

static JsonValue makeDblClickSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "来自snapshot的元素引用";
    props["ref"] = ref;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    s["required"] = required;
    return s;
}

static std::string toolDblClick(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    QWidget* w = qobject_cast<QWidget*>(obj);
    if (!w) return "ref对应的对象不是QWidget";

    QPoint pt = w->rect().center();
    addQueued([w, pt]() {
        QTest::mouseDClick(w, Qt::LeftButton, Qt::NoModifier, pt);
    });

    return appendSnapshot(agent, "✓ double-clicked");
}

// ============================================================
//  Tool: hover
// ============================================================

static JsonValue makeHoverSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "来自snapshot的元素引用";
    props["ref"] = ref;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    s["required"] = required;
    return s;
}

static std::string toolHover(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    // 检查是否是 QMenu
    if (auto* menu = qobject_cast<QMenu*>(obj))
    {
        // 展开菜单
        QPoint pt = menu->parentWidget() ?
            menu->parentWidget()->mapToGlobal(menu->pos()) : QPoint(0,0);
        QTest::mouseMove(menu);

        return appendSnapshot(agent,
            std::string("✓ hovered QMenu \"") +
            menu->title().toUtf8().constData() + "\"");
    }

    QWidget* w = qobject_cast<QWidget*>(obj);
    if (!w) return "ref对应的对象不是QWidget";

    addQueued([w]() {
        QTest::mouseMove(w, w->rect().center());
    });

    return appendSnapshot(agent, "✓ hovered");
}

// ============================================================
//  Tool: fill
// ============================================================

static JsonValue makeFillSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "输入框的ref引用";
    props["ref"] = ref;
    JsonValue text;
    text["type"] = "string";
    text["description"] = "要填入的文本";
    props["text"] = text;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    required.append("text");
    s["required"] = required;
    return s;
}

static std::string toolFill(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    std::string text = args["text"].toString();
    QString qtext = QString::fromUtf8(text.c_str());

    if (auto* le = qobject_cast<QLineEdit*>(obj))
    {
        le->setText(qtext);
    }
    else if (auto* te = qobject_cast<QTextEdit*>(obj))
    {
        te->setText(qtext);
    }
    else if (auto* pe = qobject_cast<QPlainTextEdit*>(obj))
    {
        pe->setPlainText(qtext);
    }
    else if (auto* spin = qobject_cast<QAbstractSpinBox*>(obj))
    {
        // lineEdit() is protected — use key clicks on the focused spinbox
        spin->setFocus();
        spin->selectAll();
        spin->setFocus();
        addQueued([spin, qtext]() {
            QTest::keyClicks(spin, qtext);
        });
    }
    else if (auto* combo = qobject_cast<QComboBox*>(obj))
    {
        if (combo->isEditable())
            combo->setEditText(qtext);
        else
            return "QComboBox不可编辑，请使用 select_option";
    }
    else
    {
        return "fill不支持此控件类型，请使用 type";
    }

    return appendSnapshot(agent, "✓ filled \"" + text + "\"");
}

// ============================================================
//  Tool: type
// ============================================================

static JsonValue makeTypeSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "输入框的ref引用";
    props["ref"] = ref;
    JsonValue text;
    text["type"] = "string";
    text["description"] = "要输入的文本";
    props["text"] = text;
    JsonValue delay;
    delay["type"] = "integer";
    delay["description"] = "每个字符间隔(毫秒)，默认30";
    delay["default"] = 30;
    props["delay"] = delay;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    required.append("text");
    s["required"] = required;
    return s;
}

static std::string toolType(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    QWidget* w = qobject_cast<QWidget*>(obj);
    if (!w) return "ref对应的对象不是QWidget";

    std::string text = args["text"].toString();
    int delay = !args["delay"].isNull() ? args["delay"].toInt() : 30;

    w->setFocus();
    addQueued([w, text, delay]() {
        QTest::keyClicks(w, QString::fromUtf8(text.c_str()), Qt::NoModifier, delay);
    });

    return appendSnapshot(agent, "✓ typed \"" + text + "\"");
}

// ============================================================
//  Tool: press_key
// ============================================================

static JsonValue makePressKeySchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue key;
    key["type"] = "string";
    key["description"] = "按键名称。如'Enter'、'Ctrl+S'、'ArrowDown'、'F5'";
    props["key"] = key;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "要先聚焦的元素的ref。不指定则向当前焦点发送";
    ref["default"] = "";
    props["ref"] = ref;
    s["properties"] = props;
    JsonValue required;
    required.append("key");
    s["required"] = required;
    return s;
}

static Qt::Key parseKey(const std::string& keyStr)
{
    if (keyStr == "Enter" || keyStr == "Return") return Qt::Key_Enter;
    if (keyStr == "Tab")       return Qt::Key_Tab;
    if (keyStr == "Escape")    return Qt::Key_Escape;
    if (keyStr == "Backspace") return Qt::Key_Backspace;
    if (keyStr == "Delete")    return Qt::Key_Delete;
    if (keyStr == "Space")     return Qt::Key_Space;
    if (keyStr == "Home")      return Qt::Key_Home;
    if (keyStr == "End")       return Qt::Key_End;
    if (keyStr == "PageUp")    return Qt::Key_PageUp;
    if (keyStr == "PageDown")  return Qt::Key_PageDown;
    if (keyStr == "ArrowUp" || keyStr == "Up")       return Qt::Key_Up;
    if (keyStr == "ArrowDown" || keyStr == "Down")   return Qt::Key_Down;
    if (keyStr == "ArrowLeft" || keyStr == "Left")   return Qt::Key_Left;
    if (keyStr == "ArrowRight" || keyStr == "Right") return Qt::Key_Right;

    // F1-F12
    for (int i = 1; i <= 12; i++)
    {
        if (keyStr == ("F" + std::to_string(i)))
            return static_cast<Qt::Key>(Qt::Key_F1 + i - 1);
    }

    // 字母/数字
    if (keyStr.length() == 1)
    {
        char c = keyStr[0];
        if (c >= 'a' && c <= 'z') return static_cast<Qt::Key>(Qt::Key_A + (c - 'a'));
        if (c >= 'A' && c <= 'Z') return static_cast<Qt::Key>(Qt::Key_A + (c - 'A'));
        if (c >= '0' && c <= '9') return static_cast<Qt::Key>(Qt::Key_0 + (c - '0'));
    }

    return Qt::Key_unknown;
}

static std::string toolPressKey(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string keyStr = args["key"].toString();

    // 如果指定了 ref，先聚焦
    if (!args["ref"].isNull() && !args["ref"].toString().empty())
    {
        std::string err;
        QObject* obj = resolveRef(args, agent, err);
        if (!obj) return err;
        QWidget* w = qobject_cast<QWidget*>(obj);
        if (w) w->setFocus();
    }

    // 解析组合键
    Qt::KeyboardModifiers mods = Qt::NoModifier;
    std::string mainKey = keyStr;

    auto parseModifiers = [&]() {
        if (mainKey.find("Ctrl+") != std::string::npos ||
            mainKey.find("ctrl+") != std::string::npos)
        {
            mods |= Qt::ControlModifier;
            size_t pos = mainKey.find("Ctrl+");
            if (pos == std::string::npos) pos = mainKey.find("ctrl+");
            mainKey = mainKey.substr(pos + 5);
        }
        if (mainKey.find("Shift+") != std::string::npos ||
            mainKey.find("shift+") != std::string::npos)
        {
            mods |= Qt::ShiftModifier;
            size_t pos = mainKey.find("Shift+");
            if (pos == std::string::npos) pos = mainKey.find("shift+");
            mainKey = mainKey.substr(pos + 6);
        }
        if (mainKey.find("Alt+") != std::string::npos ||
            mainKey.find("alt+") != std::string::npos)
        {
            mods |= Qt::AltModifier;
            size_t pos = mainKey.find("Alt+");
            if (pos == std::string::npos) pos = mainKey.find("alt+");
            mainKey = mainKey.substr(pos + 4);
        }
        if (mainKey.find("Meta+") != std::string::npos ||
            mainKey.find("meta+") != std::string::npos)
        {
            mods |= Qt::MetaModifier;
            size_t pos = mainKey.find("Meta+");
            if (pos == std::string::npos) pos = mainKey.find("meta+");
            mainKey = mainKey.substr(pos + 5);
        }
    };
    parseModifiers();

    Qt::Key key = parseKey(mainKey);

    QWidget* focusWidget = QApplication::focusWidget();
    if (!focusWidget)
        focusWidget = QApplication::activeWindow();

    if (focusWidget)
    {
        addQueued([focusWidget, key, mods]() {
            if (mods != Qt::NoModifier)
                QTest::keyClick(focusWidget, key, mods);
            else
                QTest::keyClick(focusWidget, key);
        });
    }

    return appendSnapshot(agent, "✓ pressed " + keyStr);
}

// ============================================================
//  Tool: select_option
// ============================================================

static JsonValue makeSelectOptionSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "QComboBox的ref引用";
    props["ref"] = ref;
    JsonValue values;
    values["type"] = "array";
    values["items"]["type"] = "string";
    values["description"] = "要选择的选项文本列表";
    props["values"] = values;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    required.append("values");
    s["required"] = required;
    return s;
}

static std::string toolSelectOption(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    QComboBox* combo = qobject_cast<QComboBox*>(obj);
    if (!combo) return "ref对应的对象不是QComboBox";

    auto vals = args["values"].getArray();
    std::string result;

    for (auto& val : vals)
    {
        std::string v = val.toString();
        int idx = combo->findText(QString::fromUtf8(v.c_str()), Qt::MatchContains);
        if (idx >= 0)
        {
            combo->setCurrentIndex(idx);
            result += "✓ selected \"" + v + "\"\n";
        }
        else
        {
            result += "✗ 未找到选项: \"" + v + "\"\n";
        }
    }

    return appendSnapshot(agent, result);
}

// ============================================================
//  Tool: drag
// ============================================================

static JsonValue makeDragSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue startRef;
    startRef["type"] = "string";
    startRef["description"] = "源元素ref";
    props["startRef"] = startRef;
    JsonValue endRef;
    endRef["type"] = "string";
    endRef["description"] = "目标元素ref";
    props["endRef"] = endRef;
    s["properties"] = props;
    JsonValue required;
    required.append("startRef");
    required.append("endRef");
    s["required"] = required;
    return s;
}

static std::string toolDrag(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* srcObj = resolveRef(args, agent, err);
    if (!srcObj) return err;

    // 需要独立的 startRef 参数
    std::string startRefStr = args["startRef"].toString();
    if (startRefStr[0] == 'e') startRefStr = startRefStr.substr(1);
    std::string endRefStr = args["endRef"].toString();
    if (endRefStr[0] == 'e') endRefStr = endRefStr.substr(1);

    QWidget* srcW = agent->widget(std::stoi(startRefStr));
    QWidget* dstW = agent->widget(std::stoi(endRefStr));

    if (!srcW) return "源元素无效";
    if (!dstW) return "目标元素无效";

    QPoint srcPt = srcW->mapToGlobal(srcW->rect().center());
    QPoint dstPt = dstW->mapToGlobal(dstW->rect().center());

    QTest::mousePress(srcW, Qt::LeftButton, Qt::NoModifier, srcW->rect().center());
    QTest::mouseMove(dstW, dstW->mapFromGlobal(srcPt));
    QTest::mouseMove(dstW, dstW->rect().center());
    QTest::mouseRelease(dstW, Qt::LeftButton, Qt::NoModifier, dstW->rect().center());

    return appendSnapshot(agent, "✓ dragged");
}

// ============================================================
//  Tool: screenshot
// ============================================================

static JsonValue makeScreenshotSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "要截图的元素ref。不指定则截取整个窗口";
    ref["default"] = "";
    props["ref"] = ref;
    s["properties"] = props;
    return s;
}

static std::string toolScreenshot(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    QWidget* target = nullptr;
    if (!args["ref"].isNull() && !args["ref"].toString().empty())
    {
        std::string err;
        QObject* obj = resolveRef(args, agent, err);
        target = qobject_cast<QWidget*>(obj);
    }

    if (!target)
        target = QApplication::activeWindow();

    if (!target)
        return "✗ 没有可截图的窗口";

    QImage img = target->grab().toImage();
    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "PNG");

    std::string b64 = ba.toBase64().toStdString();
    return "[image:base64," + b64 + "]";
}

// ============================================================
//  Tool: wait
// ============================================================

static JsonValue makeWaitSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ms;
    ms["type"] = "integer";
    ms["description"] = "等待毫秒数，最大30000";
    ms["minimum"] = 10;
    ms["maximum"] = 30000;
    props["ms"] = ms;
    s["properties"] = props;
    JsonValue required;
    required.append("ms");
    s["required"] = required;
    return s;
}

static std::string toolWait(const JsonValue& args)
{
    int ms = args["ms"].toInt();
    if (ms <= 0) ms = 1000;
    if (ms > 30000) ms = 30000;

    QTest::qWait(ms);
    return "✓ waited " + std::to_string(ms) + "ms";
}

// ============================================================
//  Tool: dialog_accept
// ============================================================

static JsonValue makeDialogAcceptSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue text;
    text["type"] = "string";
    text["description"] = "接受按钮文本。不指定则使用对话框默认按钮";
    text["default"] = "";
    props["text"] = text;
    s["properties"] = props;
    return s;
}

static std::string toolDialogAccept(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    QWidget* modal = agent->activeModalDialog();
    if (!modal) return "✗ 没有活动的模态对话框";

    QDialog* dlg = qobject_cast<QDialog*>(modal);
    if (!dlg) return "✗ 当前模态窗口不是QDialog";

    // 如果指定了按钮文本，查找并点击
    if (!args["text"].isNull() && !args["text"].toString().empty())
    {
        std::string btnText = args["text"].toString();
        auto buttons = dlg->findChildren<QPushButton*>();
        for (auto* btn : buttons)
        {
            if (btn->text().toStdString().find(btnText) != std::string::npos)
            {
                QTest::mouseClick(btn, Qt::LeftButton, Qt::NoModifier, btn->rect().center());
                return appendSnapshot(agent, "✓ 已点击\"" + btnText + "\"");
            }
        }
    }

    dlg->accept();
    return appendSnapshot(agent, "✓ 已接受对话框");
}

// ============================================================
//  Tool: dialog_dismiss
// ============================================================

static JsonValue makeDialogDismissSchema()
{
    JsonValue s;
    s["type"] = "object";
    // 无参数工具，不需要properties
    return s;
}

static std::string toolDialogDismiss(const JsonValue& /*args*/)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    QWidget* modal = agent->activeModalDialog();
    if (!modal) return "✗ 没有活动的模态对话框";

    QDialog* dlg = qobject_cast<QDialog*>(modal);
    if (!dlg) return "✗ 当前模态窗口不是QDialog";

    dlg->reject();
    return appendSnapshot(agent, "✓ 已关闭对话框");
}

// ============================================================
//  Tool: get_property
// ============================================================

static JsonValue makeGetPropertySchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "元素ref引用";
    props["ref"] = ref;
    JsonValue prop;
    prop["type"] = "string";
    prop["description"] = "属性名。'*'返回所有可用属性";
    props["property"] = prop;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    required.append("property");
    s["required"] = required;
    return s;
}

static std::string toolGetProperty(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    std::string propName = args["property"].toString();

    if (propName == "*")
    {
        // 返回所有属性
        const QMetaObject* meta = obj->metaObject();
        std::ostringstream out;
        out << "{\n";
        for (int i = 0; i < meta->propertyCount(); i++)
        {
            auto prop = meta->property(i);
            QVariant val = prop.read(obj);
            out << "  \"" << prop.name() << "\": "
                << val.toString().toStdString() << ",\n";
        }
        out << "}";
        return out.str();
    }

    QVariant val = obj->property(propName.c_str());
    return val.toString().toStdString();
}

// ============================================================
//  Tool: invoke
// ============================================================

static JsonValue makeInvokeSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "元素ref引用";
    props["ref"] = ref;
    JsonValue method;
    method["type"] = "string";
    method["description"] = "要调用的方法名";
    props["method"] = method;
    JsonValue argsProp;
    argsProp["type"] = "array";
    argsProp["description"] = "方法参数列表";
    argsProp["default"] = JsonValue(std::vector<JsonValue>{});
    props["args"] = argsProp;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    required.append("method");
    s["required"] = required;
    return s;
}

static std::string toolInvoke(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    std::string method = args["method"].toString();

    // 尝试调用
    QMetaObject::invokeMethod(obj, method.c_str(), Qt::DirectConnection);

    return appendSnapshot(agent, "✓ invoked " + method);
}

// ============================================================
//  Tool: table_get
// ============================================================

static JsonValue makeTableGetSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "QTableView/QTableWidget的ref引用";
    props["ref"] = ref;
    JsonValue startRow;
    startRow["type"] = "integer";
    startRow["description"] = "起始行(从0开始)，默认0";
    startRow["default"] = 0;
    props["startRow"] = startRow;
    JsonValue rowCount;
    rowCount["type"] = "integer";
    rowCount["description"] = "获取行数，默认20，-1=全部";
    rowCount["default"] = 20;
    props["rowCount"] = rowCount;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    s["required"] = required;
    return s;
}

static std::string toolTableGet(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj);
    if (!view) return "ref对应的对象不是表格视图";

    auto* model = view->model();
    if (!model) return "表格没有数据模型";

    int startRow = !args["startRow"].isNull() ? args["startRow"].toInt() : 0;
    int rowCnt   = !args["rowCount"].isNull() ? args["rowCount"].toInt() : 20;
    int totalRows = model->rowCount();
    int totalCols = model->columnCount();

    if (rowCnt == -1 || rowCnt > totalRows - startRow)
        rowCnt = totalRows - startRow;

    std::ostringstream out;

    // 表头
    out << "| # |";
    for (int c = 0; c < totalCols; c++)
    {
        out << " " << model->headerData(c, Qt::Horizontal).toString().toUtf8().constData() << " |";
    }
    out << "\n";

    // 分隔线
    out << "|---|";
    for (int c = 0; c < totalCols; c++) out << "---|";
    out << "\n";

    // 数据行
    for (int r = startRow; r < startRow + rowCnt; r++)
    {
        out << "| " << r << " |";
        for (int c = 0; c < totalCols; c++)
        {
            auto idx = model->index(r, c);
            out << " " << model->data(idx).toString().toUtf8().constData() << " |";
        }
        out << "\n";
    }

    if (totalRows > startRow + rowCnt)
        out << "| ... | ... 共" << totalRows << "行，仅显示" << rowCnt << "行 |\n";

    return out.str();
}

// ============================================================
//  Tool: table_click
// ============================================================

static JsonValue makeTableClickSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "QTableView/QTableWidget的ref引用";
    props["ref"] = ref;
    JsonValue row;
    row["type"] = "integer";
    row["description"] = "行号(从0开始)";
    props["row"] = row;
    JsonValue column;
    column["type"] = "integer";
    column["description"] = "列号(从0开始)";
    props["column"] = column;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    required.append("row");
    required.append("column");
    s["required"] = required;
    return s;
}

static std::string toolTableClick(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj);
    if (!view) return "ref对应的对象不是表格视图";

    int row = args["row"].toInt();
    int col = args["column"].toInt();
    auto idx = view->model()->index(row, col);
    QRect rect = view->visualRect(idx);
    QPoint pt = rect.center();

    QTest::mouseClick(view->viewport(), Qt::LeftButton, Qt::NoModifier, pt);

    return appendSnapshot(agent,
        "✓ clicked cell (" + std::to_string(row) + "," + std::to_string(col) + ")");
}

// ============================================================
//  Tool: tree_get
// ============================================================

static JsonValue makeTreeGetSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue ref;
    ref["type"] = "string";
    ref["description"] = "QTreeView/QTreeWidget的ref引用";
    props["ref"] = ref;
    JsonValue depth;
    depth["type"] = "integer";
    depth["description"] = "最大深度，默认4";
    depth["default"] = 4;
    props["depth"] = depth;
    s["properties"] = props;
    JsonValue required;
    required.append("ref");
    s["required"] = required;
    return s;
}

static std::string toolTreeGet(const JsonValue& args)
{
    auto* agent = UiPilotAgent::instance();
    if (!agent) return "Agent未初始化";

    std::string err;
    QObject* obj = resolveRef(args, agent, err);
    if (!obj) return err;

    QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj);
    if (!view) return "ref对应的对象不是树形视图";

    auto* model = view->model();
    if (!model) return "树形视图没有数据模型";

    int maxDepth = !args["depth"].isNull() ? args["depth"].toInt() : 4;

    std::ostringstream out;
    // 递归遍历树
    std::function<void(const QModelIndex&, int)> walkTree;
    walkTree = [&](const QModelIndex& idx, int d) {
        if (d > maxDepth) return;
        int rowCount = model->rowCount(idx);
        for (int r = 0; r < rowCount; r++)
        {
            auto child = model->index(r, 0, idx);
            for (int i = 0; i < d; i++) out << "  ";
            out << "- " << model->data(child).toString().toUtf8().constData() << "\n";
            if (model->hasChildren(child))
                walkTree(child, d + 1);
        }
    };

    walkTree(QModelIndex(), 0);
    return out.str();
}

// ============================================================
//  Tool: resize
// ============================================================

static JsonValue makeResizeSchema()
{
    JsonValue s;
    s["type"] = "object";
    JsonValue props;
    JsonValue width;
    width["type"] = "integer";
    width["description"] = "宽度(像素)";
    props["width"] = width;
    JsonValue height;
    height["type"] = "integer";
    height["description"] = "高度(像素)";
    props["height"] = height;
    s["properties"] = props;
    JsonValue required;
    required.append("width");
    required.append("height");
    s["required"] = required;
    return s;
}

static std::string toolResize(const JsonValue& args)
{
    int w = args["width"].toInt();
    int h = args["height"].toInt();

    QWidget* win = QApplication::activeWindow();
    if (win)
        win->resize(w, h);

    auto* agent = UiPilotAgent::instance();

    return appendSnapshot(agent,
        "✓ resized to " + std::to_string(w) + "×" + std::to_string(h));
}

// ============================================================
//  registerTools: 注册所有 19 个工具
// ============================================================

void UiPilotSession::registerTools()
{
    auto& tools = session().tools();
    tools.clearTools();

    // 1. snapshot
    {
        auto* t = tools.addTool(toolSnapshot);
        t->setName("snapshot");
        t->setDescription(u8"获取当前Qt界面的结构化快照。返回控件树文本，每个可交互元素带有唯一[ref=eN]标识。ref在控件生命周期内稳定不变。存在模态对话框时仅返回对话框内容。");
        t->setParameters(makeSnapshotSchema());
    }
    // 2. click
    {
        auto* t = tools.addTool(toolClick);
        t->setName("click");
        t->setDescription(u8"点击指定元素。支持左键(left)、右键(right)、中键(middle)。点击后自动返回新的snapshot。");
        t->setParameters(makeClickSchema());
    }
    // 3. dblclick
    {
        auto* t = tools.addTool(toolDblClick);
        t->setName("dblclick");
        t->setDescription(u8"双击指定元素。双击后自动返回新的snapshot。");
        t->setParameters(makeDblClickSchema());
    }
    // 4. hover
    {
        auto* t = tools.addTool(toolHover);
        t->setName("hover");
        t->setDescription(u8"鼠标悬停在元素上，用于触发tooltip、下拉菜单或悬停效果。悬停后自动返回新的snapshot。悬停QMenu会展开其子项。");
        t->setParameters(makeHoverSchema());
    }
    // 5. fill
    {
        auto* t = tools.addTool(toolFill);
        t->setName("fill");
        t->setDescription(u8"替换输入框的全部文本（清空后填入）。支持QLineEdit、QTextEdit、QPlainTextEdit、QAbstractSpinBox、可编辑的QComboBox。与type的区别：fill瞬间完成(触发textChanged)，type逐字符输入(触发textEdited)。填写后自动返回新的snapshot。");
        t->setParameters(makeFillSchema());
    }
    // 6. type
    {
        auto* t = tools.addTool(toolType);
        t->setName("type");
        t->setDescription(u8"逐字符输入文本，模拟真实键盘输入。适用于需要实时响应输入的场景。输入后自动返回新的snapshot。");
        t->setParameters(makeTypeSchema());
    }
    // 7. press_key
    {
        auto* t = tools.addTool(toolPressKey);
        t->setName("press_key");
        t->setDescription(u8"向焦点控件发送键盘按键。支持功能键(Enter, Tab, Escape...)、方向键(ArrowUp, ArrowDown...)、组合键(Ctrl+S, Alt+F4...)。按键后自动返回新的snapshot。");
        t->setParameters(makePressKeySchema());
    }
    // 8. select_option
    {
        auto* t = tools.addTool(toolSelectOption);
        t->setName("select_option");
        t->setDescription(u8"在QComboBox中选择指定选项。根据选项文本匹配。选择后自动返回新的snapshot。");
        t->setParameters(makeSelectOptionSchema());
    }
    // 9. drag
    {
        auto* t = tools.addTool(toolDrag);
        t->setName("drag");
        t->setDescription(u8"将源元素拖拽到目标元素。拖放后自动返回新的snapshot。");
        t->setParameters(makeDragSchema());
    }
    // 10. screenshot
    {
        auto* t = tools.addTool(toolScreenshot);
        t->setName("screenshot");
        t->setDescription(u8"对指定元素或窗口截图，返回Base64编码的PNG。用于视觉确认、自绘控件分析、测试证据。不自动返回snapshot，需要时请手动调用，但前提是你具有图片识别的能力。");
        t->setParameters(makeScreenshotSchema());
    }
    // 11. wait
    {
        auto* t = tools.addTool(toolWait);
        t->setName("wait");
        t->setDescription(u8"等待指定毫秒数。用于动画、异步操作、界面更新。完成后不自动返回snapshot。");
        t->setParameters(makeWaitSchema());
    }
    // 12. dialog_accept
    {
        auto* t = tools.addTool(toolDialogAccept);
        t->setName("dialog_accept");
        t->setDescription(u8"接受当前活动的模态对话框（点击确定按钮）。操作后自动返回对话框关闭后的snapshot。");
        t->setParameters(makeDialogAcceptSchema());
    }
    // 13. dialog_dismiss
    {
        auto* t = tools.addTool(toolDialogDismiss);
        t->setName("dialog_dismiss");
        t->setDescription(u8"关闭当前活动的模态对话框（点击取消或关闭按钮）。操作后自动返回snapshot。");
        t->setParameters(makeDialogDismissSchema());
    }
    // 14. get_property
    {
        auto* t = tools.addTool(toolGetProperty);
        t->setName("get_property");
        t->setDescription(u8"获取指定元素的属性值。常用属性：text, enabled, visible, checked, currentIndex, currentText, value, minimum, maximum, rect, toolTip。property='*'返回全部。");
        t->setParameters(makeGetPropertySchema());
    }
    // 15. invoke
    {
        auto* t = tools.addTool(toolInvoke);
        t->setName("invoke");
        t->setDescription(u8"通过Qt元对象系统调用元素的Q_INVOKABLE方法。用于标准工具未覆盖的操作。调用后自动返回新的snapshot。");
        t->setParameters(makeInvokeSchema());
    }
    // 16. table_get
    {
        auto* t = tools.addTool(toolTableGet);
        t->setName("table_get");
        t->setDescription(u8"获取QTableView/QTableWidget的数据，返回Markdown格式表格。");
        t->setParameters(makeTableGetSchema());
    }
    // 17. table_click
    {
        auto* t = tools.addTool(toolTableClick);
        t->setName("table_click");
        t->setDescription(u8"点击表格中指定行列的单元格。点击后自动返回新的snapshot。");
        t->setParameters(makeTableClickSchema());
    }
    // 18. tree_get
    {
        auto* t = tools.addTool(toolTreeGet);
        t->setName("tree_get");
        t->setDescription(u8"获取QTreeView/QTreeWidget的树形结构数据。");
        t->setParameters(makeTreeGetSchema());
    }
    // 19. resize
    {
        auto* t = tools.addTool(toolResize);
        t->setName("resize");
        t->setDescription(u8"调整应用程序窗口大小。调整后自动返回新的snapshot。");
        t->setParameters(makeResizeSchema());
    }
}

AST_NAMESPACE_END
