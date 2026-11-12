///
/// @file      JavaScriptExecutor.cpp
/// @brief     JavaScript 脚本执行器实现
/// @details   基于 Duktape 实现 ScriptExecutor 接口
///            每个实例拥有独立的堆与全局命名空间，支持变量读写、语句执行与表达式求值
/// @author    axel
/// @date      2026-10-27
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

#include "JavaScriptExecutor.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"

#ifdef AST_WITH_DUKTAPE
// 第三方 C 头必须放在 AST_NAMESPACE_BEGIN 之前，否则符号会被包进 ast 命名空间
#include "duktape.h"
#endif

#include <cmath>
#include <limits>
#include <string>

AST_NAMESPACE_BEGIN

#ifdef AST_WITH_DUKTAPE

namespace {

/// @brief 把栈上 idx 处的值安全地转为字符串
/// @note Duktape 内部为 CESU-8；BMP 内的字符（含中文）与 UTF-8 一致，
///       非 BMP 字符不在当前支持范围内
std::string dukExecutorToString(duk_context* ctx, duk_idx_t idx)
{
    duk_size_t len = 0;
    const char* str = duk_safe_to_lstring(ctx, idx, &len);
    std::string result;
    if(str)
        result.assign(str, len);
    return result;
}

/// @brief 用于在受保护调用中读取错误对象的 stack 属性
/// @details 属性读取可能触发 getter 或 Proxy trap 并抛错，
//  直接读取会让错误进入 fatal handler；返回值类型不定，由调用方判断是否为字符串
duk_ret_t dukExecutorReadStack(duk_context* ctx, void* /*udata*/)
{
    // 实参位于索引 0，即待检查的错误对象
    duk_get_prop_string(ctx, 0, "stack");
    return 1;
}

/// @brief 取出栈顶的异常对象（消息 + stack）并弹出
std::string dukExecutorTakeError(duk_context* ctx)
{
    std::string stack;
    // duk_safe_to_lstring 会就地替换栈上的值，必须先取出 stack 再转换错误对象
    if(duk_is_object(ctx, -1))
    {
        duk_dup(ctx, -1); // 保留原始错误值，safe_call 会消费实参
        
        /*
        duk_safe_call(ctx, func, udata, nargs=1, nrets=1)
        用 safe_call 隔离属性读取：
        getter 或 Proxy trap 抛错时只丢弃这次读取，原始错误值保持不变，后续照常转换
        nargs = nrets = 1 时无论成功还是失败，栈顶都恰好是 stack 值或捕获到的错误
        调用前:  [ ..., error ]                      ← error 在 -1
        dup 后:  [ ..., error, error ]               ← 实参拷贝
        safe_call(nargs=1, nrets=1):
            成功:  [ ..., error, stackString ]       ← func return 1 的值
            失败:  [ ..., error, thrownValue ]       ← catch 到的错误
        pop 后:  [ ..., error ]                      ← 原始错误仍在 -1，供后面转换
        */
        if(duk_safe_call(ctx, dukExecutorReadStack, nullptr, 1, 1) == 0
           && duk_is_string(ctx, -1))
        {
            stack = dukExecutorToString(ctx, -1);
        }
        duk_pop(ctx);
    }
    std::string message = dukExecutorToString(ctx, -1);
    duk_pop(ctx);

    // 没有可附加的 stack 时只返回消息
    if(stack.empty())
        return message;
    // Duktape 默认的 stack 首行就是 String(error)，此时 stack 已包含完整消息，直接用
    else if(StringView(stack).starts_with(message))
        return stack;
    // 自定义 stack 不含消息时，消息和 stack 都要保留
    return message + "\n" + stack;
}

/// @brief 把栈上 idx 处的值转换为 Value
SharedPtr<Value> dukExecutorToValue(duk_context* ctx, duk_idx_t idx)
{
    if(duk_is_number(ctx, idx))
        return aNewValueDouble(duk_get_number(ctx, idx));
    if(duk_is_boolean(ctx, idx))
        return aNewValueBool(duk_get_boolean(ctx, idx) != 0);
    if(duk_is_string(ctx, idx))
    {
        duk_size_t len = 0;
        const char* str = duk_get_lstring(ctx, idx, &len);
        std::string text;
        if(str)
            text.assign(str, len);
        return aNewValueString(text);
    }
    if(duk_is_null_or_undefined(ctx, idx))
        return aValueNull();
    return aNewValueString(dukExecutorToString(ctx, idx));
}

} // namespace

/// @brief JavaScriptExecutor 的内部实现
/// @note 每个实例持有独立的 Duktape 堆；同一个堆不能在多个线程中并发使用
class JavaScriptExecutor::Impl
{
public:
    ~Impl() { finalize(); }

    void finalize()
    {
        if(ctx)
        {
            duk_destroy_heap(ctx);
            ctx = nullptr;
        }
    }

    duk_context* ctx{nullptr};
};


JavaScriptExecutor::JavaScriptExecutor()
    : impl_(new Impl())
{
}

JavaScriptExecutor::~JavaScriptExecutor() = default;


errc_t JavaScriptExecutor::initialize()
{
    if(impl_->ctx)
        return eNoError;
    impl_->ctx = duk_create_heap_default();
    if(!impl_->ctx)
    {
        lastError_ = _("JavaScript 引擎上下文创建失败");
        return eError;
    }
    return eNoError;
}


void JavaScriptExecutor::finalize()
{
    impl_->finalize();
    lastError_.clear();
}


std::string JavaScriptExecutor::getLastError() const
{
    return lastError_;
}


errc_t JavaScriptExecutor::execute(StringView script, ScriptResult* resultOut)
{
    lastError_.clear();
    if(resultOut)
        resultOut->error_.clear();

    duk_context* ctx = impl_->ctx;
    if(!ctx)
    {
        lastError_ = _("JavaScript 执行器未初始化");
        if(resultOut)
            resultOut->error_ = lastError_;
        return eErrorNotInit;
    }

    const char* src = script.data() ? script.data() : "";
    // 按 program 编译，保证脚本顶层的 var 声明成为全局变量（与 JScript 一致）
    duk_int_t rc = duk_pcompile_lstring(ctx, 0, src, (duk_size_t)script.size());
    if(rc == 0)
        rc = duk_pcall(ctx, 0);
    if(rc != 0)
    {
        lastError_ = dukExecutorTakeError(ctx);
        if(resultOut)
            resultOut->error_ = lastError_;
        return eError;
    }
    duk_pop(ctx); // 执行结果
    return eNoError;
}


errc_t JavaScriptExecutor::evaluate(StringView expression, ScriptResult* resultOut)
{
    lastError_.clear();
    if(resultOut)
        resultOut->error_.clear();

    duk_context* ctx = impl_->ctx;
    if(!ctx)
    {
        lastError_ = _("JavaScript 执行器未初始化");
        if(resultOut)
            resultOut->error_ = lastError_;
        return eErrorNotInit;
    }

    const char* src = expression.data() ? expression.data() : "";
    if(duk_peval_lstring(ctx, src, (duk_size_t)expression.size()) != 0)
    {
        lastError_ = dukExecutorTakeError(ctx);
        if(resultOut)
            resultOut->error_ = lastError_;
        return eError;
    }
    if(resultOut)
        resultOut->value_ = dukExecutorToValue(ctx, -1);
    duk_pop(ctx);
    return eNoError;
}


errc_t JavaScriptExecutor::setVariable(StringView name, StringView value)
{
    duk_context* ctx = impl_->ctx;
    if(!ctx)
        return eErrorNotInit;
    if(name.empty())
    {
        lastError_ = _("变量名不能为空");
        return eErrorInvalidParam;
    }
    std::string key(name);
    duk_push_global_object(ctx);
    duk_push_string(ctx, key.c_str());
    duk_push_lstring(ctx, value.data() ? value.data() : "", (duk_size_t)value.size());
    duk_put_prop(ctx, -3);
    duk_pop(ctx);
    return eNoError;
}


errc_t JavaScriptExecutor::setVariable(StringView name, double value)
{
    duk_context* ctx = impl_->ctx;
    if(!ctx)
        return eErrorNotInit;
    if(name.empty())
    {
        lastError_ = _("变量名不能为空");
        return eErrorInvalidParam;
    }
    std::string key(name);
    duk_push_global_object(ctx);
    duk_push_string(ctx, key.c_str());
    duk_push_number(ctx, value);
    duk_put_prop(ctx, -3);
    duk_pop(ctx);
    return eNoError;
}


errc_t JavaScriptExecutor::setVariable(StringView name, int value)
{
    return setVariable(name, static_cast<double>(value));
}


errc_t JavaScriptExecutor::setVariable(StringView name, bool value)
{
    duk_context* ctx = impl_->ctx;
    if(!ctx)
        return eErrorNotInit;
    if(name.empty())
    {
        lastError_ = _("变量名不能为空");
        return eErrorInvalidParam;
    }
    std::string key(name);
    duk_push_global_object(ctx);
    duk_push_string(ctx, key.c_str());
    duk_push_boolean(ctx, value ? 1 : 0);
    duk_put_prop(ctx, -3);
    duk_pop(ctx);
    return eNoError;
}


errc_t JavaScriptExecutor::getVariable(StringView name, std::string& value) const
{
    duk_context* ctx = impl_->ctx;
    if(!ctx)
        return eErrorNotInit;
    std::string key(name);
    if(!duk_get_global_string(ctx, key.c_str()))
    {
        duk_pop(ctx); // 未命中时压入的 undefined
        return eErrorNotFound;
    }
    errc_t rc = eErrorInvalidType;
    if(duk_is_string(ctx, -1))
    {
        duk_size_t len = 0;
        const char* str = duk_get_lstring(ctx, -1, &len);
        if(str)
            value.assign(str, len);
        else
            value.clear();
        rc = eNoError;
    }
    else if(duk_is_boolean(ctx, -1))
    {
        value = duk_get_boolean(ctx, -1) ? "true" : "false";
        rc = eNoError;
    }
    duk_pop(ctx);
    return rc;
}


errc_t JavaScriptExecutor::getVariable(StringView name, double& value) const
{
    duk_context* ctx = impl_->ctx;
    if(!ctx)
        return eErrorNotInit;
    std::string key(name);
    if(!duk_get_global_string(ctx, key.c_str()))
    {
        duk_pop(ctx);
        return eErrorNotFound;
    }
    errc_t rc = eErrorInvalidType;
    if(duk_is_number(ctx, -1))
    {
        value = duk_get_number(ctx, -1);
        rc = eNoError;
    }
    else if(duk_is_boolean(ctx, -1))
    {
        value = duk_get_boolean(ctx, -1) ? 1.0 : 0.0;
        rc = eNoError;
    }
    duk_pop(ctx);
    return rc;
}


errc_t JavaScriptExecutor::getVariable(StringView name, int& value) const
{
    duk_context* ctx = impl_->ctx;
    if(!ctx)
        return eErrorNotInit;
    std::string key(name);
    if(!duk_get_global_string(ctx, key.c_str()))
    {
        duk_pop(ctx);
        return eErrorNotFound;
    }
    errc_t rc = eErrorInvalidType;
    if(duk_is_number(ctx, -1))
    {
        double number = duk_get_number(ctx, -1);
        // 加括号防止 Windows.h 的 min/max 宏展开
        if(std::isfinite(number)
           && number >= static_cast<double>((std::numeric_limits<int>::min)())
           && number <= static_cast<double>((std::numeric_limits<int>::max)()))
        {
            value = static_cast<int>(number);
            rc = eNoError;
        }
        else
        {
            rc = eErrorOutOfRange;
        }
    }
    else if(duk_is_boolean(ctx, -1))
    {
        value = duk_get_boolean(ctx, -1) ? 1 : 0;
        rc = eNoError;
    }
    duk_pop(ctx);
    return rc;
}


errc_t JavaScriptExecutor::getVariable(StringView name, bool& value) const
{
    duk_context* ctx = impl_->ctx;
    if(!ctx)
        return eErrorNotInit;
    std::string key(name);
    if(!duk_get_global_string(ctx, key.c_str()))
    {
        duk_pop(ctx);
        return eErrorNotFound;
    }
    errc_t rc = eErrorInvalidType;
    if(duk_is_boolean(ctx, -1))
    {
        value = duk_get_boolean(ctx, -1) != 0;
        rc = eNoError;
    }
    else if(duk_is_number(ctx, -1))
    {
        // 与 JavaScript 的真值语义保持一致：0 与 NaN 为假
        double number = duk_get_number(ctx, -1);
        value = (number != 0.0 && !std::isnan(number));
        rc = eNoError;
    }
    duk_pop(ctx);
    return rc;
}

#else // AST_WITH_DUKTAPE

namespace {

errc_t dukExecutorUnavailable(std::string& lastError, ScriptResult* resultOut)
{
    lastError = _("编译时未启用 JavaScript 引擎 Duktape");
    if(resultOut)
        resultOut->error_ = lastError;
    return eErrorUnsupported;
}

} // namespace

class JavaScriptExecutor::Impl
{
};

JavaScriptExecutor::JavaScriptExecutor()
    : impl_(new Impl())
{
}

JavaScriptExecutor::~JavaScriptExecutor() = default;


errc_t JavaScriptExecutor::initialize()
{
    return dukExecutorUnavailable(lastError_, nullptr);
}


void JavaScriptExecutor::finalize()
{
    lastError_.clear();
}


std::string JavaScriptExecutor::getLastError() const
{
    return lastError_;
}


errc_t JavaScriptExecutor::execute(StringView, ScriptResult* resultOut)
{
    return dukExecutorUnavailable(lastError_, resultOut);
}


errc_t JavaScriptExecutor::evaluate(StringView, ScriptResult* resultOut)
{
    return dukExecutorUnavailable(lastError_, resultOut);
}


errc_t JavaScriptExecutor::setVariable(StringView, StringView)
{
    return dukExecutorUnavailable(lastError_, nullptr);
}


errc_t JavaScriptExecutor::setVariable(StringView, double)
{
    return dukExecutorUnavailable(lastError_, nullptr);
}


errc_t JavaScriptExecutor::setVariable(StringView, int)
{
    return dukExecutorUnavailable(lastError_, nullptr);
}


errc_t JavaScriptExecutor::setVariable(StringView, bool)
{
    return dukExecutorUnavailable(lastError_, nullptr);
}


errc_t JavaScriptExecutor::getVariable(StringView, std::string&) const
{
    return eErrorUnsupported;
}


errc_t JavaScriptExecutor::getVariable(StringView, double&) const
{
    return eErrorUnsupported;
}


errc_t JavaScriptExecutor::getVariable(StringView, int&) const
{
    return eErrorUnsupported;
}


errc_t JavaScriptExecutor::getVariable(StringView, bool&) const
{
    return eErrorUnsupported;
}

#endif // AST_WITH_DUKTAPE

AST_NAMESPACE_END
