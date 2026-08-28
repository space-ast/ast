///
/// @file      DownloadLfs.hpp
/// @brief     Git LFS 对象下载
/// @details   提供从 Git LFS 批量接口下载对象的函数。
///            流程：解析指针 -> 推导端点 -> batch POST 换取签发下载地址 -> GET 下载 -> 落盘。
///            复用 AstUtil/Network 与 AstUtil/ParseFormat/Json 现有能力。
/// @author    axel
/// @date      2026-08-27
/// @copyright 版权所有 (C) 2026-present, ast项目.
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
#include <cstddef>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Network
    @{
*/

/// Git LFS 指针信息
/// @details 从指针文本解析出的对象元信息：内容 sha256（oid）与字节数。
struct LfsPointerInfo
{
    std::string oid{};          ///< 对象内容的 sha256 十六进制串
    std::size_t size = 0;       ///< 对象字节数
};

/// 解析 Git LFS 指针文本
/// @details 指针是形如下面几行的纯文本，不含任何下载地址，只有对象指纹：
///     version https://git-lfs.github.com/spec/v1
///     oid sha256:06b7...    <- 对象内容的 sha256 十六进制
///     size 61440            <- 对象字节数
/// @param pointerText 指针文本（可直接取文件内容）
/// @param info 输出解析结果；缺 oid 时报错并保持 info 为空
/// @return eNoError 成功；eErrorNotFound 缺少 oid；eErrorInvalidParam 参数为空
AST_UTIL_API errc_t aLfsParsePointer(StringView pointerText, LfsPointerInfo& info);

/// 从 git 远端地址推导 LFS 批量端点
/// @details 规则为「远端地址 + /info/lfs」，并【保留 .git 后缀】。
///     https://github.com/<owner>/<repo>.git
///         -> https://github.com/<owner>/<repo>.git/info/lfs
/// @note GitHub 的 LFS 端点必须保留 .git，去掉会返回 422。
/// @param remoteUrl git 远端地址
/// @return LFS 批量端点
AST_UTIL_API std::string aLfsEndpoint(StringView remoteUrl);

/// 构建 Git LFS batch download 请求体
/// @details 生成形如 {"operation":"download","transfers":["basic"],
///          "objects":[{"oid":..,"size":..}]} 的 JSON 文本。
/// @param oid 对象 sha256 十六进制
/// @param size 对象字节数
/// @return 请求体 JSON 文本
AST_UTIL_API std::string aLfsBatchRequestBody(StringView oid, std::size_t size);

/// 下载 Git LFS 对象到本地文件
/// @details 发送 POST batch 请求换取带签名的下载地址，再 GET 下载。
///          对象先写入 outputFile+".part"，成功后才重命名为 outputFile，
///          任何失败都会清理临时文件，避免在目标路径残留截断/伪冒文件。
/// @param pointerFile 指针文件路径
/// @param outputFile 目标文件路径
/// @param remoteUrl git 远端地址（如 https://github.com/<owner>/<repo>.git）
/// @return eNoError 表示下载成功
AST_UTIL_CAPI errc_t aDownloadLfs(StringView pointerFile, StringView outputFile, StringView remoteUrl);

/*! @} */

AST_NAMESPACE_END
