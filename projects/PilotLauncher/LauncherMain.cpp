///
/// @file      LauncherMain.cpp
/// @brief     AstUiPilot 启动器 — 命令行模式 / 交互模式 / 录制 / 回放
/// @author    axel
/// @date      2026-06-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "ast/AstGUIAPI.hpp"
#include "AstUiPilot/PilotAgent.hpp"
#include "AstUiPilot/PilotSession.hpp"
#include "AstUiPilot/PilotRecorder.hpp"
#include "AstUiPilot/PilotPlayer.hpp"
#include "AstUiPilot/PilotCommander.hpp"
#include "AstUiPilot/UiPilotConsole.hpp"
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

// ---- 参数解析 ----

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

/// @brief 检查命令行参数中是否包含某标志
static bool hasFlag(int argc, char* argv[], const std::string& flag)
{
#ifdef _WIN32
    int wargc;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
    for (int i = 1; i < wargc; i++)
    {
        if (aWideToUtf8(wargv[i]) == flag) { LocalFree(wargv); return true; }
    }
    LocalFree(wargv);
#else
    for (int i = 1; i < argc; i++)
        if (std::strcmp(argv[i], flag.c_str()) == 0) return true;
#endif
    return false;
}

/// @brief 获取命令行参数中某个标志后面的值，如 --replay foo.json
static std::string getFlagValue(int argc, char* argv[], const std::string& flag)
{
#ifdef _WIN32
    int wargc;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
    for (int i = 1; i < wargc - 1; i++)
    {
        if (aWideToUtf8(wargv[i]) == flag)
        {
            std::string val = aWideToUtf8(wargv[i + 1]);
            LocalFree(wargv);
            return val;
        }
    }
    LocalFree(wargv);
#else
    for (int i = 1; i < argc - 1; i++)
        if (std::strcmp(argv[i], flag.c_str()) == 0) return argv[i + 1];
#endif
    return "";
}

// ---- 命令行模式 ----

static int commandLineMode(PilotCommander* commander, const std::string& command)
{
    ast_printf("[AstUiPilot] 指令: %s\n", command.c_str());

    // 先处理事件让界面渲染
    QApplication::processEvents();
    QTest::qWait(300);

    std::string response = commander->execute(command);
    ast_printf("%s\n", response.c_str());

    QApplication::processEvents();
    return 0;
}

// ---- 回放模式 ----

static int replayMode(PilotSession* session, const std::string& scriptPath)
{
    ast_printf("[AstUiPilot] 回放脚本: %s\n", scriptPath.c_str());
    QApplication::processEvents();
    QTest::qWait(500);

    auto* player = new PilotPlayer(session);
    if (!player->loadScript(scriptPath))
    {
        ast_printf("[AstUiPilot] 无法加载脚本: %s\n", scriptPath.c_str());
        delete player;
        return 1;
    }

    ast_printf("[AstUiPilot] 共 %d 个步骤\n", player->totalSteps());

    QObject::connect(player, &PilotPlayer::stepStarted,
        [](int idx, const std::string& desc) {
            ast_printf("\n[步骤 %d] %s\n", idx + 1, desc.c_str());
        });

    QObject::connect(player, &PilotPlayer::stepCompleted,
        [](int idx, const std::string&) {
            ast_printf("[步骤 %d] ✓ 完成\n", idx + 1);
        });

    QObject::connect(player, &PilotPlayer::stepFailed,
        [](int idx, const std::string& error) {
            ast_printf("[步骤 %d] ✗ 失败: %s\n", idx + 1, error.c_str());
        });

    QObject::connect(player, &PilotPlayer::playbackFinished,
        [player]() {
            ast_printf("\n[AstUiPilot] 回放完成\n");
            player->deleteLater();
            QApplication::quit();
        });

    player->play();
    return qApp->exec();
}

// ---- main ----

int main(int argc, char *argv[])
{
    aQAppInit(argc, argv);

    // 1. 创建核心组件
    auto* agent = new PilotAgent();
    qApp->installEventFilter(agent);

    auto* session = new PilotSession(agent);
    auto* recorder = new PilotRecorder(agent);
    auto* commander = new PilotCommander(session, recorder);

    // 2. 创建主窗口
    QMainWindow* mainWindow = aUiNewMainWindow();
    mainWindow->showMaximized();

    ast_printf("[AstUiPilot] PID=%lld Qt=%s\n",
               static_cast<long long>(qApp->applicationPid()), qVersion());

    // 3. 检测 --record 标记：自动开始录制
    if (hasFlag(argc, argv, "--record"))
    {
        recorder->start();
        ast_printf("[AstUiPilot] 录制已自动开始\n");
    }

    // 4. 检测 --console 标记：启用内嵌控制台
    if (hasFlag(argc, argv, "--console"))
    {
        auto* console = new UiPilotConsole(commander);
        if (!console->autoDock())
        {
            console->setWindowTitle(QString::fromUtf8("Pilot Console"));
            console->resize(600, 300);
            console->show();
        }
    }

    // 5. 回放模式：--replay <script>
    std::string replayPath = getFlagValue(argc, argv, "--replay");
    if (!replayPath.empty())
    {
        return replayMode(session, replayPath);
    }

    // 6. 命令行模式：-- 之后的所有参数作为单次指令
    std::string command = extractCommand(argc, argv);
    if (!command.empty())
    {
        return commandLineMode(commander, command);
    }

    // 7. 交互模式
    // 监听录制步骤
    QObject::connect(recorder, &PilotRecorder::stepRecorded,
        [](int idx, const std::string& desc) {
            ast_printf("  [%d] %s\n", idx + 1, desc.c_str());
        });

    // 监听退出请求
    QObject::connect(commander, &PilotCommander::quitRequested,
        qApp, [recorder]() {
            g_running = false;
            if (recorder->isRecording())
                recorder->stop();
            QApplication::quit();
        });

    // 启动 stdin 命令循环
    commander->startStdinLoop();

    int ret = qApp->exec();
    g_running = false;

    // 清理
    commander->stop();
    if (recorder->isRecording()) recorder->stop();
    delete commander;
    delete recorder;
    delete session;
    delete agent;
    return ret;
}
