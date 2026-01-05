///
/// @file      StringView.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      1.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
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
#include "String.hpp"
#include <algorithm>
#include <cstring>
#include <ostream>
#include <string>
#include <iterator>
 
AST_NAMESPACE_BEGIN


// 轻量级的string_view实现
template<typename _Char>
class StringViewBasic
{
public:
    // 类型定义
    typedef _Char                                            value_type;
    typedef const _Char*                                     pointer;
    typedef const _Char&                                     reference;
    typedef const _Char&                                     const_reference;
    typedef const _Char*                                     const_iterator;
    typedef const _Char*                                     iterator;
    typedef typename std::reverse_iterator<const_iterator>   const_reverse_iterator;
    typedef typename std::reverse_iterator<iterator>         reverse_iterator;
    typedef size_t                                           size_type;
    typedef ptrdiff_t                                        difference_type;

    static const size_type npos = static_cast<size_type>(-1);

private:
    const _Char* m_data;
    size_type m_size;

public:
    // 构造函数
    constexpr StringViewBasic() noexcept
        : m_data(nullptr), m_size(0)
    {}

    constexpr StringViewBasic(const _Char* str)
        : m_data(str), m_size(str ? traits_type::length(str) : 0)
    {}

    constexpr StringViewBasic(const _Char* str, size_type len)
        : m_data(str), m_size(len)
    {}

    template<typename _Allocator>
    StringViewBasic(const std::basic_string<_Char, std::char_traits<_Char>, _Allocator>& str)
        : m_data(str.data()), m_size(str.size())
    {}

    // 拷贝构造和赋值
    constexpr StringViewBasic(const StringViewBasic& other) noexcept = default;

    StringViewBasic& operator=(const StringViewBasic& other) noexcept = default;

    // 容量相关
    constexpr size_type size() const noexcept
    {
        return m_size;
    }
    constexpr size_type length() const noexcept
    {
        return m_size;
    }
    constexpr bool empty() const noexcept
    {
        return m_size == 0;
    }

    // 元素访问
    constexpr const _Char& operator[](size_type pos) const noexcept
    {
        return m_data[pos];
    }

    constexpr const _Char& front() const noexcept
    {
        return m_data[0];
    }

    constexpr const _Char& back() const noexcept
    {
        return m_data[m_size - 1];
    }

    constexpr const _Char* data() const noexcept
    {
        return m_data;
    }

    // 迭代器
    constexpr const_iterator begin() const noexcept
    {
        return m_data;
    }
    constexpr const_iterator end() const noexcept
    {
        return m_data + m_size;
    }
    constexpr const_iterator cbegin() const noexcept
    {
        return m_data;
    }
    constexpr const_iterator cend() const noexcept
    {
        return m_data + m_size;
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    // 修改操作（不修改底层数据，只修改视图）
    void remove_prefix(size_type n) noexcept
    {
        if (n > m_size) n = m_size;
        m_data += n;
        m_size -= n;
    }

    void remove_suffix(size_type n) noexcept
    {
        if (n > m_size) n = m_size;
        m_size -= n;
    }

    void swap(StringViewBasic& other) noexcept
    {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
    }

    // 子串操作
    StringViewBasic substr(size_type pos = 0, size_type count = npos) const
    {
        if (pos > m_size) {
            // 抛出异常或返回空视图
            return StringViewBasic();
        }
        size_type rlen = std::min(count, m_size - pos);
        return StringViewBasic(m_data + pos, rlen);
    }

    // 比较操作
    int compare(StringViewBasic other) const noexcept
    {
        size_type len = std::min(m_size, other.m_size);
        int result = traits_type::compare(m_data, other.m_data, len);
        if (result != 0) return result;
        if (m_size < other.m_size) return -1;
        if (m_size > other.m_size) return 1;
        return 0;
    }

    bool operator==(StringViewBasic other) const noexcept
    {
        return m_size == other.m_size &&
            (m_data == other.m_data ||
                traits_type::compare(m_data, other.m_data, m_size) == 0);
    }

    bool operator!=(StringViewBasic other) const noexcept
    {
        return !(*this == other);
    }

    bool operator<(StringViewBasic other) const noexcept
    {
        return compare(other) < 0;
    }

    bool operator<=(StringViewBasic other) const noexcept
    {
        return compare(other) <= 0;
    }

    bool operator>(StringViewBasic other) const noexcept
    {
        return compare(other) > 0;
    }

    bool operator>=(StringViewBasic other) const noexcept
    {
        return compare(other) >= 0;
    }

    // 查找操作
    size_type find(_Char ch, size_type pos = 0) const noexcept
    {
        if (pos >= m_size) return npos;
        const _Char* result = traits_type::find(m_data + pos, m_size - pos, ch);
        return result ? static_cast<size_type>(result - m_data) : npos;
    }

    size_type find(StringViewBasic str, size_type pos = 0) const noexcept
    {
        if (pos > m_size || str.empty() || str.size() > m_size - pos)
            return npos;

        const _Char* start = m_data + pos;
        const _Char* end = m_data + m_size - str.size() + 1;

        for (const _Char* p = start; p < end; ++p) {
            if (traits_type::compare(p, str.data(), str.size()) == 0) {
                return static_cast<size_type>(p - m_data);
            }
        }

        return npos;
    }

    size_type rfind(_Char ch, size_type pos = npos) const noexcept
    {
        if (m_size == 0) return npos;
        if (pos >= m_size) pos = m_size - 1;

        for (size_type i = pos + 1; i > 0; --i) {
            if (m_data[i - 1] == ch) {
                return i - 1;
            }
        }

        return npos;
    }

    // 检查是否以指定前缀/后缀开头/结尾
    bool starts_with(StringViewBasic str) const noexcept
    {
        return m_size >= str.size() &&
            traits_type::compare(m_data, str.data(), str.size()) == 0;
    }

    bool starts_with(_Char ch) const noexcept
    {
        return !empty() && front() == ch;
    }

    bool ends_with(StringViewBasic str) const noexcept
    {
        return m_size >= str.size() &&
            traits_type::compare(m_data + m_size - str.size(), str.data(), str.size()) == 0;
    }

    bool ends_with(_Char ch) const noexcept
    {
        return !empty() && back() == ch;
    }

    // 转换为字符串
    std::basic_string<_Char> to_string() const
    {
        return std::basic_string<_Char>(m_data, m_size);
    }

    // 显式转换到 std::string
    explicit operator std::basic_string<_Char>() const
    {
        return to_string();
    }

private:
    typedef std::char_traits<_Char> traits_type;
};


// 注意：如果ast工程不启用命名空间编译，而同时又使用了using namespace std; 则会造成冲突
// using namespace std在大型工程里是不好的做法
// 但如果已经这样做了，则需要启用ast命名空间编译，且不要直接using namespace ast

#if defined AST_ENABLE_OVERRIDE_STDLIB   // 覆盖标准库函数

// 特化常用的字符类型
typedef StringViewBasic<char>     string_view;
typedef StringViewBasic<wchar_t>  wstring_view;
typedef StringViewBasic<char16_t> u16string_view;
typedef StringViewBasic<char32_t> u32string_view;

#endif

typedef StringViewBasic<char>     StringView;
typedef StringViewBasic<wchar_t>  WStringView;
typedef StringViewBasic<char16_t> U16StringView;
typedef StringViewBasic<char32_t> U32StringView;


// 流输出操作符
template<typename _Char>
std::basic_ostream<_Char>& operator<<(std::basic_ostream<_Char>& os, const StringViewBasic<_Char>& sv)
{
    if (sv.size() > 0) {
        os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
    }
    return os;
}

// 字面量操作符（C++11不支持用户定义字面量用于模板，这里只提供char版本）
inline StringView operator"" _sv(const char* str, size_t len) noexcept
{
    return StringView(str, len);
}

inline WStringView operator"" _sv(const wchar_t* str, size_t len) noexcept
{
    return WStringView(str, len);
}


 
 
AST_NAMESPACE_END
 

// 哈希支持
namespace std
{
    template<typename _Char>
    struct hash<_AST StringViewBasic<_Char>>
    {
        size_t operator()(const _AST StringViewBasic<_Char>& sv) const noexcept
        {
            // 简单的FNV-1a哈希算法
            size_t result = 2166136261U;
            for (auto ch : sv) {
                result = (result ^ static_cast<size_t>(ch)) * 16777619U;
            }
            return result;
        }
    };
}
