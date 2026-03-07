///
/// @file      StringSplitter.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-06
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
#include "StringView.hpp"
#include <vector>
#include <cctype>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup String
    @{
*/


namespace strings_internal {

// 基础分隔符概念
struct DelimiterConcept {
    StringView Find(StringView text, size_t pos) const {
        return StringView();
    }
};

/// @brief 重复字符分隔符
class ByRepeatedChar {
public:
    explicit ByRepeatedChar(char c) : c_(c) {}
    
    StringView Find(StringView text, size_t pos) const {
        size_t start = text.find(c_, pos);
        if (start == StringView::npos) {
            return StringView(text.data() + text.size(), 0); // 未找到
        }
        size_t end = start;
        while (end < text.size() && text[end] == c_) {
            ++end;
        }
        return text.substr(start, end - start);
    }
private:
    char c_;
};

/// @brief 重复空白字符分隔符
class ByRepeatedWhitespace {
public:
    StringView Find(StringView text, size_t pos) const {
        // 查找第一个空白字符
        while (pos < text.size()) {
            if(std::isspace(static_cast<unsigned char>(text[pos])))
            {
                size_t start = pos;
                // 连续跳过所有空白字符
                while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
                    ++pos;
                }
                return text.substr(start, pos - start);
            }
            ++pos;
        }
        // 未找到分隔符，返回文本末尾的空视图
        return StringView(text.data() + text.size(), 0);
    }
};

/// @brief ASCII空白字符分隔符
class ByAsciiWhitespace
 {
public:
    StringView Find(StringView text, size_t pos) const {
        while(pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
            pos++;
        }
        return text.substr(pos, 1);
    }
};

/// @brief 字符串分隔符
class ByString {
public:
    explicit ByString(StringView sp) 
        : delimiter_(sp) {}
    
    StringView Find(StringView text, size_t pos) const {
        if(delimiter_.empty())
        {
            return text.substr(pos + 1, 0);
        }
        else if(delimiter_.size() == 1)
        {
            auto found = text.find(delimiter_[0], pos);
            if (found == StringView::npos) {
                return text.substr(text.size());
            }
            return text.substr(found, 1);
        }
        else{
            auto found = text.find(delimiter_, pos);
            if (found == StringView::npos) {
                return text.substr(text.size());
            }
            return text.substr(found, delimiter_.size());
        }
    }
private:
    StringView delimiter_;
};

/// @brief 字符分隔符
class ByChar {
public:
    explicit ByChar(char c) : c_(c) {}
    
    StringView Find(StringView text, size_t pos) const {
        auto found = text.find(c_, pos);
        if (found == StringView::npos) {
            return StringView(text.data() + text.size(), 0);
        }
        return text.substr(found, 1);
    }

private:
    char c_;
};

/// @brief 任意字符分隔符
class ByAnyChar {
public:
    explicit ByAnyChar(StringView sp) : delimiters_(sp) {}
    
    StringView Find(StringView text, size_t pos) const {
        auto found = text.find_first_of(delimiters_, pos);
        if (found == StringView::npos) {
            return text.substr(text.size());
        }
        return text.substr(found, 1);
    }

private:
    StringView delimiters_;
};

/// @brief 固定长度分隔符
class ByLength {
public:
    explicit ByLength(std::ptrdiff_t length) : length_(length) {}
    
    StringView Find(StringView text, size_t pos) const {
        if (pos >= text.size()) {
            return text.substr(text.size());
        }
        size_t end = pos + length_;
        if (end > text.size()) {
            end = text.size();
        }
        return text.substr(end, 0);
    }

private:
    const std::ptrdiff_t length_;
};

/// @brief 最大分割次数分隔符
template <typename Delimiter>
class MaxSplitsImpl {
public:
    MaxSplitsImpl(Delimiter delimiter, int limit) 
        : delimiter_(std::move(delimiter)), limit_(limit), count_(0) {}
    
    StringView Find(StringView text, size_t pos) {
        if (count_++ == limit_) {
            return StringView(text.data() + text.size(), 0);
        }
        return delimiter_.Find(text, pos);
    }

private:
    Delimiter delimiter_;
    const int limit_;
    int count_;
};

// 谓词概念
struct AllowEmpty {
    bool operator()(StringView) const { return true; }
};

struct SkipEmpty {
    bool operator()(StringView sp) const { return !sp.empty(); }
};

struct SkipWhitespace {
    bool operator()(StringView sp) const {
        // 手动实现跳过空白字符的逻辑
        size_t start = 0;
        while (start < sp.size()) {
            char c = sp[start];
            if (c != ' ' && c != '\t' && c != '\n' && c != '\r' && c != '\f' && c != '\v') {
                break;
            }
            start++;
        }
        return start < sp.size();
    }
};

// 选择分隔符类型的元函数
template <typename Delimiter>
struct SelectDelimiter {
    using type = Delimiter;
};

template <>
struct SelectDelimiter<char> {
    using type = ByChar;
};

template <>
struct SelectDelimiter<const char*> {
    using type = ByString;
};

template <>
struct SelectDelimiter<StringView> {
    using type = ByString;
};

template <>
struct SelectDelimiter<std::string> {
    using type = ByString;
};

/// @brief 字符串分隔器
/// @details 该类用于将字符串 text 按照分隔符 delimiter 进行分割,
///          并根据分割条件 predicate 进行筛选。
/// @tparam  Delimiter -  分隔符类型
/// @tparam  Predicate -  分割条件类型
/// @tparam  StringType -  字符串类型
template <typename Delimiter, typename Predicate, typename StringType=StringView>
class Splitter {
public:
    Splitter(StringView text, Delimiter delimiter, Predicate predicate)
        : text_(std::move(text))
        , delimiter_(std::move(delimiter))
        , predicate_(std::move(predicate)) 
    {}

    /// @brief 字符串分割迭代器
    class Iterator {
    public:
        using value_type = StringType;
        using difference_type = std::ptrdiff_t;
        using pointer = const StringType*;
        using reference = const StringType&;
        using iterator_category = std::forward_iterator_tag;

        Iterator(const Splitter* splitter, std::nullptr_t)
            : splitter_(splitter)
            , delimiter_(splitter_->delimiter_)
            , predicate_(splitter_->predicate_)
            , current_{nullptr, StringType::npos}
            , pos_(0)
        {
            
        }

        Iterator(const Splitter* splitter)
            : splitter_(splitter)
            , delimiter_(splitter_->delimiter_)
            , predicate_(splitter_->predicate_)
            , current_{}
            , pos_(0) 
        {
            advance();
        }

        reference operator*() const {
            return current_;
        }

        pointer operator->() const {
            return &current_;
        }

        Iterator& operator++() {
            advance();
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return current_.data() == other.current_.data();
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
        bool at_end() const {
            return current_.size() == StringType::npos;
        }

    private:
        void advance() {
            do{
                if(at_end()) {
                    return;
                }
                if(current_.end() == splitter_->text_.end()) {
                    current_ = {nullptr, StringType::npos};
                    return;
                }else{
                    auto delimiter_view = delimiter_.Find(splitter_->text_, pos_);
                    /*!
                    delimiter_view.data() 指向当前字符的结束
                    delimiter_view.end()  指向下个字符的开始
                    */
                    size_t current_end = delimiter_view.data() - splitter_->text_.data();
                    current_ = splitter_->text_.substr(pos_, current_end - pos_);
                    pos_ = current_end + delimiter_view.size();
                }
            }while(!predicate_(current_));
        }

        const Splitter* splitter_;
        Delimiter delimiter_;
        Predicate predicate_;
        StringType current_;        ///< 当前字符的开始
        size_t pos_;                ///< 下个字符的开始
    };
    // end of iterator

    /// @brief 开始迭代器
    Iterator begin() const {
        return Iterator(this);
    }

    /// @brief 结束迭代器
    Iterator end() const {
        return Iterator(this, nullptr);
    }

    /// @brief 转换为容器类型
    template <typename Container>
    operator Container() const {
        Container result;
        using ValueType = typename Container::value_type;
        auto it = std::inserter(result, result.end());
        for (const auto& part : *this) {
            *it++ = ValueType(part);
        }
        return result;
    }
    /// @brief 转换为 std::vector<std::string>
    operator std::vector<std::string>() const{
        const std::vector<StringView> v = this->operator std::vector<StringView>();
        return std::vector<std::string>(v.begin(), v.end());
    }
    /// @brief 转换为 std::vector<StringView>
    operator std::vector<StringView>() const {
        struct raw_view {
            const char* data;
            size_t size;
            operator StringView() const {
                return {data, size};
            }
        };
        std::vector<StringView> v;
        std::array<raw_view, 16> ar;
        for (auto it = this->begin(); !it.at_end();) {
            size_t index = 0;
            do {
                ar[index].data = it->data();
                ar[index].size = it->size();
                ++it;
            } while (++index != ar.size() && !it.at_end());
            v.insert(v.end(), ar.begin(),
                     ar.begin() + static_cast<ptrdiff_t>(index));
        }
        return v;
    }

private:
    StringType text_;
    Delimiter delimiter_;
    Predicate predicate_;
};

} // namespace strings_internal



// 分隔符类型
using ByString = strings_internal::ByString;
using ByChar = strings_internal::ByChar;
using ByAnyChar = strings_internal::ByAnyChar;
using ByLength = strings_internal::ByLength;
using ByRepeatedWhitespace = strings_internal::ByRepeatedWhitespace;
using ByRepeatedChar = strings_internal::ByRepeatedChar;

// 谓词类型
using AllowEmpty = strings_internal::AllowEmpty;
using SkipEmpty = strings_internal::SkipEmpty;
using SkipWhitespace = strings_internal::SkipWhitespace;

// 字符串分割器
using strings_internal::Splitter;

/// @brief  最大分割次数条件
template <typename Delimiter>
inline strings_internal::MaxSplitsImpl<typename strings_internal::SelectDelimiter<Delimiter>::type>
MaxSplits(Delimiter delimiter, int limit) {
    using DelimiterType = typename strings_internal::SelectDelimiter<Delimiter>::type;
    return strings_internal::MaxSplitsImpl<DelimiterType>(DelimiterType(delimiter), limit);
}



/*! @} */

AST_NAMESPACE_END