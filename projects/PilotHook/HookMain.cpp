///
/// @file      HookMain.cpp
/// @brief     AstUiPilot 全局消息钩子 — 自动注入任意 Qt 应用
/// @author    axel
/// @date      2026-06-13
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// 通过 SetWindowsHookEx(WH_GETMESSAGE) 安装全局钩子。
/// 当任意 GUI 进程收到窗口消息时，Windows 自动加载 AstUiPilotBoot.dll。
/// 只有已加载 Qt 的进程能成功解析 DLL 依赖，非 Qt 进程静默跳过。

#include <windows.h>
#include <string>
#include <cstdio>

// ---- 全局状态 ----
static HHOOK   g_hook    = NULL;
static HMODULE g_hBootDll = NULL;

// 用于 --uninstall 的互斥体名
static const wchar_t* kMutexName = L"Global\\AstUiPilotHookRunning";

// ---- 辅助 ----
static bool hasFlag(int argc, wchar_t* argv[], const std::wstring& flag)
{
    for (int i = 1; i < argc; i++)
        if (argv[i] == flag) return true;
    return false;
}

static void printLastError(const char* context)
{
    DWORD err = GetLastError();
    LPWSTR msgBuf = NULL;
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&msgBuf, 0, NULL);
    wprintf(L"[PilotHook] %hs (错误 %d): %s\n", context, (int)err, msgBuf);
    LocalFree(msgBuf);
}

static void printUsage()
{
    printf(
        "PilotHook — 全局钩子，自动将 AstUiPilot 注入到 Qt 应用\n\n"
        "用法:\n"
        "  PilotHook --install     安装全局钩子（前台常驻）\n"
        "  PilotHook --uninstall   卸载已安装的钩子\n"
        "  PilotHook --status      查看钩子状态\n\n"
        "安装后，正常启动任意 Qt 应用即可自动注入。\n"
        "按 Ctrl+C 卸载钩子并退出。\n"
    );
}

// ---- 安装钩子 ----
static int installHook()
{
    // 防止重复运行
    HANDLE hMutex = CreateMutexW(NULL, TRUE, kMutexName);
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        printf("[PilotHook] 钩子已在运行中（检测到互斥体）。\n");
        printf("  如需重新安装，请先执行: PilotHook --uninstall\n");
        CloseHandle(hMutex);
        return 1;
    }

    // 加载 AstUiPilotBoot.dll（由此进程持有引用）
    g_hBootDll = LoadLibraryW(L"AstUiPilotBoot.dll");
    if (!g_hBootDll)
    {
        printLastError("LoadLibrary(AstUiPilotBoot.dll)");
        printf("  请将 AstUiPilotBoot.dll 放在 PilotHook.exe 同目录\n");
        CloseHandle(hMutex);
        return 1;
    }

    // 获取钩子过程
    HOOKPROC hookProc = (HOOKPROC)GetProcAddress(g_hBootDll, "PilotBootHookProc");
    if (!hookProc)
    {
        printf("[PilotHook] AstUiPilotBoot.dll 中未找到 PilotBootHookProc 导出\n");
        printf("  请确保使用最新版本的 AstUiPilotBoot.dll\n");
        FreeLibrary(g_hBootDll);
        CloseHandle(hMutex);
        return 1;
    }

    // 安装全局 WH_GETMESSAGE 钩子
    g_hook = SetWindowsHookExW(WH_GETMESSAGE, hookProc, g_hBootDll, 0);
    if (!g_hook)
    {
        printLastError("SetWindowsHookEx");
        FreeLibrary(g_hBootDll);
        CloseHandle(hMutex);
        return 1;
    }

    printf("[PilotHook] 全局钩子已安装。\n");
    printf("[PilotHook] 现在启动任意 Qt 应用，AstUiPilot 将自动注入。\n");
    printf("[PilotHook] 按 Ctrl+C 卸载钩子并退出。\n\n");

    // 消息循环 —— 保持钩子存活
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // 清理（通常不会到达这里，WM_QUIT 由 --uninstall 发送）
    printf("[PilotHook] 正在卸载钩子...\n");
    UnhookWindowsHookEx(g_hook);
    g_hook = NULL;
    FreeLibrary(g_hBootDll);
    g_hBootDll = NULL;
    CloseHandle(hMutex);
    printf("[PilotHook] 钩子已卸载。\n");
    return 0;
}

// ---- 卸载钩子 ----
static int uninstallHook()
{
    // 查找已有实例并发送 WM_QUIT
    HANDLE hMutex = OpenMutexW(SYNCHRONIZE, FALSE, kMutexName);
    if (!hMutex)
    {
        printf("[PilotHook] 未检测到正在运行的钩子。\n");
        return 0;
    }
    CloseHandle(hMutex);

    // 查找 PilotHook 窗口来发送退出消息
    HWND hwnd = FindWindowW(NULL, L"PilotHookMessageWindow");
    if (hwnd)
    {
        PostMessageW(hwnd, WM_QUIT, 0, 0);
        printf("[PilotHook] 已发送卸载信号。\n");
    }
    else
    {
        printf("[PilotHook] 检测到钩子实例但无法找到其窗口。\n");
        printf("  钩子会在 PilotHook.exe 进程退出时自动卸载。\n");
    }
    return 0;
}

// ---- 查看状态 ----
static int checkStatus()
{
    HANDLE hMutex = OpenMutexW(SYNCHRONIZE, FALSE, kMutexName);
    if (hMutex)
    {
        printf("[PilotHook] 状态: 已安装（互斥体存在）\n");
        CloseHandle(hMutex);
    }
    else
    {
        printf("[PilotHook] 状态: 未安装\n");
    }
    return 0;
}

// ---- main ----
int wmain(int argc, wchar_t* argv[])
{
    if (hasFlag(argc, argv, L"--uninstall"))
        return uninstallHook();

    if (hasFlag(argc, argv, L"--status"))
        return checkStatus();

    if (hasFlag(argc, argv, L"--install") || hasFlag(argc, argv, L"--help") || argc <= 1)
    {
        if (hasFlag(argc, argv, L"--help"))
        {
            printUsage();
            return 0;
        }
        return installHook();
    }

    printUsage();
    return 0;
}
