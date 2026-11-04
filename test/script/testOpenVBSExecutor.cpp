///
/// @file      testOpenVBSExecutor.cpp
/// @brief     OpenVBSExecutor 测试
/// @details   测试基于 OpenVBS 的 VBScript 执行器：脚本执行、表达式求值、
///            变量传递、跨 execute 的全局状态、错误处理与实例隔离
/// @author    axel
/// @date      2026-10-31
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

#include "ast/ScriptExecutor.hpp"
#include "ast/ScriptExecutorProvider.hpp"
#include "ast/PluginRegistry.hpp"
#include "ast/Value.hpp"
#include "ast/Test.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <vector>

AST_USING_NAMESPACE

namespace {

/// @brief 扫描一次插件目录，再创建 OpenVBS 执行器；插件缺失时返回空指针
std::unique_ptr<ScriptExecutor> newOpenVBSExecutor()
{
    static const errc_t scanResult = aScanPlugins();
    (void)scanResult;
    // 先查询接口，避免插件缺失时工厂逐次打印错误日志
    IScriptExecutorProvider* provider = aPluginRegistry().find<IScriptExecutorProvider>("OpenVBS");
    #if defined(AST_WITH_OPENVBS)
    // 记一次失败：后续 GTEST_SKIP() 不会把它降级成跳过，构建或部署缺插件必须暴露出来
    EXPECT_NE(provider, nullptr) << "AstPluginOpenVBS is missing from an AST_WITH_OPENVBS build";
    #endif
    if(provider == nullptr)
        return nullptr;
    return std::unique_ptr<ScriptExecutor>(provider->create());
}

/// @brief 创建并初始化 OpenVBS 执行器；插件或引擎不可用时返回空指针
std::unique_ptr<ScriptExecutor> newInitializedOpenVBSExecutor()
{
    std::unique_ptr<ScriptExecutor> executor = newOpenVBSExecutor();
    if(!executor)
        return nullptr;
    if(executor->initialize() != eNoError)
        return nullptr;
    return executor;
}

} // namespace


TEST(OpenVBSExecutorTest, PluginProvidesInterface)
{
    static const errc_t scanResult = aScanPlugins();
    (void)scanResult;

    const std::vector<std::string> keys = aPluginRegistry().keys<IScriptExecutorProvider>();
    if(std::find(keys.begin(), keys.end(), std::string("OpenVBS")) == keys.end())
    {
        GTEST_SKIP() << "OpenVBS plugin is not available";
        return;
    }

    // 索引里有键却转换失败，说明接口 RTTI 没有跨 DLL 统一，是错误而非插件缺失
    IScriptExecutorProvider* provider = aPluginRegistry().find<IScriptExecutorProvider>("OpenVBS");
    ASSERT_TRUE(provider != nullptr);

    std::unique_ptr<ScriptExecutor> executor(provider->create());
    ASSERT_TRUE(executor != nullptr);
    EXPECT_EQ(executor->initialize(), eNoError);
}


TEST(OpenVBSExecutorTest, VariableRoundTrip)
{
    std::unique_ptr<ScriptExecutor> executor = newOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "OpenVBS engine initialization failed";

    EXPECT_EQ(executor->setVariable("name", "ast"), eNoError);
    EXPECT_EQ(executor->setVariable("value", 42.5), eNoError);
    EXPECT_EQ(executor->setVariable("count", 7), eNoError);
    EXPECT_EQ(executor->setVariable("flag", true), eNoError);

    std::string name;
    double value = 0.0;
    int count = 0;
    bool flag = false;
    EXPECT_EQ(executor->getVariable("name", name), eNoError);
    EXPECT_EQ(name, "ast");
    EXPECT_EQ(executor->getVariable("value", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 42.5);
    EXPECT_EQ(executor->getVariable("count", count), eNoError);
    EXPECT_EQ(count, 7);
    EXPECT_EQ(executor->getVariable("flag", flag), eNoError);
    EXPECT_TRUE(flag);

    // VBScript 的整数按 64 位保存，也能按 double 读回
    double countAsDouble = 0.0;
    EXPECT_EQ(executor->getVariable("count", countAsDouble), eNoError);
    EXPECT_DOUBLE_EQ(countAsDouble, 7.0);
}


TEST(OpenVBSExecutorTest, VariableNamesAreCaseInsensitive)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // VBScript 标识符不区分大小写
    EXPECT_EQ(executor->setVariable("MiXeD", 12.5), eNoError);
    double value = 0.0;
    EXPECT_EQ(executor->getVariable("mixed", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 12.5);
    EXPECT_EQ(executor->getVariable("MIXED", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 12.5);
}


TEST(OpenVBSExecutorTest, RepeatedAssignment)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    EXPECT_EQ(executor->setVariable("x", 1.5), eNoError);
    double number = 0.0;
    EXPECT_EQ(executor->getVariable("x", number), eNoError);
    EXPECT_DOUBLE_EQ(number, 1.5);

    // 同一变量可以切换类型
    EXPECT_EQ(executor->setVariable("x", "text"), eNoError);
    std::string text;
    EXPECT_EQ(executor->getVariable("x", text), eNoError);
    EXPECT_EQ(text, "text");

    EXPECT_EQ(executor->setVariable("x", false), eNoError);
    bool flag = true;
    EXPECT_EQ(executor->getVariable("x", flag), eNoError);
    EXPECT_FALSE(flag);
}


TEST(OpenVBSExecutorTest, ScriptVariableIsGlobal)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // Dim 声明的变量应当成为全局变量，可以在下一次 execute 中继续使用
    EXPECT_EQ(executor->execute("dim temp = 3 : dim doubled = temp * 2"), eNoError);

    double doubled = 0.0;
    EXPECT_EQ(executor->getVariable("doubled", doubled), eNoError);
    EXPECT_DOUBLE_EQ(doubled, 6.0);

    // 不写 Dim 的隐式赋值同样要保留
    EXPECT_EQ(executor->execute("implicit = doubled + 1"), eNoError);
    double implicit = 0.0;
    EXPECT_EQ(executor->getVariable("implicit", implicit), eNoError);
    EXPECT_DOUBLE_EQ(implicit, 7.0);
}


TEST(OpenVBSExecutorTest, ScriptAssignsPreexistingVariables)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    EXPECT_EQ(executor->setVariable("x", 1.0), eNoError);
    EXPECT_EQ(executor->setVariable("y", 2.0), eNoError);
    EXPECT_EQ(executor->execute("x = x + 10 : y = y * 3"), eNoError);

    double x = 0.0;
    double y = 0.0;
    EXPECT_EQ(executor->getVariable("x", x), eNoError);
    EXPECT_DOUBLE_EQ(x, 11.0);
    EXPECT_EQ(executor->getVariable("y", y), eNoError);
    EXPECT_DOUBLE_EQ(y, 6.0);
}


TEST(OpenVBSExecutorTest, ControlFlowAndFunctions)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // 循环 + 条件 + 末尾赋值，验证复合语句可以正常收尾
    const char* script =
        "dim total\n"
        "total = 0\n"
        "for i = 1 to 3\n"
        "    total = total + i\n"
        "next\n"
        "if total = 6 then\n"
        "    matched = true\n"
        "else\n"
        "    matched = false\n"
        "end if\n";
    EXPECT_EQ(executor->execute(script), eNoError);

    double total = 0.0;
    EXPECT_EQ(executor->getVariable("total", total), eNoError);
    EXPECT_DOUBLE_EQ(total, 6.0);

    bool matched = false;
    EXPECT_EQ(executor->getVariable("matched", matched), eNoError);
    EXPECT_TRUE(matched);

    // 函数定义后可以在后续 execute 中调用，并且能访问全局变量
    EXPECT_EQ(executor->execute("function scale(v)\n scale = v * total\nend function"), eNoError);
    EXPECT_EQ(executor->setVariable("factor", 2), eNoError);
    EXPECT_EQ(executor->execute("function scale2(v)\n scale2 = scale(v) * factor\nend function"), eNoError);
    EXPECT_EQ(executor->execute("dim result = scale2(3)"), eNoError);

    double result = 0.0;
    EXPECT_EQ(executor->getVariable("result", result), eNoError);
    EXPECT_DOUBLE_EQ(result, 36.0);
}


TEST(OpenVBSExecutorTest, ArraysAndStrings)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    EXPECT_EQ(executor->execute("dim arr(2)\narr(0) = 5\narr(1) = 7\narrsum = arr(0) + arr(1)"), eNoError);
    double arrsum = 0.0;
    EXPECT_EQ(executor->getVariable("arrsum", arrsum), eNoError);
    EXPECT_DOUBLE_EQ(arrsum, 12.0);

    // 中文与连接运算符
    EXPECT_EQ(executor->execute("greeting = \"你好，\" & \"世界\""), eNoError);
    std::string greeting;
    EXPECT_EQ(executor->getVariable("greeting", greeting), eNoError);
    EXPECT_EQ(greeting, "你好，世界");
}


TEST(OpenVBSExecutorTest, Evaluate)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    EXPECT_EQ(executor->setVariable("x", 10.0), eNoError);
    {
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("8.0/2.0", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isDouble());
        EXPECT_DOUBLE_EQ(result.value_->toDouble(), 4.0);
    }
    {
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("\"abc\"", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isString());
        EXPECT_EQ(result.value_->toString(), "abc");
    }
    {
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("true", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isBool());
        EXPECT_TRUE(result.value_->toBool());
    }
    {
        // VBScript 的整数保持整数类型
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("10 * 10", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isInt());
        EXPECT_EQ(result.value_->toInt(), 100);
    }
    {
        // 表达式可以引用已有的全局变量
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("x + 5", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_DOUBLE_EQ(result.value_->toDouble(), 15.0);
    }
}


TEST(OpenVBSExecutorTest, ErrorHandlingAndRecovery)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // 语法错误
    std::string error;
    EXPECT_NE(executor->execute("if then", &error), eNoError);
    EXPECT_FALSE(error.empty());
    EXPECT_FALSE(executor->getLastError().empty());

    // 发生错误后仍然可以继续执行
    error.clear();
    EXPECT_EQ(executor->execute("dim recovered = 5", &error), eNoError);
    EXPECT_TRUE(error.empty());

    double recovered = 0.0;
    EXPECT_EQ(executor->getVariable("recovered", recovered), eNoError);
    EXPECT_DOUBLE_EQ(recovered, 5.0);

    // 运行时错误（除零）
    error.clear();
    EXPECT_NE(executor->execute("dim bad = 1/0", &error), eNoError);
    EXPECT_FALSE(error.empty());

    // 错误之后的状态依然可用
    EXPECT_EQ(executor->execute("recovered = recovered + 1"), eNoError);
    EXPECT_EQ(executor->getVariable("recovered", recovered), eNoError);
    EXPECT_DOUBLE_EQ(recovered, 6.0);
}



TEST(OpenVBSExecutorTest, StrictTypeConversions)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // 字符串不能按数值读取
    EXPECT_EQ(executor->setVariable("text", "enable"), eNoError);
    double number = -1.0;
    EXPECT_NE(executor->getVariable("text", number), eNoError);
    bool flag = true;
    EXPECT_NE(executor->getVariable("text", flag), eNoError);

    // 布尔值可以按 0/1 读取
    EXPECT_EQ(executor->setVariable("enabled", true), eNoError);
    EXPECT_EQ(executor->getVariable("enabled", number), eNoError);
    EXPECT_DOUBLE_EQ(number, -1.0);

    // 不存在的变量
    EXPECT_NE(executor->getVariable("missing", number), eNoError);
}


TEST(OpenVBSExecutorTest, StringVariableConversions)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // 数值不能按字符串读取
    std::string text = "unchanged";

    EXPECT_EQ(executor->setVariable("enabled", true), eNoError);
    EXPECT_EQ(executor->getVariable("enabled", text), eNoError);
    EXPECT_EQ(text, "-1");

    EXPECT_EQ(executor->setVariable("disabled", false), eNoError);
    EXPECT_EQ(executor->getVariable("disabled", text), eNoError);
    EXPECT_EQ(text, "0");

    // 不存在的变量
    EXPECT_NE(executor->getVariable("missing", text), eNoError);
}


TEST(OpenVBSExecutorTest, IntegerAndBoolConversions)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // 布尔可以按 1/0 读取为整数
    EXPECT_EQ(executor->setVariable("yes", true), eNoError);
    int integer = 12345;
    EXPECT_EQ(executor->getVariable("yes", integer), eNoError);
    EXPECT_EQ(integer, -1);

    EXPECT_EQ(executor->setVariable("no", false), eNoError);
    EXPECT_EQ(executor->getVariable("no", integer), eNoError);
    EXPECT_EQ(integer, 0);

    // 数值按非零为真转换
    bool flag = true;
    EXPECT_EQ(executor->setVariable("zero", 0.0), eNoError);
    EXPECT_EQ(executor->getVariable("zero", flag), eNoError);
    EXPECT_FALSE(flag);

    EXPECT_EQ(executor->setVariable("nonzero", 2.5), eNoError);
    EXPECT_EQ(executor->getVariable("nonzero", flag), eNoError);
    EXPECT_TRUE(flag);

    EXPECT_EQ(executor->setVariable("nanBool", std::numeric_limits<double>::quiet_NaN()), eNoError);
    EXPECT_EQ(executor->getVariable("nanBool", flag), eNoError);
    EXPECT_TRUE(flag);

    // 字符串不能按布尔读取
    EXPECT_EQ(executor->setVariable("text", "abc"), eNoError);
    EXPECT_NE(executor->getVariable("text", flag), eNoError);
    EXPECT_NE(executor->getVariable("missingBool", flag), eNoError);
}


TEST(OpenVBSExecutorTest, EmptyVariableName)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // 空变量名对每种重载都应被拒绝
    EXPECT_NE(executor->setVariable("", "value"), eNoError);
    EXPECT_NE(executor->setVariable("", 1.5), eNoError);
    EXPECT_NE(executor->setVariable("", 2), eNoError);
    EXPECT_NE(executor->setVariable("", true), eNoError);
    EXPECT_FALSE(executor->getLastError().empty());

    // 非标识符也应当被拒绝
    EXPECT_NE(executor->setVariable("not a name", 1.0), eNoError);
}


TEST(OpenVBSExecutorTest, IsolatedInstances)
{
    std::unique_ptr<ScriptExecutor> first = newOpenVBSExecutor();
    std::unique_ptr<ScriptExecutor> second = newOpenVBSExecutor();
    if(!first || !second)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }
    if(first->initialize() != eNoError || second->initialize() != eNoError)
        GTEST_SKIP() << "OpenVBS engine initialization failed";

    EXPECT_EQ(first->setVariable("shared", 1.0), eNoError);
    EXPECT_EQ(second->setVariable("shared", 2.0), eNoError);
    EXPECT_EQ(first->execute("dim onlyFirst = 10"), eNoError);

    double value = 0.0;
    EXPECT_EQ(first->getVariable("shared", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 1.0);
    EXPECT_EQ(second->getVariable("shared", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 2.0);
    EXPECT_EQ(first->getVariable("onlyFirst", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 10.0);
    EXPECT_NE(second->getVariable("onlyFirst", value), eNoError);
}


TEST(OpenVBSExecutorTest, ExecuteBeforeInitialize)
{
    std::unique_ptr<ScriptExecutor> executor = newOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    EXPECT_NE(executor->execute("dim x = 1"), eNoError);
    EXPECT_NE(executor->evaluate("1 + 1"), eNoError);
    EXPECT_NE(executor->setVariable("x", 1.0), eNoError);
    double value = 0.0;
    EXPECT_NE(executor->getVariable("x", value), eNoError);
}


TEST(OpenVBSExecutorTest, InitializeTwiceAndFinalize)
{
    std::unique_ptr<ScriptExecutor> executor = newOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // 重复初始化不应重建全局命名空间
    EXPECT_EQ(executor->initialize(), eNoError);
    EXPECT_EQ(executor->execute("dim beforeReinit = 1"), eNoError);
    EXPECT_EQ(executor->initialize(), eNoError);

    double beforeReinit = 0.0;
    EXPECT_EQ(executor->getVariable("beforeReinit", beforeReinit), eNoError);
    EXPECT_DOUBLE_EQ(beforeReinit, 1.0);

    // 先制造一次错误，确认 finalize 会清空最近一次的错误信息
    EXPECT_NE(executor->execute("if then"), eNoError);
    EXPECT_FALSE(executor->getLastError().empty());

    // finalize 之后所有接口都应报告未初始化
    executor->finalize();
    EXPECT_TRUE(executor->getLastError().empty());

    EXPECT_NE(executor->execute("dim x = 1"), eNoError);
    EXPECT_NE(executor->evaluate("1 + 1"), eNoError);
    EXPECT_NE(executor->setVariable("x", "text"), eNoError);
    EXPECT_NE(executor->setVariable("x", 1.0), eNoError);
    EXPECT_NE(executor->setVariable("x", 1), eNoError);
    EXPECT_NE(executor->setVariable("x", true), eNoError);

    std::string text;
    double number = 0.0;
    int integer = 0;
    bool flag = false;
    EXPECT_NE(executor->getVariable("x", text), eNoError);
    EXPECT_NE(executor->getVariable("x", number), eNoError);
    EXPECT_NE(executor->getVariable("x", integer), eNoError);
    EXPECT_NE(executor->getVariable("x", flag), eNoError);

    // finalize 后可以重新初始化并继续使用
    EXPECT_EQ(executor->initialize(), eNoError);
    EXPECT_EQ(executor->execute("dim afterReinit = 2"), eNoError);
    double afterReinit = 0.0;
    EXPECT_EQ(executor->getVariable("afterReinit", afterReinit), eNoError);
    EXPECT_DOUBLE_EQ(afterReinit, 2.0);
}


TEST(OpenVBSExecutorTest, EmptyScriptAndExpression)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedOpenVBSExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }

    // 默认构造的 StringView 数据指针为空
    EXPECT_EQ(executor->execute(StringView()), eNoError);
    EXPECT_EQ(executor->execute(StringView("")), eNoError);

    ScriptResult result;
    EXPECT_EQ(executor->evaluate(StringView(), &result), eNoError);
}



TEST(OpenVBSExecutorTest, FactoryFallbacks)
{
    // 不存在的插件名返回空指针并输出错误日志
    EXPECT_EQ(aNewScriptExecutorFromPlugin("NoSuchScriptExecutor"), nullptr);

#ifndef _WIN32
    // 非 Windows 平台没有 Active Scripting，eVBScript 由 OpenVBS 插件执行
    std::unique_ptr<ScriptExecutor> probe = newOpenVBSExecutor();
    if(!probe)
    {
        GTEST_SKIP() << "OpenVBS engine is not available";
        return;
    }
    std::unique_ptr<ScriptExecutor> vbscript(aNewScriptExecutor(EScriptLanguage::eVBScript));
    ASSERT_TRUE(vbscript != nullptr);
    EXPECT_EQ(vbscript->initialize(), eNoError);
    EXPECT_EQ(vbscript->execute("dim routedToPlugin = 4"), eNoError);
    double routedToPlugin = 0.0;
    EXPECT_EQ(vbscript->getVariable("routedToPlugin", routedToPlugin), eNoError);
    EXPECT_DOUBLE_EQ(routedToPlugin, 4.0);
#endif
}


GTEST_MAIN()
