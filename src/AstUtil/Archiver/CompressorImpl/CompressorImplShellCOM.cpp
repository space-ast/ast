#include "CompressorImplShellCOM.hpp"

#ifdef _WIN32

#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/ComInit.hpp"
#include "AstUtil/String/Encode.hpp"
#include "AstUtil/StringView.hpp"
#include "../ShellCOMUtils.hpp"

#include <Windows.h>
#include <shlobj.h>    // Folder, FolderItem
#include <shldisp.h>   // IShellDispatch
#include <cstdio>
#include <cstring>
#include <cwctype>
#include <string>

AST_NAMESPACE_BEGIN

// ============================================================
// 辅助函数
// ============================================================

errc_t CompressorImplShellCOM::createEmptyZip(const std::wstring& path)
{
    // 空的 ZIP End of Central Directory Record（22 字节）
    // Shell 需要此记录来识别 zip 容器，后续 CopyHere 会填充实际内容
    const unsigned char eocd[] = {
        0x50, 0x4B, 0x05, 0x06,  // PK\x05\x06 signature
        0x00, 0x00,              // Number of this disk
        0x00, 0x00,              // Disk where central directory starts
        0x00, 0x00,              // Number of entries on this disk
        0x00, 0x00,              // Total number of entries
        0x00, 0x00, 0x00, 0x00, // Size of central directory
        0x00, 0x00, 0x00, 0x00, // Offset of central directory
        0x00, 0x00               // Comment length
    };

    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        aError("CompressorImplShellCOM: cannot create empty zip file");
        return eErrorInvalidFile;
    }

    DWORD written = 0;
    BOOL ok = WriteFile(hFile, eocd, sizeof(eocd), &written, nullptr);
    CloseHandle(hFile);

    if (!ok || written != sizeof(eocd))
    {
        aError("CompressorImplShellCOM: write empty zip failed");
        DeleteFileW(path.c_str());
        return eError;
    }

    return eNoError;
}

// ============================================================
// 接口实现
// ============================================================

CompressorImplShellCOM& CompressorImplShellCOM::Instance()
{
    static CompressorImplShellCOM instance;
    return instance;
}


bool CompressorImplShellCOM::canCompress(StringView source, StringView target) const
{
    (void)source;
    size_t len = target.size();
    if (len >= 4)
    {
        StringView ext(target.data() + len - 4, 4);
        if ((ext[0] == '.')
            && (ext[1] == 'z' || ext[1] == 'Z')
            && (ext[2] == 'i' || ext[2] == 'I')
            && (ext[3] == 'p' || ext[3] == 'P'))
        {
            return true;
        }
    }
    return false;
}

errc_t CompressorImplShellCOM::compress(StringView source, StringView target, StringView curdir) const
{
    if (source.empty() || target.empty())
    {
        aError("CompressorImplShellCOM: source or target is empty");
        return eErrorInvalidParam;
    }

    // 转换路径并确保使用反斜杠（Shell API 需要）
    std::wstring wSource = aUtf8ToWide(source);
    std::wstring wTarget = aUtf8ToWide(target);
    for (auto& ch : wSource) if (ch == L'/') ch = L'\\';
    for (auto& ch : wTarget) if (ch == L'/') ch = L'\\';

    // 转为绝对路径
    WCHAR absSource[MAX_PATH];
    WCHAR absTarget[MAX_PATH];
    if (GetFullPathNameW(wSource.c_str(), MAX_PATH, absSource, nullptr) == 0 ||
        GetFullPathNameW(wTarget.c_str(), MAX_PATH, absTarget, nullptr) == 0)
    {
        aError("CompressorImplShellCOM: cannot resolve path");
        return eErrorInvalidParam;
    }

    // 检查源路径是否存在
    DWORD srcAttr = GetFileAttributesW(absSource);
    if (srcAttr == INVALID_FILE_ATTRIBUTES)
    {
        aError("CompressorImplShellCOM: source does not exist: %s", source.data());
        return eErrorInvalidFile;
    }

    // 步骤 1: 创建空的 ZIP 容器
    errc_t ret = createEmptyZip(absTarget);
    if (ret != eNoError) return ret;

    // 步骤 2: 初始化 COM（STA 线程模型，Shell 对象必需）
    HRESULT hrCom = aEnsureCoInitialized();
    if (FAILED(hrCom))
    {
        aError("CompressorImplShellCOM: COM initialization failed: 0x%08X", hrCom);
        DeleteFileW(absTarget);
        return eError;
    }

    // 步骤 3: 创建 Shell.Application COM 对象（直接接口，非 late-bound IDispatch）
    IShellDispatch* pShell = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_Shell, nullptr, CLSCTX_INPROC_SERVER,
                                   IID_IShellDispatch, reinterpret_cast<void**>(&pShell));
    if (FAILED(hr) || !pShell)
    {
        aError("CompressorImplShellCOM: CoCreateInstance(CLSID_Shell) failed: 0x%08X", hr);
        DeleteFileW(absTarget);
        return eError;
    }

    // 步骤 4: 获取 ZIP 文件的 Folder 对象
    Folder* pZipFolder = nullptr;
    {
        VARIANT vZipPath;
        VariantInit(&vZipPath);
        vZipPath.vt = VT_BSTR;
        vZipPath.bstrVal = SysAllocString(absTarget);
        hr = pShell->NameSpace(vZipPath, &pZipFolder);
        VariantClear(&vZipPath);

        if (FAILED(hr) || !pZipFolder)
        {
            aError("CompressorImplShellCOM: NameSpace(zip) failed: 0x%08X", hr);
            pShell->Release();
            DeleteFileW(absTarget);
            return eError;
        }
    }

    // 步骤 5: 确定 NameSpace 的工作目录和源项名称
    // curdir 为空 → 旧行为：取 source 的父目录
    // curdir 指定 → 以 curdir 为工作目录，source 取其相对名称
    std::wstring workDir, srcName;
    if (curdir.empty())
    {
        size_t pos = std::wstring(absSource).find_last_of(L"\\/");
        if (pos != std::wstring::npos)
        {
            workDir = std::wstring(absSource).substr(0, pos);
            srcName = std::wstring(absSource).substr(pos + 1);
        }
        else
        {
            workDir = L".";
            srcName = absSource;
        }
    }
    else
    {
        std::wstring wCurdir = aUtf8ToWide(curdir);
        for (auto& ch : wCurdir) if (ch == L'/') ch = L'\\';
        WCHAR absCurdir[MAX_PATH];
        if (GetFullPathNameW(wCurdir.c_str(), MAX_PATH, absCurdir, nullptr) == 0)
        {
            aError("CompressorImplShellCOM: cannot resolve curdir path");
            DeleteFileW(absTarget);
            return eErrorInvalidParam;
        }
        workDir = absCurdir;

        // 验证 source 和 curdir 在同一驱动器
        std::wstring absSrcStr(absSource);
        if (absSrcStr.size() >= 2 && workDir.size() >= 2)
        {
            if ((absSrcStr[1] == L':' ? towupper(absSrcStr[0]) : 0) !=
                (workDir[1] == L':' ? towupper(workDir[0]) : 0))
            {
                aError("CompressorImplShellCOM: source and curdir must be on the same drive");
                DeleteFileW(absTarget);
                return eErrorInvalidParam;
            }
        }

        // 计算 source 相对于 curdir 的路径
        if (absSrcStr.size() < workDir.size())
        {
            // source 路径比 workDir 短，说明不在同一目录树下
            srcName = absSrcStr.substr(absSrcStr.find_last_of(L"\\/") + 1);
        }
        else
        {
            srcName = absSrcStr.substr(workDir.size());
            while (!srcName.empty() && (srcName[0] == L'\\' || srcName[0] == L'/'))
                srcName.erase(0, 1);
        }
        if (srcName.empty()) srcName = L".";
    }

    // 步骤 6: 获取工作目录的 Folder 对象
    Folder* pSrcParentFolder = nullptr;
    {
        VARIANT vDir;
        VariantInit(&vDir);
        vDir.vt = VT_BSTR;
        vDir.bstrVal = SysAllocString(workDir.c_str());
        hr = pShell->NameSpace(vDir, &pSrcParentFolder);
        VariantClear(&vDir);

        if (FAILED(hr) || !pSrcParentFolder)
        {
            aError("CompressorImplShellCOM: NameSpace(work dir) failed: 0x%08X", hr);
            pZipFolder->Release();
            pShell->Release();
            DeleteFileW(absTarget);
            return eError;
        }
    }

    // 步骤 7: 根据 srcName 添加项到 ZIP
    if (srcName == L"." || srcName.empty())
    {
        // curdir == source（目录本身）：枚举目录内容直接添加，不带目录前缀
        FolderItems* pItems = nullptr;
        hr = pSrcParentFolder->Items(&pItems);
        if (FAILED(hr) || !pItems)
        {
            aError("CompressorImplShellCOM: Items() failed: 0x%08X", hr);
            pSrcParentFolder->Release();
            pZipFolder->Release();
            pShell->Release();
            DeleteFileW(absTarget);
            return eError;
        }

        // 收集名称用于等待
        long count = 0;
        pItems->get_Count(&count);
        std::vector<std::wstring> itemNames;
        for (long i = 0; i < count; ++i)
        {
            VARIANT vIdx; VariantInit(&vIdx); vIdx.vt = VT_I4; vIdx.lVal = i;
            FolderItem* pChild = nullptr;
            if (SUCCEEDED(pItems->Item(vIdx, &pChild)) && pChild)
            {
                BSTR bn = nullptr;
                if (SUCCEEDED(pChild->get_Name(&bn)) && bn)
                { itemNames.push_back(std::wstring(bn, SysStringLen(bn))); SysFreeString(bn); }
                pChild->Release();
            }
        }

        if (count > 0)
        {
            VARIANT vIt; VariantInit(&vIt); vIt.vt = VT_DISPATCH;
            vIt.pdispVal = static_cast<IDispatch*>(pItems);
            VARIANT vFl; VariantInit(&vFl); vFl.vt = VT_I4; vFl.lVal = 0x0400;
            hr = pZipFolder->CopyHere(vIt, vFl);
            if (FAILED(hr))
            {
                aError("CompressorImplShellCOM: CopyHere(dir items) failed: 0x%08X", hr);
                pItems->Release();
                pSrcParentFolder->Release();
                pZipFolder->Release();
                pShell->Release();
                DeleteFileW(absTarget);
                return eError;
            }
            bool allOk = true;
            for (const auto& n : itemNames)
            {
                if (!aShellWaitForItem(pZipFolder, n, 30000))
                {
                    aError("CompressorImplShellCOM: waitForItem timeout for: %S", n.c_str());
                    allOk = false;
                }
            }
            if (!allOk)
            {
                pItems->Release();
                pSrcParentFolder->Release();
                pZipFolder->Release();
                pShell->Release();
                DeleteFileW(absTarget);
                return eError;
            }
        }
        pItems->Release();
    }
    else
    {
        // 单文件：ParseName + CopyHere
        FolderItem* pItem = nullptr;
        {
            BSTR bstrItemName = SysAllocString(srcName.c_str());
            hr = pSrcParentFolder->ParseName(bstrItemName, &pItem);
            SysFreeString(bstrItemName);
            if (FAILED(hr) || !pItem)
            {
                aError("CompressorImplShellCOM: ParseName failed for: %s", source.data());
                pSrcParentFolder->Release();
                pZipFolder->Release();
                pShell->Release();
                DeleteFileW(absTarget);
                return eError;
            }
        }

        VARIANT vItem;
        VariantInit(&vItem);
        vItem.vt = VT_DISPATCH;
        vItem.pdispVal = static_cast<IDispatch*>(pItem);
        VARIANT vFlags;
        VariantInit(&vFlags);
        vFlags.vt = VT_I4;
        vFlags.lVal = 0x0400;
        hr = pZipFolder->CopyHere(vItem, vFlags);
        if (FAILED(hr))
        {
            aError("CompressorImplShellCOM: CopyHere(file) failed: 0x%08X", hr);
            pItem->Release();
            pSrcParentFolder->Release();
            pZipFolder->Release();
            pShell->Release();
            DeleteFileW(absTarget);
            return eError;
        }
        if (!aShellWaitForItem(pZipFolder, srcName, 30000))
        {
            aError("CompressorImplShellCOM: waitForItem timeout for: %s", source.data());
            pItem->Release();
            pSrcParentFolder->Release();
            pZipFolder->Release();
            pShell->Release();
            DeleteFileW(absTarget);
            return eError;
        }
        pItem->Release();
    }

    pSrcParentFolder->Release();
    pZipFolder->Release();
    pShell->Release();

    return eNoError;
}

AST_NAMESPACE_END

#endif // _WIN32
