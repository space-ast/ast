///
/// @file      UiPilotConsole.cpp
/// @brief     内嵌控制台控件实现
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

#include "UiPilotConsole.hpp"
#include "PilotCommander.hpp"
#include "PilotAgent.hpp"
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QDockWidget>
#include <QApplication>
#include <QKeyEvent>
#include <QFont>
#include <QScrollBar>

AST_NAMESPACE_BEGIN

// ============================================================
//  构造 / 析构
// ============================================================

UiPilotConsole::UiPilotConsole(PilotCommander* commander, QWidget* parent)
    : QWidget(parent)
    , commander_(commander)
    , outputView_(nullptr)
    , inputLine_(nullptr)
{
    setupUi();

    // 监听 commander 输出信号
    if (commander_)
    {
        connect(commander_, &PilotCommander::outputReady,
                this, &UiPilotConsole::onCommanderOutput);
    }
}

UiPilotConsole::~UiPilotConsole()
{
}

// ============================================================
//  setupUi
// ============================================================

void UiPilotConsole::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    // ---- 输出区域 ----
    outputView_ = new QTextEdit(this);
    outputView_->setReadOnly(true);
    outputView_->setFont(QFont("Consolas, Monaco, monospace", 10));
    outputView_->setStyleSheet(
        "QTextEdit {"
        "  background-color: #1e1e1e;"
        "  color: #d4d4d4;"
        "  border: 1px solid #3c3c3c;"
        "  border-radius: 2px;"
        "}"
    );
    outputView_->setMinimumHeight(80);
    outputView_->setMaximumHeight(250);

    // ---- 输入区域 ----
    inputLine_ = new QLineEdit(this);
    inputLine_->setFont(QFont("Consolas, Monaco, monospace", 10));
    inputLine_->setStyleSheet(
        "QLineEdit {"
        "  background-color: #2d2d2d;"
        "  color: #d4d4d4;"
        "  border: 1px solid #3c3c3c;"
        "  border-radius: 2px;"
        "  padding: 4px 6px;"
        "}"
    );
    inputLine_->setPlaceholderText(QString::fromUtf8("输入指令... (snap / rec start / <自然语言>)"));
    inputLine_->setClearButtonEnabled(true);

    layout->addWidget(outputView_);
    layout->addWidget(inputLine_);

    // 连接信号
    connect(inputLine_, &QLineEdit::returnPressed,
            this, &UiPilotConsole::onCommandEntered);

    // 打印欢迎信息
    appendOutput(QString::fromUtf8(
        "╔══════════════════════════════════════╗\n"
        "║   AstUiPilot Console Ready          ║\n"
        "╠══════════════════════════════════════╣\n"
        "║  snap        — 界面快照               ║\n"
        "║  rec start   — 开始录制               ║\n"
        "║  rec stop    — 停止录制               ║\n"
        "║  replay <p>  — 回放脚本               ║\n"
        "║  <自然语言>   — LLM 操控              ║\n"
        "║  help        — 帮助                  ║\n"
        "║  quit        — 退出控制台             ║\n"
        "╚══════════════════════════════════════╝"
    ));
}

// ============================================================
//  appendOutput / onCommanderOutput
// ============================================================

void UiPilotConsole::appendOutput(const QString& text)
{
    if (!outputView_) return;

    outputView_->moveCursor(QTextCursor::End);
    outputView_->insertPlainText(text + "\n");

    // 自动滚动到底部
    QScrollBar* sb = outputView_->verticalScrollBar();
    if (sb)
        sb->setValue(sb->maximum());
}

void UiPilotConsole::onCommanderOutput(const QString& text)
{
    appendOutput(text);
}

// ============================================================
//  onCommandEntered / executeCommand
// ============================================================

void UiPilotConsole::onCommandEntered()
{
    if (!inputLine_) return;

    QString cmd = inputLine_->text().trimmed();
    if (cmd.isEmpty()) return;

    inputLine_->clear();

    // 回显命令
    appendOutput(QString::fromUtf8("> ") + cmd);

    executeCommand(cmd);
}

void UiPilotConsole::executeCommand(const QString& cmd)
{
    if (!commander_) return;

    std::string utf8Cmd = cmd.toUtf8().toStdString();

    // quit 命令：隐藏控制台而非退出应用
    if (utf8Cmd == "quit" || utf8Cmd == "exit")
    {
        appendOutput(QString::fromUtf8("[控制台] 已关闭（应用继续运行）"));
        emit closeRequested();
        return;
    }

    // 同步执行（主线程直接调用）
    std::string result = commander_->execute(utf8Cmd);

    if (!result.empty())
    {
        QString qResult = QString::fromUtf8(result.c_str());
        // 限制输出长度，避免快照刷屏
        if (qResult.length() > 5000)
        {
            qResult = qResult.left(5000);
            qResult += QString::fromUtf8("\n\n... (输出截断，共 %1 字符)").arg(result.length());
        }
        appendOutput(qResult);
    }
}

// ============================================================
//  dockInto / autoDock
// ============================================================

bool UiPilotConsole::dockInto(QMainWindow* mainWindow)
{
    if (!mainWindow) return false;

    auto* dock = new QDockWidget(QString::fromUtf8("Pilot Console"), mainWindow);
    dock->setWidget(this);
    dock->setObjectName("UiPilotConsoleDock");
    dock->setFeatures(QDockWidget::DockWidgetMovable |
                      QDockWidget::DockWidgetClosable);

    mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dock);

    // 控制台关闭时隐藏 dock
    connect(this, &UiPilotConsole::closeRequested, dock, &QDockWidget::hide);

    return true;
}

bool UiPilotConsole::autoDock()
{
    auto tops = QApplication::topLevelWidgets();
    for (auto* top : tops)
    {
        if (auto* mw = qobject_cast<QMainWindow*>(top))
        {
            if (mw->isVisible())
                return dockInto(mw);
        }
    }
    return false;
}

// ============================================================
//  setVisible
// ============================================================

void UiPilotConsole::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if (visible && inputLine_)
    {
        inputLine_->setFocus();
    }
}

AST_NAMESPACE_END
