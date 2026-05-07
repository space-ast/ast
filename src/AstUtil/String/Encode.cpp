/// @file      Encode.cpp
/// @brief     
/// @details   ~
/// @author    axel
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
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <clocale>
#include <locale>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <codecvt>
#endif

AST_NAMESPACE_BEGIN


#ifdef _WIN32   
errc_t aAnsiToWide(const char* ansi, std::wstring& wide)
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

errc_t aWideToAnsi(const wchar_t* wide, std::string& ansi)
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


struct LocaleDeleter
{
    void operator()(_locale_t locale) const
    {
        if (locale) {
            _free_locale(locale);
        }
    }
};


A_THREAD_LOCAL std::unique_ptr<std::remove_pointer<_locale_t>::type, LocaleDeleter> t_ansi_locale;    // ANSI locale
A_THREAD_LOCAL std::unique_ptr<std::remove_pointer<_locale_t>::type, LocaleDeleter> t_utf8_locale;     // utf-8 locale

_locale_t aUTF8Locale()
{
    static const char* locale_strs[]{
            ".UTF-8",
            "zh_CN.UTF-8",
            "en_US.UTF-8",
            "C.UTF-8",
            // 下面两个为fallback处理，避免崩溃
            "C",
            ""
    };

    if (A_UNLIKELY(!t_utf8_locale)) {
        // 设置控制台I/O编码为UTF-8
        {
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
        }
        for (const char* locale_str : locale_strs) {
            _locale_t locale = _create_locale(LC_CTYPE, locale_str);
            if (locale) {
                t_utf8_locale.reset(locale);
                break;
            }
        }
        if (!t_utf8_locale) {
            aError("failed to create utf-8 locale");
        }
    }
    return t_utf8_locale.get();
}

_locale_t aAnsiLocale()
{
    if (A_UNLIKELY(!t_ansi_locale)) {
        t_ansi_locale.reset(_create_locale(LC_CTYPE, ""));
        if (!t_ansi_locale) {
            aError("failed to create ANSI locale");
        }
    }
    return t_ansi_locale.get();
}

static size_t ansitowcs(wchar_t* wide, const char* ansi, size_t size)
{
    return _mbstowcs_l(wide, ansi, size, aAnsiLocale());
}

static size_t wcstoansi(char* ansi, const wchar_t* wide, size_t size)
{
    return _wcstombs_l(ansi, wide, size, aAnsiLocale());
}



errc_t aUtf8ToWide(const char* utf8, std::wstring& wide)
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

std::wstring aUtf8ToWide(StringView utf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), nullptr, 0);
    if (len > 0)
    {
        std::wstring wide(len, L'\0');
        wchar_t* buffer = &wide[0];
        MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), buffer, len);
        return wide;
    }
    return {};
}


errc_t aWideToUtf8(const wchar_t* wide, std::string& utf8)
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

#else

#if defined(__wasm__)
errc_t aUtf8ToWide(const char* utf8, std::wstring& wide) {
    aError("utf8ToWide not supported on wasm");
    return -1;
}

errc_t aWideToUtf8(const wchar_t* wide, std::string& utf8) 
{
    aError("wideToUtf8 not supported on wasm");
    return -1;
}

#else
errc_t aUtf8ToWide(const char* utf8, std::wstring& wide) {
    if (utf8 == nullptr) {
        return eErrorNullInput;
    }
    try {
        // 创建转换器：UTF-8 <-> wchar_t (UCS-2/UTF-16 或 UCS-4，取决于平台)
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        wide = converter.from_bytes(utf8);
        return eNoError;
    } catch (const std::range_error&) {
        // 输入包含无效的 UTF-8 序列
        wide.clear();
        return eErrorInvalidParam;
    } catch (...) {
        wide.clear();
        return eErrorInvalidParam;
    }
}

errc_t aWideToUtf8(const wchar_t* wide, std::string& utf8) 
{
    if (wide == nullptr) {
        return eErrorNullInput;
    }
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        utf8 = converter.to_bytes(wide);
        return eNoError;
    } catch (const std::range_error&) {
        // 宽字符串包含无法表示为 UTF-8 的字符（例如 Windows 上的非法代理对）
        utf8.clear();
        return eErrorInvalidParam;
    } catch (...) {
        utf8.clear();
        return eErrorInvalidParam;
    }
}
#endif

#endif



errc_t aWideToCRT(const wchar_t* wide, std::string& crt)
{
    if (!wide) {
        return eErrorInvalidParam;
    }
    auto func = &wcstombs;
    #ifdef _WIN32
    char* locale = setlocale(LC_CTYPE, nullptr);
    if (locale && strcmp(locale, "C") == 0) 
    {
        func = &wcstoansi;
    }
    #endif

    // 首先获取所需的缓冲区大小
    size_t size = func(nullptr, wide, 0);
    if (size == static_cast<size_t>(-1)) {
        size = wcslen(wide) * 4;
    }

    // 分配缓冲区并转换
    std::vector<char> buffer(size + 1); // +1 用于终止符
    size_t result = func(buffer.data(), wide, size + 1);
    if (result == static_cast<size_t>(-1)) {
        return eErrorInvalidParam;
    }

    crt = buffer.data();
    return eNoError;
}



errc_t aCRTToWide(const char* crt, std::wstring& wide)
{
    if (!crt) {
        return eErrorInvalidParam;
    }
    auto func = &mbstowcs;

    #ifdef _WIN32
    char* locale = setlocale(LC_CTYPE, nullptr);
    if (locale && strcmp(locale, "C") == 0) 
    {
        func = &ansitowcs;
    }
    #endif

    // 首先获取所需的缓冲区大小
    size_t size = func(nullptr, crt, 0);
    if (size == static_cast<size_t>(-1)) {
        size = strlen(crt);
    }

    // 分配缓冲区并转换
    std::vector<wchar_t> buffer(size + 1); // +1 用于终止符
    size_t result = func(buffer.data(), crt, size + 1);
    if (result == static_cast<size_t>(-1)) {
        return eErrorInvalidParam;
    }

    wide = buffer.data();
    return eNoError;
}

bool aCRTIsUTF8()
{
    char *locale = setlocale(LC_CTYPE, nullptr);
    if (locale == nullptr)
        return false;

    // 不区分大小写比较是否为 "UTF-8"
    // 常见的变体有 "UTF-8", "utf8", "utf-8" 等
    for (const char *p = locale; *p; ++p) {
        if (tolower((unsigned char)*p) == 'u' &&
            tolower((unsigned char)*(p+1)) == 't' &&
            tolower((unsigned char)*(p+2)) == 'f') 
        {
            // 检查后续字符是否为 '-8' 或 '8'
            const char *q = p + 3;
            if (*q == '-') ++q;
            if (*q == '8')
                return true;
        }
    }
    return false;
}

errc_t aUTF8ToCRT(const char *utf8, std::string &crt)
{
    if(aCRTIsUTF8())
    {
        crt = utf8;
        return eNoError;
    }
    std::wstring wide;
    errc_t err = aUtf8ToWide(utf8, wide);
    if(err != eNoError)
        return err;
    return aWideToCRT(wide.data(), crt);
}

AST_NAMESPACE_END