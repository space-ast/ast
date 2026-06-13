///
/// @file      PilotBoot.cpp
/// @brief     注入入口：库加载时自动初始化 Agent + 交互通道
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

#include "AstUiPilot/PilotAgent.hpp"
#include "AstUiPilot/PilotSession.hpp"
#include "AstUiPilot/PilotRecorder.hpp"
#include "AstUiPilot/PilotCommander.hpp"
#include "AstUiPilot/UiPilotConsole.hpp"
#include "AstUiPilot/PilotPipeServer.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <thread>
#include <chrono>
#include <cstdlib>

AST_NAMESPACE_BEGIN

// ---- 全局状态 ----
static PilotAgent*       g_agent       = nullptr;
static PilotSession*     g_session     = nullptr;
static PilotRecorder*    g_recorder    = nullptr;
static PilotCommander*   g_commander   = nullptr;
static PilotPipeServer*  g_pipeServer  = nullptr;
static bool              g_initialized = false;

// ---- 前向声明 ----
static void startConsoleIfRequested();

/// @brief 初始化 AstUiPilot 核心组件
/// @note 必须在 QApplication 创建后、主线程中调用
static void initUiPilot()
{
    if (g_initialized) return;
    if (!qApp) return;

    // 1. 创建 PilotAgent（单例，自动安装全局事件过滤器）
    g_agent = new PilotAgent();
    qApp->installEventFilter(g_agent);

    // 2. 创建 PilotSession（LLM 对话 + 工具注册）
    g_session = new PilotSession(g_agent);

    // 3. 创建 PilotRecorder（全局事件录制）
    g_recorder = new PilotRecorder(g_agent);

    // 4. 创建 PilotCommander（命令调度 + stdin 循环）
    g_commander = new PilotCommander(g_session, g_recorder);

    // 5. 检查是否启用录制（环境变量 AST_UIPILOT_RECORD=1）
    const char* recordEnv = std::getenv("AST_UIPILOT_RECORD");
    if (recordEnv && (std::string(recordEnv) == "1" || std::string(recordEnv) == "true"))
    {
        g_recorder->start();
        qDebug() << "[AstUiPilotBoot] 录制已自动开始（AST_UIPILOT_RECORD=1）";
    }

    g_initialized = true;
    qDebug() << "[AstUiPilotBoot] Agent initialized in process"
             << qApp->applicationPid();

    // 6. 启动 stdin 命令监听循环（后台线程）
    //    环境变量 AST_UIPILOT_NO_STDIN=1 可禁用
    const char* noStdinEnv = std::getenv("AST_UIPILOT_NO_STDIN");
    if (!noStdinEnv || std::string(noStdinEnv) != "1")
    {
        g_commander->startStdinLoop();
    }

    // 7. 检查是否启用内嵌控制台（环境变量 AST_UIPILOT_CONSOLE=1）
    startConsoleIfRequested();

    // 8. 启动 Named Pipe 服务（供外部 Injector 连接发送命令）
    //    环境变量 AST_UIPILOT_NO_PIPE=1 可禁用
    const char* noPipeEnv = std::getenv("AST_UIPILOT_NO_PIPE");
    if (!noPipeEnv || std::string(noPipeEnv) != "1")
    {
        g_pipeServer = new PilotPipeServer(g_commander,
            static_cast<unsigned long>(qApp->applicationPid()));
        g_pipeServer->start();
        qDebug() << "[AstUiPilotBoot] Pipe server started:"
                 << PilotPipeServer::pipeName(
                     static_cast<unsigned long>(qApp->applicationPid())).c_str();
    }
}

// ============================================================
//  内嵌控制台
// ============================================================

static void startConsoleIfRequested()
{
    const char* consoleEnv = std::getenv("AST_UIPILOT_CONSOLE");
    if (!consoleEnv || (std::string(consoleEnv) != "1" && std::string(consoleEnv) != "true"))
        return;

    // 延迟附加控制台：等待主窗口创建完成
    QTimer::singleShot(500, qApp, []() {
        auto* console = new UiPilotConsole(g_commander);
        if (!console->autoDock())
        {
            // 没有 QMainWindow，作为独立窗口显示
            console->setWindowTitle(QString::fromUtf8("Pilot Console"));
            console->resize(600, 300);
            console->show();
        }
        qDebug() << "[AstUiPilotBoot] Console started";
    });
}

// ============================================================
//  轮询线程：等待 QApplication 创建（Qt5 兼容）
// ============================================================

static void pollForQApp()
{
    // 轮询等待 QApplication 创建
    // 每 50ms 检查一次，最多等待 60 秒
    int maxAttempts = 1200; // 60s / 50ms
    for (int i = 0; i < maxAttempts; i++)
    {
        if (qApp) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    if (!qApp)
    {
        qWarning() << "[AstUiPilotBoot] 超时：60秒内未检测到 QApplication";
        return;
    }

    // QApplication 已存在 → 在主线程上初始化
    QMetaObject::invokeMethod(qApp, []() {
        initUiPilot();
    }, Qt::QueuedConnection);
}

// ============================================================
//  自动启动入口
// ============================================================

/// @brief Qt6 风格：QCoreApplication 构造后自动调用
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
Q_COREAPP_STARTUP_FUNCTION(initUiPilot)
#endif

// ============================================================
//  MSVC 静态初始化器（MSVC 不支持 __attribute__((constructor))）
//  在 CRT 初始化期间运行（DllMain 返回前），启动轮询线程
// ============================================================
#ifdef Q_OS_WIN
#if defined(_MSC_VER) && !defined(__clang__)
namespace {
    struct PollStarter {
        PollStarter() {
            std::thread(pollForQApp).detach();
            qAddPostRoutine([]() {
                if (g_commander) {
                    g_commander->stop();
                    delete g_commander;
                    g_commander = nullptr;
                }
                delete g_session;
                g_session = nullptr;
                delete g_recorder;
                g_recorder = nullptr;
                delete g_pipeServer;
                g_pipeServer = nullptr;
                delete g_agent;
                g_agent = nullptr;
                g_initialized = false;
            });
        }
    };
    static PollStarter g_pollStarter;
}
#endif
#endif // Q_OS_WIN

AST_NAMESPACE_END

// ============================================================
//  Windows DLL 入口
// ============================================================
#ifdef Q_OS_WIN
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        // 禁止后续的线程 attach/detach 通知，减少开销
        DisableThreadLibraryCalls(hinstDLL);
    }
    return TRUE;
}
#endif // Q_OS_WIN

// ============================================================
//  __attribute__((constructor)) — Linux / macOS / MinGW
// ============================================================
#if defined(__GNUC__) || defined(__clang__)

/// @brief 库加载时启动轮询线程（Qt5 兼容）
/// @note 轮询线程会等待 QApplication 创建，然后自动调用 initUiPilot()
///       Qt6 上 Q_COREAPP_STARTUP_FUNCTION 会率先触发，initUiPilot() 中的
///       g_initialized 标志位保证不重复初始化。
static void __attribute__((constructor)) astUiPilotEntry()
{
    // 启动轮询线程等待 QApplication（兼容 Qt5）
    std::thread(ast::pollForQApp).detach();

    // 注册退出清理
    qAddPostRoutine([]() {
        if (ast::g_commander)
        {
            ast::g_commander->stop();
            delete ast::g_commander;
            ast::g_commander = nullptr;
        }
        delete ast::g_session;
        ast::g_session = nullptr;
        delete ast::g_recorder;
        ast::g_recorder = nullptr;
        delete ast::g_agent;
        ast::g_agent = nullptr;
        ast::g_initialized = false;
    });
}

#endif

// ============================================================
//  PilotBootHookProc — 供 SetWindowsHookEx 使用的钩子过程
//  当全局 WH_GETMESSAGE 钩子触发时，此函数在目标进程中执行。
//  实际初始化工作由 DllMain → PollStarter → initUiPilot 完成，
//  此函数仅将消息传递给下一个钩子。
// ============================================================
#ifdef Q_OS_WIN
extern "C" __declspec(dllexport) LRESULT CALLBACK PilotBootHookProc(
    int code, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(NULL, code, wParam, lParam);
}
#endif
