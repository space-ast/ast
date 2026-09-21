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

#include "ast/PythonExecutor.hpp"
#include "ast/PythonAPI.hpp"
#include "ast/LibraryLoader.hpp"
#include "ast/Test.h"

#include <cstdio>
#include <string>

#ifndef _WIN32
#include <dlfcn.h>
#endif

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
    EXPECT_EQ(rc, eNoError) << "import math 失败: " << exec.getLastError();

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
    rc = exec.execute("import math");
    EXPECT_EQ(rc, eNoError) << "import math 失败: " << exec.getLastError();
    rc = exec.evaluate("math.sqrt(16)", &result);
    EXPECT_EQ(rc, eNoError) << "求值 math.sqrt(16) 失败: " << exec.getLastError();
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


// ---------------------------------------------------------------------------
// 临时诊断：定位 manylinux 容器腿里 "import math 失败" 的根因。
// 只在 ci/python-diag 分支使用，定位后删除。
// ---------------------------------------------------------------------------
TEST(PythonExecutor, DiagnoseImport)
{
    PythonExecutor exec;
    if (exec.initialize() != eNoError) { GTEST_SKIP() << "Python not available"; return; }

    printf("\n===== DIAG BEGIN =====\n");

    // 对照 A：直接用 PyRun_SimpleString。它在 __main__.__dict__ 里执行，那个 dict
    // 自带 __builtins__；而 PythonExecutor 用的是自己 PyDict_New() 出来的空 dict。
    auto* api = PythonAPI::Instance();
    int rcSimple = api->PyRun_SimpleString("import math; _diag = math.sqrt(16)");
    printf("[diag] A  PyRun_SimpleString('import math')  -> rc=%d (0 表示成功)\n", rcSimple);

    // 对照 B：走 PythonExecutor 的空 dict
    errc_t rcExec = exec.execute("import math");
    printf("[diag] B  exec.execute('import math')        -> rc=%d err=[%s]\n",
           (int)rcExec, exec.getLastError().c_str());

    // 逐个 import：sys/builtins 是编译进解释器的内建模块，不需要 sys.path；
    // math/os/json/ctypes 在 Linux 上是 lib-dynload 里的 .so，需要 sys.path。
    // 若内建的过、扩展的全挂，就是 sys.path 问题；若连内建都挂，就是 __builtins__ 问题。
    const char* probes[] = {"sys", "builtins", "math", "os", "json", "ctypes"};
    for (auto* name : probes)
    {
        std::string script = std::string("import ") + name;
        errc_t r = exec.execute(script.c_str());
        printf("[diag] C  import %-9s -> rc=%d err=[%s]\n",
               name, (int)r, exec.getLastError().c_str());
    }

    // 解释器自述
    if (exec.execute("import sys") == eNoError)
    {
        const char* exprs[] = {
            "sys.version",
            "sys.prefix",
            "sys.exec_prefix",
            "sys.base_prefix",
            "sys.executable",
            "','.join(sys.builtin_module_names)",
            "'|'.join(sys.path)",
        };
        for (auto* e : exprs)
        {
            ScriptResult result;
            errc_t r = exec.evaluate(e, &result);
            printf("[diag] D  %s\n         -> rc=%d value=[%s] err=[%s]\n", e, (int)r,
                   result.value_.get() ? result.value_->toString().c_str() : "(null)",
                   exec.getLastError().c_str());
        }
    }

    // 对照组 E：直接确认命名空间里到底有没有 __builtins__。
    // 这条表达式本身就要用到内建函数 globals()，修好之前它自己也会挂。
    {
        ScriptResult result;
        errc_t r = exec.evaluate("'__builtins__' in globals()", &result);
        printf("[diag] E  \"'__builtins__' in globals()\" -> rc=%d value=[%s] err=[%s]\n",
               (int)r,
               result.value_.get() ? result.value_->toString().c_str() : "(null)",
               exec.getLastError().c_str());
    }

    // 对照组 F：逐个候选名直接试加载，看 PythonAPI 的候选表里到底哪个能命中、
    // 命中到什么路径。（PythonAPI::load 用的就是 aLoadLibrary）
    {
        const char* candidates[] = {
            "libpython3.14", "libpython3.13", "libpython3.12", "libpython3.11",
            "libpython3.10", "libpython3.9",  "libpython3.8",  "libpython3",
        };
        for (auto* name : candidates)
        {
            void* h = aLoadLibrary(name);
            if (h)
            {
                const char* path = "?";
#ifndef _WIN32
                // dladdr 要的是**库内某个符号的地址**，不是 dlopen 的句柄。
                // 上一轮直接传句柄，dladdr 必然失败，path 一直是 "?"。
                void* sym = aGetProcAddress(h, "Py_Initialize");
                Dl_info info{};
                if (sym && dladdr(sym, &info) && info.dli_fname)
                    path = info.dli_fname;
#endif
                printf("[diag] F  load %-14s -> OK   path=%s\n", name, path);
                aFreeLibrary(h);
            }
            else
            {
                printf("[diag] F  load %-14s -> FAIL (%s)\n", name, aGetLoadError());
            }
        }
    }

    printf("===== DIAG END =====\n\n");
}


GTEST_MAIN();
