/// @file      Encode.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      28.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。


#include "Encode.hpp"
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

AST_NAMESPACE_BEGIN


#ifdef _WIN32   
err_t aAnsiToWide(const char* ansi, std::wstring& wide)
{
    int len = MultiByteToWideChar(CP_ACP, 0, ansi, -1, nullptr, 0);
    if (len > 0)
    {
        std::vector<wchar_t> buffer(len);
        MultiByteToWideChar(CP_ACP, 0, ansi, -1, buffer.data(), len);
        wide = buffer.data();
        return eNoError;
    }
    return eErrorInvalidParam;
}

err_t aUtf8ToWide(const char* utf8, std::wstring& wide)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
    if (len > 0)
    {
        std::vector<wchar_t> buffer(len);
        MultiByteToWideChar(CP_UTF8, 0, utf8, -1, buffer.data(), len);
        wide = buffer.data();
        return eNoError;
    }
    return eErrorInvalidParam;
}

err_t aWideToAnsi(const wchar_t* wide, std::string& ansi)
{
    int len = WideCharToMultiByte(CP_ACP, 0, wide, -1, nullptr, 0, nullptr, nullptr);
    if (len > 0)
    {
        std::vector<char> buffer(len);
        WideCharToMultiByte(CP_ACP, 0, wide, -1, buffer.data(), len, nullptr, nullptr);
        ansi = buffer.data();
        return eNoError;
    }
    return eErrorInvalidParam;
}

err_t aWideToUtf8(const wchar_t* wide, std::string& utf8)
{
    int len = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
    if (len > 0)
    {
        std::vector<char> buffer(len);
        WideCharToMultiByte(CP_UTF8, 0, wide, -1, buffer.data(), len, nullptr, nullptr);
        utf8 = buffer.data();
        return eNoError;
    }
    return eErrorInvalidParam;
}

#endif

AST_NAMESPACE_END
 
