///
/// @file      testParseFormat.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-11
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


#include "AstUtil/ParseFormat.hpp"
#include "AstTest/AstTestMacro.h"
#include <string>

AST_USING_NAMESPACE

// 测试 aParseBool 函数
TEST(ParseFormat, aParseBool) {
    bool value;
    err_t err;
    
    // 测试 true 值
    err = aParseBool("true", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(value);
    
    err = aParseBool("TRUE", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(value);
    
    err = aParseBool("1", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(value);
    
    // 测试 false 值
    err = aParseBool("false", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_FALSE(value);
    
    err = aParseBool("FALSE", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_FALSE(value);
    
    err = aParseBool("0", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_FALSE(value);
    
    // 测试无效输入
    err = aParseBool("invalid", value);
    EXPECT_EQ(err, eErrorParse);
    
    // 测试空输入
    err = aParseBool("", value);
    EXPECT_EQ(err, eErrorNullInput);
}

// 测试 _aParseInt_LibC_2 函数
TEST(ParseFormat, _aParseInt_LibC_2) {
    int value;
    err_t err;
    
    err = _aParseInt_LibC_2("123", value);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(value, 123);
    
    err = _aParseInt_LibC_2("-456", value);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(value, -456);
    
    // atoi 对于非数字输入返回 0
    err = _aParseInt_LibC_2("abc", value);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(value, 0);
}

#if A_CXX_VERSION >= 17
// 测试 _aParseInt_FromChars 函数 (C++17)
TEST(ParseFormat, _aParseInt_FromChars) {
    int value;
    err_t err;
    
    err = _aParseInt_FromChars("123", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(value, 123);
    
    err = _aParseInt_FromChars("-456", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(value, -456);
    
    // 测试无效输入
    err = _aParseInt_FromChars("abc", value);
    EXPECT_EQ(err, eErrorParse);
}
#endif

// 测试 _aParseDouble_LibC_2 函数
TEST(ParseFormat, _aParseDouble_LibC_2) {
    double value;
    err_t err;
    
    err = _aParseDouble_LibC_2("123.456", value);
    EXPECT_EQ(err, 0);
    EXPECT_DOUBLE_EQ(value, 123.456);
    
    err = _aParseDouble_LibC_2("-789.012", value);
    EXPECT_EQ(err, 0);
    EXPECT_DOUBLE_EQ(value, -789.012);
    
    // atof 对于非数字输入返回 0.0
    err = _aParseDouble_LibC_2("abc", value);
    EXPECT_EQ(err, 0);
    EXPECT_DOUBLE_EQ(value, 0.0);
}

#if A_CXX_VERSION >= 17
// 测试 _aParseDouble_FromChars 函数 (C++17)
TEST(ParseFormat, _aParseDouble_FromChars) {
    double value;
    err_t err;
    
    err = _aParseDouble_FromChars("123.456", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_DOUBLE_EQ(value, 123.456);
    
    err = _aParseDouble_FromChars("-789.012", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_DOUBLE_EQ(value, -789.012);
    
    // 测试无效输入
    err = _aParseDouble_FromChars("abc", value);
    EXPECT_EQ(err, eErrorParse);
}
#endif

#ifdef AST_WITH_ABSEIL
// 测试 _aParseDouble_FromChars_Abseil 函数 (需要 abseil 支持)
TEST(ParseFormat, _aParseDouble_FromChars_Abseil) {
    double value;
    err_t err;
    
    err = _aParseDouble_FromChars_Abseil("123.456", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_DOUBLE_EQ(value, 123.456);
    
    err = _aParseDouble_FromChars_Abseil("-789.012", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_DOUBLE_EQ(value, -789.012);
    
    // 测试无效输入
    err = _aParseDouble_FromChars_Abseil("abc", value);
    EXPECT_EQ(err, eErrorParse);
}
#endif

// 测试 aParseColor 函数
TEST(ParseFormat, aParseColor) {
    Color value;
    err_t err;
    
    // 测试 RGB 格式
    err = aParseColor("#FF0000", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(value, 0xFF0000FF); // 注意：颜色格式为 #RRGGBBAA
    
    // 测试 RGBA 格式
    err = aParseColor("#FF000080", value);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(value, 0xFF000080);
    
    // 测试无效格式
    err = aParseColor("FF0000", value); // 缺少 #
    EXPECT_EQ(err, eErrorParse);
    
    err = aParseColor("#FF00", value); // 长度不足
    EXPECT_EQ(err, eErrorParse);
    
    err = aParseColor("#GG0000", value); // 无效字符
    EXPECT_EQ(err, eErrorParse);
}

// 测试 aFormatBool 函数
TEST(ParseFormat, aFormatBool) {
    std::string str;
    
    aFormatBool(true, str);
    EXPECT_EQ(str, "true");
    
    aFormatBool(false, str);
    EXPECT_EQ(str, "false");
}

// 测试 aFormatInt 函数
TEST(ParseFormat, aFormatInt) {
    std::string str;
    err_t err;
    
    err = aFormatInt(123, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "123");
    
    err = aFormatInt(-456, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "-456");
    
    err = aFormatInt(0, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "0");
}

// 测试 aFormatDouble 函数
TEST(ParseFormat, aFormatDouble) {
    std::string str;
    err_t err;
    
    err = aFormatDouble(123.456, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "123.456");
    
    err = aFormatDouble(-789.012, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "-789.012");
    
    err = aFormatDouble(0.0, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "0");
    
    // 测试不同精度
    err = aFormatDouble(123.456789, str, 3);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "123.457");
    
    err = aFormatDouble(123.456789, str, 6);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "123.456789");
}

// 测试 aFormatColor 函数
TEST(ParseFormat, aFormatColor) {
    std::string str;
    err_t err;
    
    // 测试 RGB 格式颜色（会被格式化为 RGBA）
    err = aFormatColor(0xFF000000, str); // 红色，透明度 0
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "#FF000000");
    
    // 测试 RGBA 格式颜色
    err = aFormatColor(0xFF000080, str); // 红色，半透明
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "#FF000080");
    
    // 测试白色
    err = aFormatColor(0xFFFFFFFF, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "#FFFFFFFF");
}

// 测试解析和格式化的一致性
TEST(ParseFormat, Consistency) {
    // 测试布尔值
    bool bool_value;
    std::string bool_str;
    aFormatBool(true, bool_str);
    aParseBool(bool_str, bool_value);
    EXPECT_TRUE(bool_value);
    
    aFormatBool(false, bool_str);
    aParseBool(bool_str, bool_value);
    EXPECT_FALSE(bool_value);
    
    // 测试整数
    int int_value;
    std::string int_str;
    aFormatInt(12345, int_str);
    aParseInt(int_str, int_value);
    EXPECT_EQ(int_value, 12345);
    
    aFormatInt(-67890, int_str);
    aParseInt(int_str, int_value);
    EXPECT_EQ(int_value, -67890);
    
    // 测试双精度浮点数
    double double_value;
    std::string double_str;
    aFormatDouble(123.456, double_str);
    aParseDouble(double_str, double_value);
    EXPECT_DOUBLE_EQ(double_value, 123.456);
    
    // 测试颜色
    Color color_value;
    std::string color_str;
    aFormatColor(0xFF000080, color_str);
    aParseColor(color_str, color_value);
    EXPECT_EQ(color_value, 0xFF000080);
}

GTEST_MAIN()