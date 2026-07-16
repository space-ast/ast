///
/// @file      CrashHandler.cpp
/// @brief     崩溃捕获与记录实现
/// @details   ~
/// @author    axel
/// @date      1.7.2026
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "CrashHandler.hpp"

#include "AstUtil/Logger.hpp"
#include "AstUtil/Posix.hpp"
#include "AstUtil/RunTime.hpp"

#include <atomic>
#include <cstring>
#include <ctime>

#ifdef _WIN32
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#   include <windows.h>
#   include <psapi.h>
#   include <process.h>
#   include <signal.h>
#   include <dbghelp.h>
#   ifdef A_MSVC
#       pragma comment(lib, "psapi.lib")
#       pragma comment(lib, "dbghelp.lib")
#   endif
#else
#   include <signal.h>
#   include <fcntl.h>
#   include <unistd.h>
#   include <sys/stat.h>
#   include <dlfcn.h>
#   ifdef __linux__
#       include <execinfo.h>
#       include <sys/syscall.h>
#       include <sys/utsname.h>
#   endif
#endif

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

// ============================================================================
// 内部常量
// ============================================================================

/// 最大路径长度
enum { kMaxPath = 512 };

/// 崩溃报告缓冲区大小
enum { kReportBufferSize = 16384 };

/// 最大回溯帧数
enum { kMaxStackFrames = 128 };

/// 备用信号栈大小 (Linux)
enum { kAltStackSize = 16384 };

/// 最大模块信息缓冲区
enum { kModuleInfoSize = 4096 };

/// 信号名称映射表大小
enum { kSignalNameCount = 32 };

// ============================================================================
// 文件作用域静态变量（全部预分配，信号安全）
// ============================================================================

/// 递归守卫：防止信号处理器中再次崩溃导致死循环
static std::atomic<int> g_crashRecursionGuard{0};

/// 是否已安装
static bool g_installed = false;

/// 用户回调
static ACrashCallback g_userCallback = nullptr;

/// 崩溃后的报告命令
static char g_reporterCmd[kMaxPath] = {0};

/// 崩溃日志目录
static char g_logDir[kMaxPath] = {0};

/// 最后一次崩溃日志完整路径
static char g_lastLogPath[kMaxPath] = {0};

/// 预分配的崩溃报告缓冲区
static char g_reportBuffer[kReportBufferSize] = {0};

/// 报告写入位置
static size_t g_reportLen = 0;

/// 回溯地址缓冲区
static void* g_stackFrames[kMaxStackFrames] = {0};

#ifdef _WIN32
// Windows: 保存之前的 SEH 过滤器
static LPTOP_LEVEL_EXCEPTION_FILTER g_prevSehFilter = nullptr;
// Windows: 保存之前的 SIGABRT 处理器
static void (*g_prevSigabrtHandler)(int) = nullptr;
#else
// Linux: 保存之前的 sigaction
static struct sigaction g_prevSigActions[NSIG] = {};
// Linux: 备用信号栈
static char g_altStack[kAltStackSize] = {0};
// Linux: 需要捕获的信号
static const int g_crashSignals[] = {SIGSEGV, SIGABRT, SIGFPE, SIGILL};
static const int g_crashSignalCount = sizeof(g_crashSignals) / sizeof(g_crashSignals[0]);
#endif

/// 信号编号到名称的映射表（信号安全：纯数据，无指针）
static const char* const g_signalNames[kSignalNameCount] = {
    nullptr, nullptr, nullptr, nullptr,     // 0-3
    "SIGILL",  "SIGTRAP", nullptr, nullptr, // 4-7
    "SIGFPE",  nullptr,   nullptr,  "SIGSEGV", // 8-11
    nullptr,   nullptr,   nullptr,  "SIGBUS",  // 12-15
    nullptr,   nullptr,   nullptr,  nullptr,   // 16-19
    nullptr,   nullptr,   "SIGABRT", nullptr,  // 20-23
    nullptr,   nullptr,   nullptr,  nullptr,   // 24-27
    nullptr,   nullptr,   nullptr,  nullptr    // 28-31
};

// ============================================================================
// 信号安全的辅助函数（仅整数运算 + 固定缓冲区操作，无任何堆分配）
// ============================================================================

/// @brief 安全地将字符追加到报告缓冲区
static void _aCrashAppendChar(char c)
{
    if (g_reportLen + 1 < kReportBufferSize)
    {
        g_reportBuffer[g_reportLen++] = c;
        g_reportBuffer[g_reportLen] = '\0';
    }
}

/// @brief 安全地将 C 字符串追加到报告缓冲区
static void _aCrashAppendStr(const char* str)
{
    if (!str) return;
    while (*str && g_reportLen + 1 < kReportBufferSize)
    {
        g_reportBuffer[g_reportLen++] = *str++;
    }
    g_reportBuffer[g_reportLen] = '\0';
}

/// @brief 安全地将无符号整数转为十进制字符串并追加
static void _aCrashAppendUtoA(unsigned int value)
{
    char buf[32];
    int pos = 31;
    buf[pos--] = '\0';
    if (value == 0)
    {
        buf[pos--] = '0';
    }
    else
    {
        while (value > 0 && pos >= 0)
        {
            buf[pos--] = '0' + (char)(value % 10);
            value /= 10;
        }
    }
    _aCrashAppendStr(&buf[pos + 1]);
}

/// @brief 安全地将无符号整数（64位）转为十六进制字符串并追加
static void _aCrashAppendUtoHex(unsigned long long value)
{
    const char kHexChars[] = "0123456789ABCDEF";
    char buf[32];
    int pos = 31;
    buf[pos--] = '\0';

    // 始终输出 16 位十六进制
    for (int i = 0; i < 16; ++i)
    {
        buf[pos--] = kHexChars[value & 0xF];
        value >>= 4;
    }

    _aCrashAppendStr("0x");
    _aCrashAppendStr(&buf[pos + 1]);
}

/// @brief 写入当前时间戳到报告缓冲区
static void _aCrashWriteTimestamp()
{
    time_t t = time(nullptr);
    if (t == (time_t)-1)
    {
        _aCrashAppendStr("Unknown");
        return;
    }

    // 使用 gmtime 分解
    struct tm utc;
#ifdef _WIN32
    gmtime_s(&utc, &t);
#else
    gmtime_r(&t, &utc);
#endif

    char buf[32];
    // YYYY-MM-DD HH:MM:SS
    buf[0]  = '0' + (char)((utc.tm_year + 1900) / 1000);
    buf[1]  = '0' + (char)(((utc.tm_year + 1900) / 100) % 10);
    buf[2]  = '0' + (char)(((utc.tm_year + 1900) / 10) % 10);
    buf[3]  = '0' + (char)((utc.tm_year + 1900) % 10);
    buf[4]  = '-';
    buf[5]  = '0' + (char)((utc.tm_mon + 1) / 10);
    buf[6]  = '0' + (char)((utc.tm_mon + 1) % 10);
    buf[7]  = '-';
    buf[8]  = '0' + (char)(utc.tm_mday / 10);
    buf[9]  = '0' + (char)(utc.tm_mday % 10);
    buf[10] = ' ';
    buf[11] = '0' + (char)(utc.tm_hour / 10);
    buf[12] = '0' + (char)(utc.tm_hour % 10);
    buf[13] = ':';
    buf[14] = '0' + (char)(utc.tm_min / 10);
    buf[15] = '0' + (char)(utc.tm_min % 10);
    buf[16] = ':';
    buf[17] = '0' + (char)(utc.tm_sec / 10);
    buf[18] = '0' + (char)(utc.tm_sec % 10);
    buf[19] = ' ';
    buf[20] = '(';
    buf[21] = 'U';
    buf[22] = 'T';
    buf[23] = 'C';
    buf[24] = ')';
    buf[25] = '\0';

    _aCrashAppendStr(buf);
}

// ============================================================================
// 报告构建函数
// ============================================================================

/// @brief 构建崩溃报告头
static void _aCrashBuildReportHeader(const char* crashType, const void* faultAddr)
{
    g_reportBuffer[0] = '\0';
    g_reportLen = 0;

    _aCrashAppendStr("================================================================"
                     "========\n");
    _aCrashAppendStr("                         AST Crash Report\n");
    _aCrashAppendStr("                   Generated by AstFault\n");
    _aCrashAppendStr("================================================================"
                     "========\n\n");

    _aCrashAppendStr("[Crash Information]\n");
    _aCrashAppendStr("  Timestamp    : ");
    _aCrashWriteTimestamp();
    _aCrashAppendStr("\n");
    _aCrashAppendStr("  Crash Type   : ");
    _aCrashAppendStr(crashType ? crashType : "Unknown");
    _aCrashAppendStr("\n");
    _aCrashAppendStr("  Fault Address: ");
    _aCrashAppendUtoHex((unsigned long long)(uintptr_t)faultAddr);
    _aCrashAppendStr("\n\n");
}

/// @brief 构建系统信息部分
static void _aCrashBuildReportSystemInfo()
{
    _aCrashAppendStr("[System Information]\n");

#ifdef _WIN32
    _aCrashAppendStr("  OS           : Windows\n");

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    _aCrashAppendStr("  CPU Cores    : ");
    _aCrashAppendUtoA(sysInfo.dwNumberOfProcessors);
    _aCrashAppendStr("\n");
#elif defined(__linux__)
    struct utsname uts;
    if (uname(&uts) == 0)
    {
        _aCrashAppendStr("  OS           : ");
        _aCrashAppendStr(uts.sysname);
        _aCrashAppendStr(" ");
        _aCrashAppendStr(uts.release);
        _aCrashAppendStr(" ");
        _aCrashAppendStr(uts.machine);
        _aCrashAppendStr("\n");
        _aCrashAppendStr("  Hostname     : ");
        _aCrashAppendStr(uts.nodename);
        _aCrashAppendStr("\n");
    }
#else
    _aCrashAppendStr("  OS           : Unix\n");
#endif
    _aCrashAppendStr("\n");
}

/// @brief 构建构建信息部分
static void _aCrashBuildReportBuildInfo()
{
    _aCrashAppendStr("[Build Information]\n");
#ifdef NDEBUG
    _aCrashAppendStr("  Build Type   : Release\n");
#else
    _aCrashAppendStr("  Build Type   : Debug\n");
#endif

#ifdef A_MSVC
    _aCrashAppendStr("  Compiler     : MSVC ");
    _aCrashAppendUtoA((unsigned int)A_MSVC);
    _aCrashAppendStr("\n");
#elif defined(A_CLANG)
    _aCrashAppendStr("  Compiler     : Clang ");
    _aCrashAppendUtoA((unsigned int)A_CLANG);
    _aCrashAppendStr("\n");
#elif defined(A_GCC)
    _aCrashAppendStr("  Compiler     : GCC ");
    _aCrashAppendUtoA((unsigned int)A_GCC);
    _aCrashAppendStr("\n");
#endif

#ifdef A_WINDOWS
    _aCrashAppendStr("  Platform     : Windows\n");
#elif defined(A_LINUX)
    _aCrashAppendStr("  Platform     : Linux\n");
#elif defined(A_APPLE)
    _aCrashAppendStr("  Platform     : macOS\n");
#elif defined(A_WASM)
    _aCrashAppendStr("  Platform     : WebAssembly\n");
#endif
    _aCrashAppendStr("\n");
}

/// @brief 构建堆栈跟踪部分（仅输出原始地址，不做符号解析）
static void _aCrashBuildReportStackTrace(int frameCount)
{
    _aCrashAppendStr("[Stack Trace (raw addresses)]\n");
    if (frameCount <= 0)
    {
        _aCrashAppendStr("  (no stack trace available)\n");
    }
    else
    {
        for (int i = 0; i < frameCount; ++i)
        {
            _aCrashAppendStr("  #");
            if (i < 10) _aCrashAppendChar('0');
            _aCrashAppendUtoA((unsigned int)i);
            _aCrashAppendStr(" ");
            _aCrashAppendUtoHex((unsigned long long)(uintptr_t)g_stackFrames[i]);
            _aCrashAppendStr("\n");
        }
    }
    _aCrashAppendStr("\n");
}

#ifdef __linux__
/// @brief 读取 /proc/self/maps 并追加到报告（Linux）
static void _aCrashBuildReportModuleInfo()
{
    _aCrashAppendStr("[Loaded Modules]\n");

    int fd = ::open("/proc/self/maps", O_RDONLY);
    if (fd < 0)
    {
        _aCrashAppendStr("  (unable to read /proc/self/maps)\n");
        return;
    }

    char buf[256];
    ssize_t n;
    size_t total = 0;
    while ((n = ::read(fd, buf, sizeof(buf) - 1)) > 0 && total < kModuleInfoSize)
    {
        for (ssize_t i = 0; i < n && total < kModuleInfoSize; ++i, ++total)
        {
            _aCrashAppendChar(buf[i]);
        }
    }
    ::close(fd);
    _aCrashAppendStr("\n");
}
#else
/// @brief 构建模块信息部分（非 Linux 平台：简化或空）
static void _aCrashBuildReportModuleInfo()
{
    _aCrashAppendStr("[Loaded Modules]\n");
#ifdef _WIN32
    // 在 SEH 上下文中获取模块列表
    HMODULE hMods[64];
    DWORD cbNeeded;
    HANDLE hProcess = GetCurrentProcess();
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        int count = (int)(cbNeeded / sizeof(HMODULE));
        for (int i = 0; i < count; ++i)
        {
            MODULEINFO modInfo;
            if (GetModuleInformation(hProcess, hMods[i], &modInfo, sizeof(modInfo)))
            {
                // 获取模块名
                char nameBuf[kMaxPath] = {0};
                if (GetModuleBaseNameA(hProcess, hMods[i], nameBuf, sizeof(nameBuf) - 1))
                {
                    _aCrashAppendStr("  ");
                    _aCrashAppendUtoHex((unsigned long long)(uintptr_t)modInfo.lpBaseOfDll);
                    _aCrashAppendStr("  ");
                    _aCrashAppendStr(nameBuf);
                    _aCrashAppendStr("\n");
                }
            }
        }
    }
    else
    {
        _aCrashAppendStr("  (unable to enumerate modules)\n");
    }
#else
    _aCrashAppendStr("  (not available on this platform)\n");
#endif
    _aCrashAppendStr("\n");
}
#endif

/// @brief 构建报告尾部
static void _aCrashBuildReportFooter(int pid)
{
    _aCrashAppendStr("================================================================"
                     "========\n");
    _aCrashAppendStr("  End of Crash Report — PID: ");
    _aCrashAppendUtoA((unsigned int)pid);
    _aCrashAppendStr("\n");
    _aCrashAppendStr("================================================================"
                     "========\n");
}

// ============================================================================
// 文件写入函数
// ============================================================================

/// @brief 安全地将报告缓冲区写入文件
/// @return true 成功，false 失败
static bool _aCrashWriteReportFile(const char* crashType, int signalNum, const void* faultAddr, int frameCount)
{
    // 构建报告
    _aCrashBuildReportHeader(crashType, faultAddr);
    _aCrashBuildReportSystemInfo();
    _aCrashBuildReportBuildInfo();
    _aCrashBuildReportStackTrace(frameCount);
    _aCrashBuildReportModuleInfo();

#ifdef _WIN32
    int pid = (int)_getpid();
#else
    int pid = (int)getpid();
#endif
    _aCrashBuildReportFooter(pid);

    // 获取当前时间用于文件名
    time_t t = time(nullptr);
    struct tm utc;
#ifdef _WIN32
    gmtime_s(&utc, &t);
#else
    gmtime_r(&t, &utc);
#endif

    // 构造文件名: crash_YYYYMMDD_HHMMSS_PID.log
    char filename[128];
    {
        int pos = 0;
        const char* prefix = "crash_";
        while (*prefix) filename[pos++] = *prefix++;
        filename[pos++] = '0' + (char)((utc.tm_year + 1900) / 1000);
        filename[pos++] = '0' + (char)(((utc.tm_year + 1900) / 100) % 10);
        filename[pos++] = '0' + (char)(((utc.tm_year + 1900) / 10) % 10);
        filename[pos++] = '0' + (char)((utc.tm_year + 1900) % 10);
        filename[pos++] = '0' + (char)((utc.tm_mon + 1) / 10);
        filename[pos++] = '0' + (char)((utc.tm_mon + 1) % 10);
        filename[pos++] = '0' + (char)(utc.tm_mday / 10);
        filename[pos++] = '0' + (char)(utc.tm_mday % 10);
        filename[pos++] = '_';
        filename[pos++] = '0' + (char)(utc.tm_hour / 10);
        filename[pos++] = '0' + (char)(utc.tm_hour % 10);
        filename[pos++] = '0' + (char)(utc.tm_min / 10);
        filename[pos++] = '0' + (char)(utc.tm_min % 10);
        filename[pos++] = '0' + (char)(utc.tm_sec / 10);
        filename[pos++] = '0' + (char)(utc.tm_sec % 10);
        filename[pos++] = '_';

        // PID 转字符串
        char pidBuf[16];
        int pidLen = 0;
        unsigned int upid = (unsigned int)pid;
        if (upid == 0) { pidBuf[0] = '0'; pidLen = 1; }
        else {
            char tmp[16]; int tmpLen = 0;
            while (upid > 0) { tmp[tmpLen++] = '0' + (char)(upid % 10); upid /= 10; }
            while (tmpLen > 0) pidBuf[pidLen++] = tmp[--tmpLen];
        }
        for (int i = 0; i < pidLen; ++i) filename[pos++] = pidBuf[i];

        filename[pos++] = '.';
        filename[pos++] = 'l';
        filename[pos++] = 'o';
        filename[pos++] = 'g';
        filename[pos] = '\0';
    }

    // 组合完整路径
    char filepath[kMaxPath];
    {
        int i = 0;
        // 复制目录路径
        const char* dir = g_logDir[0] ? g_logDir : ".";
        while (*dir && i < kMaxPath - 2) filepath[i++] = *dir++;
#ifdef _WIN32
        if (i > 0 && filepath[i-1] != '\\' && filepath[i-1] != '/') filepath[i++] = '\\';
#else
        if (i > 0 && filepath[i-1] != '/') filepath[i++] = '/';
#endif
        // 复制文件名
        const char* fn = filename;
        while (*fn && i < kMaxPath - 2) filepath[i++] = *fn++;
        filepath[i] = '\0';

        // 保存最后日志路径
        int j = 0;
        while (j < kMaxPath - 1 && filepath[j]) {
            g_lastLogPath[j] = filepath[j];
            ++j;
        }
        g_lastLogPath[j] = '\0';
    }

    // 写入文件
#ifdef _WIN32
    // Windows: 使用 CreateFileW + WriteFile
    // 将 UTF-8 路径转为宽字符
    wchar_t wfilepath[kMaxPath];
    int wlen = MultiByteToWideChar(CP_UTF8, 0, filepath, -1, wfilepath, kMaxPath);
    if (wlen <= 0) return false;

    HANDLE hFile = CreateFileW(wfilepath, GENERIC_WRITE, 0, nullptr,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        // 尝试创建目录后再试
        // (简化处理：如果打开失败，尝试在当前目录写入)
        hFile = CreateFileW(L"crash_report.log", GENERIC_WRITE, 0, nullptr,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) return false;

        // 更新 lastLogPath
        const char* fallback = "crash_report.log";
        int j = 0;
        while (j < kMaxPath - 1 && fallback[j]) { g_lastLogPath[j] = fallback[j]; ++j; }
        g_lastLogPath[j] = '\0';
    }

    DWORD written = 0;
    BOOL ok = WriteFile(hFile, g_reportBuffer, (DWORD)g_reportLen, &written, nullptr);
    CloseHandle(hFile);
    return ok != FALSE;
#else
    // Linux: 使用 ::open/::write/::close（系统调用，信号安全）
    int fd = ::open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        // 回退到当前目录
        fd = ::open("crash_report.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) return false;

        const char* fallback = "crash_report.log";
        int j = 0;
        while (j < kMaxPath - 1 && fallback[j]) { g_lastLogPath[j] = fallback[j]; ++j; }
        g_lastLogPath[j] = '\0';
    }

    ssize_t written = ::write(fd, g_reportBuffer, g_reportLen);
    ::close(fd);
    return written == (ssize_t)g_reportLen;
#endif
}

// ============================================================================
// 信号名称查找（信号安全）
// ============================================================================

/// @brief 根据信号编号获取名称
static const char* _aCrashGetSignalName(int signum)
{
    if (signum >= 0 && signum < kSignalNameCount && g_signalNames[signum])
        return g_signalNames[signum];
    return "UNKNOWN";
}

// 前置声明（定义在公共 API 区域之后）
static void _aCrashSpawnReporter(const char* logPath);

// ============================================================================
// 平台特定：Linux 信号处理器
// ============================================================================

#ifndef _WIN32

/// @brief Linux 信号处理器（extern "C" 确保正确的调用约定）
extern "C" {
static void _aCrashLinuxHandler(int signum, siginfo_t* info, void* /*ucontext*/);
}

static void _aCrashLinuxHandler(int signum, siginfo_t* info, void* /*ucontext*/)
{
    // 递归守卫
    if (g_crashRecursionGuard.fetch_add(1, std::memory_order_relaxed) > 0)
    {
        // 递归崩溃：恢复默认处理器并强制退出
        signal(signum, SIG_DFL);
        _exit(128 + signum);
    }

    // 获取故障地址
    const void* faultAddr = (info) ? info->si_addr : nullptr;

    // 获取回溯（backtrace 是 async-signal-safe 的）
    int frameCount = backtrace(g_stackFrames, kMaxStackFrames);

    // 获取信号名
    const char* crashType = _aCrashGetSignalName(signum);

    // 调用用户回调（注意：回调中不能分配内存）
    if (g_userCallback)
    {
        g_userCallback(crashType, signum, faultAddr, g_stackFrames, frameCount);
    }

    // 写崩溃报告到文件
    _aCrashWriteReportFile(crashType, signum, faultAddr, frameCount);

    // 启动崩溃报告进程（弹窗）
    _aCrashSpawnReporter(g_lastLogPath);

    // 恢复默认处理器
    signal(signum, SIG_DFL);

    // 对 SIGSEGV/SIGILL/SIGBUS/SIGFPE 重新抛出以获取 core dump
    // 对 SIGABRT 直接退出
    g_crashRecursionGuard.fetch_sub(1, std::memory_order_relaxed);
    raise(signum);
    _exit(128 + signum);
}

static errc_t _aCrashInstallPlatformHandler()
{
    // 设置备用信号栈（防止栈溢出时无法处理）
    stack_t altStack;
    altStack.ss_sp = g_altStack;
    altStack.ss_size = kAltStackSize;
    altStack.ss_flags = 0;
    if (sigaltstack(&altStack, nullptr) != 0)
    {
        // sigaltstack 失败不是致命的
    }

    // 为每个信号安装处理器
    struct sigaction sa;
    sa.sa_sigaction = _aCrashLinuxHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_NODEFER;

    for (int i = 0; i < g_crashSignalCount; ++i)
    {
        int sig = g_crashSignals[i];
        sigaction(sig, &sa, &g_prevSigActions[sig]);
    }

    return eNoError;
}

static void _aCrashUninstallPlatformHandler()
{
    // 恢复每个信号的原处理器
    for (int i = 0; i < g_crashSignalCount; ++i)
    {
        int sig = g_crashSignals[i];
        sigaction(sig, &g_prevSigActions[sig], nullptr);
    }

    // 恢复默认的 sigaltstack
    stack_t altStack;
    altStack.ss_sp = nullptr;
    altStack.ss_size = 0;
    altStack.ss_flags = SS_DISABLE;
    sigaltstack(&altStack, nullptr);
}

#else // _WIN32

// ============================================================================
// 平台特定：Windows SEH 过滤器
// ============================================================================

/// @brief 将 Windows 异常代码转为可读字符串
static const char* _aCrashGetExceptionName(DWORD exceptionCode)
{
    switch (exceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION (SIGSEGV)";
    case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";
    case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";
    case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
    case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO (SIGFPE)";
    case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";
    case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";
    case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";
    case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";
    case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO (SIGFPE)";
    case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";
    case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";
    case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";
    case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION (SIGILL)";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
    case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";
    case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";
    case 0xE06D7363:                         return "C++ Exception";
    default:                                 return "UNKNOWN EXCEPTION";
    }
}

/// @brief Windows SEH 未处理异常过滤器
static LONG __stdcall _aCrashWinSehFilter(EXCEPTION_POINTERS* exInfo)
{
    // 递归守卫
    if (g_crashRecursionGuard.fetch_add(1, std::memory_order_relaxed) > 0)
    {
        if (g_prevSehFilter)
            return g_prevSehFilter(exInfo);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    // 提取异常信息
    DWORD exceptionCode = exInfo->ExceptionRecord->ExceptionCode;
    const void* faultAddr = exInfo->ExceptionRecord->ExceptionAddress;

    // 获取回溯
    int frameCount = CaptureStackBackTrace(0, kMaxStackFrames, g_stackFrames, nullptr);

    // 获取异常名称
    const char* crashType = _aCrashGetExceptionName(exceptionCode);

    // 调用用户回调
    if (g_userCallback)
    {
        g_userCallback(crashType, (int)exceptionCode, faultAddr, g_stackFrames, frameCount);
    }

    // 写崩溃报告
    _aCrashWriteReportFile(crashType, (int)exceptionCode, faultAddr, frameCount);

    // 启动崩溃报告进程（弹窗）
    _aCrashSpawnReporter(g_lastLogPath);

    // 在返回前才递减守卫，最小化竞态窗口
    {
        LONG result = g_prevSehFilter ? g_prevSehFilter(exInfo) : EXCEPTION_CONTINUE_SEARCH;
        g_crashRecursionGuard.fetch_sub(1, std::memory_order_relaxed);
        return result;
    }
}

/// @brief Windows SIGABRT 信号处理器
static void _aCrashWinSigabrtHandler(int signum)
{
    // 递归守卫
    if (g_crashRecursionGuard.fetch_add(1, std::memory_order_relaxed) > 0)
    {
        if (g_prevSigabrtHandler)
            g_prevSigabrtHandler(signum);
        _exit(128 + signum);
    }

    const char* crashType = _aCrashGetSignalName(signum);

    // 获取回溯
    int frameCount = CaptureStackBackTrace(0, kMaxStackFrames, g_stackFrames, nullptr);

    // 调用用户回调
    if (g_userCallback)
    {
        g_userCallback(crashType, signum, nullptr, g_stackFrames, frameCount);
    }

    // 写崩溃报告
    _aCrashWriteReportFile(crashType, signum, nullptr, frameCount);

    // 启动崩溃报告进程（弹窗）
    _aCrashSpawnReporter(g_lastLogPath);

    g_crashRecursionGuard.fetch_sub(1, std::memory_order_relaxed);

    // 调用之前的处理器
    if (g_prevSigabrtHandler)
    {
        signal(SIGABRT, g_prevSigabrtHandler);
        raise(signum);
    }
    _exit(128 + signum);
}

static errc_t _aCrashInstallPlatformHandler()
{
    // 安装 SEH 未处理异常过滤器
    g_prevSehFilter = SetUnhandledExceptionFilter(_aCrashWinSehFilter);

    // 安装 SIGABRT 处理器
    g_prevSigabrtHandler = signal(SIGABRT, _aCrashWinSigabrtHandler);

    return eNoError;
}

static void _aCrashUninstallPlatformHandler()
{
    // 恢复 SEH 过滤器
    if (g_prevSehFilter)
    {
        SetUnhandledExceptionFilter(g_prevSehFilter);
        g_prevSehFilter = nullptr;
    }

    // 恢复 SIGABRT 处理器
    if (g_prevSigabrtHandler)
    {
        signal(SIGABRT, g_prevSigabrtHandler);
        g_prevSigabrtHandler = nullptr;
    }
}

#endif // _WIN32

// ============================================================================
// std::terminate 处理器（平台无关）
// ============================================================================

/// @brief 前一个 terminate 处理器
static std::terminate_handler g_prevTerminateHandler = nullptr;

/// @brief Terminate 处理器
static void _aCrashTerminateHandler()
{
    // 递归守卫
    if (g_crashRecursionGuard.fetch_add(1, std::memory_order_relaxed) > 0)
    {
        if (g_prevTerminateHandler)
            g_prevTerminateHandler();
        abort();
    }

    const char* crashType = "TERMINATE";

    // 尝试获取异常类型
    // 注意：std::current_exception() 在 terminate 处理器中可能是安全的
    // 但我们不做任何可能分配内存的操作

    // 获取回溯
    int frameCount = 0;
#ifndef _WIN32
#ifdef __linux__
    frameCount = backtrace(g_stackFrames, kMaxStackFrames);
#endif
#else
    frameCount = CaptureStackBackTrace(0, kMaxStackFrames, g_stackFrames, nullptr);
#endif

    // 调用用户回调
    if (g_userCallback)
    {
        g_userCallback(crashType, 0, nullptr, g_stackFrames, frameCount);
    }

    // 写崩溃报告
    _aCrashWriteReportFile(crashType, 0, nullptr, frameCount);

    // 启动崩溃报告进程（弹窗）
    _aCrashSpawnReporter(g_lastLogPath);

    // 注意：不在此处递减递归守卫。因为下面调用 abort() 会再次触发
    // SIGABRT 处理器，保持守卫 > 0 可让 SIGABRT 处理器跳过重复写日志。

    // 调用之前的 terminate 处理器或 abort
    if (g_prevTerminateHandler)
    {
        g_prevTerminateHandler();
    }
    abort();
}

// ============================================================================
// 公共 API 实现
// ============================================================================

errc_t aCrashHandlerInstall(ACrashCallback callback)
{
    if (g_installed)
        return eNoError;

    // 初始化日志目录
    if (g_logDir[0] == '\0')
    {
        std::string dataDir = aDataDir();
        int i = 0;
        const char* src = dataDir.c_str();
        // 注意：这里还在正常上下文中（非信号处理器），可以使用 std::string
        while (*src && i < kMaxPath - 8) g_logDir[i++] = *src++;
#ifdef _WIN32
        g_logDir[i++] = '\\';
#else
        g_logDir[i++] = '/';
#endif
        const char* crashDir = "crash";
        while (*crashDir && i < kMaxPath - 2) g_logDir[i++] = *crashDir++;
        g_logDir[i] = '\0';
    }

    // 保存回调
    g_userCallback = callback;

    // 安装平台特定的处理器
    errc_t rc = _aCrashInstallPlatformHandler();
    if (rc != eNoError)
        return rc;

    // 安装 terminate 处理器
    g_prevTerminateHandler = std::set_terminate(_aCrashTerminateHandler);

    g_installed = true;
    return eNoError;
}

void aCrashHandlerUninstall()
{
    if (!g_installed)
        return;

    // 卸载平台特定的处理器
    _aCrashUninstallPlatformHandler();

    // 恢复 terminate 处理器
    if (g_prevTerminateHandler)
    {
        std::set_terminate(g_prevTerminateHandler);
        g_prevTerminateHandler = nullptr;
    }

    g_userCallback = nullptr;
    g_installed = false;
}

errc_t aCrashHandlerSetLogDir(const char* dir)
{
    if (!dir)
        return eError;

    int i = 0;
    while (i < kMaxPath - 2 && dir[i])
    {
        g_logDir[i] = dir[i];
        ++i;
    }

    if (dir[i] != '\0')
        return eError; // 路径过长

    g_logDir[i] = '\0';
    return eNoError;
}

const char* aCrashHandlerLastLogPath()
{
    return g_lastLogPath;
}

// ============================================================================
// 崩溃报告进程启动（信号处理器末尾调用）
// ============================================================================

/// @brief 启动崩溃报告进程/显示对话框
/// @param logPath 崩溃日志文件路径
static void _aCrashSpawnReporter(const char* logPath)
{
    if (!logPath || !logPath[0]) return;

#ifdef _WIN32
    if (g_reporterCmd[0])
    {
        // 构建命令行： "<reporter>" "<logPath>"
        // 使用手动字符串操作，避免 snprintf（非信号安全）
        char cmdLine[kMaxPath * 4] = {'"'};
        int pos = 1;

        // 复制 reporter 路径
        const char* src = g_reporterCmd;
        while (*src && pos < (int)sizeof(cmdLine) - 6) cmdLine[pos++] = *src++;
        cmdLine[pos++] = '"';
        cmdLine[pos++] = ' ';
        cmdLine[pos++] = '"';

        // 复制日志路径
        src = logPath;
        while (*src && pos < (int)sizeof(cmdLine) - 3) cmdLine[pos++] = *src++;
        cmdLine[pos++] = '"';
        cmdLine[pos] = '\0';

        STARTUPINFOA si{};
        si.cb = sizeof(STARTUPINFOA);
        PROCESS_INFORMATION pi{};
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOW;

        CreateProcessA(nullptr, cmdLine, nullptr, nullptr, FALSE,
                       CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        // 默认：简单的 MessageBox（SEH 上下文中 MessageBeep+MessageBox 相对安全）
        // 使用固定消息避免 snprintf
        const char* msg1 = "The application has crashed.\n\nCrash report:\n";
        const char* msg2 = "\n\nPlease send this file to the development team.";

        // 手动拼接（logPath 最大 511，各部分总和不超过 kMaxPath*2）
        char msgBuf[kMaxPath * 2];
        int p = 0;
        const char* s = msg1;
        while (*s && p < (int)sizeof(msgBuf) - 2) msgBuf[p++] = *s++;
        s = logPath;
        while (*s && p < (int)sizeof(msgBuf) - 2) msgBuf[p++] = *s++;
        s = msg2;
        while (*s && p < (int)sizeof(msgBuf) - 2) msgBuf[p++] = *s++;
        msgBuf[p] = '\0';

        MessageBoxA(nullptr, msgBuf, "Application Crash",
                    MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    }
#else
    if (g_reporterCmd[0])
    {
        // 用户指定了报告命令：fork + exec（信号安全）
        pid_t pid = fork();
        if (pid == 0)
        {
            execl(g_reporterCmd, g_reporterCmd, logPath, (char*)nullptr);
            _exit(127);
        }
    }
    else
    {
        // 默认：写入 stderr
        const char* msg = "\n*** Application crashed. Report saved to: ";
        if (write(STDERR_FILENO, msg, strlen(msg)) < 0) {}
        if (write(STDERR_FILENO, logPath, strlen(logPath)) < 0) {}
        if (write(STDERR_FILENO, "\n", 1) < 0) {}
    }
#endif
}

errc_t aCrashSetReporterCommand(const char* command)
{
    if (!command)
    {
        g_reporterCmd[0] = '\0';
        return eNoError;
    }

    int i = 0;
    while (i < kMaxPath - 2 && command[i])
    {
        g_reporterCmd[i] = command[i];
        ++i;
    }

    if (command[i] != '\0')
        return eError;

    g_reporterCmd[i] = '\0';
    return eNoError;
}

const char* aCrashGetReporterCommand()
{
    return g_reporterCmd;
}

// ============================================================================
// 离线符号解析（正常上下文，非信号处理器）
// ============================================================================

#ifdef _WIN32

/// @brief 从崩溃日志中提取模块列表，加载额外符号
static void _aCrashWinLoadModulesFromLog(const std::vector<std::string>& lines,
                                          const char* searchDir)
{
    bool inModuleSection = false;

    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string& line = lines[i];

        if (line.find("[Loaded Modules]") != std::string::npos)
        {
            inModuleSection = true;
            continue;
        }
        if (inModuleSection && !line.empty() && line[0] == '[')
            break;
        if (!inModuleSection) continue;

        // 解析 "  0xBASE  name" 格式
        const char* p = line.c_str();
        while (*p == ' ') ++p;
        if (*p != '0') continue;

        DWORD64 baseAddr = (DWORD64)strtoull(p, nullptr, 16);
        while (*p && *p != ' ') ++p;
        while (*p == ' ') ++p;

        if (*p)
        {
            std::string moduleName = p;
            while (!moduleName.empty() && (moduleName.back() == '\r' || moduleName.back() == '\n'))
                moduleName.pop_back();

            if (!moduleName.empty())
            {
                // 尝试用完整路径加载（搜索顺序：crash日志目录→AstFault所在目录→仅模块名）
                DWORD64 loadBase = 0;

                // 1) 从 crash 日志目录搜索
                if (searchDir && searchDir[0])
                {
                    std::string fullPath = std::string(searchDir) + "\\" + moduleName;
                    loadBase = SymLoadModuleEx(GetCurrentProcess(), nullptr,
                                                fullPath.c_str(), moduleName.c_str(),
                                                baseAddr, 0, nullptr, 0);
                }

                // 2) 从 AstFault.dll 自身所在目录搜索
                if (loadBase == 0)
                {
                    char selfDir[512] = {0};
                    HMODULE hSelf = nullptr;
                    // 获取当前模块（AstFault.dll）的路径
                    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                       (LPCSTR)&_aCrashWinLoadModulesFromLog, &hSelf);
                    if (hSelf)
                    {
                        GetModuleFileNameA(hSelf, selfDir, sizeof(selfDir) - 1);
                        char* lastSep = strrchr(selfDir, '\\');
                        if (lastSep) *lastSep = '\0';
                        std::string fullPath = std::string(selfDir) + "\\" + moduleName;
                        loadBase = SymLoadModuleEx(GetCurrentProcess(), nullptr,
                                                    fullPath.c_str(), moduleName.c_str(),
                                                    baseAddr, 0, nullptr, 0);
                    }
                }

                // 3) 仅用模块名（让系统自动搜索）
                if (loadBase == 0)
                {
                    SymLoadModuleEx(GetCurrentProcess(), nullptr,
                                    moduleName.c_str(), moduleName.c_str(),
                                    baseAddr, 0, nullptr, 0);
                }
            }
        }
    }
}

/// @brief 初始化 Windows 符号处理器
static bool _aCrashWinSymInit(const char* extraPath)
{
    HANDLE hProcess = GetCurrentProcess();

    // 构建符号搜索路径：额外目录 + 当前目录 + MS 符号服务器
    std::string symPath;
    if (extraPath && extraPath[0])
    {
        symPath = extraPath;
        symPath += ";";
    }

    // 加入 AstFault.dll 自身所在目录（通常也是其他项目 .exe/.pdb 所在目录）
    char selfDir[512] = {0};
    HMODULE hSelf = nullptr;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCSTR)&_aCrashWinSymInit, &hSelf);
    if (hSelf)
    {
        GetModuleFileNameA(hSelf, selfDir, sizeof(selfDir) - 1);
        char* lastSep = strrchr(selfDir, '\\');
        if (lastSep) *lastSep = '\0';
        symPath += selfDir;
        symPath += ";";
    }

    symPath += ".;"
               "srv*https://msdl.microsoft.com/download/symbols";

    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    if (SymInitialize(hProcess, symPath.c_str(), TRUE))
        return true;
    return false;
}

/// @brief 清理 Windows 符号处理器
static void _aCrashWinSymCleanup()
{
    SymCleanup(GetCurrentProcess());
}

/// @brief 解析单个 Windows 地址为符号字符串
static std::string _aCrashWinResolveAddress(DWORD64 addr)
{
    HANDLE hProcess = GetCurrentProcess();

    // 分配符号信息缓冲区
    char symBuf[sizeof(SYMBOL_INFO) + 256];
    SYMBOL_INFO* sym = (SYMBOL_INFO*)symBuf;
    sym->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym->MaxNameLen = 255;

    std::string result;

    DWORD64 displacement = 0;
    if (SymFromAddr(hProcess, addr, &displacement, sym))
    {
        result += sym->Name;
        if (displacement > 0)
        {
            char dispBuf[32];
            snprintf(dispBuf, sizeof(dispBuf), "+0x%llX", (unsigned long long)displacement);
            result += dispBuf;
        }
    }

    // 尝试获取源文件和行号
    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    DWORD lineDisp = 0;
    if (SymGetLineFromAddr64(hProcess, addr, &lineDisp, &line))
    {
        char lineBuf[256];
        snprintf(lineBuf, sizeof(lineBuf), "  [%s:%lu]", line.FileName, (unsigned long)line.LineNumber);
        result += lineBuf;
    }

    return result;
}

#else // Linux / POSIX

/// @brief 解析单个 Linux 地址为符号字符串
static std::string _aCrashLinuxResolveAddress(uintptr_t addr)
{
    std::string result;

    // 方法1: 使用 dladdr 获取函数名
    Dl_info info = {};
    if (dladdr((const void*)addr, &info) && info.dli_sname)
    {
        result += info.dli_sname;

        // 计算偏移
        uintptr_t offset = addr - (uintptr_t)info.dli_saddr;
        if (offset > 0)
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "+0x%lX", (unsigned long)offset);
            result += buf;
        }

        // 如果知道是哪个 .so，也标注
        if (info.dli_fname)
        {
            result += "  (";
            result += info.dli_fname;
            result += ")";
        }
    }

    // 方法2: 尝试 addr2line 获取文件名和行号（需要调试符号）
    // 仅当 dladdr 得到了二进制路径时才尝试
    if (info.dli_fname && info.dli_fname[0] && !result.empty())
    {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "addr2line -e \"%s\" -f -C -p 0x%lX 2>/dev/null",
                 info.dli_fname, (unsigned long)addr);

        FILE* pipe = popen(cmd, "r");
        if (pipe)
        {
            char lineBuf[256];
            if (fgets(lineBuf, sizeof(lineBuf), pipe))
            {
                // 去掉末尾换行
                size_t len = strlen(lineBuf);
                if (len > 0 && lineBuf[len - 1] == '\n') lineBuf[len - 1] = '\0';
                if (len > 0 && lineBuf[0])
                {
                    result += "  [";
                    result += lineBuf;
                    result += "]";
                }
            }
            pclose(pipe);
        }
    }

    return result;
}

#endif // _WIN32

/// @brief 判断一行是否为堆栈帧行（格式: "  #NN 0x..."）
static bool _aCrashIsStackFrameLine(const char* line, uintptr_t& outAddr)
{
    // 匹配 "  #NN 0x..."
    if (!line) return false;

    // 跳过前导空格
    const char* p = line;
    while (*p == ' ') ++p;
    if (*p != '#') return false;
    ++p;

    // 跳过多余的 '0'（如 #00）
    while (*p >= '0' && *p <= '9') ++p;

    // 跳过空格
    while (*p == ' ') ++p;

    // 匹配 "0x" 或 "0X"
    if (p[0] != '0' || (p[1] != 'x' && p[1] != 'X')) return false;
    p += 2;

    // 解析十六进制地址
    char* end = nullptr;
    outAddr = (uintptr_t)strtoull(p, &end, 16);

    return end > p;
}

/// @brief 解析地址并附加符号信息到行尾
static std::string _aCrashAnnotateLine(const std::string& line)
{
    uintptr_t addr = 0;
    if (!_aCrashIsStackFrameLine(line.c_str(), addr))
        return line;

    // 去掉行尾换行
    std::string result = line;
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
        result.pop_back();

#ifdef _WIN32
    std::string sym = _aCrashWinResolveAddress((DWORD64)addr);
#else
    std::string sym = _aCrashLinuxResolveAddress(addr);
#endif

    if (!sym.empty())
    {
        result += "  ";
        result += sym;
    }

    result += '\n';
    return result;
}

errc_t aCrashResolveSymbols(const char* logPath, const char* outPath)
{
    if (!logPath)
        return eError;

    // 读取原始日志文件
    FILE* fin = fopen(logPath, "r");
    if (!fin)
        return eError;

    std::vector<std::string> lines;
    char buf[1024];
    while (fgets(buf, sizeof(buf), fin))
        lines.push_back(std::string(buf));
    fclose(fin);

    // 提取日志所在目录作为符号搜索路径
    std::string logDir;
    {
        const char* lastSlash = nullptr;
        for (const char* p = logPath; *p; ++p)
            if (*p == '/' || *p == '\\') lastSlash = p;
        if (lastSlash)
            logDir.assign(logPath, lastSlash - logPath);
    }

#ifdef _WIN32
    // 初始化符号处理器（自动加载当前进程的所有模块）
    if (!_aCrashWinSymInit(logDir.c_str()))
        return eError;

    // 从崩溃日志的 [Loaded Modules] 中加载额外模块符号
    _aCrashWinLoadModulesFromLog(lines, logDir.c_str());
#endif

    // 确定输出路径
    const char* actualOutPath = (outPath && outPath[0]) ? outPath : logPath;

    // 写入输出文件
    FILE* fout = fopen(actualOutPath, "w");
    if (!fout)
    {
#ifdef _WIN32
        _aCrashWinSymCleanup();
#endif
        return eError;
    }

    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::string annotated = _aCrashAnnotateLine(lines[i]);
        fputs(annotated.c_str(), fout);
    }

    fclose(fout);

#ifdef _WIN32
    _aCrashWinSymCleanup();
#endif

    return eNoError;
}

AST_NAMESPACE_END
