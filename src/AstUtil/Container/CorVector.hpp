///
/// @file      CorVector.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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
#include <memory>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <type_traits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Container
    @{
*/


/// @brief  Copy-On-Resize/Reserve Vector
/// @details  可以多个CorVector对象共享同一份内存，读取和修改数据
///           当容器需要更改大小时(除了`resize(0)`和`reserve(0)`外)，会进行复制操作
template<typename T>
class CorVector
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using allocator_type = std::allocator<T>;

    // 构造函数
    CorVector() noexcept : data_(nullptr), size_(0), allocator_() {}
    
    explicit CorVector(size_type count) : CorVector() {
        reserve(count);
        resize(count);
    }
    
    CorVector(size_type count, const T& value) : CorVector() {
        reserve(count);
        resize(count, value);
    }
    
    template<typename InputIt,
             typename std::enable_if<!std::is_integral<InputIt>::value, int>::type = 0>
    CorVector(InputIt first, InputIt last) : CorVector() {
        size_type count = std::distance(first, last);
        reserve(count);
        for (; first != last; ++first) {
            push_back(*first);
        }
    }
    
    CorVector(std::initializer_list<T> init) : CorVector(init.begin(), init.end()) {}
    
    CorVector(const CorVector& other) 
        : CorVector() 
    {
        reserve(other.size_);
        for (size_type i = 0; i < other.size_; ++i) {
            push_back(other.data_[i]);
        }
    }
    
    CorVector(CorVector&& other) noexcept 
    {
        this->data_ = other.data_;
        this->size_ = other.size_;
        this->allocator_ = other.allocator_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.allocator_.end_of_storage_ = nullptr;
    }
    
    ~CorVector() {
        if (end_of_storage()) {
            clear();
            allocator().deallocate(data_, end_of_storage() - data_);
        }
    }
    
    // 赋值操作符
    CorVector& operator=(const CorVector& other) {
        if (this != &other) {
            CorVector temp(other);
            swap(temp);
        }
        return *this;
    }
    
    CorVector& operator=(CorVector&& other) noexcept {
        if (this != &other) {
            clear();
            if (end_of_storage()) {
                allocator().deallocate(data_, end_of_storage() - data_);
            }
            data_ = other.data_;
            size_ = other.size_;
            allocator_ = other.allocator_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.allocator_.end_of_storage_ = nullptr;
        }
        return *this;
    }
    
    CorVector& operator=(std::initializer_list<T> init) {
        CorVector temp(init);
        swap(temp);
        return *this;
    }
    
    // 元素访问
    reference operator[](size_type pos) {
        return data_[pos];
    }
    
    const_reference operator[](size_type pos) const {
        return data_[pos];
    }
    
    reference at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range("CorVector::at: index out of range");
        }
        return data_[pos];
    }
    
    const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw std::out_of_range("CorVector::at: index out of range");
        }
        return data_[pos];
    }
    
    reference front() {
        return data_[0];
    }
    
    const_reference front() const {
        return data_[0];
    }
    
    reference back() {
        return data_[size_ - 1];
    }
    
    const_reference back() const {
        return data_[size_ - 1];
    }
    
    T* data() noexcept {
        return data_;
    }
    
    const T* data() const noexcept {
        return data_;
    }
    
    // 迭代器
    iterator begin() noexcept {
        return data_;
    }
    
    const_iterator begin() const noexcept {
        return data_;
    }
    
    const_iterator cbegin() const noexcept {
        return data_;
    }
    
    iterator end() noexcept {
        return data_ + size();
    }
    
    const_iterator end() const noexcept {
        return data_ + size();
    }
    
    const_iterator cend() const noexcept {
        return data_ + size();
    }
    
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }
    
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }
    
    // 容量
    bool empty() const noexcept {
        return size_ == 0;
    }
    
    size_type size() const noexcept {
        return size_;
    }
    
    size_type max_size() const noexcept {
        return allocator().max_size();
    }
    
    size_type capacity() const noexcept {
        return end_of_storage() ? (end_of_storage() - data_) : 0;
    }
    
    void reserve(size_type new_cap) {
        if (new_cap > capacity()) {
            // 如果当前是共享内存（capacity_为nullptr）或者需要更大的容量，进行复制
            allocate_and_copy(new_cap);
        }
    }
    
    void shrink_to_fit() {
        if (capacity() > size()) {
            allocate_and_copy(size());
        }
    }
    
    // 修改器
    void clear() noexcept {
        if (end_of_storage()) {
            for (size_type i = 0; i < size(); ++i) {
                allocator().destroy(data_ + i);
            }
            size_ = 0;
        }
    }
    
    iterator insert(const_iterator pos, const T& value) {
        difference_type offset = pos - cbegin();
        if (size_ >= capacity()) {
            reserve(size_ + 1);
        }
        std::move_backward(data_ + offset, data_ + size_, data_ + size_ + 1);
        allocator().construct(data_ + offset, value);
        ++size_;
        return data_ + offset;
    }
    
    iterator insert(const_iterator pos, T&& value) {
        difference_type offset = pos - cbegin();
        if (size_ >= capacity()) {
            reserve(size_ + 1);
        }
        std::move_backward(data_ + offset, data_ + size_, data_ + size_ + 1);
        allocator().construct(data_ + offset, std::move(value));
        ++size_;
        return data_ + offset;
    }
    
    iterator insert(const_iterator pos, size_type count, const T& value) {
        difference_type offset = pos - cbegin();
        if (size_ + count > capacity()) {
            reserve(size_ + count);
        }
        std::move_backward(data_ + offset, data_ + size_, data_ + size_ + count);
        for (size_type i = 0; i < count; ++i) {
            allocator().construct(data_ + offset + i, value);
        }
        size_ += count;
        return data_ + offset;
    }
    
    template<typename InputIt,
             typename std::enable_if<!std::is_integral<InputIt>::value, int>::type = 0>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        difference_type offset = pos - cbegin();
        size_type count = std::distance(first, last);
        if (size_ + count > capacity()) {
            reserve(size_ + count);
        }
        std::move_backward(data_ + offset, data_ + size_, data_ + size_ + count);
        T* cur = data_ + offset;
        for (; first != last; ++first, ++cur) {
            allocator().construct(cur, *first);
        }
        size_ += count;
        return data_ + offset;
    }
    
    iterator insert(const_iterator pos, std::initializer_list<T> init) {
        return insert(pos, init.begin(), init.end());
    }
    
    iterator erase(const_iterator pos) {
        difference_type offset = pos - cbegin();
        std::move(data_ + offset + 1, data_ + size_, data_ + offset);
        --size_;
        allocator().destroy(data_ + size_);
        return data_ + offset;
    }
    
    iterator erase(const_iterator first, const_iterator last) {
        difference_type start = first - cbegin();
        difference_type count = last - first;
        std::move(data_ + start + count, data_ + size_, data_ + start);
        for (difference_type i = 0; i < count; ++i) {
            allocator().destroy(data_ + size_ - i - 1);
        }
        size_ -= count;
        return data_ + start;
    }
    
    void push_back(const T& value) {
        if (size_ >= capacity()) {
            reserve(size_ == 0 ? 1 : size_ * 2);
        }
        allocator().construct(data_ + size_, value);
        ++size_;
    }
    
    void push_back(T&& value) {
        if (size_ >= capacity()) {
            reserve(size_ == 0 ? 1 : size_ * 2);
        }
        allocator().construct(data_ + size_, std::move(value));
        ++size_;
    }
    
    void pop_back() {
        assert(size_ > 0);
        --size_;
        allocator().destroy(data_ + size_);
    }
    
    void resize(size_type count) {
        if (count > size_) {
            if (count > capacity()) {
                reserve(count);
            }
            for (size_type i = size_; i < count; ++i) {
                allocator().construct(data_ + i);
            }
        } else if (count < size_) {
            for (size_type i = count; i < size_; ++i) {
                allocator().destroy(data_ + i);
            }
        }
        size_ = count;
    }
    
    void resize(size_type count, const T& value) {
        if (count > size_) {
            if (count > capacity()) {
                reserve(count);
            }
            for (size_type i = size_; i < count; ++i) {
                allocator().construct(data_ + i, value);
            }
        } else if (count < size_) {
            for (size_type i = count; i < size_; ++i) {
                allocator().destroy(data_ + i);
            }
        }
        size_ = count;
    }
    
    void swap(CorVector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(allocator_, other.allocator_);
    }
public:
    /// @brief 判断容器是否为借用内存
    bool is_borrowed() const noexcept {
        return end_of_storage() == nullptr;
    }
    /// @brief 判断容器是否为拥有内存
    bool is_owned() const noexcept {
        return end_of_storage() != nullptr;
    }
    /// @brief 从外部借用内存
    void borrow_from(T* data, size_type size) noexcept {
        data_ = data;
        size_ = size;
        allocator_.end_of_storage_ = nullptr;
    }
protected:
    // EBCO
    struct CorVectorAllocator: allocator_type {
        T* end_of_storage_{nullptr};   ///< 如果该指针为nullptr，则表示容器为借用内存，如果该指针为非空，则表示容器当前容量，指向容器可用内存末尾的下一个位置
    };
    allocator_type& allocator() noexcept{
        return allocator_;
    }
    const allocator_type& allocator() const noexcept{
        return allocator_;
    }
    T* end_of_storage() const noexcept {
        return allocator_.end_of_storage_;
    }
protected:
    T*                 data_{nullptr};      ///< 指向数据的指针
    size_t             size_{0};            ///< 容器当前大小
    CorVectorAllocator allocator_{};        ///< 内存分配器
protected:

    // 分配内存并复制数据
    void allocate_and_copy(size_type new_cap) {
        if (new_cap == 0) {
            if (end_of_storage()) {
                clear();
                allocator().deallocate(data_, end_of_storage() - data_);
                data_ = nullptr;
                allocator_.end_of_storage_ = nullptr;
            }
            return;
        }
        
        T* new_data = allocator().allocate(new_cap);
        size_type copy_size = std::min(size_, new_cap);
        
        // 复制现有数据
        for (size_type i = 0; i < copy_size; ++i) {
            allocator().construct(new_data + i, data_[i]);
        }
        
        // 清理旧数据
        if (end_of_storage()) {
            clear();
            allocator().deallocate(data_, end_of_storage() - data_);
        }
        
        data_ = new_data;
        allocator_.end_of_storage_ = data_ + new_cap;
        size_ = copy_size;
    }
};

/*! @} */

AST_NAMESPACE_END