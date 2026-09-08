///
/// @file      Any.hpp
/// @brief     类型擦除的值容器，与 C++17 的 std::any 接口兼容
/// @details   
/// @author    axel
/// @date      2026-09-08
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

#pragma once

#include "AstGlobal.h"
#include <cstddef>
#include <new>
#include <type_traits>
#include <typeinfo>
#include <utility>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Container
    @{
*/

// Any 是重度模板/类型擦除的运行时容器（含 SFINAE 构造），SWIG 无法解析其签名，
#ifndef SWIG

/// @brief any_cast 失败时抛出的异常
class bad_any_cast : public std::bad_cast
{
public:
    const char* what() const noexcept override
    {
        return "bad_any_cast";
    }
};


/// @brief 类型擦除的值容器，与 C++17 的 std::any 接口兼容
/// @details 在任意时刻持有至多一个任意类型的值，或为空。类型在运行时被类型擦除，
///          通过 any_cast<T>(any) 在已知目标类型时安全地取回。
///
/// 使用方式：
/// @code
///   Any a = 42;                        // 持有 int
///   int v = any_cast<int>(a);          // 42
///
///   a.emplace<std::string>("hello");
///   std::string s = any_cast<std::string>(a);
///
///   Any empty;
///   empty.has_value();                 // false
///   empty.type() == typeid(void);      // true
/// @endcode
///
/// 依赖说明：仅支持具基础对齐（alignof(T) <= alignof(max_align_t)）且可拷贝构造的类型；
/// 与 std::any 一致，不存储引用类型。拷贝赋值以 copy-and-swap 提供强异常安全保证。
class Any
{
public:
    // ============================================================
    // 构造 / 析构
    // ============================================================

    /// @brief 默认构造：空
    Any() noexcept = default;

    /// @brief 拷贝构造：深拷贝 other 持有的值
    Any(const Any& other)
    {
        copyFrom(other);
    }

    /// @brief 移动构造：接管 other 的内容，other 变为空
    Any(Any&& other) noexcept
    {
        stealFrom(other);
    }

    ~Any() { reset(); }

    /// @brief 从任意可拷贝构造类型构造（类型被 decay）
    template<typename ValueType,
        typename D = typename std::decay<ValueType>::type,
        typename std::enable_if<!std::is_same<D, Any>::value &&
                                std::is_copy_constructible<D>::value, int>::type = 0>
    Any(ValueType&& value)
    {
        emplace<D>(std::forward<ValueType>(value));
    }

    // ============================================================
    // 赋值
    // ============================================================

    /// @brief 拷贝赋值（copy-and-swap，强异常安全）
    Any& operator=(const Any& other)
    {
        if (this != &other)
        {
            Any tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /// @brief 移动赋值：接管 other 的内容，other 变为空
    Any& operator=(Any&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            stealFrom(other);
        }
        return *this;
    }

    /// @brief 从任意可拷贝构造类型赋值（类型被 decay）
    template<typename ValueType,
        typename D = typename std::decay<ValueType>::type,
        typename std::enable_if<!std::is_same<D, Any>::value &&
                                std::is_copy_constructible<D>::value, int>::type = 0>
    Any& operator=(ValueType&& value)
    {
        emplace<D>(std::forward<ValueType>(value));
        return *this;
    }

    // ============================================================
    // 观察
    // ============================================================

    /// @brief 是否持有任一值
    bool has_value() const noexcept { return data_ != nullptr; }

    /// @brief 当前持有值的类型信息；空时返回 typeid(void)
    const std::type_info& type() const noexcept
    {
        return data_ ? *ops_->type : typeid(void);
    }

    // ============================================================
    // 修改
    // ============================================================

    /// @brief 丢弃当前值并置为空
    void reset() noexcept
    {
        if (data_)
        {
            ops_->destroy(data_);
            ::operator delete(data_);
            data_ = nullptr;
            ops_  = nullptr;
        }
    }

    /// @brief 以给定参数就地构造 T 并替换当前值；返回新值的引用
    template<typename T, typename... Args>
    typename std::enable_if<!std::is_reference<T>::value, T&>::type
    emplace(Args&&... args)
    {
        reset();
        void* storage = ::operator new(sizeof(T));
        try
        {
            ::new (storage) T(std::forward<Args>(args)...);
        }
        catch (...)
        {
            ::operator delete(storage);
            throw;
        }
        ops_  = &opsFor<T>();
        data_ = storage;
        return *static_cast<T*>(data_);
    }

    /// @brief 与之交换内容（常量时间，noexcept）
    void swap(Any& other) noexcept
    {
        std::swap(ops_, other.ops_);
        std::swap(data_, other.data_);
    }

private:
    // ---- 类型擦除操作表 ----
    struct TypeOps
    {
        void (*destroy)(void*);                       ///< 析构持有的 T
        void (*copyConstruct)(void* dst, const void* src);  ///< 在 dst 处拷贝构造 T
        const std::type_info* type;                   ///< typeid(T) 的地址（静态生命周期，永不悬垂）
        size_t size;                                  ///< sizeof(T)
    };

    template<typename T>
    static const TypeOps& opsFor()
    {
        static const TypeOps ops = {
            [](void* p) { static_cast<T*>(p)->~T(); },
            [](void* dst, const void* src) { ::new (dst) T(*static_cast<const T*>(src)); },
            &typeid(T),
            sizeof(T),
        };
        return ops;
    }

    void copyFrom(const Any& other)
    {
        if (!other.data_)
            return;
        void* storage = ::operator new(other.ops_->size);
        try
        {
            other.ops_->copyConstruct(storage, other.data_);
        }
        catch (...)
        {
            ::operator delete(storage);
            throw;
        }
        ops_  = other.ops_;
        data_ = storage;
    }

    void stealFrom(Any& other) noexcept
    {
        ops_  = other.ops_;
        data_ = other.data_;
        other.ops_  = nullptr;
        other.data_ = nullptr;
    }

    // ---- 成员 ----
    const TypeOps* ops_{nullptr};    ///< nullptr 表示空
    void*          data_{nullptr};   ///< 指向存储的 T；空时 nullptr

    // 允许 any_cast 家族访问私有数据
    template<typename T> friend const T* any_cast(const Any*) noexcept;
    template<typename T> friend T* any_cast(Any*) noexcept;
    template<typename T> friend T any_cast(const Any&);
    template<typename T> friend T any_cast(Any&);
    template<typename T> friend T any_cast(Any&&);
};


// ================================================================
// any_cast — 类型安全的取回
// ================================================================

/// @brief 探测式取回（const）：类型匹配时返回指针，否则 nullptr
template<typename T>
inline const T* any_cast(const Any* operand) noexcept
{
    using U = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
    if (operand && operand->type() == typeid(U))
        return static_cast<const T*>(operand->data_);
    return nullptr;
}

/// @brief 探测式取回（可变量）：类型匹配时返回指针，否则 nullptr
template<typename T>
inline T* any_cast(Any* operand) noexcept
{
    using U = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
    if (operand && operand->type() == typeid(U))
        return static_cast<T*>(operand->data_);
    return nullptr;
}

/// @brief 取回（const，按引用或值返回）：类型不匹配时抛 bad_any_cast
template<typename T>
inline T any_cast(const Any& operand)
{
    using U = typename std::decay<T>::type;
    const U* p = any_cast<U>(&operand);
    if (!p)
        throw bad_any_cast();
    return *p;
}

/// @brief 取回（可变量，按引用或值返回）：类型不匹配时抛 bad_any_cast
template<typename T>
inline T any_cast(Any& operand)
{
    using U = typename std::decay<T>::type;
    U* p = any_cast<U>(&operand);
    if (!p)
        throw bad_any_cast();
    return *p;
}

/// @brief 取回（可变量，移动返回）：类型不匹配时抛 bad_any_cast
template<typename T>
inline T any_cast(Any&& operand)
{
    using U = typename std::decay<T>::type;
    U* p = any_cast<U>(&operand);
    if (!p)
        throw bad_any_cast();
    return static_cast<U&&>(*p);
}


// ================================================================
// make_any — 便捷构造
// ================================================================

/// @brief 生成一个持有 T 的 Any
template<typename T, typename... Args>
inline Any make_any(Args&&... args)
{
    Any a;
    a.emplace<T>(std::forward<Args>(args)...);
    return a;
}

#endif // SWIG

/*! @} */

AST_NAMESPACE_END
