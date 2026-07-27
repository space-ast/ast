#include "ExtractorImplShellCOM.hpp"

#ifdef _WIN32

#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/ComInit.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/ShellCOMUtils.hpp"

#include <Windows.h>
#include <shlobj.h>    // Folder, FolderItem, FolderItems
#include <shldisp.h>   // IShellDispatch
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

// ============================================================
// 辅助函数
// ============================================================

// ============================================================
// 接口实现
// ============================================================

ExtractorImplShellCOM& ExtractorImplShellCOM::Instance()
{
    static ExtractorImplShellCOM instance;
    return instance;
}


bool ExtractorImplShellCOM::canExtract(StringView source) const
{
    if (source.empty()) return false;

    // 检查 .zip 扩展名
    size_t len = source.size();
    if (len >= 4)
    {
        StringView ext(source.data() + len - 4, 4);
        if ((ext[0] == '.')
            && (ext[1] == 'z' || ext[1] == 'Z')
            && (ext[2] == 'i' || ext[2] == 'I')
            && (ext[3] == 'p' || ext[3] == 'P'))
        {
            return true;
        }
    }

    // 检查 PK 魔数
    unsigned char magic[4] = {};
    FILE* fp = posix::fopen(source.data(), "rb");
    if (!fp) return false;
    size_t nr = fread(magic, 1, 4, fp);
    fclose(fp);
    if (nr == 4 && magic[0] == 0x50 && magic[1] == 0x4B
        && magic[2] == 0x03 && magic[3] == 0x04)
    {
        return true;
    }

    return false;
}

errc_t ExtractorImplShellCOM::extract(StringView source, StringView target) const
{
    if (source.empty() || target.empty())
    {
        aError("ExtractorImplShellCOM: source or target is empty");
        return eErrorInvalidParam;
    }

    // 转换路径并确保使用反斜杠（Shell API 需要）
    std::wstring wSource = aUtf8ToWide(source);
    std::wstring wTarget = aUtf8ToWide(target);
    for (auto& ch : wSource) if (ch == L'/') ch = L'\\';
    for (auto& ch : wTarget) if (ch == L'/') ch = L'\\';

    WCHAR absSource[MAX_PATH];
    WCHAR absTarget[MAX_PATH];
    DWORD srcLen = GetFullPathNameW(wSource.c_str(), MAX_PATH, absSource, nullptr);
    DWORD tgtLen = GetFullPathNameW(wTarget.c_str(), MAX_PATH, absTarget, nullptr);
    if (srcLen == 0 || srcLen >= MAX_PATH ||
        tgtLen == 0 || tgtLen >= MAX_PATH)
    {
        aError("ExtractorImplShellCOM: cannot resolve path");
        return eErrorInvalidParam;
    }

    // 确保目标目录存在
    fs::path targetPath = std::string(target);
    if (!fs::exists(targetPath))
    {
        if (!fs::create_directories(targetPath))
        {
            aError("cannot create target directory: '%.*s'", target.size(), target.data());
            return eErrorInvalidFile;
        }
    }

    // 初始化 COM（STA 线程模型，Shell 对象必需）
    HRESULT hrCom = aEnsureCoInitialized();
    if (FAILED(hrCom))
    {
        aError("ExtractorImplShellCOM: COM initialization failed: 0x%08X", hrCom);
        return eError;
    }

    // 创建 Shell.Application COM 对象（直接接口，非 late-bound IDispatch）
    IShellDispatch* pShell = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_Shell, nullptr, CLSCTX_INPROC_SERVER,
                                   IID_IShellDispatch, reinterpret_cast<void**>(&pShell));
    if (FAILED(hr) || !pShell)
    {
        aError("ExtractorImplShellCOM: CoCreateInstance(CLSID_Shell) failed: 0x%08X", hr);
        return eError;
    }

    // 步骤 1: 获取 ZIP 文件的 Folder 对象
    Folder* pZipFolder = nullptr;
    {
        VARIANT vZipPath;
        VariantInit(&vZipPath);
        vZipPath.vt = VT_BSTR;
        vZipPath.bstrVal = SysAllocString(absSource);
        hr = pShell->NameSpace(vZipPath, &pZipFolder);
        VariantClear(&vZipPath);

        if (FAILED(hr) || !pZipFolder)
        {
            aError("ExtractorImplShellCOM: NameSpace(zip) failed: 0x%08X", hr);
            pShell->Release();
            return eError;
        }
    }

    // 步骤 2: 获取目标目录的 Folder 对象
    Folder* pDestFolder = nullptr;
    {
        VARIANT vDestPath;
        VariantInit(&vDestPath);
        vDestPath.vt = VT_BSTR;
        vDestPath.bstrVal = SysAllocString(absTarget);
        hr = pShell->NameSpace(vDestPath, &pDestFolder);
        VariantClear(&vDestPath);

        if (FAILED(hr) || !pDestFolder)
        {
            aError("ExtractorImplShellCOM: NameSpace(dest) failed: 0x%08X", hr);
            pZipFolder->Release();
            pShell->Release();
            return eError;
        }
    }

    // 步骤 3: 获取 zip 内所有项的集合
    FolderItems* pItems = nullptr;
    hr = pZipFolder->Items(&pItems);
    if (FAILED(hr) || !pItems)
    {
        aError("ExtractorImplShellCOM: cannot enumerate zip contents: 0x%08X", hr);
        pDestFolder->Release();
        pZipFolder->Release();
        pShell->Release();
        return eError;
    }

    // 步骤 4: 收集所有项的名称（用于后续轮询等待）
    long count = 0;
    pItems->get_Count(&count);
    std::vector<std::wstring> itemNames;
    for (long i = 0; i < count; ++i)
    {
        VARIANT vIndex;
        VariantInit(&vIndex);
        vIndex.vt = VT_I4;
        vIndex.lVal = i;

        FolderItem* pItem = nullptr;
        hr = pItems->Item(vIndex, &pItem);
        if (SUCCEEDED(hr) && pItem)
        {
            BSTR bstrName = nullptr;
            if (SUCCEEDED(pItem->get_Name(&bstrName)) && bstrName)
            {
                itemNames.push_back(std::wstring(bstrName, SysStringLen(bstrName)));
                SysFreeString(bstrName);
            }
            pItem->Release();
        }
    }

    if (count == 0)
    {
        // 空 zip，无需解压
        pItems->Release();
        pDestFolder->Release();
        pZipFolder->Release();
        pShell->Release();
        return eNoError;
    }

    // 步骤 5: CopyHere 一次性解压全部内容到目标目录
    {
        VARIANT vItems;
        VariantInit(&vItems);
        vItems.vt = VT_DISPATCH;
        vItems.pdispVal = static_cast<IDispatch*>(pItems);

        VARIANT vFlags;
        VariantInit(&vFlags);
        vFlags.vt = VT_I4;
        vFlags.lVal = 0x0400; // FOF_NO_CONNECTED_FILES

        hr = pDestFolder->CopyHere(vItems, vFlags);

        if (FAILED(hr))
        {
            aError("ExtractorImplShellCOM: CopyHere failed: 0x%08X", hr);
            pItems->Release();
            pDestFolder->Release();
            pZipFolder->Release();
            pShell->Release();
            return eError;
        }
    }

    // 步骤 6: 轮询等待每个文件/文件夹出现在目标目录
    bool allOk = true;
    std::vector<std::wstring> extractedItems; // 记录已成功提取的项，用于失败时回滚
    for (const auto& name : itemNames)
    {
        if (!aShellWaitForItem(pDestFolder, name, 30000))
        {
            aError("waitForItem timeout for: %ls", name.c_str());
            allOk = false;
            break; // 发生超时后不再等待剩余项
        }
        extractedItems.push_back(name);
    }

    if (!allOk)
    {
        // 尝试清理已提取的项，避免残留不完整的解压结果
        aError("extraction incomplete, attempting to clean up partial files");
        for (const auto& name : extractedItems)
        {
            std::wstring itemPath = std::wstring(absTarget) + L"\\" + name;
            // 删除文件或递归删除目录
            DWORD attr = GetFileAttributesW(itemPath.c_str());
            if (attr != INVALID_FILE_ATTRIBUTES)
            {
                if (attr & FILE_ATTRIBUTE_DIRECTORY)
                {
                    // 使用 SHFileOperationW 或简单递归——这里使用 RemoveDirectory + fallback
                    // 对于非空目录，RemoveDirectory 会失败；记录警告而不中断
                    if (!RemoveDirectoryW(itemPath.c_str()))
                    {
                        // 目录可能非空，由调用者决定是否手动清理
                        aError("ExtractorImplShellCOM: cannot remove directory during rollback: %ls", itemPath.c_str());
                    }
                }
                else
                {
                    DeleteFileW(itemPath.c_str());
                }
            }
        }
    }

    pItems->Release();
    pDestFolder->Release();
    pZipFolder->Release();
    pShell->Release();

    return allOk ? eNoError : eError;
}

AST_NAMESPACE_END

#endif // _WIN32
