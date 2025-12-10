///
/// @file      ParseFormat.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-10
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

#include "ParseFormat.hpp"
#include "AstUtil/Color.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <string>
#include <iomanip>

AST_NAMESPACE_BEGIN


err_t aParseBool(StringView str, bool& value)
{
    if (str.empty() || !str.data())
    {
        return eErrorNullInput;
    }

    // 转换为小写进行比较
    std::string lower_str(str.data(), str.size());
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);

    if (lower_str == "true" || lower_str == "1")
    {
        value = true;
        return eNoError;
    }
    else if (lower_str == "false" || lower_str == "0")
    {
        value = false;
        return eNoError;
    }

    return eErrorParse;
}



err_t aParseInt(StringView str, int& value)
{
    if (str.empty() || !str.data())
    {
        return eErrorNullInput;
    }

    // 使用标准库函数strtol进行转换
    char* endptr = nullptr;
    long result = std::strtol(str.data(), &endptr, 10);
    
    // 检查转换是否成功
    if (endptr == str.data())
    {
        return eErrorParse;
    }

    // 检查是否超出int范围
    if (result < INT_MIN || result > INT_MAX)
    {
        return eErrorInvalidParam;
    }

    value = static_cast<int>(result);
    return eNoError;
}


err_t aParseDouble(StringView str, double& value)
{
    if (str.empty() || !str.data())
    {
        return eErrorNullInput;
    }

    // 使用标准库函数strtod进行转换
    char* endptr = nullptr;
    double result = std::strtod(str.data(), &endptr);

    // 检查转换是否成功
    if (endptr == str.data() )
    {
        return eErrorParse;
    }

    value = result;
    return eNoError;
}



err_t aParseColor(StringView str, Color& value)
{
    if (str.empty() || !str.data())
    {
        return eErrorNullInput;
    }

    // 支持RGB和RGBA格式：#RRGGBB 或 #RRGGBBAA
    if (str.size() == 7 && str[0] == '#')
    {
        // RGB格式
        uint32_t rgb = 0;
        char* endptr = nullptr;
        rgb = std::strtoul(str.data() + 1, &endptr, 16);
        if (endptr != str.data() + 7)
        {
            return eErrorParse;
        }
        // 设置透明度为255
        value = (rgb << 8) | 0xFF;
        return eNoError;
    }
    else if (str.size() == 9 && str[0] == '#')
    {
        // RGBA格式
        uint32_t rgba = 0;
        char* endptr = nullptr;
        rgba = std::strtoul(str.data() + 1, &endptr, 16);
        if (endptr != str.data() + 9)
        {
            return eErrorParse;
        }
        value = rgba;
        return eNoError;
    }

    return eErrorParse;
}

void aFormatBool(bool value, std::string &str)
{
    str = value ? "true" : "false";
}

err_t aFormatInt(int value, std::string& str)
{
    char int_buffer[32];
    int len = std::snprintf(int_buffer, sizeof(int_buffer), "%d", value);
    if (len < 0 || len >= static_cast<int>(sizeof(int_buffer)))
    {
        return eErrorInvalidParam;
    }

    str = std::string(int_buffer, len);
    return eNoError;
}


err_t aFormatDouble(double value, std::string& str, int precision)
{
    char double_buffer[32];
    // 限制精度范围
    if (precision < 0 || precision > 30)
    {
        precision = 15;
    }

    int len = std::snprintf(double_buffer, sizeof(double_buffer), "%.*g", precision, value);
    if (len < 0 || len >= static_cast<int>(sizeof(double_buffer)))
    {
        return eErrorInvalidParam;
    }

    str = std::string(double_buffer, len);
    return eNoError;
}


err_t aFormatColor(Color value, std::string& str)
{
    char color_buffer[32];
    // 格式化为#RRGGBBAA
    int len = std::snprintf(color_buffer, sizeof(color_buffer), "#%08X", value);
    if (len < 0 || len >= static_cast<int>(sizeof(color_buffer)))
    {
        return eErrorInvalidParam;
    }

    str = std::string(color_buffer, len);
    return eNoError;
}


AST_NAMESPACE_END
