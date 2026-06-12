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

static int commandLineMode(PilotSession* session, const std::string& command)
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

// ---- 回放模式 ----

static int replayMode(PilotSession* session, const std::string& scriptPath)
{
    ast_printf("[AstUiPilot] 回放脚本: %s\n", scriptPath.c_str());
    QApplication::processEvents();
    QTest::qWait(500);

    PilotPlayer player(session);
    if (!player.loadScript(scriptPath))
    {
        ast_printf("[AstUiPilot] 无法加载脚本: %s\n", scriptPath.c_str());
        return 1;
    }

    ast_printf("[AstUiPilot] 共 %d 个步骤\n", player.totalSteps());

    QObject::connect(&player, &PilotPlayer::stepStarted,
        [](int idx, const std::string& desc) {
            ast_printf("\n[步骤 %d] %s\n", idx + 1, desc.c_str());
        });

    QObject::connect(&player, &PilotPlayer::stepCompleted,
        [](int idx, const std::string& result) {
            ast_printf("[步骤 %d] ✓ 完成\n", idx + 1);
        });

    QObject::connect(&player, &PilotPlayer::stepFailed,
        [](int idx, const std::string& error) {
            ast_printf("[步骤 %d] ✗ 失败: %s\n", idx + 1, error.c_str());
        });

    QObject::connect(&player, &PilotPlayer::playbackFinished,
        [&]() { ast_printf("\n[AstUiPilot] 回放完成\n"); QApplication::quit(); });

    player.play();
    return qApp->exec();
}

// ---- 交互模式 ----

static void commandLoop(PilotSession* session, PilotRecorder* recorder)
{
    ast_printf("\n========================================\n");
    ast_printf("[AstUiPilot] Agent 已就绪 (交互模式)\n");
    ast_printf("  snap              — 查看界面快照\n");
    ast_printf("  rec start         — 开始录制\n");
    ast_printf("  rec stop          — 停止录制并导出\n");
    ast_printf("  rec export <path> — 导出到指定文件\n");
    ast_printf("  rec polish        — LLM润色并导出\n");
    ast_printf("  replay <path>     — 回放脚本\n");
    ast_printf("  <自然语言>         — LLM 操控界面\n");
    ast_printf("  quit              — 退出\n");
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
            if (recorder->isRecording()) 
                recorder->stop();
            QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
            break;
        }

        if (utf8Line == "snap" || utf8Line == "snapshot")
        {
            QMetaObject::invokeMethod(qApp, []() {
                ast_printf("%s\n", PilotAgent::instance()->snapshot().c_str());
            }, Qt::BlockingQueuedConnection);
            continue;
        }

        // ---- 录制命令 ----
        if (utf8Line == "rec start")
        {
            QMetaObject::invokeMethod(qApp, [recorder]() {
                recorder->start();
                ast_printf("[录制] 已开始\n");
            }, Qt::BlockingQueuedConnection);
            continue;
        }

        if (utf8Line == "rec stop")
        {
            QMetaObject::invokeMethod(qApp, [recorder]() {
                recorder->stop();
                ast_printf("[录制] 已停止，共 %d 步\n", recorder->stepCount());
                // 自动导出
                std::string path = "record_output.json";
                recorder->saveToFile(path);
                ast_printf("[录制] 已导出到 %s\n", path.c_str());
            }, Qt::BlockingQueuedConnection);
            continue;
        }

        if (utf8Line.rfind("rec export ", 0) == 0)
        {
            std::string path = utf8Line.substr(11);
            QMetaObject::invokeMethod(qApp, [recorder, path]() {
                recorder->saveToFile(path);
                ast_printf("[录制] 已导出到 %s\n", path.c_str());
            }, Qt::BlockingQueuedConnection);
            continue;
        }

        if (utf8Line == "rec polish")
        {
            QMetaObject::invokeMethod(qApp, [recorder, session]() {
                if (recorder->stepCount() == 0) {
                    ast_printf("[录制] 没有步骤可润色\n");
                    return;
                }
                ast_printf("[录制] LLM 润色中...\n");
                std::string json = recorder->polish(session);
                ast_printf("[录制] 润色完成\n%s\n", json.c_str());
                recorder->saveToFile("record_polished.json");
                ast_printf("[录制] 已导出到 record_polished.json\n");
            }, Qt::BlockingQueuedConnection);
            continue;
        }

        // ---- 回放命令 ----
        if (utf8Line.rfind("replay ", 0) == 0)
        {
            std::string path = utf8Line.substr(7);
            QMetaObject::invokeMethod(qApp, [session, path]() {
                PilotPlayer player(session);
                if (!player.loadScript(path)) {
                    ast_printf("[回放] 无法加载脚本: %s\n", path.c_str());
                    return;
                }
                ast_printf("[回放] 共 %d 个步骤\n", player.totalSteps());

                QObject::connect(&player, &PilotPlayer::stepStarted,
                    [](int idx, const std::string& desc) {
                        ast_printf("\n[步骤 %d] %s\n", idx + 1, desc.c_str());
                    });
                QObject::connect(&player, &PilotPlayer::stepCompleted,
                    [](int, const std::string&) { ast_printf("  ✓\n"); });
                QObject::connect(&player, &PilotPlayer::stepFailed,
                    [](int, const std::string& err) { ast_printf("  ✗ %s\n", err.c_str()); });
                QObject::connect(&player, &PilotPlayer::playbackFinished,
                    []() { ast_printf("[回放] 完成\n"); });

                player.play();
            }, Qt::BlockingQueuedConnection);
            continue;
        }

        if (utf8Line == "help")
        {
            ast_printf("命令:\n"
                       "  snap / snapshot  — 查看界面快照\n"
                       "  rec start        — 开始录制\n"
                       "  rec stop         — 停止录制并导出\n"
                       "  rec export <p>   — 导出到文件\n"
                       "  rec polish       — LLM润色并导出\n"
                       "  replay <path>    — 回放脚本\n"
                       "  <自然语言>       — LLM 操控界面\n"
                       "  quit / exit      — 退出\n");
            continue;
        }

        // 自然语言指令
        ast_printf("[AstUiPilot] 执行: %s\n", utf8Line.c_str());
        QMetaObject::invokeMethod(qApp, [session, utf8Line]() {
            session->chat(utf8Line);
        }, Qt::BlockingQueuedConnection);
    }
}

// ---- main ----

int main(int argc, char *argv[])
{
    aQAppInit(argc, argv);

    auto* agent = new PilotAgent();
    qApp->installEventFilter(agent);

    auto* session = new PilotSession(agent);
    auto* recorder = new PilotRecorder(agent);

    QMainWindow* mainWindow = aUiNewMainWindow();
    mainWindow->showMaximized();

    ast_printf("[AstUiPilot] PID=%lld Qt=%s\n",
               static_cast<long long>(qApp->applicationPid()), qVersion());

    // 检测录制标记
    if (hasFlag(argc, argv, "--record"))
    {
        recorder->start();
        ast_printf("[AstUiPilot] 录制已自动开始\n");
    }

    // 回放模式
    std::string replayPath = getFlagValue(argc, argv, "--replay");
    if (!replayPath.empty())
    {
        return replayMode(session, replayPath);
    }

    // 命令行模式
    std::string command = extractCommand(argc, argv);
    if (!command.empty())
    {
        return commandLineMode(session, command);
    }

    // 交互模式
    // 监听录制步骤
    QObject::connect(recorder, &PilotRecorder::stepRecorded,
        [](int idx, const std::string& desc) {
            ast_printf("  [%d] %s\n", idx + 1, desc.c_str());
        });

    std::thread cmdThread(commandLoop, session, recorder);
    cmdThread.detach();

    int ret = qApp->exec();
    g_running = false;

    if (recorder->isRecording()) recorder->stop();
    delete session;
    delete agent;
    return ret;
}
