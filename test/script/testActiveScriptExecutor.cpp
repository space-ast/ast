///
/// @file      testActiveScriptExecutor.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-30
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

#include "ast/JScriptExecutor.hpp"
#include "ast/VBScriptExecutor.hpp"
#include "ast/ScopedPtr.hpp"
#include "ast/IO.hpp"
#include "ast/Test.h"


AST_USING_NAMESPACE

#ifdef _WIN32

TEST(ActiveScriptExecutorTest, JScript)
{
    JScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    // 输入变量（根据实际需求修改）
    rc = executor.setVariable("FinalRMag", 7000); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("CurrentApoapsisMag", 6800); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("CurrentEccentricity", 0.2); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("CurrentTrueAnomaly", 2.1); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("StopOnPeriapsisCoast", false); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("StopOnApoapsisCoast", false); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("StopOnPeriapsisTransfer", false); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("StopOnApoapsisTransfer", false); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("SkipInitialCoast", ""); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("DesiredRMagScript", 0); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("DesiredRMagDC", 0); EXPECT_EQ(rc, 0);

    // 检测是否成功设置变量
    {
        double finalRMag, currentApoapsisMag, currentEccentricity, currentTrueAnomaly;
        rc = executor.getVariable("FinalRMag", finalRMag);
        EXPECT_EQ(rc, 0);
        rc = executor.getVariable("CurrentApoapsisMag", currentApoapsisMag);
        EXPECT_EQ(rc, 0);
        rc = executor.getVariable("CurrentEccentricity", currentEccentricity);
        EXPECT_EQ(rc, 0);
        rc = executor.getVariable("CurrentTrueAnomaly", currentTrueAnomaly);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(finalRMag, 7000.0);
        EXPECT_EQ(currentApoapsisMag, 6800.0);
        EXPECT_EQ(currentEccentricity, 0.2);
        EXPECT_EQ(currentTrueAnomaly, 2.1);
    }

    // 这是用户提供的脚本
    const char* userScript = u8R"script(
        // This script sets up a Hohmann transfer sequence.
        // ... (完整脚本内容如下)
        var startTrueAnomA;
        var startTrueAnomB;

        if (FinalRMag > CurrentApoapsisMag)
        {
            StopOnPeriapsisCoast = true;
            StopOnApoapsisCoast = false;
            StopOnPeriapsisTransfer = false;
            StopOnApoapsisTransfer = true;
            startTrueAnomA = 0.0;
            startTrueAnomB = 360.0;
        }
        else
        {
            StopOnPeriapsisCoast = false;
            StopOnApoapsisCoast = true;
            StopOnPeriapsisTransfer = true;
            StopOnApoapsisTransfer = false;
            startTrueAnomA = 180.0;
            startTrueAnomB = 180.0;
        }

        if ( CurrentEccentricity < 0.01 ||
             Math.abs(startTrueAnomA - CurrentTrueAnomaly) < 0.01 ||
             Math.abs(startTrueAnomB - CurrentTrueAnomaly) < 0.01 )
        {
            SkipInitialCoast = "Enable";
        }
        else
        {
            SkipInitialCoast = "Disable";
        }

        DesiredRMagScript = FinalRMag;
        DesiredRMagDC = FinalRMag;
    )script";

    // 执行脚本
    rc = executor.execute(userScript);
    EXPECT_EQ(rc, 0);

    // 输出变量
    {
        std::string skipInitialCoast;
        rc = executor.getVariable("SkipInitialCoast", skipInitialCoast);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(skipInitialCoast, "Disable");
    }
    {
        bool stopOnPeriapsisCoast;
        rc = executor.getVariable("StopOnPeriapsisCoast", stopOnPeriapsisCoast);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(stopOnPeriapsisCoast, true);
    }
    {
        double desiredRMagScript;
        rc = executor.getVariable("DesiredRMagScript", desiredRMagScript);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(desiredRMagScript, 7000.0);
    }
    {
        double desiredRMagDC;
        rc = executor.getVariable("DesiredRMagDC", desiredRMagDC);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(desiredRMagDC, 7000.0);
    }
}


TEST(ActiveScriptExecutorTest, VBScript)
{
    VBScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    // 输入变量
    rc = executor.setVariable("a", 10.); EXPECT_EQ(rc, 0);
    rc = executor.setVariable("b", 3.); EXPECT_EQ(rc, 0);

    // 检测是否成功设置变量
    {
        double a, b;
        rc = executor.getVariable("a", a);
        EXPECT_EQ(rc, 0);
        rc = executor.getVariable("b", b);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(a, 10.0);
        EXPECT_EQ(b, 3.0);
    }

    const char* script = u8R"(
Option Explicit

' 定义两个操作数
'Dim a, b
'a = 10
'b = 3

' 进行简单数学运算
Dim sum, diff, prod, quot, power
sum = a + b          ' 加法
diff = a - b         ' 减法
prod = a * b         ' 乘法
quot = a / b         ' 除法
power = a ^ b        ' 幂运算
)";
    // 执行脚本
    rc = executor.execute(script);
    EXPECT_EQ(rc, 0);

    // 输出变量
    double sum, diff, prod, quot, power;
    rc = executor.getVariable("sum", sum);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(sum, 13.0);
    rc = executor.getVariable("diff", diff);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(diff, 7.0);
    rc = executor.getVariable("prod", prod);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(prod, 30.0);
    rc = executor.getVariable("quot", quot);
    EXPECT_EQ(rc, 0);
    EXPECT_DOUBLE_EQ(quot, 3.333333333333333333333333333);
    rc = executor.getVariable("power", power);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(power, 1000.0);

    {
        rc = executor.setVariable("value_bool", true);
        EXPECT_EQ(rc, 0);
        bool value_bool = false;
        rc = executor.getVariable("value_bool", value_bool);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_bool, true);
    }
    {
        rc = executor.setVariable("value_int", 123);
        EXPECT_EQ(rc, 0);
        int value_int = 0;
        rc = executor.getVariable("value_int", value_int);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_int, 123);
    }
}


/// @brief 检查脚本执行器的错误处理行为
/// @param executor           已初始化的脚本执行器
/// @param runtimeErrorScript 引用未定义变量、触发运行时错误的脚本
/// @param recoveryScript     正常脚本，用于验证出错后执行器仍可继续使用
/// @param syntaxErrorScript  含语法错误的脚本
void checkErrorHandling(ScriptExecutor& executor, StringView runtimeErrorScript,
                        StringView recoveryScript, StringView syntaxErrorScript)
{
    errc_t rc;

    // 执行运行时错误的脚本：没有定义的变量
    {
        std::string errorOut;
        rc = executor.execute(runtimeErrorScript, &errorOut);
        EXPECT_NE(rc, 0);
        EXPECT_TRUE(!errorOut.empty());
    }

    // 测试执行错误的脚本后，是否能继续执行正常的脚本
    {
        std::string errorOut;
        rc = executor.execute(recoveryScript, &errorOut);
        EXPECT_EQ(rc, 0);
        EXPECT_TRUE(errorOut.empty());

        int a = 1;
        rc = executor.getVariable("a", a);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(a, 1);
    }

    // 执行语法错误的脚本
    {
        std::string errorOut;
        rc = executor.execute(syntaxErrorScript, &errorOut);
        EXPECT_NE(rc, 0);
        EXPECT_TRUE(!errorOut.empty());
    }
}



TEST(ActiveScriptExecutorTest, ErrorHandling_JScript)
{
    JScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    // JScript 读取未定义的变量会抛出 "xxx 未定义" 错误
    checkErrorHandling(executor,
        u8R"(
            if (FinalRMag > CurrentApoapsisMag)
            {
                StopOnPeriapsisCoast = true;
                StopOnApoapsisCoast = false;
                StopOnPeriapsisTransfer = false;
                StopOnApoapsisTransfer = true;
                startTrueAnomA = 0.0;
                startTrueAnomB = 360.0;
            }
        )",
        u8R"(var a = 1)",
        u8R"(....=@# ! abcx9279$%323)");
}


TEST(ActiveScriptExecutorTest, ErrorHandling_VBScript)
{
    VBScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    // VBScript 默认允许使用未声明的变量，需 Option Explicit 才能得到 "变量未定义" 错误；
    checkErrorHandling(executor,
        u8R"(
            Option Explicit

            If FinalRMag > CurrentApoapsisMag Then
                StopOnPeriapsisCoast = True
                StopOnApoapsisCoast = False
                startTrueAnomA = 0.0
                startTrueAnomB = 360.0
            End If
        )",
        u8R"(
            Dim a
            a = 1
        )",
        u8R"(....=@# ! abcx9279$%323)");
}



/// @brief 检查对同一个名称的变量反复赋值的行为
/// @details setVariable 的各重载都会先在脚本中声明变量（JScript 用 var、VBScript 用 Dim），
///          对已有变量重复调用时不应因重复声明而失败，且最后一次赋值应当生效
/// @param executor 已初始化的脚本执行器
void checkSetVariableRepeatedly(ScriptExecutor& executor)
{
    errc_t rc;

    // 同一名称的浮点变量反复赋值
    for (int i = 0; i < 3; ++i)
    {
        double expected = 1.5 * i;
        rc = executor.setVariable("value_repeat", expected);
        EXPECT_EQ(rc, 0);
        double value = -1.0;
        rc = executor.getVariable("value_repeat", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, expected);
    }

    // 同一名称的变量在不同类型之间反复赋值
    {
        rc = executor.setVariable("value_repeat_type", 123);
        EXPECT_EQ(rc, 0);
        int value_int = 0;
        rc = executor.getVariable("value_repeat_type", value_int);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_int, 123);
    }
    {
        rc = executor.setVariable("value_repeat_type", true);
        EXPECT_EQ(rc, 0);
        bool value_bool = false;
        rc = executor.getVariable("value_repeat_type", value_bool);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_bool, true);
    }
    {
        rc = executor.setVariable("value_repeat_type", 2.5);
        EXPECT_EQ(rc, 0);
        double value_double = 0.0;
        rc = executor.getVariable("value_repeat_type", value_double);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_double, 2.5);
    }
    {
        rc = executor.setVariable("value_repeat_type", "hello");
        EXPECT_EQ(rc, 0);
        std::string value_string;
        rc = executor.getVariable("value_repeat_type", value_string);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_string, "hello");
    }
}


TEST(ActiveScriptExecutorTest, SetVariable_Repeatedly_JScript)
{
    JScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    checkSetVariableRepeatedly(executor);
}


TEST(ActiveScriptExecutorTest, SetVariable_Repeatedly_VBScript)
{
    VBScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    checkSetVariableRepeatedly(executor);
}


TEST(ActiveScriptExecutorTest, SetVariable_Repeatedly_VBScriptOptionExplicit)
{
    VBScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    // Option Explicit 必须位于脚本首部，这里用它模拟「脚本先声明变量」的场景
    rc = executor.execute(u8R"(
        Option Explicit

        Dim value_declared
        value_declared = 1
    )");
    EXPECT_EQ(rc, 0);

    // 脚本中声明的变量应当可以读回
    {
        int value_declared = 0;
        rc = executor.getVariable("value_declared", value_declared);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_declared, 1);
    }

    // Option Explicit 生效后，反复 setVariable 的行为应当与之前一致
    // Option Explicit 只会影响其所在脚本段的执行，不会影响后续脚本段的执行
    // 所以内部实现通过 Dim 声明变量是安全的
    checkSetVariableRepeatedly(executor);

    // 脚本已经声明过的变量，宿主再反复 setVariable 覆盖
    // （setVariable 会再执行一次 Dim，这里验证重复声明不会失败）
    for (int i = 0; i < 3; ++i)
    {
        double expected = 10.0 + i;
        rc = executor.setVariable("value_declared", expected);
        EXPECT_EQ(rc, 0);
        double value = 0.0;
        rc = executor.getVariable("value_declared", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, expected);
    }
    {
        rc = executor.setVariable("value_declared", 7);
        EXPECT_EQ(rc, 0);
        int value = 0;
        rc = executor.getVariable("value_declared", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, 7);
    }
    {
        rc = executor.setVariable("value_declared", true);
        EXPECT_EQ(rc, 0);
        bool value = false;
        rc = executor.getVariable("value_declared", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, true);
    }
    {
        rc = executor.setVariable("value_declared", StringView("done"));
        EXPECT_EQ(rc, 0);
        std::string value;
        rc = executor.getVariable("value_declared", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, "done");
    }

    // 严格模式下，宿主设置的变量可以直接被脚本使用，脚本侧也能对它们反复赋值
    rc = executor.setVariable("value_declared", 3);
    EXPECT_EQ(rc, 0);
    rc = executor.setVariable("value_host", 100);
    EXPECT_EQ(rc, 0);
    rc = executor.execute(u8R"(
        Option Explicit

        value_declared = value_declared + 1
        value_declared = value_declared + 1
        value_host = value_host * 2
    )");
    EXPECT_EQ(rc, 0);
    {
        int value_declared = 0;
        rc = executor.getVariable("value_declared", value_declared);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_declared, 5);

        int value_host = 0;
        rc = executor.getVariable("value_host", value_host);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_host, 200);
    }

    // Option Explicit 只约束它所在的那段脚本，不会延续到后续的 execute：
    // 后面的脚本里使用未声明变量仍会被静默创建，而不是报「变量未定义」
    {
        std::string errorOut;
        rc = executor.execute("undeclared_probe = 1", &errorOut);
        EXPECT_EQ(rc, 0);
        EXPECT_TRUE(errorOut.empty());

        double probe = 0.0;
        rc = executor.getVariable("undeclared_probe", probe);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(probe, 1.0);
    }
}



TEST(ActiveScriptExecutorTest, DimSameNameInOneScript_VBScript)
{
    VBScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    // 同一脚本段内重复 Dim 同名变量 → 「名称重定义」错误
    {
        std::string errorOut;
        rc = executor.execute(u8R"(
            Dim value_dup
            Dim value_dup
            value_dup = 1
            value_side_effect = 1
        )", &errorOut);
        EXPECT_NE(rc, 0);
        EXPECT_TRUE(!errorOut.empty());

        // 重定义是编译期错误，整段脚本被整体拒绝：后面的赋值语句一行都没执行
        double value_side_effect = 0.0;
        rc = executor.getVariable("value_side_effect", value_side_effect);
        EXPECT_NE(rc, 0);
    }

    // 两次 Dim 之间隔着赋值语句，同样报错
    {
        std::string errorOut;
        rc = executor.execute(u8R"(
            Dim value_dup2
            value_dup2 = 1
            Dim value_dup2
        )", &errorOut);
        EXPECT_NE(rc, 0);
        EXPECT_TRUE(!errorOut.empty());
    }

    // 同一行里 Dim 两次，同样报错
    {
        std::string errorOut;
        rc = executor.execute(u8R"(Dim value_dup3, value_dup3)", &errorOut);
        EXPECT_NE(rc, 0);
        EXPECT_TRUE(!errorOut.empty());
    }

    // 不同作用域中的同名 Dim 不冲突：Sub 内部的同名局部变量不影响全局变量
    {
        rc = executor.execute(u8R"(
            Dim value_scoped
            value_scoped = 1

            Sub SetScoped()
                Dim value_scoped
                value_scoped = 2
            End Sub

            SetScoped
            value_scoped = value_scoped + 3
        )");
        EXPECT_EQ(rc, 0);

        int value_scoped = 0;
        rc = executor.getVariable("value_scoped", value_scoped);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_scoped, 4);
    }

    // 重复 Dim 报错后，执行器仍然可以继续使用
    {
        rc = executor.execute(u8R"(
            Dim value_after
            value_after = 42
        )");
        EXPECT_EQ(rc, 0);

        int value_after = 0;
        rc = executor.getVariable("value_after", value_after);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_after, 42);
    }
}


TEST(ActiveScriptExecutorTest, DimSameNameAcrossScripts_VBScript)
{
    VBScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    // 第一段：声明并赋值
    rc = executor.execute(u8R"(
        Dim value_cross
        value_cross = 5
    )");
    EXPECT_EQ(rc, 0);
    {
        int value = 0;
        rc = executor.getVariable("value_cross", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, 5);
    }

    // 第二段：跨段重复 Dim 同名变量是合法的（与同段内重定义不同）
    rc = executor.execute(u8R"(
        Dim value_cross
    )");
    EXPECT_EQ(rc, 0);

    // 裸声明不会重置已有变量的值
    {
        int value = 0;
        rc = executor.getVariable("value_cross", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, 5);
    }

    // 第三段：跨段之后仍然是同一个变量，读写都作用在它上面
    rc = executor.execute(u8R"(
        value_cross = value_cross + 1
    )");
    EXPECT_EQ(rc, 0);
    {
        int value = 0;
        rc = executor.getVariable("value_cross", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, 6);
    }

    // 宿主 setVariable 设置的变量，同样可以被后续脚本跨段裸 Dim 重声明
    rc = executor.setVariable("value_host", 100);
    EXPECT_EQ(rc, 0);
    rc = executor.execute(u8R"(
        Dim value_host
    )");
    EXPECT_EQ(rc, 0);
    {
        int value = 0;
        rc = executor.getVariable("value_host", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, 100);
    }

    // 脚本隐式创建（未 Dim）的变量，宿主再 setVariable 也应当正常
    // （setVariable 内部会为该变量执行一次 Dim，此时它已经存在）
    rc = executor.execute(u8R"(value_implicit = 5)");
    EXPECT_EQ(rc, 0);
    rc = executor.setVariable("value_implicit", 7);
    EXPECT_EQ(rc, 0);
    {
        int value = 0;
        rc = executor.getVariable("value_implicit", value);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value, 7);
    }
}


TEST(ActiveScriptExecutorTest, DimSameNameInOneScript_JScript)
{
    JScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);

    // 与 VBScript 不同，JScript 的 var 重复声明同名变量是合法的（幂等），不会报错
    rc = executor.execute(u8R"(
        var value_dup = 1;
        var value_dup = 2;
        value_dup = value_dup + 1;
    )");
    EXPECT_EQ(rc, 0);

    int value_dup = 0;
    rc = executor.getVariable("value_dup", value_dup);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(value_dup, 3);
}



/// @brief 检查脚本执行器的变量类型转换行为
/// @details 调用前，脚本中需已定义以下变量：
///          value_int=123、value_double=1.34、value_string_true="true"、
///          value_string_false="false"、value_bool_true=true、
///          value_bool_false=false、value_null=null（VBScript 中为 Null）
void checkVariableTypeConversion(ScriptExecutor& executor)
{
    errc_t rc;

    // 测试获取整数变量
    {
        int value_int = 0;
        rc = executor.getVariable("value_int", value_int);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_int, 123);

        // 测试类型转换
        rc = executor.getVariable("value_double", value_int);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_int, 1);

        // 类型不匹配
        rc = executor.getVariable("value_string_true", value_int);
        EXPECT_NE(rc, 0);

        rc = executor.getVariable("value_string_false", value_int);
        EXPECT_NE(rc, 0);

        rc = executor.getVariable("value_bool_true", value_int);
        EXPECT_EQ(rc, 0);
        EXPECT_NE(value_int, 0);

        rc = executor.getVariable("value_bool_false", value_int);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_int, 0);

        rc = executor.getVariable("value_null", value_int);
        EXPECT_NE(rc, 0);
    }
    // 测试获取浮点数变量
    {
        double value_double = 0.0;
        rc = executor.getVariable("value_double", value_double);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_double, 1.34);

        rc = executor.getVariable("value_int", value_double);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_double, 123);
    }
    // 测试获取布尔变量
    {
        bool value_bool = false;
        rc = executor.getVariable("value_bool_true", value_bool);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_bool, true);

        rc = executor.getVariable("value_bool_false", value_bool);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_bool, false);
    }
    // 测试获取字符串变量
    {
        std::string value_string;
        rc = executor.getVariable("value_string_true", value_string);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_string, "true");

        rc = executor.getVariable("value_string_false", value_string);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_string, "false");
    }
}


TEST(ActiveScriptExecutorTest, GetVariable_TypeConversion_JScript)
{
    JScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);
    const char* script = u8R"(
        var value_int = 123;
        var value_double = 1.34;
        var value_string_true = "true";
        var value_string_false = "false";
        var value_bool_true = true;
        var value_bool_false = false;
        var value_null = null;
    )";
    rc = executor.execute(script);
    EXPECT_EQ(rc, 0);

    checkVariableTypeConversion(executor);
}


TEST(ActiveScriptExecutorTest, GetVariable_TypeConversion_VBScript)
{
    VBScriptExecutor executor;
    errc_t rc = executor.initialize();
    EXPECT_EQ(rc, 0);
    // VBScript 的变量需先声明，且没有 null 字面量，空值写作 Null
    const char* script = u8R"(
        Dim value_int, value_double
        Dim value_string_true, value_string_false
        Dim value_bool_true, value_bool_false
        Dim value_null

        value_int = 123
        value_double = 1.34
        value_string_true = "true"
        value_string_false = "false"
        value_bool_true = True
        value_bool_false = False
        value_null = Null
    )";
    rc = executor.execute(script);
    EXPECT_EQ(rc, 0);

    checkVariableTypeConversion(executor);
}

TEST(ActiveScriptExecutorTest, SetVariable_EdgeCases)
{
    {
        JScriptExecutor executor;
        errc_t rc = executor.initialize();
        EXPECT_EQ(rc, 0);

        rc = executor.setVariable("value_nan", std::numeric_limits<double>::quiet_NaN());
        EXPECT_EQ(rc, 0);
        double value_nan = 0.0;
        rc = executor.getVariable("value_nan", value_nan);
        EXPECT_EQ(rc, 0);
        EXPECT_TRUE(std::isnan(value_nan));

        rc = executor.setVariable("value_inf", std::numeric_limits<double>::infinity());
        EXPECT_EQ(rc, 0);
        double value_inf = 0.0;
        rc = executor.getVariable("value_inf", value_inf);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_inf, std::numeric_limits<double>::infinity());

    }
    {
        VBScriptExecutor executor;
        errc_t rc = executor.initialize();
        EXPECT_EQ(rc, 0);

        rc = executor.setVariable("value_nan", std::numeric_limits<double>::quiet_NaN());
        EXPECT_EQ(rc, 0);
        
        double value_nan = 0.0;
        rc = executor.getVariable("value_nan", value_nan);
        EXPECT_EQ(rc, 0);
        EXPECT_TRUE(std::isnan(value_nan));

        rc = executor.setVariable("value_inf", std::numeric_limits<double>::infinity());
        EXPECT_EQ(rc, 0);
        double value_inf = 0.0;
        rc = executor.getVariable("value_inf", value_inf);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(value_inf, std::numeric_limits<double>::infinity());
    }


}




TEST(ActiveScriptExecutorTest, OutputMessage)
{
 #ifndef _WIN32
    GTEST_SKIP();
    #endif
    ScopedPtr<ScriptExecutor> executor = aNewScriptExecutor(EScriptLanguage::eVBScript);
    ASSERT_TRUE(executor);
    errc_t rc = executor->initialize();
    ASSERT_EQ(rc, eNoError);

    rc = executor->execute(R"(
        
        OutputMessage 4, "hello world"
    )");
    ASSERT_EQ(rc, eNoError);
}


TEST(ActiveScriptExecutorTest, EvalExpression)
{
    ScopedPtr<ScriptExecutor> executor = aNewScriptExecutor(EScriptLanguage::eVBScript);
    ASSERT_TRUE(executor);
    errc_t rc = executor->initialize();
    ASSERT_EQ(rc, eNoError);

    ScriptResult result;
    rc = executor->evaluate(R"(8.0/2.0)", &result);
    ASSERT_EQ(rc, eNoError);
    ASSERT_TRUE(result.value());
    EXPECT_EQ(result.value()->toDouble(), 4);
}

#endif


GTEST_MAIN()

