///
/// @file      testScriptQuantity.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-24
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

#include "AstScript/ScriptAPI.hpp"
#include "AstTest/AstTestMacro.h"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/IO.hpp"

AST_USING_NAMESPACE
using namespace _AST units;

void testParseQuantity(StringView quantityStr, const Quantity& expectedQuantity)
{
    ast_printf("testParseQuantity: %s\n", quantityStr.data());
    Value* val = aEval(quantityStr.to_string());
    ASSERT_TRUE(val != nullptr);
    ASSERT_TRUE(aValueIsQuantity(val));
    Quantity quantity = aValueUnboxQuantity(val);
    ASSERT_EQ(quantity, expectedQuantity);
}


TEST(ScriptQuantity, Parser)
{
    // 基本单位测试
    testParseQuantity("123.456[m]", 123.456 * m);
    testParseQuantity("123.456 [kg]", 123.456 * kg);
    testParseQuantity("90[deg]", 90 * deg);
    testParseQuantity("3.14[rad]", 3.14 * rad);
    testParseQuantity("60[s]", 60 * s);
    testParseQuantity("1[min]", 1 * min);
    
    // 整数格式
    testParseQuantity("100[m]", 100 * m);
    testParseQuantity("50 [kg]", 50 * kg);
    
    // 不同进制的数字（如果支持的话）
    // testParseQuantity("0x64[m]", 100 * m); // 十六进制
    // testParseQuantity("0b1100100[m]", 100 * m); // 二进制
    
    // 科学计数法
    testParseQuantity("1.23e3[m]", 1230 * m);
    testParseQuantity("1.23E-2[kg]", 0.0123 * kg);
    
    // 各种长度单位
    testParseQuantity("1[mm]", 1 * mm);
    testParseQuantity("10[cm]", 10 * cm);
    testParseQuantity("5[dm]", 5 * dm);
    testParseQuantity("1.5[km]", 1.5 * km);
    testParseQuantity("12[in]", 12 * in);
    testParseQuantity("3[ft]", 3 * ft);
    testParseQuantity("1[yd]", 1 * yd);
    testParseQuantity("0.5[mi]", 0.5 * mi);
    
    // 各种时间单位
    testParseQuantity("1000[ms]", 1000 * ms);
    testParseQuantity("3600[s]", 3600 * s);
    testParseQuantity("24[h]", 24 * h);
    testParseQuantity("7[day]", 7 * day);
    
    // 各种质量单位
    testParseQuantity("1000[g]", 1000 * g);
    testParseQuantity("1000[mg]", 1000 * mg);
    testParseQuantity("2.2[lb]", 2.2 * lb);
    
    // 复合单位
    testParseQuantity("100[km/h]", 100 * (km/h));
    testParseQuantity("50[m/s]", 50 * (m/s));
    testParseQuantity("9.8[m/s²]", 9.8 * (m/(s*s)));
    testParseQuantity("1000[kg/m³]", 1000 * (kg/(m*m*m)));
    
    // 面积和体积单位
    testParseQuantity("100[m2]", 100 * m2);
    testParseQuantity("500[m3]", 500 * m3);
    testParseQuantity("2[L]", 2 * L);
    
    // 带空格和不带空格的格式
    testParseQuantity("1.2[m]", 1.2 * m);
    testParseQuantity("1.2 [m]", 1.2 * m);
    testParseQuantity("1 [km/h]", 1 * (km/h));
    
    // 特殊单位
    testParseQuantity("1[N]", 1 * N);
    testParseQuantity("5[A]", 5 * A);
}



GTEST_MAIN()