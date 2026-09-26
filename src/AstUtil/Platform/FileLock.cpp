///
/// @file      FileLock.cpp
/// @brief     跨进程文件锁实现
/// @details   Windows 使用 LockFileEx 锁定锁文件的一段字节，POSIX 使用 flock，
///            WebAssembly 不支持跨进程加锁。
/// @author    axel
/// @date      2026-09-24
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "FileLock.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <string>
#include <system_error>
#include <utility>

#if defined(_WIN32)
#   include <windows.h>
#   include "AstUtil/Encode.hpp"          // for aUtf8ToWide
#elif defined(A_WASM) || defined(__EMSCRIPTEN__)
// WebAssembly 暂不支持跨进程文件锁
#else
#   include <cerrno>
#   include <cstring>                    // for strerror
#   include <fcntl.h>                    // for open
#   include <sys/file.h>                 // for flock
#   include <time.h>                     // for nanosleep
#   include <unistd.h>                   // for close/ftruncate/getpid/lseek/write
#   ifndef O_CLOEXEC
#       define O_CLOEXEC 0
#   endif
#endif

AST_NAMESPACE_BEGIN

using ELockMode = FileLock::ELockMode;
using ELockResult = FileLock::ELockResult;

namespace {

/// @brief 带超时的 tryLock() 的轮询间隔 [ms]
const int kLockFileRetryIntervalMs = 20;

#if defined(_WIN32)
/// @brief Windows 上锁定的字节偏移
/// @details 取一个远大于属主信息长度的偏移，使锁定的字节范围不与文件内容重叠：
///          Windows 的字节范围锁会阻止其它句柄读写被锁定的范围，如果锁定
///          文件开头的字节，外部程序就无法查看锁文件的内容。
const DWORD kLockFileLockOffset = 4096;
#endif

/// @brief 睡眠指定的毫秒数
void _sleepMs(int milliseconds)
{
#if defined(_WIN32)
    ::Sleep(static_cast<DWORD>(milliseconds));
#elif defined(A_WASM) || defined(__EMSCRIPTEN__)
    A_UNUSED(milliseconds);
#else
    timespec interval;
    interval.tv_sec = milliseconds / 1000;
    interval.tv_nsec = static_cast<long>(milliseconds % 1000) * 1000000L;
    while (::nanosleep(&interval, &interval) != 0 && errno == EINTR) {
        // 被信号打断，按剩余时间继续等待
    }
#endif
}

} // namespace

#if defined(_WIN32)
class FileLock::Impl
{
public:
    errc_t open(StringView path)
    {
        std::wstring wpath = aUtf8ToWide(path);
        HANDLE handle = ::CreateFileW(
            wpath.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
        if (handle == INVALID_HANDLE_VALUE || handle == nullptr) {
            DWORD error = ::GetLastError();
            if (error == ERROR_PATH_NOT_FOUND || error == ERROR_FILE_NOT_FOUND) {
                return eErrorNotFound;
            }
            aError(_("打开锁文件 '%.*s' 失败: %lu"), static_cast<int>(path.size()), path.data(), static_cast<unsigned long>(error));
            return eErrorInvalidFile;
        }
        handle_ = handle;
        return eNoError;
    }
    errc_t close()
    {
        if (handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE) {
            ::CloseHandle(handle_);
            handle_ = nullptr;
        }
        return eNoError;
    }
    errc_t lock(ELockMode mode)
    {
        OVERLAPPED overlapped{};
        overlapped.Offset = kLockFileLockOffset;
        DWORD flags = 0;
        if (mode == ELockMode::eExclusive)
            flags |= LOCKFILE_EXCLUSIVE_LOCK;
        if (!::LockFileEx(handle_, flags, 0, 1, 0, &overlapped)) {
            aWarning(_("锁定文件失败: %lu"), static_cast<unsigned long>(::GetLastError()));
            return eError;
        }
        return eNoError;
    }
    errc_t tryLock(ELockMode mode)
    {
        OVERLAPPED overlapped{};
        overlapped.Offset = kLockFileLockOffset;
        DWORD flags = LOCKFILE_FAIL_IMMEDIATELY;
        if (mode == ELockMode::eExclusive)
            flags |= LOCKFILE_EXCLUSIVE_LOCK;
        if (!::LockFileEx(handle_, flags, 0, 1, 0, &overlapped)) {
            DWORD error = ::GetLastError();
            if (error == ERROR_LOCK_VIOLATION || error == ERROR_IO_PENDING) 
            {
                aInfo(_("文件已被锁定进程占用"));
                return ELockResult::eBusy;
            }
            aWarning(_("锁定文件失败: %lu"), static_cast<unsigned long>(error));
            return ELockResult::eError;
        }
        return eNoError;
    }
    errc_t unlock()
    {
        OVERLAPPED overlapped{};
        overlapped.Offset = kLockFileLockOffset;
        if (!::UnlockFileEx(handle_, 0, 1, 0, &overlapped)) {
            aWarning(_("解锁文件失败: %lu"), static_cast<unsigned long>(::GetLastError()));
            return eError;
        }
        return eNoError;
    }
public:
    HANDLE handle_{nullptr};                 ///< 锁文件句柄
};

#elif defined(A_WASM) || defined(__EMSCRIPTEN__)
// WebAssembly 上暂不支持跨进程文件锁
class FileLock::Impl
{
public:
    errc_t open(StringView path)
    {
        return eErrorInvalidFile;
    }
    errc_t close()
    {
        return eNoError;
    }
    errc_t lock(ELockMode mode)
    {
        return eErrorInvalidFile;
    }
    errc_t tryLock(ELockMode mode)
    {
        return ELockResult::eError;
    }
    errc_t unlock()
    {
        return eErrorInvalidFile;
    }
};
#else

class FileLock::Impl
{
public:
    errc_t open(StringView path)
    {
        std::string pathStr(path);
        int fd = ::open(pathStr.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0666);
        if (fd < 0) {
            if (errno == ENOENT || errno == ENOTDIR) {
                return eErrorNotFound;
            }
            aError(_("打开锁文件 '%s' 失败: %s"), pathStr.c_str(), std::strerror(errno));
            return eErrorInvalidFile;
        }
        fd_ = fd;
        return eNoError;    
    }
    errc_t close()
    {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
        return eNoError;
    }
    errc_t lock(ELockMode mode)
    {
        int operation = (mode == ELockMode::eExclusive ? LOCK_EX : LOCK_SH);
        while (::flock(fd_, operation) != 0) {
            if (errno == EINTR)
                continue;                       // 被信号打断，继续等待
            aError(_("锁定文件失败: %s"), std::strerror(errno));
            return eError;
        }
        return eNoError;
    }
    errc_t tryLock(ELockMode mode)
    {
        int operation = (mode == ELockMode::eExclusive ? LOCK_EX : LOCK_SH);
        if (::flock(fd_, operation | LOCK_NB) != 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
                return ELockResult::eBusy;
            }
            aError(_("锁定文件失败: %s"), std::strerror(errno));
            return ELockResult::eError;
        }
        return eNoError;
    }
    errc_t unlock()
    {
        if (::flock(fd_, LOCK_UN) != 0) {
            aError(_("解锁文件失败: %s"), std::strerror(errno));
            return eError;
        }
        return eNoError;
    }
private:
    int  fd_{-1};                            ///< 锁文件描述符
};

#endif

FileLock::FileLock(StringView path)
{
    static_assert(sizeof(Impl) <= sizeof(impl_), "storage is too small for Impl");
    new(&impl_) Impl();
    impl().open(path);
}

FileLock::~FileLock()
{
    impl().unlock();
    impl().close();
    impl().~Impl();
}


errc_t FileLock::lock(ELockMode mode)
{
    return impl().lock(mode);
}

errc_t FileLock::tryLock(ELockMode mode)
{
    return impl().tryLock(mode);
}

errc_t FileLock::tryLock(double timeoutSeconds, ELockMode mode)
{
    if (timeoutSeconds <= 0.0)
        return tryLock(mode);
    auto& impl = this->impl();
    typedef std::chrono::steady_clock Clock;
    const Clock::time_point deadline = Clock::now() +
        std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(timeoutSeconds));
    for (;;) {
        errc_t result = impl.tryLock(mode);
        if (result != ELockResult::eBusy)
            return result;                  // 成功，或打开/加锁出错：不再等待
        if (Clock::now() >= deadline)
            return ELockResult::eBusy;      // 超时仍被占用
        _sleepMs(kLockFileRetryIntervalMs);
    }
}

errc_t FileLock::unlock()
{
    return impl().unlock();
}

FileLock::Impl &FileLock::impl()
{
    return *reinterpret_cast<FileLock::Impl*>(&impl_);
}

AST_NAMESPACE_END
