/// @file      IO.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      30.11.2025
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
 
#include "IO.hpp"
#include "AstUtil/Encode.hpp"
#include <clocale>              // for _create_locale, _free_locale
#include <cstdarg>              // for va_list, va_start, va_end
#include <memory>               // for std::unique_ptr
#include <type_traits>          // for std::remove_pointer


AST_NAMESPACE_BEGIN
 
#ifdef _WIN32

struct LocaleDeleter
{
    void operator()(_locale_t locale) const
    {
        if (locale) {
            _free_locale(locale);
        }
    }
};

A_THREAD_LOCAL std::unique_ptr<std::remove_pointer<_locale_t>::type, LocaleDeleter> t_locale;  // utf-8 locale

_locale_t _ast_locale_ensure()
{
    static const char* locale_strs[]{
            ".UTF-8",
            "zh_CN.UTF-8",
            "en_US.UTF-8",
            "C.UTF-8",
            ""
    };

    if (!t_locale) {
        for (const char* locale_str : locale_strs) {
            _locale_t locale = _create_locale(LC_ALL, locale_str);
            if (locale) {
                t_locale.reset(locale);
                break;
            }
        }
    }
    return t_locale.get();
}

std::FILE* ast_fopen(const char* filepath, const char* mode)
{
    // support utf8
    std::wstring wpath;
    std::wstring wmode;
    aUtf8ToWide(filepath, wpath);
    aUtf8ToWide(mode, wmode);
    return ::_wfopen(wpath.c_str(), wmode.c_str());
}

int ast_printf(const char* format, ...)
{
    va_list args;
    int result;
    va_start(args, format);
    result = ast_vprintf(format, args);
    va_end(args);
    return result;
}

int ast_vprintf(const char* format, va_list args)
{
    _locale_t locale = _ast_locale_ensure();
    return _vprintf_l(format, locale, args);
}

#else

int ast_printf(const char* format, ...)
{
    va_list args;
    int result;
    va_start(args, format);
    result = ast_vprintf(format, args);
    va_end(args);
    return result;
}

#endif


AST_NAMESPACE_END
 
