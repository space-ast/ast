///
/// @file      GenericValue.hpp
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
#include "AstUtil/ParseFormat.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 通用值类
class GenericValue
{
public:
    GenericValue(StringView value)
        : value_(value){}
    GenericValue(const char* value)
        : value_(value){}
    GenericValue(const std::string& value)
        : value_(value){}
    GenericValue(int value)
        : value_(aFormatInt(value)){}
    GenericValue(double value)
        : value_(aFormatDouble(value)){}
    GenericValue(bool value)
        : value_(aFormatBool(value)){}
    GenericValue(Color value)
        : value_(aFormatColor(value)){}

    const std::string& value() const { return value_; }
    const char* c_str() const { return value_.c_str(); }
protected:
    std::string value_;
};

AST_NAMESPACE_END
