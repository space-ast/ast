///
/// @file      testCrashHandler.cpp
/// @brief     AstFault 崩溃处理器单元测试
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

#include "ast/AstFault.hpp"
#include "ast/AstTestMacro.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#ifdef _WIN32
#   include <windows.h>
#   include <process.h>
#else
#   include <sys/wait.h>
#   include <unistd.h>
#endif

AST_USING_NAMESPACE

// ============================================================================
// 子进程崩溃测试模式
// ============================================================================
// 当以 --crash-test=<type> 参数启动时，直接触发对应类型的崩溃并退出。
// 正常模式下运行 gtest 测试用例。
// ============================================================================

/// @brief 获取临时测试目录
static std::string getTestLogDir()
{
#ifdef _WIN32
    const char* tmp = getenv("TEMP");
    if (!tmp) tmp = ".";
    return std::string(tmp) + "\\ast_crash_test";
#else
    const char* tmp = getenv("TMPDIR");
    if (!tmp) tmp = "/tmp";
    return std::string(tmp) + "/ast_crash_test";
#endif
}

/// @brief 清理测试日志目录中的所有崩溃日志
static void cleanTestLogDir()
{
    std::string dir = getTestLogDir();
#ifdef _WIN32
    std::string cmd = "del /q \"" + dir + "\\crash_*.log\" 2>nul";
#else
    std::string cmd = "rm -f \"" + dir + "/crash_*.log\" 2>/dev/null";
#endif
    system(cmd.c_str());

    // 确保目录存在
#ifdef _WIN32
    cmd = "mkdir \"" + dir + "\" 2>nul";
#else
    cmd = "mkdir -p \"" + dir + "\" 2>/dev/null";
#endif
    system(cmd.c_str());
}

/// @brief 运行子进程触发指定类型的崩溃，返回退出码
static int runChildCrash(const char* crashType)
{
    std::string exePath;
#ifdef _WIN32
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    exePath = buf;
#else
    char buf[512];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n > 0) { buf[n] = '\0'; exePath = buf; }
#endif

    std::string logDir = getTestLogDir();
    std::string cmd = "\"" + exePath + "\""
                    + " --crash-test=" + crashType
                    + " --crash-log-dir=" + logDir;
#ifdef _WIN32
    cmd += " >nul 2>&1";
#else
    cmd += " >/dev/null 2>&1";
#endif

    return system(cmd.c_str());
}

/// @brief 检查崩溃日志是否包含预期的崩溃类型
static bool crashLogContains(const char* expectedType)
{
    std::string dir = getTestLogDir();
#ifdef _WIN32
    std::string pattern = dir + "\\crash_*.log";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(pattern.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    do {
        std::string fpath = dir + "\\" + findData.cFileName;
        FILE* f = fopen(fpath.c_str(), "r");
        if (f)
        {
            char lineBuf[512];
            while (fgets(lineBuf, sizeof(lineBuf), f))
            {
                if (strstr(lineBuf, "Crash Type") && strstr(lineBuf, expectedType))
                {
                    found = true;
                    break;
                }
            }
            fclose(f);
        }
        if (found) break;
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);
    return found;
#else
    // 使用简单的 shell 检查
    std::string expect(expectedType);
    std::string cmd = "grep -l '" + expect + "' \"" + dir + "\"/crash_*.log >/dev/null 2>&1";
    return system(cmd.c_str()) == 0;
#endif
}

// ============================================================================
// 子进程模式：直接触发崩溃
// ============================================================================

/// @brief 安装处理器并触发指定类型的崩溃
/// @note 此函数不会返回
static void crash_test_trigger(const char* crashType, const char* logDir, const char* reporter)
{
    aCrashHandlerSetLogDir(logDir);
    if (reporter && reporter[0])
        aCrashSetReporterCommand(reporter);
    aCrashHandlerInstall(nullptr);

    if (strcmp(crashType, "segfault") == 0)
    {
        volatile int* p = (volatile int*)nullptr;
        *p = 0; // SIGSEGV
    }
    else if (strcmp(crashType, "abort") == 0)
    {
        abort(); // SIGABRT
    }
    else if (strcmp(crashType, "terminate") == 0)
    {
        struct TestException {};
        throw TestException(); // std::terminate
    }

    // 不应到达这里
    exit(99);
}

// ============================================================================
// 生命周期测试
// ============================================================================

TEST(CrashHandler, InstallUninstall)
{
    EXPECT_EQ(eNoError, aCrashHandlerInstall(nullptr));
    aCrashHandlerUninstall();
    // 重复卸载安全
    aCrashHandlerUninstall();
    SUCCEED();
}

TEST(CrashHandler, DuplicateInstall)
{
    EXPECT_EQ(eNoError, aCrashHandlerInstall(nullptr));
    // 重复安装应安全返回
    EXPECT_EQ(eNoError, aCrashHandlerInstall(nullptr));
    aCrashHandlerUninstall();
}

TEST(CrashHandler, SetLogDir)
{
    EXPECT_EQ(eNoError, aCrashHandlerInstall(nullptr));

    std::string testDir = getTestLogDir();
    EXPECT_EQ(eNoError, aCrashHandlerSetLogDir(testDir.c_str()));

    // 无崩溃时返回空字符串
    const char* lastLog = aCrashHandlerLastLogPath();
    ASSERT_NE(nullptr, lastLog);
    EXPECT_STREQ("", lastLog);

    aCrashHandlerUninstall();
}

TEST(CrashHandler, SetLogDirNull)
{
    EXPECT_EQ(eNoError, aCrashHandlerInstall(nullptr));
    EXPECT_EQ(eError, aCrashHandlerSetLogDir(nullptr));
    aCrashHandlerUninstall();
}

TEST(CrashHandler, ReinstallAfterUninstall)
{
    EXPECT_EQ(eNoError, aCrashHandlerInstall(nullptr));
    aCrashHandlerUninstall();

    EXPECT_EQ(eNoError, aCrashHandlerInstall(nullptr));
    aCrashHandlerUninstall();
}

TEST(CrashHandler, UninstallWithoutInstall)
{
    // 未安装时卸载应该是安全的
    aCrashHandlerUninstall();
    SUCCEED();
}

// ============================================================================
// 子进程崩溃测试（需要 fork/spawn 支持）
// ============================================================================

TEST(CrashHandler, SegfaultWriteLog)
{
    cleanTestLogDir();
    int rc = runChildCrash("segfault");
    // 子进程崩溃，退出码非 0
    A_UNUSED(rc);
    EXPECT_TRUE(crashLogContains("SIGSEGV"));
}

TEST(CrashHandler, AbortWriteLog)
{
    cleanTestLogDir();
    int rc = runChildCrash("abort");
    A_UNUSED(rc);
    EXPECT_TRUE(crashLogContains("SIGABRT"));
}

TEST(CrashHandler, TerminateWriteLog)
{
    cleanTestLogDir();
    int rc = runChildCrash("terminate");
    A_UNUSED(rc);
    EXPECT_TRUE(crashLogContains("TERMINATE"));
}

TEST(CrashHandler, ResolveSymbols)
{
    cleanTestLogDir();

    // 先生成一份崩溃日志
    int rc = runChildCrash("segfault");
    A_UNUSED(rc);
    EXPECT_TRUE(crashLogContains("SIGSEGV"));

    // 找到生成的日志文件
    std::string dir = getTestLogDir();
    std::string logPath;
#ifdef _WIN32
    std::string pattern = dir + "\\crash_*.log";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(pattern.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        logPath = dir + "\\" + findData.cFileName;
        FindClose(hFind);
    }
#else
    // 简单方式：找最新文件
    std::string cmd = "ls -t \"" + dir + "\"/crash_*.log 2>/dev/null | head -1";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe)
    {
        char buf[512];
        if (fgets(buf, sizeof(buf), pipe))
        {
            logPath = buf;
            // 去掉末尾换行
            while (!logPath.empty() && (logPath.back() == '\n' || logPath.back() == '\r'))
                logPath.pop_back();
        }
        pclose(pipe);
    }
#endif

    ASSERT_FALSE(logPath.empty());

    // 调用符号解析
    std::string resolvedPath = logPath + ".resolved";
    errc_t result = aCrashResolveSymbols(logPath.c_str(), resolvedPath.c_str());
    EXPECT_EQ(eNoError, result);

    // 检查解析后的文件是否存在且可以读取
    FILE* f = fopen(resolvedPath.c_str(), "r");
    ASSERT_NE(nullptr, f);

    bool hasFrameLine = false;
    char lineBuf[512];
    while (fgets(lineBuf, sizeof(lineBuf), f))
    {
        // 检查是否仍然包含堆栈帧信息
        if (strstr(lineBuf, "#00 0x") || strstr(lineBuf, "#0 0x"))
            hasFrameLine = true;
    }
    fclose(f);

    EXPECT_TRUE(hasFrameLine);

    // 清理
    remove(resolvedPath.c_str());
}

// ============================================================================
// main — 处理子进程模式或运行测试
// ============================================================================

int main(int argc, char** argv)
{
    if(aIsCI())
        return 0;

    // 检查是否是子进程崩溃模式
    const char* crashType = nullptr;
    const char* logDir = nullptr;
    const char* reporter = nullptr;

    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "--crash-test=", 13) == 0)
            crashType = argv[i] + 13;
        else if (strncmp(argv[i], "--crash-log-dir=", 16) == 0)
            logDir = argv[i] + 16;
        else if (strncmp(argv[i], "--reporter=", 11) == 0)
            reporter = argv[i] + 11;
    }

    if (crashType)
    {
        if (!logDir)
            logDir = getTestLogDir().c_str();
        crash_test_trigger(crashType, logDir, reporter);
        return 1; // 不应到达这里
    }

    // 检查是否是符号解析模式
    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "--resolve=", 10) == 0)
        {
            const char* logPath = argv[i] + 10;
            std::string outPath = std::string(logPath) + ".resolved";
            errc_t rc = aCrashResolveSymbols(logPath, outPath.c_str());
            std::printf("Resolve returned: %d\n", (int)rc);
            std::printf("Output: %s\n", outPath.c_str());

            // 打印堆栈帧
            FILE* f = fopen(outPath.c_str(), "r");
            if (f)
            {
                char buf[512];
                while (fgets(buf, sizeof(buf), f))
                {
                    if (buf[0] == ' ' && buf[1] == ' ' && buf[2] == '#')
                        std::printf("%s", buf);
                }
                fclose(f);
            }
            return (rc == eNoError) ? 0 : 1;
        }
    }

    // 正常测试模式
    std::printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
