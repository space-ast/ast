///
/// @file      FileStream.hpp
/// @brief     以 UTF-8 编码解释文件路径的 C++ 流
/// @details   std::ifstream 等在 Windows 上按当前 ANSI 代码页解释窄字符路径
///            本文件流类提供一层封装，支持以 UTF-8 编码的文件路径
/// @author    axel
/// @date      09.10.2026
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Encode.hpp"   // for aUtf8ToWide
#include <ios>                  // for std::ios_base, std::ios::in, ...
#include <fstream>              // for std::filebuf, std::ifstream, std::ofstream, ...
#include <string>               // for std::string

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Platform
    @{
*/


/// @brief C++ 标准库的补充与修正
/// @details 存放对 std 标准库的包装与增强，例如以 UTF-8 编码解释路径的文件流
/// @ingroup Platform
namespace cxx
{

#if defined(_WIN32) || defined(AST_PARSED_BY_DOXYGEN)
namespace detail
{

/// @brief 以 UTF-8 编码的路径打开文件缓冲
/// @param[out] buf 待打开的文件缓冲
/// @param[in] filepath 文件路径，utf-8 编码
/// @param[in] mode 打开模式
/// @return 打开的缓冲指针，失败时为空
/// @note Windows 上标准库默认按 ANSI 代码页解释窄字符路径，本函数转换为宽字符以支持 utf-8 编码的路径
inline std::filebuf* openfilebuf(std::filebuf& buf, const char* filepath, std::ios_base::openmode mode)
{
#if defined(_WIN32) 
    std::wstring wpath;
    errc_t rc = aUtf8ToWide(filepath, wpath);
    if(rc == eNoError)
        return buf.open(wpath.c_str(), mode);
    else
        return nullptr;
#else
    return buf.open(filepath, mode);
#endif
}

}   // namespace detail


/// @brief 以 UTF-8 编码的路径打开的文件输入流
/// @details 
/// Windows 上 `std::ifstream` 用当前 ANSI 代码页解释窄字符路径
/// 本类在打开前把 UTF-8 路径转换为宽字符再交给底层文件缓冲，
/// 其余接口与 `std::ifstream` 一致；
/// @note 路径参数只接受 UTF-8 编码
/// @ingroup Platform
class ifstream : public std::ifstream
{
public:
    ifstream() = default;

    explicit ifstream(const char* filepath, std::ios_base::openmode mode = std::ios::in)
    {
        open(filepath, mode);
    }

    explicit ifstream(const std::string& filepath, std::ios_base::openmode mode = std::ios::in)
    {
        open(filepath.c_str(), mode);
    }

    /// @brief 打开文件
    /// @param[in] filepath 文件路径，utf-8 编码
    /// @param[in] mode 打开模式
    void open(const char* filepath, std::ios_base::openmode mode = std::ios::in)
    {
        // 与 std::ifstream::open 一致：始终补上 in
        if (detail::openfilebuf(*rdbuf(), filepath, mode | std::ios::in))
            clear();
        else
            setstate(std::ios::failbit);
    }
};


/// @brief 以 UTF-8 编码的路径打开的文件输出流，语义见 ifstream
/// @ingroup Platform
class ofstream : public std::ofstream
{
public:
    ofstream() = default;

    explicit ofstream(const char* filepath, std::ios_base::openmode mode = std::ios::out)
    {
        open(filepath, mode);
    }

    explicit ofstream(const std::string& filepath, std::ios_base::openmode mode = std::ios::out)
    {
        open(filepath.c_str(), mode);
    }

    /// @brief 打开文件
    /// @param[in] filepath 文件路径，utf-8 编码
    /// @param[in] mode 打开模式
    void open(const char* filepath, std::ios_base::openmode mode = std::ios::out)
    {
        // 与 std::ofstream::open 一致：始终补上 out
        if (detail::openfilebuf(*rdbuf(), filepath, mode | std::ios::out))
            clear();
        else
            setstate(std::ios::failbit);
    }
};


/// @brief 以 UTF-8 编码的路径打开的文件输入输出流，语义见 ifstream
/// @ingroup Platform
class fstream : public std::fstream
{
public:
    fstream() = default;

    explicit fstream(const char* filepath, std::ios_base::openmode mode = std::ios::in | std::ios::out)
    {
        open(filepath, mode);
    }

    explicit fstream(const std::string& filepath, std::ios_base::openmode mode = std::ios::in | std::ios::out)
    {
        open(filepath.c_str(), mode);
    }

    /// @brief 打开文件
    /// @param[in] filepath 文件路径，utf-8 编码
    /// @param[in] mode 打开模式
    void open(const char* filepath, std::ios_base::openmode mode = std::ios::in | std::ios::out)
    {
        if (detail::openfilebuf(*rdbuf(), filepath, mode))
            clear();
        else
            setstate(std::ios::failbit);
    }
};

#else

using std::ifstream;
using std::ofstream;
using std::fstream;

#endif

}   // namespace cxx


/*! @} */

AST_NAMESPACE_END
