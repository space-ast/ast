///
/// @file      ValueView.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-08
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/Color.hpp"
#include "AstUtil/GenericValue.hpp"
#include "AstUtil/StringSplit.hpp"
#include "AstUtil/StringUtil.hpp"

AST_NAMESPACE_BEGIN

class ByCommaAndRepeatedWhitespace;
struct SkipBracket;

/// @brief 值视图类
/// @brief 提供对值的视图操作，包括解析和格式化值。
/// @ingroup ParseFormat
class ValueView
{
public:
    ValueView() = default;
    ValueView(StringView value)
        : value_(value){}
    ValueView(const char* value)
        : value_(value){}
    ValueView(const std::string& value)
        : value_(value){}
    ValueView(GenericValue value)
        : value_(value.value()) {}
    
    /// @brief 获取值视图
    const StringView& value() const { return value_; }
    StringView& value() { return value_; }

    /// @brief 转换为整数
    int toInt() const { return aParseInt(value_); }
    err_t toInt(int& value) const { return aParseInt(value_, value); }

    /// @brief 转换为浮点数，不支持 Fortran 格式浮点数
    double toDouble() const { return aParseDouble(value_); }
    err_t toDouble(double& value) const { return aParseDouble(value_, value); }
    
    /// @brief 转换为浮点数，支持 Fortran 格式浮点数，例如 "1.23D2"，"-1.23d3"
    double toFortranDouble() const { return aParseFortranDouble(value_); }
    err_t toFortranDouble(double& value) const { return aParseFortranDouble(value_, value); }

    /// @brief 转换为布尔值
    bool toBool() const { return aParseBool(value_); }
    err_t toBool(bool& value) const { return aParseBool(value_, value); }
    
    /// @brief 转换为颜色值
    Color toColor() const { return aParseColor(value_); }
    err_t toColor(Color& value) const { return aParseColor(value_, value); }
    
    /// @brief 转换为字符串
    std::string toString() const { return std::string(value_); }
    void toString(std::string& value) const { value = std::string(value_); }

    /// @brief 转换为值视图向量
    std::vector<ValueView> toVector() const;
    void toVector(std::vector<ValueView>& value) const;

    /// @brief 转换为浮点数向量，支持 Fortran 格式浮点数，例如 "1.23D2"，"-1.23d3"
    std::vector<double> toFortranDoubleVector() const;
    err_t toFortranDoubleVector(std::vector<double>& value) const;

    /// @brief 转换为浮点数向量
    std::vector<double> toDoubleVector() const;
    err_t toDoubleVector(std::vector<double>& value) const;

    /// @brief 转换为整数向量
    std::vector<int> toIntVector() const;
    err_t toIntVector(std::vector<int>& value) const;
    
    /// @brief 转换为字符串视图
    const StringView& toStringView() const { return value_; }

    /// @brief 转换为通用值
    GenericValue toValue() const { return GenericValue(value_); }

    operator StringView() const { return value_; }

    const char* data() const { return value_.data(); }
    size_t size() const { return value_.size(); }
protected:
    using SplitterType = Splitter<ByCommaAndRepeatedWhitespace, SkipBracket>;
    SplitterType split() const;
public:
    StringView  value_;
};


inline std::vector<ValueView> ValueView::toVector() const
{
    std::vector<ValueView> value;
    this->toVector(value);
    return value;
}


/// @brief 逗号和重复空格分隔符
class ByCommaAndRepeatedWhitespace {
public:
    StringView Find(StringView text, size_t pos) const {
        while (pos < text.size()) {
            char c = text[pos];
            // 情况1：当前字符是空白
            if (std::isspace(static_cast<unsigned char>(c))) {
                size_t whitespace_start = pos;
                // 跳过连续空白
                while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
                    ++pos;
                }
                // 检查空白之后是否是逗号
                if (pos < text.size() && text[pos] == ',') {
                    // 空白后紧跟逗号，继续跳过逗号后的空白
                    ++pos; // 跳过逗号
                    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
                        ++pos;
                    }
                }
                return text.substr(whitespace_start, pos - whitespace_start);
            }
            
            // 情况2：当前字符是逗号
            if (c == ',') {
                size_t comma_start = pos;
                ++pos; // 跳过逗号
                // 跳过逗号后的空白
                while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
                    ++pos;
                }
                // 返回从逗号开始到其后空白结束的序列
                return text.substr(comma_start, pos - comma_start);
            }
            
            // 其他字符，继续向后查找
            ++pos;
        }
        // 未找到任何分隔符，返回文本末尾的空视图
        return StringView(text.data() + text.size(), 0);
    }
};

/// @brief 跳过括号
struct SkipBracket
{
    bool operator()(StringView& text) const
    {
        if (text.size() == 1) {
            if (text[0] == '(' || text[0] == ')') {
                return false;
            }
        }
        return true;
    }
};

inline void ValueView::toVector(std::vector<ValueView>& value) const
{
    value = split().operator std::vector<ValueView>();
}

inline std::vector<double> ValueView::toFortranDoubleVector() const
{
    std::vector<double> value;
    this->toFortranDoubleVector(value);
    return value;
}

inline err_t ValueView::toFortranDoubleVector(std::vector<double> &value) const
{
    for (auto& v : split()) {
        double d;
        if (err_t rc = ValueView(v).toFortranDouble(d)) {
            return rc;
        }
        value.push_back(d);
    }
    return eNoError;
}

inline std::vector<double> ValueView::toDoubleVector() const
{
    std::vector<double> value;
    this->toDoubleVector(value);
    return value;
}

inline err_t ValueView::toDoubleVector(std::vector<double> &value) const
{
    for (auto& v : split()) {
        double d;
        if (err_t rc = ValueView(v).toDouble(d)) {
            return rc;
        }
        value.push_back(d);
    }
    return eNoError;
}

inline std::vector<int> ValueView::toIntVector() const
{
    std::vector<int> value;
    this->toIntVector(value);
    return value;
}

inline err_t ValueView::toIntVector(std::vector<int>& value) const
{
    for (auto& v : split()) {
        int i;
        if (err_t rc = ValueView(v).toInt(i)) {
            return rc;
        }
        value.push_back(i);
    }
    return eNoError;
}

inline ValueView::SplitterType ValueView::split() const
{
    return aStrSplit(aStripAsciiWhitespace(value_), ByCommaAndRepeatedWhitespace(), SkipBracket());
}

AST_NAMESPACE_END
