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
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <initializer_list>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Util
    @{
*/

constexpr size_t dynamic_extent = static_cast<size_t>(-1);

template<typename T, size_t Extent>
class SpanStorage{
public:
    SpanStorage(T* data, size_t size) noexcept 
        : data_(data) 
    {
        static_assert(size == Extent, "Size must match the extent for fixed-size spans");
    }
    T* data() const noexcept { return data_; }
    constexpr size_t size() const noexcept { return Extent; }
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

    static constexpr std::ptrdiff_t extent = Extent;

    // 构造函数
    constexpr Span() noexcept 
        : storage_(nullptr, 0) 
        {}

    constexpr Span(std::initializer_list<T> init) noexcept
        : storage_(init.begin(), init.size()) {}

    constexpr Span(pointer ptr, index_type count) noexcept
        : storage_(ptr, count) {}

    constexpr Span(pointer first, pointer last) noexcept
        : storage_(first, last - first) {}

    template <std::size_t N>
    constexpr Span(element_type (&arr)[N]) noexcept
        : storage_(arr, N) {}

    template <typename U, std::size_t N>
    constexpr Span(std::array<U, N>& arr) noexcept
        : storage_(arr.data(), N) {}

    template <typename U, std::size_t N>
    constexpr Span(const std::array<U, N>& arr) noexcept
        : storage_(arr.data(), N) {}

    template <typename Container>
    constexpr Span(Container& cont) noexcept
        : storage_(cont.data(), cont.size()) {}

    template <typename Container>
    constexpr Span(const Container& cont) noexcept
        : storage_(cont.data(), cont.size()) {}

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
        return storage_.size_;
    }

    constexpr index_type size_bytes() const noexcept {
        return size() * sizeof(element_type);
    }

    // 子视图
    constexpr Span subspan(index_type offset, index_type count = static_cast<index_type>(-1)) const noexcept {
        if (count == static_cast<index_type>(-1)) {
            count = size() - offset;
        }
        return Span(data() + offset, count);
    }

    constexpr Span first(index_type count) const noexcept {
        return Span(data(), count);
    }

    constexpr Span last(index_type count) const noexcept {
        return Span(data() + size() - count, count);
    }

private:
    SpanStorage<T, Extent> storage_;
};

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