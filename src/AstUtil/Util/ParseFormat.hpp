///
/// @file      ParseFormat.hpp
/// @brief     解析和格式化字符串
/// @details   
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Logger.hpp"
#include <string>


AST_NAMESPACE_BEGIN

/// @brief 将字符串转换为布尔值
/// @param str 输入字符串
/// @param value 输出布尔值
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t aParseBool(StringView str, bool& value);


/// @brief 将字符串转换为整数
/// @param str 输入字符串
/// @param value 输出整数
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t aParseInt(StringView str, int& value);


/// @brief 内部函数，将字符串转换为整数（基于C标准库函数）
/// @param str 输入字符串
/// @param value 输出整数
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t _aParseInt_LibC_1(StringView str, int& value);      // 基于 strtol 函数(stol函数)
AST_UTIL_CAPI err_t _aParseInt_LibC_2(StringView str, int& value);      // 基于 atoi 函数

#if A_CXX_VERSION >= 17
AST_UTIL_CAPI err_t _aParseInt_FromChars(StringView str, int& value);    // 基于 std::from_chars 函数
#endif


/// @brief 内部函数，将字符串转换为整数（简单实现）
/// @param str 输入字符串
/// @param value 输出整数
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t _aParseInt_Simple(StringView str, int& value);


/// @brief 内部函数，将字符串转换为整数（基于 stringstream 流解析）
/// @param str 输入字符串
/// @param value 输出整数
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t _aParseInt_StringStream(StringView str, int& value);


/// @brief 内部函数，将字符串转换为整数（基于 scanf 函数）
/// @param str 输入字符串
/// @param value 输出整数
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t _aParseInt_Scanf(StringView str, int& value);      // 基于 scanf 函数


/// @brief 将字符串转换为双精度浮点数
/// @param str 输入字符串
/// @param value 输出双精度浮点数
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t aParseDouble(StringView str, double& value);


/// @brief 内部函数，将字符串转换为双精度浮点数（基于C标准库函数）
/// @param str 输入字符串
/// @param value 输出双精度浮点数
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t _aParseDouble_LibC_1(StringView str, double& value);  // 基于 strtod 函数(stod函数)
AST_UTIL_CAPI err_t _aParseDouble_LibC_2(StringView str, double& value);  // 基于 atof 函数


#if A_CXX_VERSION >= 17
AST_UTIL_CAPI err_t _aParseDouble_FromChars(StringView str, double& value);    // 基于 std::from_chars 函数
#endif

#ifdef AST_WITH_ABSEIL
AST_UTIL_CAPI err_t _aParseDouble_FromChars_Abseil(StringView str, double& value);
#endif

/// @brief 内部函数，将字符串转换为双精度浮点数（基于 stringstream 流解析）
/// @param str 输入字符串
/// @param value 输出双精度浮点数
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t _aParseDouble_StringStream(StringView str, double& value);



/// @brief 内部函数，将字符串转换为双精度浮点数（基于 scanf 函数）
/// @param str 输入字符串
/// @param value 输出双精度浮点数
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t _aParseDouble_Scanf(StringView str, double& value);      // 基于 scanf 函数



/// @brief 将字符串转换为颜色值
/// @param str 输入字符串
/// @param value 输出颜色值
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t aParseColor(StringView str, Color& value);



/// @brief 将布尔值格式化为字符串
/// @param value 输入布尔值
/// @param str 输出字符串
AST_UTIL_CAPI void aFormatBool(bool value, std::string& str);



/// @brief 将整数格式化为字符串
/// @param value 输入整数
/// @param str 输出字符串
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t aFormatInt(int value, std::string& str);


enum {kDefaultFormatDoubleEps = -1};

/// @brief 将双精度浮点数格式化为字符串
/// @param value 输入双精度浮点数
/// @param str 输出字符串
/// @param precision 格式化精度，默认自适应
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t aFormatDouble(double value, std::string& str, int precision = kDefaultFormatDoubleEps);


/// @brief 将颜色值格式化为字符串
/// @param value 输入颜色值
/// @param str 输出字符串
/// @return 0 成功，其他值 失败
AST_UTIL_CAPI err_t aFormatColor(Color value, std::string& str);


// alias
// 下面是内联函数的实现


/// @brief 将布尔值格式化为字符串
/// @param value 输入布尔值
/// @return 格式化后的字符串
A_ALWAYS_INLINE
std::string aFormatBool(bool value)
{
    std::string str;
    aFormatBool(value, str);
    return str;
}


/// @brief 将整数格式化为字符串
/// @param value 输入整数
/// @return 格式化后的字符串
A_ALWAYS_INLINE
std::string aFormatInt(int value)
{
    std::string str;
    err_t err = aFormatInt(value, str);
    if (A_UNLIKELY(err != eNoError))
    {
        aWarning("failed to format int value, err = %d", err);
    }
    return str;
}


/// @brief 将双精度浮点数格式化为字符串
/// @param value 输入双精度浮点数
/// @param precision 格式化精度，默认值为15
/// @return 格式化后的字符串
A_ALWAYS_INLINE
std::string aFormatDouble(double value, int precision = kDefaultFormatDoubleEps)
{
    std::string str;
    err_t err = aFormatDouble(value, str, precision);
    if (A_UNLIKELY(err != eNoError))
    {
        aWarning("failed to format double value, err = %d", err);
    }
    return str;
}

/// @brief 将颜色值格式化为字符串
/// @param value 输入颜色值
/// @return 格式化后的字符串
A_ALWAYS_INLINE
std::string aFormatColor(Color value)
{
    std::string str;
    err_t err = aFormatColor(value, str);
    if (A_UNLIKELY(err != eNoError))
    {
        aWarning("failed to format color value, err = %d", err);
    }
    return str;
}


/// @brief 将字符串转换为布尔值
/// @param str 输入字符串
/// @return 转换后的布尔值
A_ALWAYS_INLINE
bool aParseBool(StringView str)
{
    bool value;
    err_t err = aParseBool(str, value);
    if (A_UNLIKELY(err != eNoError))
    {
        aWarning("failed to parse bool value, err = %d", err);
    }
    return value;
}

/// @brief 将字符串转换为整数
/// @param str 输入字符串
/// @return 转换后的整数
A_ALWAYS_INLINE
int aParseInt(StringView str)
{
    int value;
    err_t err = aParseInt(str, value);
    if (A_UNLIKELY(err != eNoError))
    {
        aWarning("failed to parse int value, err = %d", err);
    }
    return value;
}


/// @brief 将字符串转换为双精度浮点数
/// @param str 输入字符串
/// @return 转换后的双精度浮点数
A_ALWAYS_INLINE
double aParseDouble(StringView str)
{
    double value;
    err_t err = aParseDouble(str, value);
    if (A_UNLIKELY(err != eNoError))
    {
        aWarning("failed to parse double value, err = %d", err);
    }
    return value;
}


/// @brief 将字符串转换为颜色值
/// @param str 输入字符串
/// @return 转换后的颜色值
A_ALWAYS_INLINE
Color aParseColor(StringView str)
{
    Color value;
    err_t err = aParseColor(str, value);
    if (A_UNLIKELY(err != eNoError))
    {
        aWarning("failed to parse color value, err = %d", err);
    }
    return value;
}



AST_NAMESPACE_END
