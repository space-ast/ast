///
/// @file      FileLock.hpp
/// @brief     跨进程文件锁
/// @details   基于操作系统的 advisory 文件锁实现：
///            Windows 使用 LockFileEx，POSIX 使用 flock
///            锁由内核随句柄/进程终止自动释放，进程崩溃不会残留死锁
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include <string>
#include <memory>           // for std::unique_ptr<FileLock::Impl>
#include <type_traits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Platform
    @{
*/


/// @brief 跨进程文件锁
/// @details 用于协调多个进程（或同一进程内的多个句柄）对同一资源的访问：
///          持锁方在锁文件上持有一把操作系统级的 advisory 锁，其它进程通过
///          tryLock() 判断资源是否已被占用。
///
/// 特点：
/// - RAII：析构时自动解锁并关闭句柄；
/// - 进程异常退出（崩溃、被强杀）时锁由内核自动释放，不会留下死锁；
/// - 锁文件本身保留：构造时打开（不存在则创建），残留的空文件会被后续复用；
/// - 锁是建议性的（advisory）：不阻止其它进程直接读写该文件。
///
/// 注意：
/// - 路径按 UTF-8 解释；锁文件所在目录必须已存在（不会自动创建）；
/// - 一个 FileLock 对象对应一把锁，对象本身不是线程安全的；
///
/// 使用示例：
/// @code
///   FileLock lock("data/.update.lock");
///   if (lock.tryLock() == eNoError) {
///       // 独占访问 data 目录，lock 析构或进程退出时自动解锁
///   } else {
///       // 已有其它进程在更新数据
///   }
/// @endcode
class AST_UTIL_API FileLock
{
public:
    /// @brief 文件锁模式
    enum ELockMode
    {
        eExclusive,         ///< 排他锁：同一时刻只允许一个持有者
        eShared,            ///< 共享锁：允许多个持有者同时持有，与排他锁互斥
    };

    /// @brief 加锁结果
    enum ELockResult
    {
        eLocked=0,          ///< 成功获得锁
        eBusy=eErrorBusy,   ///< 锁已被其它句柄占用（非阻塞加锁或等待超时）
        eError=-1,          ///< 打开锁文件或加锁失败，具体原因见日志
    };
public:
    /// @brief 构造锁，关联锁文件
    /// @param path 锁文件路径（UTF-8）
    explicit FileLock(StringView path);

    /// @brief 析构：自动解锁并关闭句柄
    ~FileLock();

    A_DISABLE_COPY(FileLock)

    /// @brief 阻塞加锁，直到获得锁为止
    /// @details 锁文件在构造时打开；本函数会一直等待，直到获得锁为止
    /// @param mode 锁模式，默认排他锁
    errc_t lock(ELockMode mode = ELockMode::eExclusive);

    /// @brief 非阻塞加锁
    /// @param mode 锁模式，默认排他锁
    errc_t tryLock(ELockMode mode = ELockMode::eExclusive);

    /// @brief 带超时的加锁，在 timeoutSeconds 秒内反复尝试
    /// @details 在超时前反复尝试
    /// @param timeoutSeconds 最长等待时间 [s]，小于等于 0 时只尝试一次
    /// @param mode 锁模式，默认排他锁
    errc_t tryLock(double timeoutSeconds, ELockMode mode = ELockMode::eExclusive);

    /// @brief 释放锁，但保持锁文件打开以便再次加锁
    errc_t unlock();
private:
    class Impl;
    Impl& impl();
private:
    std::aligned_storage<sizeof(void*)>::type impl_{};
};


/*! @} */

AST_NAMESPACE_END
