///
/// @file      ActiveScript.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-29
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

#include "ActiveScriptExecutor.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "VBScriptExecutor.hpp"

#if  !defined _WIN32

AST_NAMESPACE_BEGIN

class ActiveScriptExecutor::Impl{};
ActiveScriptExecutor::ActiveScriptExecutor(){aError("ActiveScriptExecutor not supported on this platform");}
ActiveScriptExecutor::ActiveScriptExecutor(const wchar_t* progId){}
ActiveScriptExecutor::~ActiveScriptExecutor() = default;
errc_t ActiveScriptExecutor::initialize() {return -1;};
void ActiveScriptExecutor::finalize() {}
errc_t ActiveScriptExecutor::execute(StringView script, ScriptResult* resultOut) {
    if (resultOut) 
        resultOut->error_ = ActiveScriptExecutor::getLastError();
    return -1;
};
errc_t ActiveScriptExecutor::evaluate(StringView expression, ScriptResult* resultOut) 
{
    return ActiveScriptExecutor::execute(expression, resultOut);
}
std::string ActiveScriptExecutor::getLastError() const {return "ScriptExecutor is not supported on this platform";}
errc_t ActiveScriptExecutor::setVariable(StringView name, StringView value) {return -1;};
errc_t ActiveScriptExecutor::setVariable(StringView name, double value) {return -1;};
errc_t ActiveScriptExecutor::setVariable(StringView name, int value) {return -1;};
errc_t ActiveScriptExecutor::setVariable(StringView name, bool value) {return -1;};
errc_t ActiveScriptExecutor::getVariable(StringView name, std::string& value) const {return -1;};
errc_t ActiveScriptExecutor::getVariable(StringView name, double& value) const {return -1;};
errc_t ActiveScriptExecutor::getVariable(StringView name, int& value) const {return -1;};
errc_t ActiveScriptExecutor::getVariable(StringView name, bool& value) const {return -1;};

AST_NAMESPACE_END

#else

#include "AstUtil/Encode.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstCOM/COMAPI.hpp"
#include "ActiveScriptGlobalFunctions.inl"
#include <comdef.h>
#include <unordered_map>
#include <Windows.h>
#include <activscp.h>
#include <dispex.h>   // IDispatchEx 定义
#include <string>
#include <vector>

// #pragma comment(lib, "oleaut32.lib")
// #pragma comment(lib, "ole32.lib")
// #pragma comment(lib, "uuid.lib")

// #define AST_DEBUG_SCRIPT_EXECUTOR

#define ERR_FAIL -1
#define ERR_OK 0

// 命名空间内的辅助函数声明
namespace {

// 将 BSTR 安全地转为 UTF-8 字符串
std::string fromBSTR(BSTR bstr);
// 从 IDispatch 读取指定属性
bool getScriptVariable(IDispatch* pDisp, const std::wstring& name, VARIANT& result);
// 向 IDispatch 写入指定属性
bool setScriptVariable(IDispatch* pDisp, const std::wstring& name, const VARIANT& value);
// 使用 IDispatchEx 设置全局变量，若不存在会自动创建
bool setScriptVariableByEx(IDispatch* pGlobalDisp, const std::wstring& name, const VARIANT& value);
// 获取根对象的Dispatch接口
IUnknown* rootDispatch();
}

#define _AST_ACTIVE_SCRIPT_NOT_INITIALIZED "script executor is not initialized."

constexpr const wchar_t* kRootItemName = L"root";   ///< 根命名项名称
constexpr const wchar_t* kGlobalFunctionsItemName = L"GlobalFunctions"; ///< 命名项名称

AST_NAMESPACE_BEGIN



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
        value = aNewValueBool(v.boolVal != 0);
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
        value = aNewValueDouble(static_cast<double>(v.llVal));
        break;
    
    case VT_UI1:
        value = aNewValueInt(static_cast<int>(v.bVal));
        break;
    
    case VT_UI2:
        value = aNewValueInt(static_cast<int>(v.uiVal));
        break;
    
    case VT_UI4:
        value = aNewValueDouble(static_cast<double>(v.ulVal));
        break;
    
    case VT_UI8:
        value = aNewValueDouble(static_cast<double>(v.ullVal));
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
        aWarning("VT_DATE type not supported in VariantToValue");
        // value = aValueNull();
        break;
    
    case VT_BSTR:
        value = aNewValueString(fromBSTR(v.bstrVal));
        break;
    
    case VT_DISPATCH:
        aWarning("VT_DISPATCH type not supported in VariantToValue");
        // value = aValueNull();
        break;
    
    case VT_VARIANT | VT_ARRAY:
        aWarning("VT_ARRAY type not supported in VariantToValue");
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
            aWarning("failed to convert variant type %d to value", v.vt);
            // value = aValueNull();
        }
        VariantClear(&v2);
        break;
    }
    }
}



// 站点实现（简化版，仅实现IActiveScriptSite的部分方法）

class SimpleActiveScriptSite final: public IActiveScriptSite
{
public:
    SimpleActiveScriptSite()
    {
        globalFunctions_ = new ActiveScriptGlobalFunctions();
        // globalFunctions_->AddRef(); // 已在ActiveScriptGlobalFunctions构造函数中设置引用计数为1
    }
    ~SimpleActiveScriptSite()
    {
        if(globalFunctions_)
        {
            globalFunctions_->Release();
            globalFunctions_ = nullptr;
        }
    }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override
    {
        if (riid == IID_IUnknown || riid == IID_IActiveScriptSite)
        {
            *ppv = static_cast<IActiveScriptSite*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&ref_); }
    STDMETHODIMP_(ULONG) Release() override
    {
        LONG r = InterlockedDecrement(&ref_);
        if (r == 0) delete this;
        return r;
    }
    // IActiveScriptSite
    STDMETHODIMP GetLCID(LCID* plcid) override { *plcid = LOCALE_USER_DEFAULT; return S_OK; }
    STDMETHODIMP GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask,
                             IUnknown** ppunkItem, ITypeInfo** ppti) override
    {
        if (ppunkItem == nullptr) return E_POINTER;
        if (ppti) *ppti = nullptr;
        if (wcscmp(pstrName, kRootItemName) == 0)
        {
            if(auto rootDisp = rootDispatch())
            {
                rootDisp->AddRef();
                *ppunkItem = rootDisp;
                return S_OK;
            }
        }
        else if (wcscmp(pstrName, kGlobalFunctionsItemName) == 0)
        {
            globalFunctions_->AddRef();
            *ppunkItem = globalFunctions_;
            return S_OK;
        }
        // 未找到命名项，返回未找到
        *ppunkItem = nullptr;
        return TYPE_E_ELEMENTNOTFOUND;
    }
    STDMETHODIMP GetDocVersionString(BSTR* pbstrVersion) override
    {
        *pbstrVersion = SysAllocString(L"1.0");
        return S_OK;
    }
    STDMETHODIMP OnScriptTerminate(const VARIANT*, const EXCEPINFO*) override { return S_OK; }
    STDMETHODIMP OnStateChange(SCRIPTSTATE) override { return S_OK; }
    STDMETHODIMP OnScriptError(IActiveScriptError* pscripterror) override
    {
        EXCEPINFO ei = {};
        pscripterror->GetExceptionInfo(&ei);
        if (ei.bstrSource)
        {
            lastError_ = fromBSTR(ei.bstrSource);
            if(ei.bstrDescription)
                lastError_ += " - " + fromBSTR(ei.bstrDescription);
            if(ei.bstrHelpFile)
                lastError_ += " - " + fromBSTR(ei.bstrHelpFile);
        }
        else
        {
            lastError_ = "Script error occurred.";
        }
        aError("%s", lastError_.c_str());
        return S_OK;
    }
    STDMETHODIMP OnEnterScript() override { return S_OK; }
    STDMETHODIMP OnLeaveScript() override { return S_OK; }

    const std::string& lastError() const {return lastError_;}
    void clearLastError() {lastError_.clear();}
    bool hasError() const {return !lastError_.empty();}
private:
    LONG ref_ = 1;
    std::string lastError_;
    ActiveScriptGlobalFunctions* globalFunctions_ = nullptr;
};



/// COM 初始化和释放守卫
class CoInitializeGuard
{
public:
    CoInitializeGuard(COINIT initFlags) {hr_ = CoInitializeEx(nullptr, initFlags);}
    ~CoInitializeGuard() {if (SUCCEEDED(hr_)) CoUninitialize();}
    HRESULT hr_;
};

/// 确保当前线程 COM 已初始化
HRESULT aEnsureCoInitialized()
{
    static A_THREAD_LOCAL CoInitializeGuard guard(COINIT_APARTMENTTHREADED);
    return guard.hr_;
}

//内部实现

class ActiveScriptExecutor::Impl
{
public:
    IActiveScript*              pScript = nullptr;          ///< 脚本引擎接口
    IActiveScriptParse*         pParse = nullptr;           ///< 解析器接口
    IDispatch*                  pGlobal = nullptr;          ///< 脚本全局对象的 IDispatch
    SimpleActiveScriptSite*     pSite = nullptr;            ///< 脚本站点
    std::wstring                progId = L"JScript";        ///< 默认脚本引擎
    
public:
    Impl() = default;
    
    ~Impl()
    {
        finalize();
    }

    errc_t initialize()
    {
        // 避免重复初始化
        if(pScript) return ERR_OK;

        // 初始化 COM
        HRESULT hr = aEnsureCoInitialized();
        if (FAILED(hr)) return ERR_FAIL;

        // 创建脚本引擎
        CLSID clsid;
        hr = CLSIDFromProgID(progId.c_str(), &clsid);
        if (FAILED(hr)) return ERR_FAIL;

        hr = CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER,
                            IID_IActiveScript, (void**)&pScript);
        if (FAILED(hr)) return ERR_FAIL;

        // 获取解析接口
        hr = pScript->QueryInterface(IID_IActiveScriptParse, (void**)&pParse);
        if (FAILED(hr)) return ERR_FAIL;

        // 创建并设置站点
        pSite = new SimpleActiveScriptSite();
        // pSite->AddRef(); // 创建时已经在构造函数里将引用计数设置为 1，这里不需要再增加
        hr = pScript->SetScriptSite(pSite);
        if (FAILED(hr)) return ERR_FAIL;

        // 初始化解析器
        hr = pParse->InitNew();
        if (FAILED(hr)) return ERR_FAIL;

        // 添加全局函数提供者
        pScript->AddNamedItem(kGlobalFunctionsItemName, SCRIPTITEM_GLOBALMEMBERS | SCRIPTITEM_ISVISIBLE);
        
        // 添加根命名项
        if(rootDispatch())
        {
            pScript->AddNamedItem(kRootItemName, SCRIPTITEM_ISVISIBLE);
        }
        else
        {
            aWarning("failed to add item `root` for script.");
        }

        // 连接到执行状态（第一次需要调用，后续保持在 CONNECTED 即可）
        SCRIPTSTATE state;
        pScript->GetScriptState(&state);
        if (state != SCRIPTSTATE_CONNECTED)
        {
            hr = pScript->SetScriptState(SCRIPTSTATE_CONNECTED);
            if (FAILED(hr)) 
            {
                aError("Failed to set script state to connected.");
                return ERR_FAIL;
            }
        }

        // 获取全局 IDispatch（此时为空，但可用于预先设置变量）
        hr = pScript->GetScriptDispatch(nullptr, &pGlobal);
        if (FAILED(hr)) pGlobal = nullptr; // 非致命
        return ERR_OK;
    }

    void finalize()
    {
        if (pGlobal) { pGlobal->Release(); pGlobal = nullptr; }
        if (pParse)  { pParse->Release();  pParse = nullptr; }
        if (pScript) { pScript->Close();   pScript->Release(); pScript = nullptr; }
        if (pSite)   { pSite->Release();   pSite = nullptr; }
    }

    std::string getLastError() const
    {
        if(pSite)
            return pSite->lastError();
        return _AST_ACTIVE_SCRIPT_NOT_INITIALIZED;
    }

    errc_t run(StringView script, bool isExpression, ScriptResult* resultOut=nullptr)
    {
        // 伪循环用于执行正常逻辑，如果中途有失败则break跳出循环，在末尾进行错误处理
        do{
            if (!pParse || !pSite)
                break;
            // 清除上一次的错误信息
            pSite->clearLastError();

            // 注意：若想每次执行前重新初始化全局变量环境，需额外处理。
            // 此处假设脚本可累积执行，即引擎状态保持。
            std::wstring wscript = aUtf8ToWide(script);

            #ifdef AST_DEBUG_SCRIPT_EXECUTOR
            aInfo("executing script: \n%.*s", (int)script.size(), script.data());
            #endif

            EXCEPINFO ei = {};
            VARIANT v; VariantInit(&v);
            DWORD flags = SCRIPTTEXT_ISVISIBLE;
            if(isExpression)
                flags |= SCRIPTTEXT_ISEXPRESSION;

            HRESULT hr = pParse->ParseScriptText(
                wscript.c_str(),
                nullptr,               // 项名
                nullptr,               // 宿主对象
                nullptr,               // 分隔符
                0,                     // 行号
                0,                     // 偏移
                flags,
                &v,               // 返回结果
                &ei);
            if(resultOut)
                VariantToValue(v, resultOut->value_);
            VariantClear(&v);
            if (FAILED(hr))  break;
            
            if (pSite->hasError())
                break;
            return ERR_OK;
        }while(false);
        if(resultOut)
            resultOut->error_ = getLastError();
        return ERR_FAIL;
    }

};

// ---------- ActiveScriptExecutor 公共接口实现 ----------

ActiveScriptExecutor::ActiveScriptExecutor()
    : impl_(new Impl())
{
}

ActiveScriptExecutor::ActiveScriptExecutor(const wchar_t* progId)
    : impl_(new Impl())
{
    if (progId)
        impl_->progId = progId;
}

ActiveScriptExecutor::~ActiveScriptExecutor() = default;

void ActiveScriptExecutor::setProgID(const wchar_t* progId)
{
    if (impl_ && progId)
        impl_->progId = progId;
}

errc_t ActiveScriptExecutor::initialize()
{
    if (!impl_) return ERR_FAIL;
    return impl_->initialize();
}

void ActiveScriptExecutor::finalize()
{
    if (!impl_) return;
    impl_->finalize();
}


errc_t ActiveScriptExecutor::execute(StringView script, ScriptResult* resultOut)
{
    if(!impl_) return eErrorNotInit;
    return impl_->run(script, false, resultOut);
}


errc_t ActiveScriptExecutor::evaluate(StringView expression, ScriptResult* resultOut)
{
    if(!impl_) return eErrorNotInit;
    return impl_->run(expression, true, resultOut);
}


std::string ActiveScriptExecutor::getLastError() const
{
    if (!impl_ || !impl_->pParse) 
        return _AST_ACTIVE_SCRIPT_NOT_INITIALIZED;
    return impl_->getLastError();
}

// ---------- setVariable 重载 ----------
errc_t ActiveScriptExecutor::setVariable(StringView name, StringView value)
{
    if (!impl_ || !impl_->pGlobal) return ERR_FAIL;
    std::wstring wname = aUtf8ToWide(name);
    std::wstring wval  = aUtf8ToWide(value);
    VARIANT v; VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(wval.c_str());
    bool ok = setScriptVariableByEx(impl_->pGlobal, wname, v);
    VariantClear(&v);
    return ok ? ERR_OK : ERR_FAIL;
}

errc_t ActiveScriptExecutor::setVariable(StringView name, double value)
{
    if (!impl_ || !impl_->pGlobal) return ERR_FAIL;
    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    v.vt = VT_R8;
    v.dblVal = value;
    bool ok = setScriptVariableByEx(impl_->pGlobal, wname, v);
    return ok ? ERR_OK : ERR_FAIL;
}

errc_t ActiveScriptExecutor::setVariable(StringView name, int value)
{
    if (!impl_ || !impl_->pGlobal) return ERR_FAIL;
    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = value;
    bool ok = setScriptVariableByEx(impl_->pGlobal, wname, v);
    return ok ? ERR_OK : ERR_FAIL;
}

errc_t ActiveScriptExecutor::setVariable(StringView name, bool value)
{
    if (!impl_ || !impl_->pGlobal) return ERR_FAIL;
    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    v.vt = VT_BOOL;
    v.boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
    bool ok = setScriptVariableByEx(impl_->pGlobal, wname, v);
    return ok ? ERR_OK : ERR_FAIL;
}

static errc_t setVBVariableByLiteral(VBScriptExecutor& exec, StringView name, StringView literal)
{
    std::string varname = std::string(name);
    std::string cmd = "Dim " + varname + "\n" + varname + " = " + std::string(literal) + "\n";
    return exec.execute(cmd);
}


errc_t VBScriptExecutor::setVariable(StringView name, StringView value)
{
    errc_t rc = execute("Dim " + std::string(name) + "\n");
    if(rc != ERR_OK)
        return rc;
    // 字符串在使用Dim声明后，使用setScriptVariable设置值，避免字符串转义等复杂问题
    std::wstring wname = aUtf8ToWide(name);
    std::wstring wval  = aUtf8ToWide(value);
    VARIANT v; VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(wval.c_str());
    bool ok = setScriptVariable(impl_->pGlobal, wname, v);
    VariantClear(&v);
    return ok ? ERR_OK : ERR_FAIL;
}


errc_t VBScriptExecutor::setVariable(StringView name, double value)
{
    errc_t rc = execute("Dim " + std::string(name) + "\n");
    if(rc != ERR_OK)
        return rc;
    // 浮点数在使用Dim声明后，使用setScriptVariable设置值，避免nan、inf等边界问题
    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    v.vt = VT_R8;
    v.dblVal = value;
    bool ok = setScriptVariable(impl_->pGlobal, wname, v);
    return ok ? ERR_OK : ERR_FAIL;    
}

errc_t VBScriptExecutor::setVariable(StringView name, int value)
{
    return setVBVariableByLiteral(*this, name, aFormatInt(value));
}
errc_t VBScriptExecutor::setVariable(StringView name, bool value)
{
    const char* boolLiteral = value ? "True" : "False";
    return setVBVariableByLiteral(*this, name, boolLiteral);
}

// ---------- getVariable 重载 ----------
errc_t ActiveScriptExecutor::getVariable(StringView name, std::string& value) const
{
    if (!impl_ || !impl_->pGlobal) return ERR_FAIL;
    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    if (!getScriptVariable(impl_->pGlobal, wname, v))
        return ERR_FAIL;
    if (v.vt == VT_BSTR)
        value = fromBSTR(v.bstrVal);
    else
    {
        VariantChangeType(&v, &v, 0, VT_BSTR); // 尝试转换
        if (v.vt == VT_BSTR)
            value = fromBSTR(v.bstrVal);
        else
        {
            VariantClear(&v);
            return ERR_FAIL;
        }
    }
    VariantClear(&v);
    return ERR_OK;
}

errc_t ActiveScriptExecutor::getVariable(StringView name, double& value) const
{
    if (!impl_ || !impl_->pGlobal) return ERR_FAIL;
    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    if (!getScriptVariable(impl_->pGlobal, wname, v))
        return ERR_FAIL;
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_R8);
    if (FAILED(hr)) { VariantClear(&v); return ERR_FAIL; }
    value = v.dblVal;
    VariantClear(&v);
    return ERR_OK;
}

errc_t ActiveScriptExecutor::getVariable(StringView name, int& value) const
{
    if (!impl_ || !impl_->pGlobal) return ERR_FAIL;
    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    if (!getScriptVariable(impl_->pGlobal, wname, v))
        return ERR_FAIL;
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_I4);
    if (FAILED(hr)) { VariantClear(&v); return ERR_FAIL; }
    value = v.lVal;
    VariantClear(&v);
    return ERR_OK;
}

errc_t ActiveScriptExecutor::getVariable(StringView name, bool& value) const
{
    if (!impl_ || !impl_->pGlobal) return ERR_FAIL;
    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    if (!getScriptVariable(impl_->pGlobal, wname, v))
        return ERR_FAIL;
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_BOOL);
    if (FAILED(hr)) { VariantClear(&v); return ERR_FAIL; }
    value = (v.boolVal == VARIANT_TRUE);
    VariantClear(&v);
    return ERR_OK;
}

AST_NAMESPACE_END

// ---------- 匿名空间辅助函数实现 ----------
namespace {



std::string fromBSTR(BSTR bstr)
{
    if (!bstr) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, bstr, -1, nullptr, 0, nullptr, nullptr);
    if (len <= 0) return {};
    std::string s(len - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, bstr, -1, &s[0], len, nullptr, nullptr);
    return s;
}

bool getScriptVariable(IDispatch* pDisp, const std::wstring& name, VARIANT& result)
{
    DISPID dispid;
    const wchar_t* wname = name.c_str();
    HRESULT hr = pDisp->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&wname), 1,
                                      LOCALE_USER_DEFAULT, &dispid);
    if (FAILED(hr)) return false;

    DISPPARAMS params = { nullptr, nullptr, 0, 0 };
    hr = pDisp->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT,
                       DISPATCH_PROPERTYGET, &params, &result, nullptr, nullptr);
    return SUCCEEDED(hr);
}

bool setScriptVariable(IDispatch* pDisp, const std::wstring& name, const VARIANT& value)
{
    DISPID dispid;
    const wchar_t* wname = name.c_str();
    HRESULT hr = pDisp->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&wname), 1,
                                      LOCALE_USER_DEFAULT, &dispid);
    if (FAILED(hr)) return false;

    // 对于属性赋值，需要将 value 放入参数数组，并标记命名参数 DISPID_PROPERTYPUT
    VARIANT v;
    VariantInit(&v);
    hr = VariantCopyInd(&v, const_cast<VARIANTARG*>(&value));
    if (FAILED(hr)) return false;

    DISPID dispidNamed = DISPID_PROPERTYPUT;
    DISPPARAMS params = { &v, &dispidNamed, 1, 1 };
    hr = pDisp->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT,
                       DISPATCH_PROPERTYPUT, &params, nullptr, nullptr, nullptr);
    VariantClear(&v);
    return SUCCEEDED(hr);
}


// 使用 IDispatchEx 设置全局变量，若不存在会自动创建
bool setScriptVariableByEx(IDispatch* pGlobalDisp, const std::wstring& name, const VARIANT& value)
{
    if (!pGlobalDisp) return false;

    // 1. 查询 IDispatchEx（JScript 全局对象一定支持）
    IDispatchEx* pDispEx = nullptr;
    HRESULT hr = pGlobalDisp->QueryInterface(IID_IDispatchEx,
                                             (void**)&pDispEx);
    if (FAILED(hr) || !pDispEx) return false;

    // 2. 获取或创建属性的 DISPID
    DISPID dispid;
    hr = pDispEx->GetDispID(const_cast<BSTR>(name.c_str()), fdexNameEnsure, &dispid);
    if (FAILED(hr))
    {
        pDispEx->Release();
        return false;
    }

    // 3. 准备参数（属性赋值需要 DISPID_PROPERTYPUT）
    VARIANT v;
    VariantInit(&v);
    hr = VariantCopyInd(&v, const_cast<VARIANTARG*>(&value));  // 深拷贝
    if (FAILED(hr))
    {
        pDispEx->Release();
        return false;
    }

    DISPID dispidNamed = DISPID_PROPERTYPUT;
    DISPPARAMS params = { &v, &dispidNamed, 1, 1 };

    // 4. 通过 InvokeEx 设置属性值
    hr = pDispEx->InvokeEx(dispid, LOCALE_USER_DEFAULT,
                           DISPATCH_PROPERTYPUT,
                           &params, nullptr, nullptr, nullptr);

    VariantClear(&v);
    pDispEx->Release();
    return SUCCEEDED(hr);
}

// 解析根对象Dispatch接口
IUnknown* _resolveRootDispatch()
{
    AST_USING_NAMESPACE
    using FuncType = decltype(&aComObjectRoot);
    FuncType func = (FuncType)aResolveProcAddress(AST_APPEND_DEBUG("AstCOM"), A_STR(aComObjectRoot));
    if (func)
        return func();
    return nullptr;
}

// 获取根对象的Dispatch接口
IUnknown* rootDispatch()
{
    static IUnknown* pRootDisp = _resolveRootDispatch();
    return pRootDisp;
}


} // anonymous namespace

#undef ERR_FAIL
#undef ERR_OK

#endif