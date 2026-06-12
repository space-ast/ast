///
/// @file      UiPilotEntry.cpp
/// @brief     注入入口：库加载时自动初始化 Agent
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
#include "PilotSession.hpp"
#include <QApplication>
#include <QDebug>
#include <QTimer>

AST_NAMESPACE_BEGIN

static PilotAgent*   g_agent   = nullptr;
static PilotSession* g_session = nullptr;
static bool            g_initialized = false;

/// @brief 延迟初始化（等待 QApplication 创建完成）
static void initUiPilot()
{
    if (g_initialized) return;
    if (!qApp) return;

    g_agent = new PilotAgent();
    qApp->installEventFilter(g_agent);

    g_session = new PilotSession(g_agent);

    g_initialized = true;
    qDebug() << "[AstUiPilot] Agent initialized in process"
             << qApp->applicationPid();
}

/// @brief Qt6 风格：QCoreApplication 构造后自动调用
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
Q_COREAPP_STARTUP_FUNCTION(initUiPilot)
#endif

AST_NAMESPACE_END

// ============================================================
//  Windows DLL 入口
// ============================================================
#ifdef Q_OS_WIN
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE /*hinstDLL*/, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        // Qt 操作不能在此处进行，延迟到 QApplication 构造后
        // 使用 Q_COREAPP_STARTUP_FUNCTION（Qt6）或 QTimer::singleShot（Qt5）
    }
    return TRUE;
}
#endif

// ============================================================
//  __attribute__((constructor)) — Linux/macOS
// ============================================================
#if defined(__GNUC__) || defined(__clang__)
#ifndef Q_OS_WIN

/// @brief 库加载时初始化（Linux LD_PRELOAD / macOS DYLD_INSERT_LIBRARIES）
/// @note 此时 QApplication 可能尚未创建，注册回调延迟初始化
static void __attribute__((constructor)) astUiPilotEntry()
{
    // 注册 initUiPilot 在事件循环开始后调用
    // Qt5: 使用 QTimer::singleShot(0, []{ initUiPilot(); });
    // Qt6: 使用 Q_COREAPP_STARTUP_FUNCTION（上方已声明）
    qAddPostRoutine([]() {
        delete g_session;
        g_session = nullptr;
        delete g_agent;
        g_agent = nullptr;
        g_initialized = false;
    });
}

#endif
#endif
