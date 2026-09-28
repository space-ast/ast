/// @file      testFileLock.cpp
/// @brief     FileLock 单元测试
/// @details   覆盖构造建文件、加锁/解锁、排他与共享互斥、带超时加锁、析构自动释放、
///            路径非法时的错误码，并通过重新启动本测试程序（子进程）验证跨进程互斥。
/// @author    axel
/// @date      2026-09-25
/// @copyright 版权所有 (C) 2026-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/FileLock.hpp"
#include "ast/FileSystem.hpp"
#include "ast/AstTestMacro.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <system_error>

#if !defined(_WIN32)
#   include <cerrno>
#   include <time.h>
#endif

AST_USING_NAMESPACE

namespace {

/// @brief 子进程参数：需要持有的锁文件路径
const char* const kHoldArgPrefix = "--filelock-hold=";
/// @brief 子进程参数：就绪标志文件路径（子进程持锁后创建）
const char* const kReadyArgPrefix = "--filelock-ready=";
/// @brief 子进程参数：结束标志文件路径（子进程退出前创建）
const char* const kDoneArgPrefix = "--filelock-done=";
/// @brief 子进程持锁时间 [ms]
const int kChildHoldMs = 1500;
/// @brief 父进程等待子进程就绪的最长时间 [ms]
const int kWaitReadyMs = 5000;
/// @brief 父进程等待子进程结束的最长时间 [ms]
const int kWaitDoneMs = 10000;

std::string makeLockPath(const std::string& name)
{
    return "FileLock_" + name + ".lock";
}

std::string makeFlagPath(const std::string& name, const char* suffix)
{
    return "FileLock_" + name + suffix;
}

/// @brief 睡眠指定的毫秒数
/// @note 这里不用 std::thread，避免测试程序对较新的 libstdc++ 符号产生依赖
void sleepMs(int milliseconds)
{
#if defined(_WIN32)
    ::_sleep(static_cast<unsigned long>(milliseconds));
#else
    timespec interval;
    interval.tv_sec = milliseconds / 1000;
    interval.tv_nsec = static_cast<long>(milliseconds % 1000) * 1000000L;
    while (::nanosleep(&interval, &interval) != 0 && errno == EINTR) {
        // 被信号打断，按剩余时间继续等待
    }
#endif
}

void writeTextFile(const std::string& path, const std::string& text)
{
    std::ofstream file(path);
    file << text;
}

void removeFileQuietly(const std::string& path)
{
    std::error_code ec;
    fs::remove(path, ec);
}

/// @brief 等待标志文件出现
bool waitForFlag(const std::string& path, int timeoutMs)
{
    for (int waited = 0; waited < timeoutMs; waited += 20) {
        if (fs::exists(path))
            return true;
        sleepMs(20);
    }
    return fs::exists(path);
}

/// @brief 拼装后台启动子进程的命令行
/// @details Windows 用 start /b，其它平台用 shell 的 & 后台执行；
///          子进程继承本进程的当前工作目录，所以可以使用相对路径。
std::string makeChildCommand(const std::string& exePath, const std::string& lockPath,
                             const std::string& readyPath, const std::string& donePath)
{
    std::string args = " \"" + exePath + "\""
        + " " + kHoldArgPrefix + lockPath
        + " " + kReadyArgPrefix + readyPath
        + " " + kDoneArgPrefix + donePath;
#if defined(_WIN32)
    return "start /b \"\"" + args + " >nul 2>&1";
#else
    return args + " >/dev/null 2>&1 &";
#endif
}

/// @brief 子进程体：持有锁一段时间，供父进程验证跨进程互斥
/// @return 0 表示正常完成，其它值表示失败
int runHoldChild(const std::string& lockPath, const std::string& readyPath, const std::string& donePath)
{
    FileLock lock(lockPath);
    if (lock.lock() != eNoError)
        return 1;
    writeTextFile(readyPath, "ready");
    sleepMs(kChildHoldMs);
    if (lock.unlock() != eNoError)
        return 2;
    writeTextFile(donePath, "done");
    return 0;
}

} // namespace

// 构造时创建锁文件
TEST(FileLock, ConstructorCreatesFile)
{
    const std::string path = makeLockPath("construct");
    removeFileQuietly(path);
    {
        FileLock lock(path);
        EXPECT_TRUE(fs::exists(path));
    }
    removeFileQuietly(path);
}


// 同一进程内不同句柄之间的排他互斥
TEST(FileLock, TryLockConflictWithinProcess)
{
    const std::string path = makeLockPath("conflict");
    removeFileQuietly(path);
    {
        FileLock first(path);
        FileLock second(path);
        EXPECT_EQ(eNoError, first.tryLock());
        EXPECT_EQ(static_cast<errc_t>(FileLock::eBusy), second.tryLock());
        EXPECT_EQ(eNoError, first.unlock());
        EXPECT_EQ(eNoError, second.tryLock());
        EXPECT_EQ(eNoError, second.unlock());
    }
    removeFileQuietly(path);
}

// 共享锁之间兼容，但与排他锁互斥
TEST(FileLock, SharedLockCompatibility)
{
    const std::string path = makeLockPath("shared");
    removeFileQuietly(path);
    {
        FileLock shared1(path);
        FileLock shared2(path);
        FileLock exclusive(path);
        EXPECT_EQ(eNoError, shared1.tryLock(FileLock::eShared));
        EXPECT_EQ(eNoError, shared2.tryLock(FileLock::eShared));
        EXPECT_EQ(static_cast<errc_t>(FileLock::eBusy), exclusive.tryLock(FileLock::eExclusive));
        EXPECT_EQ(eNoError, shared1.unlock());
        // shared2 仍持共享锁，排他锁依然拿不到
        EXPECT_EQ(static_cast<errc_t>(FileLock::eBusy), exclusive.tryLock(FileLock::eExclusive));
        EXPECT_EQ(eNoError, shared2.unlock());
        EXPECT_EQ(eNoError, exclusive.tryLock(FileLock::eExclusive));
        EXPECT_EQ(eNoError, exclusive.unlock());
    }
    removeFileQuietly(path);
}

// 带超时加锁：等待期间锁一直被占用则超时返回 eBusy
TEST(FileLock, TryLockTimeoutExpires)
{
    const std::string path = makeLockPath("timeout");
    removeFileQuietly(path);
    {
        FileLock holder(path);
        FileLock waiter(path);
        EXPECT_EQ(eNoError, holder.tryLock());
        const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        const errc_t result = waiter.tryLock(0.2);
        const double elapsed =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - begin).count();
        EXPECT_EQ(static_cast<errc_t>(FileLock::eBusy), result);
        EXPECT_GE(elapsed, 0.15);               // 确实等待过，而不是立即返回
        EXPECT_LT(elapsed, 5.0);
    }
    removeFileQuietly(path);
}

// 带超时加锁：锁释放后能及时获得
TEST(FileLock, TryLockWaitsForRelease)
{
    const std::string path = makeLockPath("wait_release");
    removeFileQuietly(path);
    {
        FileLock holder(path);
        FileLock waiter(path);
        EXPECT_EQ(eNoError, holder.tryLock());
        EXPECT_EQ(static_cast<errc_t>(FileLock::eBusy), waiter.tryLock(0.1));
        EXPECT_EQ(eNoError, holder.unlock());
        const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        EXPECT_EQ(eNoError, waiter.tryLock(1.0));
        const double elapsed =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - begin).count();
        EXPECT_LT(elapsed, 0.5);
        EXPECT_EQ(eNoError, waiter.unlock());
    }
    removeFileQuietly(path);
}

// 析构时自动释放锁
TEST(FileLock, DestructorReleasesLock)
{
    const std::string path = makeLockPath("destructor");
    removeFileQuietly(path);
    {
        FileLock lock(path);
        EXPECT_EQ(eNoError, lock.tryLock());
    }
    {
        FileLock probe(path);
        EXPECT_EQ(eNoError, probe.tryLock());
        EXPECT_EQ(eNoError, probe.unlock());
    }
    removeFileQuietly(path);
}

// 路径非法（目录不存在、空路径）时打不开锁文件，加锁返回 eError
TEST(FileLock, InvalidPathFailsLock)
{
    FileLock badDir(makeLockPath("no_such_dir/nested"));
    EXPECT_EQ(static_cast<errc_t>(FileLock::eError), badDir.tryLock());
    EXPECT_EQ(static_cast<errc_t>(FileLock::eError), badDir.lock());
    // 打开失败应当立即返回 eError，而不是等满超时后报 eBusy
    EXPECT_EQ(static_cast<errc_t>(FileLock::eError), badDir.tryLock(0.1));

    FileLock emptyPath(StringView{});
    EXPECT_EQ(static_cast<errc_t>(FileLock::eError), emptyPath.tryLock());
    EXPECT_EQ(static_cast<errc_t>(FileLock::eError), emptyPath.lock());
}

// 跨进程互斥：启动本测试程序的另一个实例持锁，验证本进程加锁失败
TEST(FileLock, CrossProcessExclusive)
{
#if defined(A_WASM) || defined(__EMSCRIPTEN__)
    GTEST_SKIP();               // WebAssembly 不支持跨进程文件锁
#else
    const std::string exePath = aExePath();
    if (exePath.empty()) {
        GTEST_SKIP();
    }
    if (exePath.empty()) {
        return;                 // 内置测试框架的 GTEST_SKIP() 不会返回，这里显式退出
    }

    const std::string lockPath = makeLockPath("cross_process");
    const std::string readyPath = makeFlagPath("cross_process", ".ready");
    const std::string donePath = makeFlagPath("cross_process", ".done");
    removeFileQuietly(lockPath);
    removeFileQuietly(readyPath);
    removeFileQuietly(donePath);

    const std::string command = makeChildCommand(exePath, lockPath, readyPath, donePath);
    std::system(command.c_str());

    const bool ready = waitForFlag(readyPath, kWaitReadyMs);
    EXPECT_TRUE(ready);
    if (ready) {
        FileLock probe(lockPath);
        // 子进程正持有排他锁
        EXPECT_EQ(static_cast<errc_t>(FileLock::eBusy), probe.tryLock());
    }

    const bool done = waitForFlag(donePath, kWaitDoneMs);
    EXPECT_TRUE(done);
    if (done) {
        FileLock probe(lockPath);
        EXPECT_EQ(eNoError, probe.tryLock());   // 子进程释放后本进程可以加锁
        EXPECT_EQ(eNoError, probe.unlock());
    }

    removeFileQuietly(lockPath);
    removeFileQuietly(readyPath);
    removeFileQuietly(donePath);
#endif
}

int main(int argc, char** argv)
{
    // 作为子进程运行时：持锁一段时间，用于验证跨进程互斥
    std::string holdPath;
    std::string readyPath;
    std::string donePath;
    const size_t holdLen = std::strlen(kHoldArgPrefix);
    const size_t readyLen = std::strlen(kReadyArgPrefix);
    const size_t doneLen = std::strlen(kDoneArgPrefix);
    for (int i = 1; i < argc; ++i) {
        if (std::strncmp(argv[i], kHoldArgPrefix, holdLen) == 0)
            holdPath = argv[i] + holdLen;
        else if (std::strncmp(argv[i], kReadyArgPrefix, readyLen) == 0)
            readyPath = argv[i] + readyLen;
        else if (std::strncmp(argv[i], kDoneArgPrefix, doneLen) == 0)
            donePath = argv[i] + doneLen;
    }
    if (!holdPath.empty() && !readyPath.empty() && !donePath.empty())
        return runHoldChild(holdPath, readyPath, donePath);

    std::printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
