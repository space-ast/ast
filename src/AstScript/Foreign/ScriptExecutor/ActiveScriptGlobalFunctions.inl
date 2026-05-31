///
/// @file      ActiveScriptGlobalFunctions.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-10
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

#pragma once

#include "AstGlobal.h"
#ifdef _WIN32

#include <windows.h>
#include <activscp.h>
#include <string>
#include <fstream>
#include <iostream>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/



// 1. 实现 IDispatch 的全局函数提供者
class ActiveScriptGlobalFunctions final: public IDispatch {
private:
    LONG m_ref;

public:
    ActiveScriptGlobalFunctions() : m_ref(1) {
    }

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_IUnknown || riid == IID_IDispatch) {
            *ppv = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() {
        LONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0) delete this;
        return ref;
    }

    // IDispatch
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) { *pctinfo = 0; return S_OK; }
    STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo**) { return E_NOTIMPL; }

    STDMETHODIMP GetIDsOfNames(REFIID, LPOLESTR* rgszNames, UINT cNames, LCID, DISPID* rgDispId) {
        // 我们只处理一个函数："OutputMessage"
        if (cNames != 1) return E_INVALIDARG;
        if (_wcsicmp(rgszNames[0], L"OutputMessage") == 0) {
            *rgDispId = 1;  // 固定 DISPID
            return S_OK;
        }
        return DISP_E_UNKNOWNNAME;
    }

    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID, LCID, WORD wFlags, DISPPARAMS* pDispParams,
                        VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) {
        if (wFlags & DISPATCH_METHOD) {
            if (dispIdMember == 1) {
                // 期望两个参数： level, message
                if (pDispParams->cArgs != 2) return DISP_E_BADPARAMCOUNT;

                // VBScript 参数是逆序的：最后一个参数在 pDispParams->rgvarg[0]
                VARIANT arg2 = pDispParams->rgvarg[0]; // 第二个参数：message
                VARIANT arg1 = pDispParams->rgvarg[1]; // 第一个参数：level

                // 转换为整数和字符串
                int level = 0;
                if (arg1.vt == VT_I4) level = arg1.lVal;
                else if (arg1.vt == VT_I2) level = arg1.iVal;

                std::wstring msg;
                VARIANT vMsg;
                VariantInit(&vMsg);
                if (SUCCEEDED(VariantChangeType(&vMsg, &arg2, 0, VT_BSTR))) {
                    msg = vMsg.bstrVal ? vMsg.bstrVal : L"";
                    VariantClear(&vMsg);
                }

                // 实现功能：输出到控制台
                std::wcout << L"[" << level << L"] " << msg << std::endl;

                return S_OK;
            }
        }
        return DISP_E_MEMBERNOTFOUND;
    }
};


/*! @} */

AST_NAMESPACE_END

#endif
