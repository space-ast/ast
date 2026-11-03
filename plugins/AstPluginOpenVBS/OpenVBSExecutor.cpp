///
/// @file      OpenVBSExecutor.cpp
/// @brief     基于 OpenVBS 的 VBScript 脚本执行器实现
/// @details   
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

#include "OpenVBSExecutor.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/Logger.hpp"

#include <limits>
#include <type_traits>

A_SUPPRESS_WARNINGS_BEGIN
#include "jujube.h"
A_SUPPRESS_WARNINGS_END

AST_NAMESPACE_BEGIN


namespace{

/// @brief 把引擎的 Error（EXCEPINFO）转成 UTF-8 文本
std::string describeError(const Error& error)
{
    std::string text;

    if (error.bstrDescription)
        text = aWideToUtf8(error.bstrDescription);
    if (text.empty() && error.bstrSource)
        text = aWideToUtf8(error.bstrSource);
    if (text.empty())
        text = "runtime error";

    // 引擎把出错行号放在 dwHelpContext 里
    if (error.dwHelpContext)
        text += " (line " + std::to_string(static_cast<unsigned long>(error.dwHelpContext)) + ")";

    return text;
}

std::string fromBSTR(BSTR bstr)
{
    return aWideToUtf8(bstr);
}

/// @brief 有符号整数转 Value
/// @details Value 只有 32 位 int，装不下的才退化为 double
template <typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
void integerToValue(T number, SharedPtr<Value>& value)
{
    if (number >= (std::numeric_limits<int>::min)()
        && number <= (std::numeric_limits<int>::max)())
        value = aNewValueInt(static_cast<int>(number));
    else
    {
        aInfo(_("整数 %lld 超出 int 范围，按 double 保存"), static_cast<long long>(number));
        value = aNewValueDouble(static_cast<double>(number));
    }
}

/// @brief 无符号整数转 Value（下界 0 天然落在 int 范围内）
template <typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
void integerToValue(T number, SharedPtr<Value>& value)
{
    if (number <= static_cast<T>((std::numeric_limits<int>::max)()))
        value = aNewValueInt(static_cast<int>(number));
    else
    {
        aInfo(_("无符号整数 %llu 超出 int 范围，按 double 保存"),
              static_cast<unsigned long long>(number));
        value = aNewValueDouble(static_cast<double>(number));
    }
}


// @note 该函数是从 ActiveScriptExecutor 复制过来的
void VariantToValue(const VARIANT& v, SharedPtr<Value>& value)
{
    switch (v.vt)
    {
    case VT_EMPTY:
        break;
    
    case VT_NULL:
        value = aValueNull();
        break;
    
    case VT_BOOL:
        value = aNewValueBool(v.boolVal != VARIANT_FALSE);
        break;
    
    case VT_I1:
        value = aNewValueInt(static_cast<int>(v.cVal));
        break;
    
    case VT_I2:
        value = aNewValueInt(static_cast<int>(v.iVal));
        break;
    
    case VT_I4:
        value = aNewValueInt(static_cast<int>(v.lVal));
        break;
    
    case VT_I8:
        // 引擎内部整数统一用 VT_I8
        integerToValue(v.llVal, value);
        break;
    
    case VT_UI1:
        value = aNewValueInt(static_cast<int>(v.bVal));
        break;
    
    case VT_UI2:
        value = aNewValueInt(static_cast<int>(v.uiVal));
        break;
    
    case VT_UI4:
        integerToValue(v.ulVal, value);
        break;

    case VT_UI8:
        integerToValue(v.ullVal, value);
        break;
    
    case VT_R4:
        value = aNewValueDouble(static_cast<double>(v.fltVal));
        break;
    
    case VT_R8:
        value = aNewValueDouble(v.dblVal);
        break;
    
    case VT_CY:
        value = aNewValueDouble(static_cast<double>(v.cyVal.int64) / 10000.0);
        break;
    
    case VT_DATE:
        aWarning(_("不支持 VT_DATE 类型"));
        // value = aValueNull();
        break;
    
    case VT_BSTR:
        value = aNewValueString(fromBSTR(v.bstrVal));
        break;
    
    case VT_DISPATCH:
        aWarning(_("不支持 VT_DISPATCH 类型"));
        // value = aValueNull();
        break;
    
    case VT_VARIANT | VT_ARRAY:
        aWarning(_("不支持 VT_ARRAY 类型"));
        // value = aValueNull();
        break;
    
    default:
    {
        VARIANT v2; 
        VariantInit(&v2);
        HRESULT hr = VariantChangeType(&v2, const_cast<VARIANTARG*>(&v), 0, VT_BSTR);
        if (SUCCEEDED(hr) && v2.vt == VT_BSTR)
        {
            value = aNewValueString(fromBSTR(v2.bstrVal));
        }
        else
        {
            aWarning(_("将变量类型 %d 转换为值失败"), v.vt);
            // value = aValueNull();
        }
        VariantClear(&v2);
        break;
    }
    }
}

}

class OpenVBSExecutor::Impl
{
public:

    Impl()
    {
        
    }

    ~Impl()
    {
        // 调用方可以不显式 finalize，析构时必须把引擎对象释放掉
        finalize();
    }

    errc_t initialize()
    {
        if(root_)
            return eNoError;
        const wchar_t* empty = L"";
        // CProcessor 只存 CProgram 的裸指针，不负责释放，得由这里持有
        program_ = new CProgram(empty);
        root_ = new CProcessor(program_, nullptr, &extension_);
        HRESULT hr = (*root_)();
        A_UNUSED(hr);
        return eNoError;
    }

    void finalize()
    {
        if(root_)
            delete root_;
        root_ = nullptr;
        // 必须在 CProcessor 析构之后再释放语法树，析构里还会用到它
        if(program_)
        {
            program_->Release();    // 引用计数归零时自销毁
            program_ = nullptr;
        }
        error_.clear();
        extension_.clear();
    }

    const std::string& error() const { return error_; }

    static std::wstring processScript(const std::wstring& source, bool isExpression)
    {
        // 引擎只处理 '\n'，这里去掉 '\r'（与 openvbs 的 IActiveScriptParse 一致）
        std::wstring code;
        code.reserve(source.size() + 1);
        for (size_t i = 0; i < source.size(); ++i)
        {
            if (source[i] != L'\r') code.push_back(source[i]);
        }
        // 补一个语句分隔符：否则最后一条语句里的算子错误会在 word_exit 的收尾刷新阶段发生，
        // 而 word_exit 随后会把 m_mode 覆盖成 clock_exit，错误既不改 HRESULT 也不填 Error。
        // 有分隔符时由 word_colon 刷新，错误能正常分派到 clock_throw_
        if(!isExpression)
        {
            if(code.empty() || code.back() != L'\n')
                code.push_back(L'\n');
        }
        return code;
    }

    /// @brief 编译并执行一段脚本（宽字符版本）
    /// @details 第一段脚本创建根 CProcessor 并执行；后续段落作为子程序追加执行，
    ///          因此前一段声明的全局变量在后续段中可见。
    errc_t run(const std::wstring& source)
    {
        if(!root_)
            return eError;
        if(source.empty())
            return eNoError;
        std::wstring code = processScript(source, false);

        const wchar_t* cursor = code.c_str();
        _prog_ptr_t program(new CProgram(cursor), false);

        size_t line = 0;
        if (!program->isReady(line))
        {
            error_ = "parse error (line " + std::to_string(line) + ")";
            return eError;
        }

        HRESULT hr = S_OK;
        
        // 追加到根处理器，operator+= 会立即执行它
        hr = (*root_ += program);

        if (FAILED(hr))
        {
            error_ = describeError(*root_->m_err);
            return eError;
        }

        error_.clear();
        return eNoError;
    }

    /// @brief 计算表达式（对应 VBScript 的 Eval）
    /// @details 直接走处理器的 DISPID_EVAL 分支：引擎把表达式解析成一个
    ///          子处理器，在当前上下文求值，再把栈顶结果交回来。
    errc_t evaluate(const std::wstring& source, VARIANT& result)
    {
        if(!root_)
            return eError;
        if(source.empty())
        {
            VariantInit(&result);
            return eNoError;
        }
        std::wstring code = processScript(source, true);
        
        _variant_t expression;
        expression.vt = VT_BSTR;
        expression.bstrVal = ::SysAllocStringLen(code.data(),
                                                 static_cast<UINT>(code.size()));
        if (!expression.bstrVal)
        {
            error_ = "out of memory";
            return eError;
        }

        DISPPARAMS params = { &expression, nullptr, 1, 0 };
        _variant_t value;       // Invoke 会把求值结果写到这里
        const HRESULT hr = root_->Invoke(DISPID_EVAL, IID_NULL, 0, DISPATCH_METHOD,
                                         &params, &value, nullptr, nullptr);
        if (FAILED(hr))
        {
            // Eval 的内部处理器不会把详细错误回传给父处理器，这里只能区分
            // 解析失败（E_INVALIDARG）和运行时失败
            error_ = (hr == E_INVALIDARG) ? "invalid expression" : "evaluation failed";
            return eError;
        }

        result = value.Detach();    // 所有权转交给调用方
        error_.clear();
        return eNoError;
    }

    /// @brief 设置宿主变量
    /// @details 变量必须是脚本全局变量：不存在就先执行一段 "Dim <name>"，
    ///          然后用 DISPATCH_PROPERTYPUT 写入。
    errc_t setVariable(const std::wstring& name, const VARIANT& value)
    {
        // 新变量先声明；已存在的名字直接写（是不是可写属性由 Invoke 判断）
        if (!declared(name) && run(L"Dim " + name) != eNoError)
            return eError;

        return putVariable(name, value);
    }

    /// @brief 读取脚本里声明的全局变量
    errc_t readVariable(const std::wstring& name, VARIANT& out)
    {
        if (!root_) return eError;

        DISPID id = DISPID_UNKNOWN;
        LPOLESTR member = const_cast<LPOLESTR>(name.c_str());
        if (FAILED(root_->GetIDsOfNames(IID_NULL, &member, 1, 0, &id)))
            return eError;

        _variant_t value;
        DISPPARAMS none = { nullptr, nullptr, 0, 0 };
        if (FAILED(root_->Invoke(id, IID_NULL, 0, DISPATCH_PROPERTYGET,
                                  &none, &value, nullptr, nullptr)))
        {
            return eError;
        }

        return SUCCEEDED(::VariantCopy(&out, &value)) ? eNoError : eError;
    }

private:
    /// @brief 通过 IDispatch 写脚本全局变量
    /// @note 依赖 vendor 的 PATCH(ast-openvbs)：PROPERTYPUT 分支遇到只解析到
    ///       Function/Sub 的名字时返回 DISP_E_MEMBERNOTFOUND，不会解引用空指针。
    errc_t putVariable(const std::wstring& name, const VARIANT& value)
    {
        if (!root_)
        {
            error_ = "variable store is not initialized";
            return eError;
        }

        DISPID id = DISPID_UNKNOWN;
        LPOLESTR member = const_cast<LPOLESTR>(name.c_str());
        if (FAILED(root_->GetIDsOfNames(IID_NULL, &member, 1, 0, &id)))
        {
            error_ = "unknown variable '" + aWideToUtf8(name.c_str()) + "'";
            return eError;
        }

        _variant_t argument;
        ::VariantCopy(&argument, const_cast<VARIANT*>(&value));

        _variant_t result;      // 属性分支会写结果指针，不能传空
        DISPPARAMS params = { &argument, nullptr, 1, 0 };
        const HRESULT hr = root_->Invoke(id, IID_NULL, 0, DISPATCH_PROPERTYPUT,
                                         &params, &result, nullptr, nullptr);
        if (FAILED(hr))
        {
            error_ = "cannot set variable '" + aWideToUtf8(name.c_str()) + "'";
            return eError;
        }

        error_.clear();
        return eNoError;
    }

    /// @brief 脚本里是否已经有这个名字（变量、函数或属性）
    bool declared(const std::wstring& name)
    {
        if (!root_) return false;

        DISPID id = DISPID_UNKNOWN;
        LPOLESTR member = const_cast<LPOLESTR>(name.c_str());
        return SUCCEEDED(root_->GetIDsOfNames(IID_NULL, &member, 1, 0, &id));
    }
private:
    std::string  error_{};          ///< 最近一次错误（UTF-8）
    CExtension   extension_{};      ///< 引擎的外部名字表
    CProcessor*  root_{nullptr};    ///< 根解释器；第一段脚本执行时创建
    CProgram*    program_{nullptr}; ///< root_ 使用的语法树，引用计数由本对象持有
};



OpenVBSExecutor::OpenVBSExecutor()
    : impl_(new Impl())
{
}

OpenVBSExecutor::~OpenVBSExecutor()
{

}

errc_t OpenVBSExecutor::initialize()
{
    return impl_->initialize();
}

void OpenVBSExecutor::finalize()
{
    impl_->finalize();
}

errc_t OpenVBSExecutor::setVariable(StringView name, double value)
{
    VARIANT v;
    ::VariantInit(&v);
    v.vt = VT_R8;
    v.dblVal = value;
    return impl_->setVariable(aUtf8ToWide(name), v);
}

errc_t OpenVBSExecutor::setVariable(StringView name, int value)
{
    VARIANT v;
    ::VariantInit(&v);
    v.vt = VT_I8;               // 引擎内部整数统一用 VT_I8
    v.llVal = static_cast<LONGLONG>(value);
    return impl_->setVariable(aUtf8ToWide(name), v);
}

errc_t OpenVBSExecutor::setVariable(StringView name, bool value)
{
    VARIANT v;
    ::VariantInit(&v);
    v.vt = VT_BOOL;
    v.boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
    return impl_->setVariable(aUtf8ToWide(name), v);
}

errc_t OpenVBSExecutor::setVariable(StringView name, StringView value)
{
    const std::wstring wide = aUtf8ToWide(value);

    VARIANT v;
    ::VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = ::SysAllocStringLen(wide.data(), static_cast<UINT>(wide.size()));

    const errc_t rc = impl_->setVariable(aUtf8ToWide(name), v);
    ::VariantClear(&v);
    return rc;
}


errc_t OpenVBSExecutor::execute(StringView script, ScriptResult* resultOut)
{
    const std::wstring source = aUtf8ToWide(script);
    errc_t rc = impl_->run(source);
    if(resultOut != nullptr)
    {
        resultOut->error_ = impl_->error();
    }
    return rc;
}

errc_t OpenVBSExecutor::evaluate(StringView script, ScriptResult* resultOut)
{
    VARIANT result;
    ::VariantInit(&result);

    const std::wstring source = aUtf8ToWide(script);
    errc_t rc = impl_->evaluate(source, result);
    if(resultOut != nullptr)
    {
        resultOut->error_ = impl_->error();
        VariantToValue(result, resultOut->value_);
    }
    return rc;
}

std::string OpenVBSExecutor::getLastError() const
{
    return impl_->error();
}

errc_t OpenVBSExecutor::getVariable(StringView name, double& value) const
{
    VARIANT v;
    ::VariantInit(&v);

    errc_t rc = eError;
    if (impl_->readVariable(aUtf8ToWide(name), v) == eNoError)
    {
        VARIANT converted;
        ::VariantInit(&converted);
        if (SUCCEEDED(::VariantChangeType(&converted, &v, 0, VT_R8)))
        {
            value = converted.dblVal;
            rc = eNoError;
        }
        ::VariantClear(&converted);
    }

    ::VariantClear(&v);
    return rc;
}

errc_t OpenVBSExecutor::getVariable(StringView name, int& value) const
{
    VARIANT v;
    ::VariantInit(&v);

    errc_t rc = eError;
    if (impl_->readVariable(aUtf8ToWide(name), v) == eNoError)
    {
        VARIANT converted;
        ::VariantInit(&converted);
        if (SUCCEEDED(::VariantChangeType(&converted, &v, 0, VT_I8)))
        {
            value = static_cast<int>(converted.llVal);
            rc = eNoError;
        }
        ::VariantClear(&converted);
    }

    ::VariantClear(&v);
    return rc;
}

errc_t OpenVBSExecutor::getVariable(StringView name, bool& value) const
{
    VARIANT v;
    ::VariantInit(&v);

    errc_t rc = eError;
    if (impl_->readVariable(aUtf8ToWide(name), v) == eNoError)
    {
        VARIANT converted;
        ::VariantInit(&converted);
        if (SUCCEEDED(::VariantChangeType(&converted, &v, 0, VT_BOOL)))
        {
            // 按 OLE 约定，VARIANT_BOOL 只要非 0 即为真。这里不能写成 `== VARIANT_TRUE`：
            // 当 vt 已是 VT_BOOL 时 VariantChangeType 是原样拷贝、不做归一化，
            // 引擎若返回非规范的 1，`== VARIANT_TRUE(-1)` 会把 true 误判成 false。
            value = (converted.boolVal != VARIANT_FALSE);
            rc = eNoError;
        }
        ::VariantClear(&converted);
    }

    ::VariantClear(&v);
    return rc;
}

errc_t OpenVBSExecutor::getVariable(StringView name, std::string& value) const
{
    VARIANT v;
    ::VariantInit(&v);

    errc_t rc = eError;
    if (impl_->readVariable(aUtf8ToWide(name), v) == eNoError)
    {
        VARIANT converted;
        ::VariantInit(&converted);
        if (SUCCEEDED(::VariantChangeType(&converted, &v, 0, VT_BSTR)))
        {
            value = aWideToUtf8(converted.bstrVal);
            rc = eNoError;
        }
        ::VariantClear(&converted);
    }

    ::VariantClear(&v);
    return rc;
}




AST_NAMESPACE_END
