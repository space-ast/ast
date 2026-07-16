///
/// @file      VariantVector.hpp
/// @brief     类型擦除向量容器
/// @details   运行时确定元素类型，但一次只能存储一种特定类型的数据。
///            类型元数据通过分配头嵌入在 data_ 前方，对象本身仅 3 个字段。
/// @author    axel
/// @date      2026-07-05
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
#include "Span.hpp"

#include <typeinfo>
#include <new>
#include <cassert>
#include <stdexcept>
#include <utility>
#include <cstddef>
#include <climits>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Container
    @{
*/


/// @brief 变类型向量容器
/// @details 该类可以存储不同数据类型的向量，无需在编译时指定具体类型，
/// 但一次只能存储一种特定类型的数据。
///
/// 类型元数据（TypeOps*、type_info*、elementSize）嵌入在分配块头部，
/// data_ 指针永远指向第一个元素。data_ == nullptr 表示未设置类型。
///
/// 使用方式：
/// @code
///   VariantVector v;
///   v.resize<double>(100);        // 首次调用确定类型为 double
///   v[0] = 3.14;                  // 类型安全访问
///   assert(v.elementType() == typeid(double));
/// @endcode
class AST_UTIL_API VariantVector
{
public:
    // ---- 类型操作表（前置声明，定义见类外）----
    struct TypeOps;

private:
    // ---- 分配头：嵌入在 data_ 指向的分配块前方 ----
    struct
#ifndef SWIG
    alignas(std::max_align_t)  // SWIG 不支持 alignas，需用宏保护
#endif
    Header
    {
        const TypeOps*       ops;           ///< 类型操作表指针
        const std::type_info* typeInfo;     ///< 元素类型信息指针
        size_t               elementSize;   ///< 元素大小(每个元素占用的字节数)
    };

    static constexpr size_t kHeaderSize = sizeof(Header);

    // ---- 成员 ----
    void*  data_{nullptr};     ///< 指向第一个元素；nullptr 表示未设置类型（Header 在 data_ 前方 kHeaderSize 字节处）
    size_t size_{0};           ///< 当前元素个数
    size_t capacity_{0};       ///< 已分配内存可容纳的元素个数（不含 Header）

public:
    // ============================================================
    // 构造 / 析构
    // ============================================================

    VariantVector() noexcept = default;
    ~VariantVector();

    VariantVector(const VariantVector& other);
    VariantVector& operator=(const VariantVector& other);

    VariantVector(VariantVector&& other) noexcept;
    VariantVector& operator=(VariantVector&& other) noexcept;

    // ============================================================
    // 容量
    // ============================================================

    size_t size()     const noexcept { return size_; }
    size_t capacity() const noexcept { return capacity_; }
    bool   empty()    const noexcept { return size_ == 0; }

    // ============================================================
    // 类型信息
    // ============================================================

    /// @brief 获取元素类型（未设置类型时返回 typeid(void) 作为哨兵）
    const std::type_info& elementType() const noexcept;

    /// @brief 是否已设置类型（即 data_ != nullptr）
    bool hasType() const noexcept { return data_ != nullptr; }

    /// @brief 元素大小（未设置类型时返回 0）
    size_t elementSize() const noexcept
    {
        return data_ ? header()->elementSize : 0;
    }

    /// @brief 获取 TypeOps 指针（未设置类型时返回 nullptr）
    const TypeOps* ops() const noexcept
    {
        return data_ ? header()->ops : nullptr;
    }

    // ============================================================
    // 原始数据访问
    // ============================================================

    void*       data()       noexcept { return data_; }
    const void* data() const noexcept { return data_; }

    // ============================================================
    // 修改器（模板，首次调用捕获类型）
    // ============================================================

    template<typename T>
    void reserve(size_t n);

    template<typename T>
    void resize(size_t n);

    template<typename T>
    void resize(size_t n, const T& value);

    /// @brief 重置元素类型及大小（先销毁旧数据再按新类型重建）
    template<typename T>
    void reset(size_t n);

    /// @brief 重置元素类型及大小（先销毁旧数据再按新类型重建，带填充值）
    template<typename T>
    void reset(size_t n, const T& value);

    template<typename T>
    void pushBack(const T& value);

    template<typename T, typename... Args>
    void emplaceBack(Args&&... args);

    void clear();
    void shrinkToFit();

    // ============================================================
    // 元素访问（模板，运行时校验类型）
    // ============================================================

    /// @brief 获取类型化指针，用于零开销元素访问
    /// @code
    ///   v.as<double>()[0] = 3.14;
    ///   double sum = v.as<double>()[5];
    /// @endcode
    template<typename T>
    T* as();

    template<typename T>
    const T* as() const;

    /// @brief 获取类型化 Span 视图（非拥有）
    /// @code
    ///   Span<double> s = v.asSpan<double>();
    ///   for (double& x : s) { ... }
    /// @endcode
    template<typename T>
    Span<T> asSpan();

    template<typename T>
    Span<const T> asSpan() const;

    /// @brief 带越界检查的元素访问
    template<typename T>
    T& at(size_t i);

    template<typename T>
    const T& at(size_t i) const;

    // ============================================================
    // 迭代器（模板）
    // ============================================================

    template<typename T>
    T* begin();

    template<typename T>
    const T* begin() const;

    template<typename T>
    T* end();

    template<typename T>
    const T* end() const;

    // ============================================================
    // 交换
    // ============================================================

    void swap(VariantVector& other) noexcept;

    // ============================================================
    // 便捷构造
    // ============================================================

    template<typename T>
    static VariantVector fromVector(const std::vector<T>& vec);

private:
    // ---- 底层操作 ----

    void        destroy() noexcept;
    void        copyFrom(const VariantVector& other);
    void        moveFrom(VariantVector& other) noexcept;

    inline Header*       header()       noexcept;
    inline const Header* header() const noexcept;

    /// @brief 确保类型已设置并校验一致性
    /// @tparam T 期望的元素类型
    /// 首次调用时分配 Header 并捕获类型，后续调用校验 T 与已捕获类型一致
    template<typename T>
    void ensureType();
};


// ================================================================
// TypeOps — 类型擦除操作表
// ================================================================

struct VariantVector::TypeOps
{
    void (*destroy)(void* data, size_t count);
    void (*copyConstruct)(void* dst, const void* src, size_t count);
    void (*moveConstruct)(void* dst, void* src, size_t count);

    /// @brief 获取类型 T 对应的 TypeOps 单例
    template<typename T>
    static const TypeOps& forType()
    {
        static const TypeOps ops = {
            // destroy — 析构 count 个元素
            [](void* data, size_t count) {
                T* p = static_cast<T*>(data);
                for (size_t i = 0; i < count; ++i)
                    p[i].~T();
            },
            // copyConstruct — 从 src 拷贝构造 count 个元素到 dst
            [](void* dst, const void* src, size_t count) {
                const T* s = static_cast<const T*>(src);
                T* d = static_cast<T*>(dst);
                for (size_t i = 0; i < count; ++i)
                    ::new (static_cast<void*>(&d[i])) T(s[i]);
            },
            // moveConstruct — 从 src 移动构造 count 个元素到 dst
            [](void* dst, void* src, size_t count) {
                T* s = static_cast<T*>(src);
                T* d = static_cast<T*>(dst);
                for (size_t i = 0; i < count; ++i)
                    ::new (static_cast<void*>(&d[i])) T(std::move(s[i]));
            },
        };
        return ops;
    }
};


// ================================================================
// Header 内联访问器
// ================================================================

inline VariantVector::Header* VariantVector::header() noexcept
{
    assert(data_ != nullptr);
    return reinterpret_cast<Header*>(static_cast<char*>(data_) - kHeaderSize);
}

inline const VariantVector::Header* VariantVector::header() const noexcept
{
    assert(data_ != nullptr);
    return reinterpret_cast<const Header*>(static_cast<const char*>(data_) - kHeaderSize);
}


// ================================================================
// 模板方法实现
// ================================================================

// ---- ensureType ----

template<typename T>
inline void VariantVector::ensureType()
{
    if (data_ != nullptr)
    {
        // 已有类型：校验一致（operator== 语义比较，跨 DLL 安全）
        assert(*header()->typeInfo == typeid(T) && "VariantVector: type mismatch");
        return;
    }

    // 首次确定类型：仅分配 Header，尚无元素空间
    const TypeOps& typeOps = TypeOps::forType<T>();
    void* block = ::operator new(kHeaderSize);
    ::new (block) Header{&typeOps, &typeid(T), sizeof(T)};
    data_ = static_cast<char*>(block) + kHeaderSize;
    // size_ 和 capacity_ 保持 0（由默认构造保证）
}

// ---- reserve ----

template<typename T>
inline void VariantVector::reserve(size_t n)
{
    ensureType<T>();

    if (n <= capacity_)
        return;

    const Header* oldHeader = header();
    size_t elemSize  = oldHeader->elementSize;

    // 检查整数溢出：n * elemSize 不能超过 SIZE_MAX - kHeaderSize
    if (n > (SIZE_MAX - kHeaderSize) / elemSize)
        throw std::length_error("VariantVector::reserve: requested size exceeds max_size");

    size_t offset    = kHeaderSize;
    size_t newBlockSize = offset + n * elemSize;
    char* newBlock = static_cast<char*>(::operator new(newBlockSize));

    // 拷贝 Header 到新块
    ::new (newBlock) Header(*oldHeader);

    // move 已有元素到新块
    char* newDataPtr = newBlock + offset;
    if (size_ > 0)
    {
        try
        {
            oldHeader->ops->moveConstruct(newDataPtr, data_, size_);
        }
        catch (...)
        {
            ::operator delete(static_cast<void*>(newBlock));
            throw;
        }
    }

    // 析构旧元素并释放旧块
    char* oldBlock = static_cast<char*>(data_) - offset;
    if (size_ > 0)
        oldHeader->ops->destroy(data_, size_);
    ::operator delete(static_cast<void*>(oldBlock));

    // 更新指针
    data_     = newDataPtr;
    capacity_ = n;
}

// ---- resize (n) ----

template<typename T>
inline void VariantVector::resize(size_t n)
{
    ensureType<T>();
    reserve<T>(n);

    if (n > size_)
    {
        // 默认构造新增元素（每成功构造一个立即递增 size_，异常安全）
        T* p = static_cast<T*>(data_);
        while (size_ < n)
        {
            ::new (static_cast<void*>(&p[size_])) T();
            ++size_;
        }
    }
    else if (n < size_)
    {
        // 析构多余元素
        T* p = static_cast<T*>(data_);
        for (size_t i = n; i < size_; ++i)
            p[i].~T();
        size_ = n;
    }
}

// ---- resize (n, value) ----

template<typename T>
inline void VariantVector::resize(size_t n, const T& value)
{
    ensureType<T>();
    reserve<T>(n);

    if (n > size_)
    {
        // 拷贝构造新增元素（每成功构造一个立即递增 size_，异常安全）
        T* p = static_cast<T*>(data_);
        while (size_ < n)
        {
            ::new (static_cast<void*>(&p[size_])) T(value);
            ++size_;
        }
    }
    else if (n < size_)
    {
        // 析构多余元素
        T* p = static_cast<T*>(data_);
        for (size_t i = n; i < size_; ++i)
            p[i].~T();
        size_ = n;
    }
}

// ---- reset ----

template<typename T>
inline void VariantVector::reset(size_t n)
{
    destroy();
    ensureType<T>();
    reserve<T>(n);

    T* p = static_cast<T*>(data_);
    while (size_ < n)
    {
        ::new (static_cast<void*>(&p[size_])) T();
        ++size_;
    }
}

template<typename T>
inline void VariantVector::reset(size_t n, const T& value)
{
    destroy();
    ensureType<T>();
    reserve<T>(n);

    T* p = static_cast<T*>(data_);
    while (size_ < n)
    {
        ::new (static_cast<void*>(&p[size_])) T(value);
        ++size_;
    }
}

// ---- pushBack ----

template<typename T>
inline void VariantVector::pushBack(const T& value)
{
    ensureType<T>();

    if (size_ >= capacity_)
    {
        size_t newCap = (capacity_ == 0) ? 1 :
                        (capacity_ > SIZE_MAX / 2) ? SIZE_MAX : capacity_ * 2;
        reserve<T>(newCap);
    }

    T* p = static_cast<T*>(data_);
    ::new (static_cast<void*>(&p[size_])) T(value);
    ++size_;
}

// ---- emplaceBack ----

template<typename T, typename... Args>
inline void VariantVector::emplaceBack(Args&&... args)
{
    ensureType<T>();

    if (size_ >= capacity_)
    {
        size_t newCap = (capacity_ == 0) ? 1 :
                        (capacity_ > SIZE_MAX / 2) ? SIZE_MAX : capacity_ * 2;
        reserve<T>(newCap);
    }

    T* p = static_cast<T*>(data_);
    ::new (static_cast<void*>(&p[size_])) T(std::forward<Args>(args)...);
    ++size_;
}

// ---- at ----

template<typename T>
inline T& VariantVector::at(size_t i)
{
    if (data_ == nullptr)
        throw std::logic_error("VariantVector::at: vector has no type set");

    if (*header()->typeInfo != typeid(T))
        throw std::logic_error("VariantVector::at: type mismatch");

    if (i >= size_)
        throw std::out_of_range("VariantVector::at: index out of range");

    return static_cast<T*>(data_)[i];
}

template<typename T>
inline const T& VariantVector::at(size_t i) const
{
    if (data_ == nullptr)
        throw std::logic_error("VariantVector::at: vector has no type set");

    if (*header()->typeInfo != typeid(T))
        throw std::logic_error("VariantVector::at: type mismatch");

    if (i >= size_)
        throw std::out_of_range("VariantVector::at: index out of range");

    return static_cast<const T*>(data_)[i];
}

// ---- as ----

template<typename T>
inline T* VariantVector::as()
{
    if (data_ == nullptr) return nullptr;
    if (*header()->typeInfo != typeid(T)) return nullptr;
    return static_cast<T*>(data_);
}

template<typename T>
inline const T* VariantVector::as() const
{
    if (data_ == nullptr) return nullptr;
    if (*header()->typeInfo != typeid(T)) return nullptr;
    return static_cast<const T*>(data_);
}

// ---- asSpan ----

template<typename T>
inline Span<T> VariantVector::asSpan()
{
    if (data_ == nullptr) return Span<T>();
    if (*header()->typeInfo != typeid(T)) return Span<T>();
    return Span<T>(static_cast<T*>(data_), size_);
}

template<typename T>
inline Span<const T> VariantVector::asSpan() const
{
    if (data_ == nullptr) return Span<const T>();
    if (*header()->typeInfo != typeid(T)) return Span<const T>();
    return Span<const T>(static_cast<const T*>(data_), size_);
}

// ---- begin / end ----

template<typename T>
inline T* VariantVector::begin()
{
    if (data_ == nullptr) return nullptr;
    if (*header()->typeInfo != typeid(T)) return nullptr;
    return static_cast<T*>(data_);
}

template<typename T>
inline const T* VariantVector::begin() const
{
    if (data_ == nullptr) return nullptr;
    if (*header()->typeInfo != typeid(T)) return nullptr;
    return static_cast<const T*>(data_);
}

template<typename T>
inline T* VariantVector::end()
{
    if (data_ == nullptr) return nullptr;
    if (*header()->typeInfo != typeid(T)) return nullptr;
    return static_cast<T*>(data_) + size_;
}

template<typename T>
inline const T* VariantVector::end() const
{
    if (data_ == nullptr) return nullptr;
    if (*header()->typeInfo != typeid(T)) return nullptr;
    return static_cast<const T*>(data_) + size_;
}

// ---- fromVector ----

template<typename T>
inline VariantVector VariantVector::fromVector(const std::vector<T>& vec)
{
    VariantVector v;
    v.ensureType<T>();
    v.reserve<T>(vec.size());

    // 拷贝构造每个元素（只构造一次）
    T* dst = static_cast<T*>(v.data_);
    for (size_t i = 0; i < vec.size(); ++i)
    {
        ::new (static_cast<void*>(&dst[i])) T(vec[i]);
        ++v.size_;
    }
    return v;
}


/*! @} */

AST_NAMESPACE_END
