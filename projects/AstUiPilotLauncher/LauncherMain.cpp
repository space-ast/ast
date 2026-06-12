///
/// @file      LauncherMain.cpp
/// @brief     AstUiPilot 启动器 — 进程内注入 Agent，支持命令行和交互两种模式
/// @author    axel
/// @date      2026-06-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "ast/AstGUIAPI.hpp"
#include "AstUiPilot/UiPilotAgent.hpp"
#include "AstUiPilot/UiPilotSession.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Encode.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QtTest/qtest.h>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#include <thread>
#include <string>
#include <atomic>
#include <cstring>

AST_USING_NAMESPACE

static std::atomic<bool> g_running{true};

/// @brief 提取 -- 之后的所有参数拼接为命令（Windows用Wide API转UTF-8）
static std::string extractCommand(int argc, char* argv[])
{
    std::string cmd;
    bool foundSep = false;

#ifdef _WIN32
    // 用GetCommandLineW获取宽字符命令行，绕过GBK问题
    int wargc;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
    for (int i = 1; i < wargc; i++)
    {
        std::string arg = aWideToUtf8(wargv[i]);
        if (arg == "--")
        {
            foundSep = true;
            continue;
        }
        if (foundSep)
        {
            if (!cmd.empty()) cmd += " ";
            cmd += arg;
        }
    }
    LocalFree(wargv);
#else
    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "--") == 0)
        {
            foundSep = true;
            continue;
        }
        if (foundSep)
        {
            if (!cmd.empty()) cmd += " ";
            cmd += argv[i];
        }
    }
#endif

    return cmd;
}

/// @brief 命令行模式：启动 → 渲染 → LLM执行 → 退出
static int commandLineMode(UiPilotSession* session, const std::string& command)
{
    ast_printf("[AstUiPilot] 指令: %s\n", command.c_str());

    // 先处理事件让界面渲染
    QApplication::processEvents();
    QTest::qWait(300);

    // 直接在主线程执行（不通过 timer，避免事件循环重入）
    std::string response = session->execute(command);
    ast_printf("%s\n", response.c_str());

    QApplication::processEvents();
    return 0;
}

/// @brief 交互模式：stdin 循环（主线程执行 GUI 操作）
static void commandLoop(UiPilotSession* session)
{
    ast_printf("\n========================================\n");
    ast_printf("[AstUiPilot] Agent 已就绪 (交互模式)\n");
    ast_printf("  snap  — 查看界面快照\n");
    ast_printf("  <指令> — LLM 操控界面\n");
    ast_printf("  quit  — 退出\n");
    ast_printf("========================================\n\n");

    std::string line;
    while (g_running && std::getline(std::cin, line))
    {
        if (line.empty()) continue;

        // Windows下stdin是本地编码(GBK)，转为UTF-8
#ifdef Q_OS_WIN
        std::string utf8Line = QString::fromLocal8Bit(line.c_str()).toUtf8().toStdString();
#else
        std::string utf8Line = line;
#endif

        if (utf8Line == "quit" || utf8Line == "exit")
        {
            g_running = false;
            QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
            break;
        }

        if (utf8Line == "snap" || utf8Line == "snapshot")
        {
            QMetaObject::invokeMethod(qApp, []() {
                std::string snap;
                snap = UiPilotAgent::instance()->snapshot();
                ast_printf("%s\n", snap.c_str());
            }, Qt::BlockingQueuedConnection);
            continue;
        }

        if (utf8Line == "help")
        {
            ast_printf("命令:\n"
                       "  snap / snapshot — 快照\n"
                       "  <自然语言>      — LLM执行\n"
                       "  quit / exit     — 退出\n");
            continue;
        }

        ast_printf("[AstUiPilot] 执行: %s\n", line.c_str());
        QMetaObject::invokeMethod(qApp, [session, line]() {
            session->chat(line);
        }, Qt::BlockingQueuedConnection);
    }
}

int main(int argc, char *argv[])
{
    aQAppInit(argc, argv);

    auto* agent = new UiPilotAgent();
    qApp->installEventFilter(agent);

    auto* session = new UiPilotSession(agent);

    QMainWindow* mainWindow = aUiNewMainWindow();
    mainWindow->showMaximized();

    ast_printf("[AstUiPilot] PID=%lld Qt=%s\n",
               static_cast<long long>(qApp->applicationPid()), qVersion());

    std::string command = extractCommand(argc, argv);

    if (!command.empty())
    {
        return commandLineMode(session, command);
    }
    else
    {
        std::thread cmdThread(commandLoop, session);
        cmdThread.detach();

        int ret = qApp->exec();
        g_running = false;
        delete session;
        delete agent;
        return ret;
    }
}
