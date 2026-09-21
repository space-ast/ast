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

/// 等待期间派发本线程的消息队列
/// @details Folder::CopyHere 是异步的：真正干活的是 Shell 的副本引擎
///          （zip 由 zipfldr.dll 实现），它跑在后台线程上，并可能通过
///          SendMessage 把进度/完成回投到调用线程。本线程是 STA，若只 Sleep
///          不泵消息，这些投递就永远排不上队——严重时后台线程卡在 SendMessage
///          上，复制永不落地，表现为轮询一路超时。
inline void aShellPumpMessages(unsigned long waitMs)
{
    unsigned long start = GetTickCount();

    while ((GetTickCount() - start) < waitMs)
    {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        unsigned long elapsed = GetTickCount() - start;
        DWORD remain = (elapsed < waitMs) ? (DWORD)(waitMs - elapsed) : 0;
        if (remain > 50) remain = 50;   // 分段等待，避免错过队列中的消息
        MsgWaitForMultipleObjectsEx(0, nullptr, remain, QS_ALLINPUT,
                                    MWMO_INPUTAVAILABLE);
    }
}

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
        aShellPumpMessages(100);
    }
    return false;
}

AST_NAMESPACE_END

#endif // _WIN32
