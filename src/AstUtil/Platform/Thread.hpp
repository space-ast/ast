///
/// @file      Thread.hpp
/// @brief     跨平台线程
/// @details   Windows / WASM 上直接沿用 std::thread；其余平台（Linux / macOS / BSD）
///            用 pthread 实现，以避免产物里出现 std::thread 的符号导致 ABI 兼容性问题
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

#pragma once

#include "AstGlobal.h"
#include <functional>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Platform
    @{
*/

/// @brief 线程
/// @details 
/// 接口是 std::thread 的一个子集，用法与它一致：构造即启动，
/// 析构时若仍 joinable 则调用 std::terminate，仅可移动、不可拷贝。
///
/// GCC 6 起 std::thread::_State 的析构、_M_start_thread 与 typeinfo 挂在 GLIBCXX_3.4.22 下，
/// 而 Ubuntu 16.04，GCC 5.4只有到 GLIBCXX_3.4.21，使用该类可以避免在这些发行版上运行时出错。
class AST_UTIL_API Thread
{
public:
    /// @brief 构造一个不关联任何线程的对象
    Thread() = default;

    /// @brief 构造并立即启动线程
    /// @param func 线程函数
    /// @note 启动失败时抛出 std::system_error，与 std::thread 一致
    explicit Thread(std::function<void()> func);

    /// @brief 析构，若线程仍 joinable 则调用 std::terminate
    ~Thread();

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    Thread(Thread&& other) noexcept;
    Thread& operator=(Thread&& other) noexcept;

    /// @brief 线程是否可 join
    /// @details 线程已结束或被 detach 之后，本函数返回 false
    bool joinable() const;

    /// @brief 等待线程结束
    void join();

    /// @brief 分离线程，此后不再可 join
    /// @note 分离后的线程退出时会自行回收资源，不要再对它调用 join
    void detach();

    /// @brief 与另一个线程对象交换
    void swap(Thread& other) noexcept;

private:
    /// @brief 平台相关的实现，定义在 Thread.cpp 中
    void* impl_{nullptr};
};

/// @brief 交换两个线程对象
A_ALWAYS_INLINE
void swap(Thread& lhs, Thread& rhs) noexcept
{
    lhs.swap(rhs);
}

/*! @} */

AST_NAMESPACE_END

