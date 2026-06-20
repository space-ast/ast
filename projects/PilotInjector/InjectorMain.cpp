///
/// @file      InjectorMain.cpp
/// @brief     AstUiPilot DLL 注入启动器
/// @author    axel
/// @date      2026-06-13
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// 以挂起方式创建目标进程 → 注入 AstUiPilotBoot.dll → 恢复运行。
/// 支持命令行模式、录制模式、内嵌控制台模式。

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "AstUtil/IO.hpp"

AST_USING_NAMESPACE;

// ---- 辅助：获取 flag 后面的值 ----
static std::string getFlagValue(int argc, wchar_t* argv[], const std::wstring& flag)
{
    for (int i = 1; i < argc - 1; i++)
    {
        if (argv[i] == flag)
        {
            char buf[1024];
            WideCharToMultiByte(CP_UTF8, 0, argv[i + 1], -1, buf, sizeof(buf), NULL, NULL);
            return buf;
        }
    }
    return "";
}

// 宽字符版
static std::wstring getFlagValueW(int argc, wchar_t* argv[], const std::wstring& flag)
{
    for (int i = 1; i < argc - 1; i++)
    {
        if (argv[i] == flag)
            return argv[i + 1];
    }
    return L"";
}

// ---- 辅助：检查 flag 是否存在 ----
static bool hasFlag(int argc, wchar_t* argv[], const std::wstring& flag)
{
    for (int i = 1; i < argc; i++)
        if (argv[i] == flag) return true;
    return false;
}

// ---- 获取 DLL 绝对路径 ----
static std::wstring getDllFullPath(const std::wstring& dllPath)
{
    WCHAR fullPath[MAX_PATH];
    DWORD len = GetFullPathNameW(dllPath.c_str(), MAX_PATH, fullPath, NULL);
    if (len == 0 || len > MAX_PATH)
    {
        // 尝试在 exe 同级目录查找
        WCHAR exeDir[MAX_PATH];
        GetModuleFileNameW(NULL, exeDir, MAX_PATH);
        WCHAR* lastSlash = wcsrchr(exeDir, L'\\');
        if (lastSlash) *(lastSlash + 1) = L'\0';
        wcscat_s(exeDir, MAX_PATH, dllPath.c_str());
        return exeDir;
    }
    return fullPath;
}

// ---- 错误信息 ----
static void printLastError(const char* context)
{
    DWORD err = GetLastError();
    LPWSTR msgBuf = NULL;
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&msgBuf, 0, NULL);
    char utf8Buf[2048];
    WideCharToMultiByte(CP_UTF8, 0, msgBuf, -1, utf8Buf, sizeof(utf8Buf), NULL, NULL);
    std::fprintf(stderr, "[PilotInjector] %s (错误 %d): %s\n", context, (int)err, utf8Buf);
    LocalFree(msgBuf);
}

// ---- 使用说明 ----
static void printUsage()
{
    ast_printf(
        "PilotInjector — 将 AstUiPilot 注入到目标 Qt 应用\n\n"
        "用法:\n"
        "  PilotInjector --target <exe路径> [选项]\n\n"
        "选项:\n"
        "  --target <path>   目标应用程序路径（必填）\n"
        "  --dll <path>      要注入的 DLL 路径（默认: AstUiPilotBoot.dll）\n"
        "  --cmd <text>      注入后执行单次指令（LLM 指令或 snap/rec start 等）\n"
        "  --record          启动后自动开始录制\n"
        "  --console         启用内嵌控制台面板\n"
        "  --args <...>      传递给目标应用的命令行参数（放在最后）\n"
        "  --wait            等待目标进程退出\n"
        "\n"
        "示例:\n"
        "  PilotInjector --target \"C:\\App\\myapp.exe\"\n"
        "  PilotInjector --target \"C:\\App\\myapp.exe\" --console --record\n"
        "  PilotInjector --target \"C:\\App\\myapp.exe\" --cmd \"点击文件菜单中的打开\"\n"
        "  PilotInjector --target \"C:\\App\\myapp.exe\" --dll \"D:\\lib\\AstUiPilotBoot.dll\"\n"
    );
}

// ---- 注入 DLL 到目标进程 ----
static bool injectDll(HANDLE hProcess, const std::wstring& dllPath)
{
    // 0. 检查 DLL 文件是否存在
    if (GetFileAttributesW(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        DWORD err = GetLastError();
        std::fprintf(stderr, "[PilotInjector] DLL 文件不存在或无法访问:\n");
        std::fprintf(stderr, "  %ls\n", dllPath.c_str());
        if (err == ERROR_FILE_NOT_FOUND)
            std::fprintf(stderr, "  请先执行: xmake build AstUiPilotBoot\n");
        else
            printLastError("GetFileAttributesW");
        return false;
    }

    size_t dllPathBytes = (dllPath.size() + 1) * sizeof(WCHAR);
    if (dllPathBytes > 65536) return false;  // too long

    // 1. 在目标进程分配内存
    LPVOID remoteMem = VirtualAllocEx(hProcess, NULL, dllPathBytes,
                                       MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMem) { printLastError("VirtualAllocEx"); return false; }

    // 2. 写入 DLL 路径
    if (!WriteProcessMemory(hProcess, remoteMem, dllPath.c_str(), dllPathBytes, NULL))
    {
        printLastError("WriteProcessMemory");
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }

    // 3. 创建远程线程执行 LoadLibraryW
    LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)
        GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
    if (!pLoadLibrary)
    {
        printLastError("GetProcAddress(LoadLibraryW)");
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
        pLoadLibrary, remoteMem, 0, NULL);
    if (!hThread)
    {
        printLastError("CreateRemoteThread");
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }

    // 4. 等待 LoadLibrary 完成
    WaitForSingleObject(hThread, 30000); // 30s timeout

    DWORD dllBase = 0;
    GetExitCodeThread(hThread, &dllBase);

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);

    if (dllBase == 0)
    {
        // LoadLibrary 可以因多种原因失败：
        //   1. DLL 文件本身不存在（已在上面检查）
        //   2. DLL 的依赖项缺失（最常见）
        //   3. DLL 位数不匹配（32位 vs 64位）
        //   4. DllMain 返回 FALSE

        std::fprintf(stderr, "[PilotInjector] 注入失败：LoadLibrary 返回 NULL。\n");
        std::fprintf(stderr, "  可能原因:\n");
        std::fprintf(stderr, "    1) DLL 依赖缺失 — AstUiPilotBoot.dll 依赖 AstUiPilot.dll / Qt5Widgets 等\n");
        std::fprintf(stderr, "       请确保所有依赖 DLL 与目标 EXE 在同一目录:\n");
        // 获取 DLL 所在目录
        WCHAR dllDir[MAX_PATH];
        wcscpy_s(dllDir, MAX_PATH, dllPath.c_str());
        WCHAR* lastSlash = wcsrchr(dllDir, L'\\');
        if (lastSlash) *(lastSlash + 1) = L'\0';
        std::fprintf(stderr, "       %ls\n", dllDir);
        std::fprintf(stderr, "    2) 32位/64位不匹配 — 检查: 目标 EXE 与 DLL 位数是否一致\n");
        std::fprintf(stderr, "    3) DllMain 初始化失败 — 检查目标进程是否已加载兼容的 Qt 版本\n");
        return false;
    }

    ast_printf("[PilotInjector] DLL 注入成功 (base=0x%p)\n", (void*)(uintptr_t)dllBase);
    return true;
}

// ---- 设置目标进程的环境变量 ----
static void setupEnvironment(bool record, bool console)
{
    if (record)   SetEnvironmentVariableW(L"AST_UIPILOT_RECORD", L"1");
    if (console)  SetEnvironmentVariableW(L"AST_UIPILOT_CONSOLE", L"1");
}

// ---- Named Pipe 客户端：连接到目标进程中的 PilotPipeServer ----
static HANDLE connectToPipe(DWORD pid, int timeoutMs = 15000)
{
    std::ostringstream pipeNameStream;
    pipeNameStream << "\\\\.\\pipe\\AstUiPilot_" << pid;
    std::string pipeName = pipeNameStream.str();

    HANDLE hPipe = INVALID_HANDLE_VALUE;
    int elapsed = 0;
    int step = 200; // 每 200ms 重试

    while (elapsed < timeoutMs)
    {
        hPipe = CreateFileA(
            pipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,              // 独占
            NULL,           // 默认安全
            OPEN_EXISTING,
            0,              // 同步模式
            NULL
        );

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        DWORD err = GetLastError();
        if (err != ERROR_PIPE_BUSY && err != ERROR_FILE_NOT_FOUND)
        {
            printLastError("CreateFile(pipe)");
            return INVALID_HANDLE_VALUE;
        }

        if (err == ERROR_PIPE_BUSY)
            WaitNamedPipeA(pipeName.c_str(), 5000);

        Sleep(step);
        elapsed += step;
    }

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        std::fprintf(stderr, "[PilotInjector] 超时: %d 秒内无法连接到 pipe\n", timeoutMs / 1000);
        std::fprintf(stderr, "  请确认目标进程已启动且 AstUiPilot 已初始化。\n");
    }

    return hPipe;
}

// ---- 通过 pipe 发送命令并接收响应 ----
static std::string sendCommand(HANDLE hPipe, const std::string& command)
{
    if (hPipe == INVALID_HANDLE_VALUE)
        return "Pipe 未连接";

    // 发送命令 + \n
    std::string cmdLine = command + "\n";
    DWORD bytesWritten;
    if (!WriteFile(hPipe, cmdLine.data(),
                    static_cast<DWORD>(cmdLine.size()), &bytesWritten, NULL))
    {
        printLastError("WriteFile(pipe)");
        return "发送失败";
    }

    // 读取响应（直到 \0）
    std::string response;
    char buf[4096];
    DWORD bytesRead;
    while (ReadFile(hPipe, buf, sizeof(buf), &bytesRead, NULL) && bytesRead > 0)
    {
        response.append(buf, bytesRead);
        if (response.back() == '\0')
            break;
    }

    // 去除末尾 \0
    if (!response.empty() && response.back() == '\0')
        response.pop_back();

    return response;
}

// ---- 交互模式：通过 pipe 转发用户输入 ----
static int interactivePipeMode(HANDLE hPipe, DWORD pid)
{
    ast_printf("\n========================================\n");
    ast_printf("[AstUiPilot] 已连接到目标进程 PID=%d\n", (int)pid);
    ast_printf("  snap              — 查看界面快照\n");
    ast_printf("  rec start         — 开始录制\n");
    ast_printf("  rec stop          — 停止录制并导出\n");
    ast_printf("  rec export <path> — 导出到指定文件\n");
    ast_printf("  rec polish        — LLM润色并导出\n");
    ast_printf("  replay <path>     — 回放脚本\n");
    ast_printf("  <自然语言>         — LLM 操控界面\n");
    ast_printf("  quit / exit       — 退出\n");
    ast_printf("========================================\n\n");

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty()) continue;

        if (line == "quit" || line == "exit")
        {
            sendCommand(hPipe, "quit");
            break;
        }

        std::string response = sendCommand(hPipe, line);
        ast_printf("%s\n", response.c_str());
    }

    CloseHandle(hPipe);
    return 0;
}

// ---- 将单次指令写入 stdin ----
static bool sendStdinCommand(HANDLE hProcess, const std::string& command)
{
    // 通过环境变量传递命令：AST_UIPILOT_CMD
    // PilotBoot 初始化后在 stdin 线程中读取此环境变量并执行
    std::wstring wcmd;
    wcmd.resize(MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, NULL, 0));
    MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, &wcmd[0], (int)wcmd.size());
    SetEnvironmentVariableW(L"AST_UIPILOT_CMD", wcmd.c_str());
    return true;
}

// ---- 构建目标进程的命令行 ----
static std::wstring buildCommandLine(const std::wstring& target, const std::wstring& args)
{
    std::wstring cmdLine = L"\"" + target + L"\"";
    if (!args.empty())
    {
        cmdLine += L" ";
        cmdLine += args;
    }
    return cmdLine;
}

// ---- main ----
int wmain(int argc, wchar_t* argv[])
{
    // 解析参数
    std::wstring targetExe = getFlagValueW(argc, argv, L"--target");
    if (targetExe.empty())
    {
        printUsage();
        return 0;
    }

    std::wstring dllPath = getFlagValueW(argc, argv, L"--dll");
    if (dllPath.empty()) dllPath = AST_LIB_LINKNAME(L"AstUiPilotBoot") ".dll";
    dllPath = getDllFullPath(dllPath);

    std::string command = getFlagValue(argc, argv, L"--cmd");
    std::wstring targetArgs = getFlagValueW(argc, argv, L"--args");
    bool record  = hasFlag(argc, argv, L"--record");
    bool console = hasFlag(argc, argv, L"--console");
    bool wait    = hasFlag(argc, argv, L"--wait");

    ast_printf("[PilotInjector] 目标: %ls\n", targetExe.c_str());
    ast_printf("[PilotInjector] DLL:  %ls\n", dllPath.c_str());

    // 设置环境变量（这些会被子进程继承）
    setupEnvironment(record, console);

    // 如果有单次指令，设置环境变量
    if (!command.empty())
    {
        sendStdinCommand(NULL, command); // NULL → current process env (inherited)
        ast_printf("[PilotInjector] 指令: %s\n", command.c_str());
    }

    // 构建命令行
    std::wstring cmdLine = buildCommandLine(targetExe, targetArgs);

    // 创建挂起的目标进程
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    ast_printf("[PilotInjector] 启动目标进程 (挂起)...\n");

    // 注意：CreateProcess 的可变参数版本需要可写的命令行缓冲区
    WCHAR* cmdLineBuf = new WCHAR[cmdLine.size() + 1];
    wcscpy_s(cmdLineBuf, cmdLine.size() + 1, cmdLine.c_str());

    if (!CreateProcessW(NULL, cmdLineBuf, NULL, NULL, FALSE,
                        CREATE_SUSPENDED, NULL, NULL, &si, &pi))
    {
        printLastError("CreateProcess");
        delete[] cmdLineBuf;
        return 1;
    }
    delete[] cmdLineBuf;

    ast_printf("[PilotInjector] 目标进程 PID=%d\n", pi.dwProcessId);

    // 注入 DLL
    if (!injectDll(pi.hProcess, dllPath))
    {
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    // 恢复主线程
    ast_printf("[PilotInjector] 恢复目标进程运行...\n");
    ResumeThread(pi.hThread);

    // 单次指令模式：通过环境变量传递（简易、无需等 pipe）
    if (!command.empty())
    {
        ast_printf("[PilotInjector] 指令已通过环境变量传递，PilotBoot 初始化后将自动执行。\n");

        if (wait)
        {
            ast_printf("[PilotInjector] 等待目标进程退出...\n");
            WaitForSingleObject(pi.hProcess, INFINITE);
            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            ast_printf("[PilotInjector] 目标进程已退出 (code=%d)\n", (int)exitCode);
        }
        else
        {
            ast_printf("[PilotInjector] 完成。目标应用正在运行。\n");
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    }

    // 等待模式（无命令，仅等待进程退出）
    if (wait)
    {
        ast_printf("[PilotInjector] 等待目标进程退出...\n");
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        ast_printf("[PilotInjector] 目标进程已退出 (code=%d)\n", (int)exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    }

    // 默认：通过 Named Pipe 进入交互模式
    ast_printf("[PilotInjector] 等待 PilotPipeServer 就绪...\n");
    HANDLE hPipe = connectToPipe(pi.dwProcessId, 20000);
    if (hPipe == INVALID_HANDLE_VALUE)
    {
        ast_printf("[PilotInjector] 无法建立通信通道。\n");
        ast_printf("[PilotInjector] 目标应用仍在运行，可通过 Ctrl+C 退出。\n");
        // 让目标进程继续运行，Injector 等待 Ctrl+C
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    int ret = interactivePipeMode(hPipe, pi.dwProcessId);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return ret;
}
