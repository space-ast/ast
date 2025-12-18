///
/// @file      QuantityParser.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-15
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
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "QuantityParser.hpp"
#include "Quantity.hpp"
#include "UnitParser.hpp"

AST_NAMESPACE_BEGIN

static void skipWhitespace(const char*& position, const char* end)
{
    while (position < end && isspace(static_cast<unsigned char>(*position)))
    {
        position++;
    }
}

err_t aQuantityParse(StringView sv, double& value, Unit& unit)
{
    std::string str = sv.to_string();
    const char* end = str.c_str() + str.size();
    const char* position = nullptr;

    value = strtod(str.c_str(), const_cast<char**>(&position));
    if (!position || position == str.c_str())
    {
        value = 0;
        unit = Unit::NaN();
        return eErrorParse;
    }

    // 如果已经解析到了字符串的末尾，则单位为空
    if (*position == '\0' || position >= end)
    {
        unit = Unit::None();
        return eNoError;
    }

    {
        skipWhitespace(position, end);
        StringView sv(position, end - position);
        if (!sv.empty() && sv.front() == '[') {
            auto pos = sv.rfind(']');
            if (pos == StringView::npos)
            {
                return eErrorParse;
            }
            sv = sv.substr(1, pos - 1);
        }

        return aUnitParse(sv, unit);
    }
}

err_t aQuantityParse(StringView str, Quantity& quantity)
{
    return aQuantityParse(str, quantity.value(), quantity.unit());
}

Quantity aQuantityParse(StringView str)
{
    Quantity quantity;
    if (aQuantityParse(str, quantity) != eNoError)
    {
        return Quantity::NaN();
    }
    return quantity;
}

AST_NAMESPACE_END
