///
/// @file      Thread.cpp
/// @brief     跨平台线程实现
/// @author    axel
/// @date      2026-09-20
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

#include "Thread.hpp"

#include <exception>
#include <memory>
#include <system_error>
#include <utility>

// Windows 和 WASM 不受 GLIBCXX 版本约束（前者没有 libstdc++，后者用的是
// Emscripten 的 libc++），直接沿用 std::thread，行为与替换前完全一致。
// 其余平台走 pthread，产物里就不会有 std::thread 的符号。
#if defined(_WIN32) || defined(__EMSCRIPTEN__) || defined(A_WASM)
#   define _AST_USE_STD_THREAD
#   include <thread>
#else
#   include <pthread.h>
#endif

AST_NAMESPACE_BEGIN

namespace {

/// @brief Thread 的实现
struct ThreadImpl
{
#ifdef _AST_USE_STD_THREAD
    std::thread handle{};
#else
    pthread_t handle{};
#endif
    bool joinable{false};
};

#ifndef _AST_USE_STD_THREAD

/// @brief pthread 的入口函数
/// @details pthread_create 只接受 C 函数，这里用它把 std::function 转发过去。
///          函数对象在启动前于堆上创建，由本函数负责释放。
extern "C" void* astThreadTrampoline(void* arg)
{
    std::unique_ptr<std::function<void()>> func(static_cast<std::function<void()>*>(arg));
    (*func)();
    return nullptr;
}

void threadStart(ThreadImpl& impl, std::function<void()> func)
{
    auto holder = new std::function<void()>(std::move(func));
    const int err = ::pthread_create(&impl.handle, nullptr, &astThreadTrampoline, holder);
    if (err != 0)
    {
        delete holder;
        throw std::system_error(err, std::generic_category(), "ast::Thread");
    }
    impl.joinable = true;
}

void threadJoin(ThreadImpl& impl)
{
    const int err = ::pthread_join(impl.handle, nullptr);
    if (err != 0)
        throw std::system_error(err, std::generic_category(), "ast::Thread::join");
    impl.joinable = false;
}

void threadDetach(ThreadImpl& impl)
{
    const int err = ::pthread_detach(impl.handle);
    if (err != 0)
        throw std::system_error(err, std::generic_category(), "ast::Thread::detach");
    impl.joinable = false;
}

#else

void threadStart(ThreadImpl& impl, std::function<void()> func)
{
    impl.handle = std::thread(std::move(func));
    impl.joinable = true;
}

void threadJoin(ThreadImpl& impl)
{
    impl.handle.join();
    impl.joinable = false;
}

void threadDetach(ThreadImpl& impl)
{
    impl.handle.detach();
    impl.joinable = false;
}

#endif

} // namespace

// ---- Thread ----

Thread::Thread(std::function<void()> func)
{
    std::unique_ptr<ThreadImpl> impl(new ThreadImpl);
    threadStart(*impl, std::move(func));
    impl_ = impl.release();
}

Thread::~Thread()
{
    if (joinable())
        std::terminate();
    delete static_cast<ThreadImpl*>(impl_);
}

Thread::Thread(Thread&& other) noexcept
    : impl_(other.impl_)
{
    other.impl_ = nullptr;
}

Thread& Thread::operator=(Thread&& other) noexcept
{
    if (this != &other)
    {
        if (joinable())
            std::terminate();
        delete static_cast<ThreadImpl*>(impl_);
        impl_ = other.impl_;
        other.impl_ = nullptr;
    }
    return *this;
}

bool Thread::joinable() const
{
    return impl_ != nullptr && static_cast<ThreadImpl*>(impl_)->joinable;
}

void Thread::join()
{
    if (!joinable())
        throw std::system_error(std::make_error_code(std::errc::invalid_argument),
                                "ast::Thread::join");
    threadJoin(*static_cast<ThreadImpl*>(impl_));
}

void Thread::detach()
{
    if (!joinable())
        throw std::system_error(std::make_error_code(std::errc::invalid_argument),
                                "ast::Thread::detach");
    threadDetach(*static_cast<ThreadImpl*>(impl_));
}

void Thread::swap(Thread& other) noexcept
{
    std::swap(impl_, other.impl_);
}

AST_NAMESPACE_END
