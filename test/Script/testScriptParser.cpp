///
/// @file      testScriptParser.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-20
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
#include "AstScript/Parser.hpp"
#include "AstScript/Lexer.hpp"
#include "AstScript/Scanner.hpp"
#include "AstTest/AstTestMacro.h"
#include "AstUtil/StringView.hpp"

AST_USING_NAMESPACE

/// @brief 创建临时测试文件
/// @param content 文件内容
/// @param filename 文件名
/// @return 是否创建成功
bool createTestFile(StringView content, StringView filename) {
    std::ofstream file(filename.data());
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();
    return true;
}

/// @brief 删除临时测试文件
/// @param filename 文件名
void deleteTestFile(StringView filename) {
    std::remove(filename.data());
}


Expr* parseByFileScanner(StringView str)
{
    const std::string filename = "test_parser_file.txt";
    EXPECT_TRUE(createTestFile(str, filename));
    FILE* file = std::fopen(filename.c_str(), "r");
    FileScanner scanner(file);
    Lexer lexer(&scanner);
    Parser parser(lexer);
    
    auto expr = parser.parseBlockExpr();
    
    std::fclose(file);
    deleteTestFile(filename);
    return expr;
}


Expr* parseByStreamScanner(StringView str)
{
    const std::string filename = "test_parser_file.txt";
    EXPECT_TRUE(createTestFile(str, filename));
    std::fstream file(filename);
    StreamScanner scanner(file);
    Lexer lexer(&scanner);
    Parser parser(lexer);
    
    auto expr = parser.parseBlockExpr();
    
    file.close();
    deleteTestFile(filename);
    return expr;
}

void testScriptParser(StringView str)
{
    std::cout << "\n=== Testing expression: " << str << " ===" << std::endl;
    
    // 第一次解析：使用aParseExpr解析原始输入
    Expr* expr = aParseExpr(str);
    std::cout << "1. Initial parse using aParseExpr: " << (expr ? "SUCCESS" : "FAILED") << std::endl;
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "   Formatted expression: " << exprStr << std::endl;
        delete expr;
        
        // 第二次解析：使用aParseExpr重新解析格式化后的表达式
        Expr* expr2 = aParseExpr(exprStr);
        std::cout << "2. Re-parse using aParseExpr: " << (expr2 ? "SUCCESS" : "FAILED") << std::endl;
        EXPECT_NE(expr2, nullptr);
        
        if (expr2) {
            std::string exprStr2 = aFormatExpr(expr2);
            std::cout << "   Formatted expression: " << exprStr2 << std::endl;
            
            bool match = (exprStr == exprStr2);
            std::cout << "   Format match: " << (match ? "YES" : "NO") << std::endl;
            EXPECT_EQ(exprStr, exprStr2);
            
            delete expr2;
            
            if (!match) {
                std::cout << "   Format mismatch detected!" << std::endl;
                return; // 提前返回，避免后续测试
            }
        }else {
            std::cout << "   Failed to re-parse expression." << std::endl;
            return; // 提前返回，避免后续测试
        }
        
        // 第三次解析：使用FileScanner解析格式化后的表达式
        Expr* expr3 = parseByFileScanner(exprStr);
        std::cout << "3. Parse using FileScanner: " << (expr3 ? "SUCCESS" : "FAILED") << std::endl;
        EXPECT_NE(expr3, nullptr);
        
        if (expr3) {
            std::string exprStr3 = aFormatExpr(expr3);
            std::cout << "   Formatted expression: " << exprStr3 << std::endl;
            
            bool match = (exprStr == exprStr3);
            std::cout << "   Format match: " << (match ? "YES" : "NO") << std::endl;
            EXPECT_EQ(exprStr, exprStr3);
            
            delete expr3;
            
            if (!match) {
                std::cout << "   Format mismatch detected!" << std::endl;
                return; // 提前返回，避免后续测试
            }
        }else {
            std::cout << "   Failed to re-parse expression." << std::endl;
            return; // 提前返回，避免后续测试
        }
        
        // 第四次解析：使用StreamScanner解析格式化后的表达式
        Expr* expr4 = parseByStreamScanner(exprStr);
        std::cout << "4. Parse using StreamScanner: " << (expr4 ? "SUCCESS" : "FAILED") << std::endl;
        EXPECT_NE(expr4, nullptr);
        
        if (expr4) {
            std::string exprStr4 = aFormatExpr(expr4);
            std::cout << "   Formatted expression: " << exprStr4 << std::endl;
            
            bool match = (exprStr == exprStr4);
            std::cout << "   Format match: " << (match ? "YES" : "NO") << std::endl;
            EXPECT_EQ(exprStr, exprStr4);
            
            delete expr4;
            
            if (!match) {
                std::cout << "   Format mismatch detected!" << std::endl;
            }
        }else {
            std::cout << "   Failed to re-parse expression." << std::endl;
        }
        
        std::cout << "=== Test completed successfully! ===\n" << std::endl;
    } else {
        std::cout << "Failed to parse expression." << std::endl;
        
        std::cout << "=== Test failed at initial parsing! ===\n" << std::endl;
    }
}

// 测试无效语法，应该解析失败
void testInvalidScriptParser(StringView str)
{
    Expr* expr = aParseExpr(str);
    EXPECT_EQ(expr, nullptr);
    std::cout << "Invalid input expression: " << str << std::endl;
    if (expr) {
        std::cout << "ERROR: Should have failed to parse!" << std::endl;
        delete expr;
    } else {
        std::cout << "OK: Correctly failed to parse invalid expression" << std::endl;
    }
}

TEST(ScriptParser, ArithmeticExpression)
{
    // 基本算术运算
    testScriptParser("1 + 2 * 3");
    testScriptParser("(4 - 5) / 6");
    testScriptParser("7%8");
    testScriptParser("-(9 + 10)");
    
    // 浮点数支持
    testScriptParser("123.456");
    testScriptParser("0.123 + 4.56");
    testScriptParser("100 - 0.5");
    testScriptParser("3.14 * 2.0");
    testScriptParser("10 / 3.0");
    
    // 幂指数运算:
    testScriptParser("2^3");
    testScriptParser("2^3^6");
    testScriptParser("2^3 + 4");
    testScriptParser("2 * 3^4");
    testScriptParser("(2 + 3)^4");
    
    // 复杂算术表达式
    testScriptParser("1 + 2 * 3 - 4 / 5");
    testScriptParser("(1 + 2) * (3 - 4) / 5");
    testScriptParser("1 + (2 * 3 - 4) / 5");
}

TEST(ScriptParser, ComparisonExpression)
{
    // 比较运算符
    testScriptParser("1 == 2");
    testScriptParser("3 != 4");
    testScriptParser("5 < 6");
    testScriptParser("7 > 8");
    testScriptParser("9 <= 10");
    testScriptParser("11 >= 12");
    
    // 带比较的复杂表达式
    testScriptParser("1 + 2 == 3");
    testScriptParser("4 * 5 > 6 + 7");
    testScriptParser("(8 - 9) != 0");
}

TEST(ScriptParser, LogicalExpression)
{
    // 逻辑运算符
    testScriptParser("true && false");
    testScriptParser("true || false");
    testScriptParser("!true");
    testScriptParser("!false");
    
    // 复合逻辑表达式
    testScriptParser("true && (false || true)");
    testScriptParser("!true || false");
    testScriptParser("(1 == 2) && (3 != 4)");
}

TEST(ScriptParser, BitwiseExpression)
{
    // 按位运算符
    testScriptParser("1 & 2");
    testScriptParser("3 | 4");
    testScriptParser("~7");
    testScriptParser("8 << 2");
    testScriptParser("16 >> 1");
    testScriptParser("16 >>> 1");
    
    // 复合按位表达式
    testScriptParser("(1 & 2) | (3 & 4)");
    testScriptParser("8 << 2 >> 1");
    testScriptParser("8 << 2 >>> 1");
}

TEST(ScriptParser, LiteralExpression)
{
    // 数值字面量
    testScriptParser("42");
    testScriptParser("0");
    testScriptParser("-123");
    testScriptParser("3.14159");
    
    // 数值字面量边界情况
    testScriptParser("0.0");
    testScriptParser("-0.0");
    testScriptParser("1.0");
    testScriptParser("-1.0");
    testScriptParser("1e10");
    testScriptParser("1e-10");
    testScriptParser("1.5e3");
    testScriptParser("-1.5e3");
    testScriptParser("1.5e-3");
    
    // 十六进制整数
    testScriptParser("0x0");
    testScriptParser("0x10");
    testScriptParser("0xFF");
    testScriptParser("0xEF");
    testScriptParser("0xEe");
    testScriptParser("0xabc");
    testScriptParser("0xABC");
    testScriptParser("0x1A2B3C");
    // testScriptParser("0x123456789ABCDEF"); // @fixme 超出int范围，需要处理这种情况
    
    // 二进制整数
    testScriptParser("0b0");
    testScriptParser("0b1");
    testScriptParser("0b1010");
    testScriptParser("0b1111");
    testScriptParser("0b10000000");
    testScriptParser("0b11111111");
    testScriptParser("0b100000000000");
    
    // 八进制整数，参照Julia语法
    testScriptParser("0o0");
    testScriptParser("0o1");
    testScriptParser("0o7");
    testScriptParser("0o10");
    testScriptParser("0o100");
    testScriptParser("0o777");
    testScriptParser("0o123");
    testScriptParser("0o345");
    // testScriptParser("0Oabc"); // 这应该会失败，因为八进制只能包含0-7
    
    // 字符串字面量
    testScriptParser("\"hello\"");
    testScriptParser("\"world\"");
    testScriptParser("\"\"" ); // 空字符串
    testScriptParser("\"a\""); // 单字符字符串
    
    // 布尔字面量
    testScriptParser("true");
    testScriptParser("false");
    
    // null字面量
    testScriptParser("null");
}

TEST(ScriptParser, SimpleAssignment)
{
    // 最简单的赋值表达式测试
    testScriptParser("x = 10");
}

TEST(ScriptParser, AssignmentExpression)
{
    // 赋值表达式
    testScriptParser("x = 10");
    testScriptParser("y = x + 5");
    testScriptParser("z = (a + b) * c");
    testScriptParser("a = b = c = 10");
    testScriptParser("x = y + z * a");
    testScriptParser("x = (y + z) * a");
    testScriptParser("x = y ? z : a");
    testScriptParser("x = y && z");
    testScriptParser("x = y || z");
    testScriptParser("x = y & z");
    testScriptParser("x = y | z");
    testScriptParser("x = y << z");
    testScriptParser("x = y >> z");
    testScriptParser("x = y ^ z");
    testScriptParser("x = y % z");
    testScriptParser("x = -y");
    testScriptParser("x = ~y");
    testScriptParser("x = !y");
}

TEST(ScriptParser, ConditionalExpression)
{
    // 条件表达式
    testScriptParser("true ? 1 : 2");
    testScriptParser("false ? 3 : 4");
    testScriptParser("(x == 10) ? \"yes\" : \"no\"");
    testScriptParser("a > b ? a : b");
}

TEST(ScriptParser, ComplexExpression)
{
    // 复杂表达式组合
    testScriptParser("x = 10 + y * 2");
    testScriptParser("(a > b ? a : b) + (c < d ? c : d)");
    testScriptParser("true && (x == 10) ? \"valid\" : \"invalid\"");
    testScriptParser("(1 + 2 * 3 - 4) / 5 && 6 < 7");
    testScriptParser("!(x == 10) || (y != 20) && true");
    
    // 运算符优先级边界情况
    testScriptParser("-a + b");
    testScriptParser("-a * b");
    testScriptParser("a + b * c + d");
    testScriptParser("a * b + c * d");
    testScriptParser("a && b || c && d");
    testScriptParser("a || b && c || d");
    testScriptParser("a == b && c != d");
    testScriptParser("a < b || c > d");
    testScriptParser("a & b | c & d");
    testScriptParser("a | b & c | d");
    testScriptParser("a << b >> c");
    testScriptParser("a + b << c - d");
    
    // 复杂嵌套表达式
    testScriptParser("(a + (b * (c - d))) / (e + f)");
    testScriptParser("((a > b) ? (c + d) : (e - f)) * g");
    testScriptParser("!(a == b || c != d) && e < f");
    testScriptParser("a ^ (b * c) + d");
    testScriptParser("(a & b) << (c | d)");
}

TEST(ScriptParser, InvalidSyntax)
{
    // 不完整的表达式
    testInvalidScriptParser("1 +");
    testInvalidScriptParser("(1 + 2");
    testInvalidScriptParser("1 ? 2");
    testInvalidScriptParser("1 ? : 2");
    testInvalidScriptParser("? 1 : 2");
    testInvalidScriptParser("1 + (2 * 3");
    testInvalidScriptParser("1 + 2 *");
    testInvalidScriptParser("x = ");
    
    // 无效的运算符使用
    testInvalidScriptParser("1 + * 2");
    testInvalidScriptParser("1 == = 2");
    testInvalidScriptParser("1 + = 2");
    testInvalidScriptParser("1 ^ ^ 2");
    testInvalidScriptParser("1 & | 2");
    testInvalidScriptParser("1 && || 2");
    testInvalidScriptParser("1 << >> 2");
    testInvalidScriptParser("1 +++ 2");
    // testInvalidScriptParser("1 -- 2"); // Julia中这是有效的（递减运算符）
    
    // 无效的字面量
    testInvalidScriptParser("0xG"); // 十六进制包含无效字符
    testInvalidScriptParser("0x"); // 空的十六进制
    testInvalidScriptParser("0b2"); // 二进制包含无效字符
    testInvalidScriptParser("0b"); // 空的二进制
    testInvalidScriptParser("0o8"); // 八进制包含无效字符
    testInvalidScriptParser("0o"); // 空的八进制
    testInvalidScriptParser("123."); // 不完整的浮点数
    testInvalidScriptParser("1..2"); // 无效的浮点数
    testInvalidScriptParser("0x123456789ABCDEF0G"); // 超长十六进制包含无效字符
    testInvalidScriptParser("0xH");
    testInvalidScriptParser("0bA");
    testInvalidScriptParser("0o9");
    // testInvalidScriptParser("1e"); // Julia中这是有效的（科学计数法的简写形式）
    // testInvalidScriptParser("1e+"); // Julia中这是有效的
    // testInvalidScriptParser("1e-"); // Julia中这是有效的
    testInvalidScriptParser("1.2.3"); // 多个小数点
    
    // 无效的标识符
    testInvalidScriptParser("1var = 10"); // 以数字开头的变量名
    testInvalidScriptParser("var name = 10"); // 包含空格的变量名
    testInvalidScriptParser("var@name = 10"); // 包含特殊字符的变量名
    // testInvalidScriptParser("var-name = 10"); // Julia中允许连字符作为变量名
    // testInvalidScriptParser("var/name = 10"); // Julia中这被解析为除法表达式
    // testInvalidScriptParser("var%name = 10"); // Julia中这被解析为取模表达式
    // testInvalidScriptParser("var^name = 10"); // Julia中这被解析为乘方表达式
    // testInvalidScriptParser("var&name = 10"); // Julia中这被解析为按位与表达式
    // testInvalidScriptParser("var|name = 10"); // Julia中这被解析为按位或表达式
    testInvalidScriptParser("var~name = 10"); // 包含取反运算符的变量名
    // testInvalidScriptParser("var<name = 10"); // Julia中这被解析为小于表达式
    // testInvalidScriptParser("var>name = 10"); // Julia中这被解析为大于表达式
    // testInvalidScriptParser("var=name = 10"); // Julia中这被解析为赋值表达式
    testInvalidScriptParser("var!name = 10"); // 包含感叹号的变量名
    testInvalidScriptParser("var?name = 10"); // 包含问号的变量名
    // testInvalidScriptParser("var:name = 10"); // 包含冒号的变量名，现在是合法的范围表达式
    testInvalidScriptParser("var,name = 10"); // 包含逗号的变量名
    // testInvalidScriptParser("var;name = 10"); // 包含分号的变量名
    // testInvalidScriptParser("var*name = 10"); // Julia中这被解析为乘法表达式
    // testInvalidScriptParser("var+name = 10"); // Julia中这被解析为加法表达式
    // testInvalidScriptParser("var.name = 10"); // Julia中这被解析为点操作表达式
    
    // 无效的字符串
    testInvalidScriptParser("\"unclosed string");
    testInvalidScriptParser("'unclosed string"); // Julia使用双引号
    testInvalidScriptParser("\"string\" with \"quotes\""); // 未转义的引号
    
    // 括号不匹配
    testInvalidScriptParser("(1 + 2))"); // 多余的右括号
    testInvalidScriptParser("((1 + 2)"); // 多余的左括号
    testInvalidScriptParser("(1 + (2 * 3)))"); // 括号不匹配
    testInvalidScriptParser("(((1 + 2) * 3"); // 多层括号不匹配
    testInvalidScriptParser("(1 + 2) * (3 + 4))"); // 外层括号不匹配
    
    // 其他无效语法
    testInvalidScriptParser("true false"); // 两个布尔值之间没有运算符
    testInvalidScriptParser("null null"); // 两个null之间没有运算符
    testInvalidScriptParser("."); // 孤立的点
    testInvalidScriptParser(":"); // 孤立的冒号
    testInvalidScriptParser("?"); // 孤立的问号
    testInvalidScriptParser("()"); // 空括号
    testInvalidScriptParser(","); // 孤立的逗号
    testInvalidScriptParser("1,,2"); // 多余的逗号
    // testInvalidScriptParser(";"); // 孤立的分号
    // testInvalidScriptParser("1;2"); // 分号分隔的表达式
    testInvalidScriptParser("1 2"); // 两个数值之间没有运算符
    testInvalidScriptParser("x y"); // 两个变量之间没有运算符
    // testInvalidScriptParser("""多引号字符串"""); // 不支持的多行字符串语法 (注释掉，因为C++不支持三引号)
    
    // 测试数值超出int范围的情况
    testInvalidScriptParser("2147483648"); // int最大值+1
    testInvalidScriptParser("-2147483649"); // int最小值-1
    testInvalidScriptParser("0x80000000"); // 十六进制超出int范围
    testInvalidScriptParser("0b10000000000000000000000000000000"); // 二进制超出int范围
    testInvalidScriptParser("0o20000000000"); // 八进制超出int范围
}

// 测试条件表达式的异常分支
TEST(ScriptParser, ConditionalExpressionError) {
    // 缺少冒号和else分支
    testInvalidScriptParser("true ? 1");
    testInvalidScriptParser("x > y ? ");
    testInvalidScriptParser("(1 + 2) ? ");
    
    // then分支解析失败
    testInvalidScriptParser("true ? +");
    testInvalidScriptParser("false ? (1 + ");
    
    // else分支解析失败
    testInvalidScriptParser("true ? 1 : +");
    testInvalidScriptParser("x == y ? 2 : (3 + ");
}

// 测试二元表达式的异常分支
TEST(ScriptParser, BinaryExpressionError) {
    // 加法右侧解析失败
    testInvalidScriptParser("1 +");
    testInvalidScriptParser("2 + +");
    
    // 乘法右侧解析失败
    testInvalidScriptParser("3 *");
    testInvalidScriptParser("4 * (");
    
    // 除法右侧解析失败
    testInvalidScriptParser("5 /");
    testInvalidScriptParser("6 / (");
    
    // 幂运算右侧解析失败
    testInvalidScriptParser("7 ^");
    testInvalidScriptParser("8 ^ +");
    
    // 比较运算右侧解析失败
    testInvalidScriptParser("9 <");
    testInvalidScriptParser("10 > (");
    
    // 逻辑运算右侧解析失败
    testInvalidScriptParser("true &&");
    testInvalidScriptParser("false || (");
    
    // 位运算右侧解析失败
    testInvalidScriptParser("11 &");
    testInvalidScriptParser("12 | +");
}

// 测试一元表达式的异常分支
TEST(ScriptParser, UnaryExpressionError) {
    // 负号后面没有表达式
    testInvalidScriptParser("-");
    testInvalidScriptParser("--");
    
    // 逻辑非后面没有表达式
    testInvalidScriptParser("!");
    testInvalidScriptParser("!!");
    
    // 按位非后面没有表达式
    testInvalidScriptParser("~");
    testInvalidScriptParser("~~");
    
    // 一元运算符后面解析失败
    testInvalidScriptParser("-(+");
    testInvalidScriptParser("!(1 +");
    testInvalidScriptParser("~(");
}

// 测试赋值表达式的异常分支
TEST(ScriptParser, AssignmentExpressionError) {
    // 赋值右侧解析失败
    testInvalidScriptParser("x =");
    testInvalidScriptParser("y = +");
    testInvalidScriptParser("z = (");
    
    // 复合赋值解析失败
    testInvalidScriptParser("x = 1 +");
    testInvalidScriptParser("y = (1 + ");
    testInvalidScriptParser("z = x +");
}

// 测试括号表达式的异常分支
TEST(ScriptParser, ParenthesisExpressionError) {
    // 缺少右括号
    testInvalidScriptParser("(");
    testInvalidScriptParser("(1");
    testInvalidScriptParser("(1 + 2");
    testInvalidScriptParser("((1 + 2)");
    
    // 括号内解析失败
    testInvalidScriptParser("(+");
    testInvalidScriptParser("(1 +");
    testInvalidScriptParser("((1 +");
}

// 测试代码块表达式的正常分支
TEST(ScriptParser, BlockExpression) {
    // 测试简单的代码块
    testScriptParser("begin 1 end");
    
    // 测试多个表达式的代码块
    testScriptParser("begin 1; 2 end");
    
    // 测试带分号和不带分号的混合
    testScriptParser("begin 1; 2; 3 end");
    
    // 测试带表达式的代码块
    testScriptParser("begin x = 1; y = 2; x + y end");

    // 测试换行符分隔的代码块
    testScriptParser("begin 1\n2 end");

    // 测试没有 begin 和 end 的代码块
    testScriptParser("x = 1; x + 2");

    // 测试没有 begin 和 end 的代码块
    testScriptParser("y = 1\nx = 2");
    
    // 测试嵌套代码块
    testScriptParser("begin begin 1 end end");

    // 空代码块
    testScriptParser("begin end");
}

// 测试代码块表达式的异常分支
TEST(ScriptParser, BlockExpressionError) {
    // 缺少end
    testInvalidScriptParser("begin 1");
    testInvalidScriptParser("begin x = 1");
    
    // 内部表达式解析失败
    testInvalidScriptParser("begin 1 + end");
    testInvalidScriptParser("begin x = + end");
    
}

// 测试宏调用表达式
TEST(ScriptParser, MacroCallExpression) {
    // 带括号的宏调用
    testScriptParser("@macro(a, b, c)");
    testScriptParser("@macro(1, 2, 3)");
    testScriptParser("@macro(a + b, c * d)");
    
    // 不带括号的宏调用
    testScriptParser("@macro a b c");
    testScriptParser("@macro 1 2 3");
    testScriptParser("@macro a a+b 1-2");
    
    // 空参数的宏调用
    testScriptParser("@macro()");
}

// 测试Range表达式
TEST(ScriptParser, RangeExpression) {
    // 基本整数范围
    testScriptParser("1:10");
    
    // 带步长的整数范围
    testScriptParser("1:2:10");
    
    // 浮点数范围
    testScriptParser("1.0:10.0");
    
    // 带步长的浮点数范围
    testScriptParser("1.0:0.5:3.0");
    
    // 负数范围
    testScriptParser("10:-1:1");
    
    // 负数步长的浮点数范围
    testScriptParser("5.0:-0.5:3.0");
    
    // 嵌套在其他表达式中的范围
    testScriptParser("x = 1:10");
    testScriptParser("x:y:z");
}

GTEST_MAIN()