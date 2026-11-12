///
/// @file      Span.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-23
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

#pragma once

#include "AstGlobal.h"
#include <vector>
#include <cstddef>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <initializer_list>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Container
    @{
*/

constexpr size_t dynamic_extent = static_cast<size_t>(-1);

template<typename T, size_t Extent>
class SpanStorage{
public:
    // 固定 extent 时长度由 Extent 决定，传入的 size 只用于与动态版本保持接口一致；
    A_CONSTEXPR_CXX14 SpanStorage(T* data, size_t size) noexcept
        : data_(data)
    {
        A_UNUSED(size);
        assert(size == Extent);
    }
    T* data() const noexcept { return data_; }
    A_CONSTEXPR_CXX14 size_t size() const noexcept { return Extent; }
public:
    T* data_;
};


template<typename T>
class SpanStorage<T, dynamic_extent>{
public:
    SpanStorage(T* data, size_t size) noexcept : data_(data), size_(size) {}
    T* data() const noexcept { return data_; }
    size_t size() const noexcept { return size_; }
public:
    T* data_;
    size_t size_;
};

/// @brief 非拥有的连续对象序列视图
/// @details 与C++20的 std::span 功能相似且API兼容，不负责管理内存生命周期。
/// @tparam T 元素类型
/// @tparam Extent 序列长度，-1表示动态长度
template <typename T, size_t Extent = dynamic_extent>
class Span
{
public:
    using element_type = T;
    using value_type = typename std::remove_cv<T>::type;
    using index_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using const_pointer = const T*;
    using const_reference = const T&;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr std::size_t extent = Extent;

    // 构造函数
    // 只有零 extent 和动态 extent 可以默认构造
    template <bool DefaultConstructible = (Extent == 0 || Extent == dynamic_extent),
              typename std::enable_if<DefaultConstructible, int>::type = 0>
    constexpr Span() noexcept
        : storage_(nullptr, 0)
    {}

    // initializer_list 的元素是 const 的，因此只能用于 const 元素类型的 span。
    // 只有零 extent 和动态 extent 可以接受初始化列表：
    // 固定非零 extent 无法在编译期校验列表长度，长度不符（包括空列表）会构造出越界的 span
    template <bool Initializable = (Extent == 0 || Extent == dynamic_extent),
              typename std::enable_if<Initializable, int>::type = 0>
    constexpr explicit Span(std::initializer_list<value_type> init) noexcept
        : storage_(init.begin(), init.size())
    {
        static_assert(std::is_const<T>::value, "element_type must be const");
    }

    constexpr Span(pointer ptr, index_type count) noexcept
        : storage_(ptr, count) {}

    // 限定 End 必须不能是 index_type 类型的值，避免 Span(ptr, 0) 中字面量 0（空指针常量）
    // 与 (pointer, index_type) 构造函数产生二义
    template <typename End,
              typename std::enable_if<!std::is_convertible<End, index_type>::value, int>::type = 0>
    constexpr Span(pointer first, End last) noexcept
        : storage_(first, last - first) {}

    // 数组长度在编译期已知，长度匹配时可隐式构造。
    template <std::size_t N,
              typename std::enable_if<(Extent == dynamic_extent || N == Extent), int>::type = 0>
    constexpr Span(element_type (&arr)[N]) noexcept
        : storage_(arr, N)
    {}

    template <typename U, std::size_t N,
              typename std::enable_if<(Extent == dynamic_extent || N == Extent), int>::type = 0>
    constexpr Span(std::array<U, N>& arr) noexcept
        : storage_(arr.data(), N)
    {}

    template <typename U, std::size_t N,
              typename std::enable_if<(Extent == dynamic_extent || N == Extent), int>::type = 0>
    constexpr Span(const std::array<U, N>& arr) noexcept
        : storage_(arr.data(), N)
    {}

    // 动态 extent 可隐式构造；固定 extent 的长度无法在编译期校验，必须显式构造。
    // C++20 可以写作 explicit(Extent != dynamic_extent)，
    // C++11/14 下按 Extent 拆成两个重载来模拟条件性 explicit。
    template <typename U,
              typename std::enable_if<(Extent == dynamic_extent) && std::is_same<U, U>::value,
                                      int>::type = 0>
    constexpr Span(std::vector<U>& vec) noexcept
        : storage_(vec.data(), vec.size()) {}

    template <typename U,
              typename std::enable_if<(Extent != dynamic_extent) && std::is_same<U, U>::value,
                                      int>::type = 0>
    constexpr explicit Span(std::vector<U>& vec) noexcept
        : storage_(vec.data(), vec.size()) {}

    template <typename U,
              typename std::enable_if<(Extent == dynamic_extent) && std::is_same<U, U>::value,
                                      int>::type = 0>
    constexpr Span(const std::vector<U>& vec) noexcept
        : storage_(vec.data(), vec.size()) {}

    template <typename U,
              typename std::enable_if<(Extent != dynamic_extent) && std::is_same<U, U>::value,
                                      int>::type = 0>
    constexpr explicit Span(const std::vector<U>& vec) noexcept
        : storage_(vec.data(), vec.size()) {}

#if 0 // 暂时不支持其他容器类型，因为无法确定其内存是否连续
    template <typename Container>
    constexpr Span(Container& cont) noexcept
        : storage_(cont.data(), cont.size()) {}

    template <typename Container>
    constexpr Span(const Container& cont) noexcept
        : storage_(cont.data(), cont.size()) {}
#endif

    // 元素访问
    constexpr reference operator[](index_type idx) const noexcept {
        return data()[idx];
    }

    constexpr reference front() const noexcept {
        return data()[0];
    }

    constexpr reference back() const noexcept {
        return data()[size() - 1];
    }

    constexpr pointer data() const noexcept {
        return storage_.data_;
    }

    // 迭代器
    constexpr iterator begin() const noexcept {
        return data();
    }

    constexpr iterator end() const noexcept {
        return data() + size();
    }

    constexpr const_iterator cbegin() const noexcept {
        return data();
    }

    constexpr const_iterator cend() const noexcept {
        return data() + size();
    }

    constexpr reverse_iterator rbegin() const noexcept {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() const noexcept {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    // 容量
    constexpr bool empty() const noexcept {
        return size() == 0;
    }

    constexpr index_type size() const noexcept {
        return storage_.size();
    }

    constexpr index_type size_bytes() const noexcept {
        return size() * sizeof(element_type);
    }

    // 子视图
    // 运行时计数版本返回动态 extent 的 span
    A_CONSTEXPR_CXX14 Span<T> subspan(index_type offset, index_type count = static_cast<index_type>(-1)) const noexcept {
        if (count == static_cast<index_type>(-1)) {
            count = size() - offset;
        }
        return Span<T>(data() + offset, count);
    }

    constexpr Span<T> first(index_type count) const noexcept {
        return Span<T>(data(), count);
    }

    constexpr Span<T> last(index_type count) const noexcept {
        return Span<T>(data() + size() - count, count);
    }

    // 编译期计数版本：返回固定 extent 的 span
    template <std::size_t Count>
    constexpr Span<T, Count> first() const noexcept {
        static_assert(Extent == dynamic_extent || Count <= Extent, "Count must not exceed the span extent");
        return Span<T, Count>(data(), Count);
    }

    template <std::size_t Count>
    constexpr Span<T, Count> last() const noexcept {
        static_assert(Extent == dynamic_extent || Count <= Extent, "Count must not exceed the span extent");
        return Span<T, Count>(data() + size() - Count, Count);
    }

    /// @brief 编译期偏移/计数的子视图
    /// @details Count 省略时取到末尾：固定 extent 的 span 返回剩余长度的 span，
    ///          动态 extent 的 span 仍返回动态 extent 的 span。
    template <std::size_t Offset, std::size_t Count = dynamic_extent>
    constexpr Span<T, (Count != dynamic_extent
                           ? Count
                           : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent))>
    subspan() const noexcept {
        static_assert(Extent == dynamic_extent || Offset <= Extent,
                      "Offset must not exceed the span extent");
        static_assert(Count == dynamic_extent || Extent == dynamic_extent || Offset + Count <= Extent,
                      "Offset + Count must not exceed the span extent");
        using Result = Span<T, (Count != dynamic_extent
                                    ? Count
                                    : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent))>;
        return Result(data() + Offset, Count == dynamic_extent ? size() - Offset : Count);
    }

private:
    SpanStorage<T, Extent> storage_;
};

#if A_CXX_VERSION < 201703L
// C++17 之前，静态 constexpr 成员被 odr-use（例如绑定到 const 引用）时需要在类外提供定义；
// C++17 起它是隐式 inline 的，无需再定义
template <typename T, std::size_t Extent>
constexpr std::size_t Span<T, Extent>::extent;
#endif

#ifdef A_CXX17
// 推导指南
template <typename T, std::size_t N>
Span(T (&)[N]) -> Span<T, N>;

template <typename T, std::size_t N>
Span(std::array<T, N>&) -> Span<T, N>;

template <typename T, std::size_t N>
Span(const std::array<T, N>&) -> Span<const T, N>;

template <typename Container>
Span(Container&) -> Span<typename Container::value_type>;

template <typename Container>
Span(const Container&) -> Span<const typename Container::value_type>;
#endif

/*! @} */

AST_NAMESPACE_END