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

AST_NAMESPACE_BEGIN

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

    /// @brief 转换为浮点数
    double toDouble() const { return aParseDouble(value_); }
    err_t toDouble(double& value) const { return aParseDouble(value_, value); }
    
    /// @brief 转换为布尔值
    bool toBool() const { return aParseBool(value_); }
    err_t toBool(bool& value) const { return aParseBool(value_, value); }
    
    /// @brief 转换为颜色值
    Color toColor() const { return aParseColor(value_); }
    err_t toColor(Color& value) const { return aParseColor(value_, value); }
    
    /// @brief 转换为字符串
    std::string toString() const { return value_.to_string(); }
    void toString(std::string& value) const { value = value_.to_string(); }
    
    /// @brief 转换为字符串视图
    const StringView& toStringView() const { return value_; }

    /// @brief 转换为通用值
    GenericValue toValue() const { return GenericValue(value_); }

    operator StringView() const { return value_; }

    const char* data() const { return value_.data(); }
    size_t size() const { return value_.size(); }
public:
    StringView  value_;
};


AST_NAMESPACE_END
