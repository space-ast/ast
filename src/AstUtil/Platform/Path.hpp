///
/// @file      Path.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-23
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

inline bool aIsPathSeparator(char c)
{
    #ifdef _WIN32
    return c == '/' || c == '\\';
    #else
    return c == '/';
    #endif
}

AST_UTIL_CAPI bool aPathIsRelative(StringView path);


/// @brief 路径是否是绝对路径，语义与 std::filesystem::path::is_absolute 一致
/// @details POSIX 上以 / 开头即为绝对；Windows 上必须同时具备根名称和根目录
///          （C:\foo、\\server\share 是绝对；\foo、/foo、C:foo 不是）
AST_UTIL_CAPI bool aPathIsAbsolute(StringView path);


#ifdef _WIN32
/// @brief 将 UTF-8 路径转换为 Win32 宽字符路径
/// @details
/// 路径长度达到 MAX_PATH - 12 时加上 \\?\ 前缀，突破 260 字符的限制
/// （未加前缀时 CreateDirectoryW 等 API 在旧版 SDK 中要求预留 12 个字符给 8.3 文件名）
/// 长路径先转成规范化的绝对路径（顺带解析 . / .. 和分隔符）
/// 带 \\?\ 前缀的路径不做规范化，
/// @param path UTF-8 路径
/// @return Win32 宽字符路径
AST_UTIL_API std::wstring aMakeWin32Path(StringView path);


/// @brief 创建 NTFS 目录联接（Junction Point）
/// @details 目标目录必须位于本地卷（不支持 UNC/网络路径），联接点所在卷必须是 NTFS
///          创建目录联接不需要管理员权限
///          失败时会删除已经创建的空目录
/// @param targetPath 本地目录路径（相对路径按当前目录解析）
/// @param junctionPath 联接点路径（必须是尚不存在的目录）
/// @return 错误码
AST_UTIL_CAPI errc_t aCreateJunction(StringView targetPath, StringView junctionPath);

#endif

/// @brief 创建符号链接
/// @details 目标可以不存在（悬空链接）；相对目标路径按符号链接所在目录解析
///          Windows 上由 isDirectory 决定创建目录符号链接还是文件符号链接，
///          类型需与目标一致，否则链接无法按目录/文件访问
///          Windows 上创建符号链接需要管理员权限或开发者模式
/// @param targetPath 目标路径
/// @param linkPath 符号链接路径（必须尚不存在）
/// @param isDirectory 是否创建目录符号链接(仅在 Windows 上有效)
/// @return 错误码
AST_UTIL_CAPI errc_t aCreateSymbolicLink(StringView targetPath, StringView linkPath, bool isDirectory);


/// @brief 创建目录链接
/// @details windows 下通过创建目录联接实现，linux 下通过创建符号链接实现
/// @param targetPath 本地目录路径（相对路径按当前目录解析）
/// @param linkPath 链接路径（必须是尚不存在的目录）
/// @return 错误码
A_ALWAYS_INLINE
errc_t aCreateDirectoryLink(StringView targetPath, StringView linkPath)
{
#ifdef _WIN32
    return aCreateJunction(targetPath, linkPath);
#else
    return aCreateSymbolicLink(targetPath, linkPath, true);
#endif
}


/// @brief 尝试获取短路径
/// @param path 原始路径
/// @return 短路径
AST_UTIL_API std::string aTryGetShortPath(StringView path);


/*! @} */

AST_NAMESPACE_END
