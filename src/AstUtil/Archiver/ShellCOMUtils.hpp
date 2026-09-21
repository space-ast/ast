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

/// 取项的“真实名称”（带扩展名）
/// @details 不能用 FolderItem::get_Name：它返回的是**显示名称**，会按资源管理器的
///          「隐藏已知文件类型的扩展名」设置把扩展名去掉。实测（同一台机器，仅切换
///          该设置）：
///             HideFileExt=0 → Name='readme.txt'
///             HideFileExt=1 → Name='readme'     就变成了不带扩展名的
///          而 .bin/.dat 这类没有注册文件关联的扩展名不受影响，始终带扩展名——这正是
///          该设置名字里“已知文件类型”的含义。CI 上该设置为默认（隐藏），于是
///          createTestDirectory 造出的 readme.txt 被读成 "readme"，拿它去 ParseName
///          永远匹配不上，轮询一路超时到 30 秒，最后删档返回失败。
///          get_Path 返回的始终是带扩展名的真实路径，取最后一段即可。
/// @return 真实名称，失败时返回空串
inline std::wstring aShellItemRealName(FolderItem* pItem)
{
    if (!pItem) return std::wstring();

    BSTR bstrPath = nullptr;
    if (SUCCEEDED(pItem->get_Path(&bstrPath)) && bstrPath)
    {
        std::wstring path(bstrPath, SysStringLen(bstrPath));
        SysFreeString(bstrPath);
        size_t pos = path.find_last_of(L"\\/");
        if (pos != std::wstring::npos)
        {
            if (pos + 1 < path.size()) return path.substr(pos + 1);
        }
        else if (!path.empty())
        {
            return path;   // 没有分隔符，Path 本身就是名字
        }
    }

    // 退路：Path 拿不到时退回显示名称（少数命名空间未实现 Path）
    BSTR bstrName = nullptr;
    if (SUCCEEDED(pItem->get_Name(&bstrName)) && bstrName)
    {
        std::wstring name(bstrName, SysStringLen(bstrName));
        SysFreeString(bstrName);
        return name;
    }
    return std::wstring();
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
