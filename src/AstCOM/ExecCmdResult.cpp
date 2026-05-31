///
/// @file      ExecCmdResult.cpp
/// @brief     命令执行结果集合类实现
/// @details   封装ExecuteCommand返回的字符串集合的具体实现
/// @author    axel
/// @date      2026-05-09
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

#include "AstCOM/ExecCmdResult.hpp"

AST_NAMESPACE_BEGIN

HRESULT __stdcall CExecCmdResult::get_Count(long* pVal)
{
    if (!pVal)
        return E_POINTER;
    *pVal = static_cast<long>(results_.size());
    return S_OK;
}

HRESULT __stdcall CExecCmdResult::get_IsSucceeded(VARIANT_BOOL* pVal)
{
    if (!pVal)
        return E_POINTER;
    *pVal = isSucceeded_;
    return S_OK;
}

HRESULT __stdcall CExecCmdResult::get_Item(long Index, BSTR* pVal)
{
    if (!pVal)
        return E_POINTER;
    
    *pVal = nullptr;
    
    if (Index < 0 || static_cast<size_t>(Index) >= results_.size())
        return E_INVALIDARG;
    
    *pVal = SysAllocString(results_[Index].c_str());
    return *pVal ? S_OK : E_OUTOFMEMORY;
}

HRESULT __stdcall CExecCmdResult::Range(long StartIndex, long EndIndex, SAFEARRAY** pVal)
{
    if (!pVal)
        return E_POINTER;
    
    *pVal = nullptr;
    
    if (StartIndex < 0 || EndIndex < 0 || StartIndex > EndIndex)
        return E_INVALIDARG;
    
    size_t start = static_cast<size_t>(StartIndex);
    size_t end = static_cast<size_t>(EndIndex);
    
    if (results_.empty())
        return E_INVALIDARG;

    if (end >= results_.size())
        end = results_.size() - 1;
    
    if (start > end)
        return E_INVALIDARG;
    
    ULONG count = static_cast<ULONG>(end - start + 1);
    long loopCount = static_cast<long>(count);
    
    SAFEARRAYBOUND bound = { count, 0 };
    SAFEARRAY* sa = SafeArrayCreate(VT_BSTR, 1, &bound);
    
    if (!sa)
        return E_OUTOFMEMORY;
    
    for (long i = 0; i < loopCount; i++)
    {
        BSTR bstr = SysAllocString(results_[start + i].c_str());
        if (!bstr)
        {
            SafeArrayDestroy(sa);
            return E_OUTOFMEMORY;
        }
        
        LONG idx = i;
        SafeArrayPutElement(sa, &idx, bstr);
        SysFreeString(bstr);
    }
    
    *pVal = sa;
    return S_OK;
}

AST_NAMESPACE_END