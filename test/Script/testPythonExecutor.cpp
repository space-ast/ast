///
/// @file      testPythonExecutor.cpp
/// @brief     PythonExecutor 测试
/// @details   测试脚本执行、表达式求值、变量传递、错误处理
/// @author    axel
/// @date      2026-05-16
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/PythonScriptExecutor.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE


TEST(PythonExecutor, InitializeFinalize)
{
    PythonExecutor exec;
    errc_t rc = exec.initialize();
    if (rc != eNoError) {
        GTEST_SKIP() << "Python not available";
        return;
    }

    // 可以重复初始化（CPython 的 Py_Initialize 是幂等的）
    rc = exec.initialize();
    EXPECT_EQ(rc, eNoError);

    exec.finalize();
}


TEST(PythonExecutor, Execute)
{
    PythonExecutor exec;
    if (exec.initialize() != eNoError) {
        GTEST_SKIP() << "Python not available";
        return;
    }

    // 执行简单语句
    errc_t rc = exec.execute("x = 1 + 2");
    EXPECT_EQ(rc, eNoError);

    // 执行多行语句
    rc = exec.execute("y = x * 10\n"
                       "z = y + 5");
    EXPECT_EQ(rc, eNoError);

    // import 模块
    rc = exec.execute("import math");
    EXPECT_EQ(rc, eNoError);

    // 验证前面设置的变量依然存在
    int val = 0;
    rc = exec.getVariable("x", val);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(val, 3);

    rc = exec.getVariable("y", val);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(val, 30);

    exec.finalize();
}


TEST(PythonExecutor, Evaluate)
{
    PythonExecutor exec;
    if (exec.initialize() != eNoError) {
        GTEST_SKIP() << "Python not available";
        return;
    }

    // 求值整数表达式
    ScriptResult result;
    errc_t rc = exec.evaluate("2 + 3", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(result.value_ != nullptr);
    EXPECT_EQ(result.value_->toInt(), 5);

    // 求值浮点表达式
    rc = exec.evaluate("3.14 * 2", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_DOUBLE_EQ(result.value_->toDouble(), 6.28);

    // 求值字符串表达式
    rc = exec.evaluate("'hello' + ' world'", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(result.value_->toString(), "hello world");

    // 求值布尔表达式
    rc = exec.evaluate("1 < 2", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(result.value_->toBool(), true);

    rc = exec.evaluate("1 > 2", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(result.value_->toBool(), false);

    // 求值函数调用
    exec.execute("import math");
    rc = exec.evaluate("math.sqrt(16)", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_DOUBLE_EQ(result.value_->toDouble(), 4.0);

    // 求值失败 —— 语法错误
    rc = exec.evaluate("1 /", &result);
    EXPECT_NE(rc, eNoError);
    EXPECT_FALSE(exec.getLastError().empty());

    exec.finalize();
}


TEST(PythonExecutor, SetGetVariable)
{
    PythonExecutor exec;
    if (exec.initialize() != eNoError) {
        GTEST_SKIP() << "Python not available";
        return;
    }

    // 设置整数
    errc_t rc = exec.setVariable("my_int", 42);
    EXPECT_EQ(rc, eNoError);
    int iVal = 0;
    rc = exec.getVariable("my_int", iVal);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(iVal, 42);

    // 设置浮点
    rc = exec.setVariable("my_double", 3.14159);
    EXPECT_EQ(rc, eNoError);
    double dVal = 0.0;
    rc = exec.getVariable("my_double", dVal);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(dVal, 3.14159, 1e-9);

    // 设置字符串
    rc = exec.setVariable("my_str", "hello python");
    EXPECT_EQ(rc, eNoError);
    std::string sVal;
    rc = exec.getVariable("my_str", sVal);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(sVal, "hello python");

    // 设置布尔
    rc = exec.setVariable("my_bool", true);
    EXPECT_EQ(rc, eNoError);
    bool bVal = false;
    rc = exec.getVariable("my_bool", bVal);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(bVal, true);

    // 从 Python 侧验证变量值
    exec.execute("assert my_int == 42");
    exec.execute("assert abs(my_double - 3.14159) < 1e-6");
    exec.execute("assert my_str == 'hello python'");

    // 获取不存在的变量
    rc = exec.getVariable("nonexistent", iVal);
    EXPECT_NE(rc, eNoError);

    exec.finalize();
}


TEST(PythonExecutor, ExecuteError)
{
    PythonExecutor exec;
    if (exec.initialize() != eNoError) {
        GTEST_SKIP() << "Python not available";
        return;
    }

    // 运行时错误
    errc_t rc = exec.execute("1 / 0");
    EXPECT_NE(rc, eNoError);
    std::string err = exec.getLastError();
    EXPECT_FALSE(err.empty());

    // 语法错误
    rc = exec.execute("for i in range(10)");
    EXPECT_NE(rc, eNoError);
    err = exec.getLastError();
    EXPECT_FALSE(err.empty());

    // 成功执行后错误信息应保持（不自动清除）
    rc = exec.execute("x = 42");
    EXPECT_EQ(rc, eNoError);

    exec.finalize();
}


TEST(PythonExecutor, IsolatedNamespaces)
{
    PythonExecutor exec1;
    if (exec1.initialize() != eNoError) {
        GTEST_SKIP() << "Python not available";
        return;
    }

    // exec1 设置变量
    exec1.setVariable("x", 100);
    exec1.execute("y = x * 2");

    // exec2 拥有独立命名空间
    PythonExecutor exec2;
    errc_t rc = exec2.initialize();
    EXPECT_EQ(rc, eNoError);

    // exec2 看不到 exec1 的变量
    int val = 0;
    rc = exec2.getVariable("x", val);
    EXPECT_NE(rc, eNoError);

    // exec2 可以设置自己的同名变量
    exec2.setVariable("x", 200);
    rc = exec2.getVariable("x", val);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(val, 200);

    // exec1 的变量不受 exec2 影响
    rc = exec1.getVariable("x", val);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(val, 100);
    rc = exec1.getVariable("y", val);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(val, 200);

    exec2.finalize();
    exec1.finalize();
}


TEST(PythonExecutor, ScriptResult)
{
    PythonExecutor exec;
    if (exec.initialize() != eNoError) {
        GTEST_SKIP() << "Python not available";
        return;
    }

    // execute 不产生返回值
    ScriptResult result;
    errc_t rc = exec.execute("a = 99", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(result.error_, "");

    // evaluate 产生各类返回值
    // 整数
    rc = exec.evaluate("10 * 10", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(result.value_->isInt());

    // 浮点
    rc = exec.evaluate("1.5 + 2.5", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(result.value_->isDouble());

    // 字符串
    rc = exec.evaluate("'test'", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(result.value_->isString());

    // 布尔
    rc = exec.evaluate("True and False", &result);
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(result.value_->isBool());

    exec.finalize();
}


TEST(PythonExecutor, TypeMismatch)
{
    PythonExecutor exec;
    if (exec.initialize() != eNoError) {
        GTEST_SKIP() << "Python not available";
        return;
    }

    // ---- 设置 int，用各类型读取 ----
    exec.setVariable("int_val", 42);

    int i = 0;
    EXPECT_EQ(exec.getVariable("int_val", i), eNoError);
    EXPECT_EQ(i, 42);

    double d = 0.0;
    EXPECT_EQ(exec.getVariable("int_val", d), eNoError);
    EXPECT_DOUBLE_EQ(d, 42.0);

    std::string s;
    EXPECT_EQ(exec.getVariable("int_val", s), eNoError);
    EXPECT_EQ(s, "42");

    bool b = false;
    EXPECT_EQ(exec.getVariable("int_val", b), eNoError);
    EXPECT_TRUE(b);

    // ---- 设置 double，用各类型读取 ----
    exec.setVariable("dbl_val", 3.14);

    EXPECT_EQ(exec.getVariable("dbl_val", d), eNoError);
    EXPECT_DOUBLE_EQ(d, 3.14);

    EXPECT_EQ(exec.getVariable("dbl_val", i), eNoError);
    EXPECT_EQ(i, 3);  // 截断

    EXPECT_EQ(exec.getVariable("dbl_val", s), eNoError);

    EXPECT_EQ(exec.getVariable("dbl_val", b), eNoError);
    EXPECT_TRUE(b);

    // ---- 设置 bool，用各类型读取 ----
    exec.setVariable("bool_val", false);

    EXPECT_EQ(exec.getVariable("bool_val", b), eNoError);
    EXPECT_FALSE(b);

    EXPECT_EQ(exec.getVariable("bool_val", i), eNoError);
    EXPECT_EQ(i, 0);

    EXPECT_EQ(exec.getVariable("bool_val", s), eNoError);
    EXPECT_EQ(s, "False");

    EXPECT_EQ(exec.getVariable("bool_val", d), eNoError);
    EXPECT_DOUBLE_EQ(d, 0.0);

    // ---- 设置字符串，用各类型读取 ----
    exec.setVariable("str_val", "hello");

    EXPECT_EQ(exec.getVariable("str_val", s), eNoError);
    EXPECT_EQ(s, "hello");

    errc_t rc;
    d = 999.0;
    rc = exec.getVariable("str_val", d);
    EXPECT_NE(rc, eNoError);
    EXPECT_DOUBLE_EQ(d, 999.0);  // 输出未被修改

    i = 999;
    rc = exec.getVariable("str_val", i);
    EXPECT_NE(rc, eNoError);
    EXPECT_EQ(i, 999);

    b = true;
    rc = exec.getVariable("str_val", b);
    EXPECT_NE(rc, eNoError);
    EXPECT_TRUE(b);

    // ---- 后续操作正常，无异常级联 ----
    exec.setVariable("after", 123);
    int after = 0;
    EXPECT_EQ(exec.getVariable("after", after), eNoError);
    EXPECT_EQ(after, 123);

    exec.finalize();
}


GTEST_MAIN();
