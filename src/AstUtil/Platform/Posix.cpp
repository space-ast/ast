///
/// @file      Posix.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-02-02
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "Posix.hpp"
#include "AstUtil/Encode.hpp"

AST_NAMESPACE_BEGIN

#ifdef _WIN32
int posix::rmdir(const char *path)
{
    std::wstring wide_path;
    aUtf8ToWide(path, wide_path);  // 转换为宽字符
    return _wrmdir(wide_path.c_str());
}

int posix::chdir(const char *path)
{
    std::wstring wide_path;
    aUtf8ToWide(path, wide_path);  // 转换为宽字符
    return _wchdir(wide_path.c_str());
}

char *posix::getcwd(char *buf, size_t size)
{
    wchar_t wide_buf[_MAX_PATH]{'\0'};
    wchar_t* cwd = _wgetcwd(wide_buf, _MAX_PATH);
    if (cwd == nullptr)
    {
        return nullptr;
    }
    std::string utf8_buf;
    aWideToUtf8(cwd, utf8_buf);
    if(size == 0){
        size = utf8_buf.size() + 1;
    }else if (size <= utf8_buf.size())
    {
        errno = ERANGE;
        return nullptr;
    }
    if(buf == nullptr){
        buf = (char*)malloc(size * sizeof(char));
    }
    std::memcpy(buf, utf8_buf.c_str(), utf8_buf.size());
    buf[utf8_buf.size()] = '\0';
    return buf;
}
#endif

AST_NAMESPACE_END

