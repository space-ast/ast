///
/// @file      ShellCOMUtils.hpp
/// @brief     Shell COM 内部共享工具（CompressorImplShellCOM 和 ExtractorImplShellCOM 共享）
/// @details   Windows 专用，不对外暴露。包含 waitForItem 轮询辅助函数。
/// @author    axel
/// @date      2026-07-25
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#if defined(_WIN32) && !defined(SWIG)

#include "AstGlobal.h"

#include <Windows.h>
#include <shlobj.h>
#include <string>

AST_NAMESPACE_BEGIN

/// 等待 Shell COM CopyHere 异步操作完成（通过 ParseName 轮询）
/// @param pFolder 目标 Folder 指针
/// @param itemName 要等待的项名称
/// @param timeoutMs 超时时间（毫秒）
/// @return true 项已出现，false 超时
inline bool aShellWaitForItem(Folder* pFolder, const std::wstring& itemName,
                               unsigned long timeoutMs = 30000)
{
    unsigned long start = GetTickCount();

    while ((GetTickCount() - start) < timeoutMs)
    {
        FolderItem* pCheck = nullptr;
        BSTR bstrName = SysAllocString(itemName.c_str());
        if (!bstrName) return false;
        HRESULT hr = pFolder->ParseName(bstrName, &pCheck);
        SysFreeString(bstrName);

        if (SUCCEEDED(hr) && pCheck != nullptr)
        {
            pCheck->Release();
            return true;
        }
        Sleep(100);
    }
    return false;
}

AST_NAMESPACE_END

#endif // _WIN32
