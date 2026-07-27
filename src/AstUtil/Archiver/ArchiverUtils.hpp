///
/// @file      ArchiverUtils.hpp
/// @brief     归档格式检测工具
/// @details   通过文件扩展名和魔数检测归档文件格式
/// @author    axel
/// @date      2026-07-25
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
#include "AstUtil/FileSystem.hpp"

#include <cstdio>
#include <cstddef>

AST_NAMESPACE_BEGIN

/// TAR 头部结构（USTAR 格式，512 字节）
/// @details 由 CompressorImplTar 和 ExtractorImplTar 共享
struct TarHeader
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12]; // 填充到 512 字节
};

// 确保结构体大小正确
static_assert(sizeof(TarHeader) == 512, "TarHeader must be exactly 512 bytes");

/// 计算 TAR 头部校验和（供 CompressorImplTar 使用）
AST_UTIL_API void aTarCalculateChecksum(TarHeader& header);

/// 将八进制字符串解析为 size_t（供 ExtractorImplTar 使用）
AST_UTIL_API size_t aTarParseOctal(const char* field, size_t len);

/// 检查头部块是否全为零（归档结束标记）
AST_UTIL_API bool aTarIsZeroBlock(const TarHeader& header);

/// 归档文件格式枚举
enum class EArchiveFormat
{
    eUnknown,    ///< 未知格式
    eTar,        ///< .tar
    eTarGz,      ///< .tar.gz / .tgz
    eTarBz2,     ///< .tar.bz2 / .tbz2
    eTarXz,      ///< .tar.xz / .txz
    eZip,        ///< .zip
    e7z,         ///< .7z
    eRar,        ///< .rar
    eGz,         ///< .gz
    eDirectory,  ///< 目录（非归档文件）
    eFile,       ///< 普通文件（非归档文件）
};

/// @brief 检测归档文件格式
/// @details 优先通过文件扩展名检测，必要时读取魔数验证
/// @param path 文件路径
/// @return EArchiveFormat 检测到的格式
AST_UTIL_API EArchiveFormat aDetectArchiveFormat(StringView path);

/// @brief 检查文件是否为归档格式（可解压的压缩包）
/// @param path 文件路径
/// @return true 如果是归档文件
AST_UTIL_API bool aIsArchiveFile(StringView path);

#ifndef SWIG

/// @brief 计算归档路径：根据 source 和 curdir 解析出工作目录和相对路径
/// @details 压缩后端共用：Tar、System、ShellCOM 统一使用此函数计算归档内路径
/// @param source 源路径
/// @param curdir 工作目录（空串=自动取 source 的父目录）
/// @param outWorkDir 输出的工作目录（带尾部 /）
/// @param outRelativePath 输出的相对路径（"." 表示工作目录本身）
void aResolveArchivePath(StringView source, StringView curdir,
                          std::string& outWorkDir, std::string& outRelativePath);

/// @brief 复制单个文件（供 Raw 后端共享）
/// @param from 源文件路径
/// @param to 目标文件路径
/// @param logPrefix 错误日志前缀
/// @return eNoError 成功，eError 失败
errc_t aCopyFile(StringView from, StringView to, const char* logPrefix);

/// @brief 从文件流复制指定字节数到目标流（分块读写）
/// @param src 已打开的源文件流
/// @param dst 已打开的目标文件流
/// @param size 要复制的字节数
/// @return eNoError 成功，否则返回错误码
AST_UTIL_API errc_t aCopyFileRange(FILE* src, FILE* dst, size_t size);

/// @brief 递归复制目录（供 Raw 后端共享）
/// @param srcDir 源目录路径
/// @param dstDir 目标目录路径
/// @param logPrefix 错误日志前缀
/// @return eNoError 成功，eError 失败
errc_t aCopyDirectoryRecursive(const fs::path& srcDir, const fs::path& dstDir, const char* logPrefix);

/// @brief 检查系统命令是否可用（通过执行测试标志验证）
/// @param cmd 命令名称（如 "tar", "zip", "7z"）
/// @return true 如果命令可用
bool aIsCommandAvailable(const char* cmd);

/// @brief 执行 shell 命令并捕获输出
/// @param cmd 完整的命令行字符串
/// @return eNoError 成功，eError 失败
errc_t aRunCommand(const std::string& cmd);

/// @brief 获取 tar 压缩格式对应的 flag 字符
/// @param fmt 归档格式
/// @return flag 字符串（"z"/"j"/"J"），非压缩格式返回 nullptr
const char* aTarCompressFlag(EArchiveFormat fmt);

#endif

AST_NAMESPACE_END
