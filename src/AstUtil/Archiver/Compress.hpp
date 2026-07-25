///
/// @file      Compress.hpp
/// @brief     压缩 API 入口
/// @details   工厂函数与便捷函数，对标 Extract.hpp 中的 aExtractGetImpl() / aExtract()。
///           按优先级探测后端，返回第一个可用的实现。
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

#include "CompressorInterface.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/// @brief 获取默认的压缩实现
/// @details 按优先级探测后端，返回第一个可用的实现：
/// 1. CompressorImplShellCOM — Windows 上对 .zip 始终可用
/// 2. CompressorImplSystem   — 系统安装了 tar/zip/7z
/// 3. CompressorImplTar      — 纯 C++ TAR 写入器
/// 4. CompressorImplRaw      — 始终可用（文件/目录复制）
/// @param source 待压缩的源文件或目录路径（用于 canCompress 检查）
/// @param target 压缩目标路径（用于 canCompress 检查目标格式）
/// @return CompressorInterface* 可用的压缩实现，如果全部不可用则返回 nullptr
AST_UTIL_API CompressorInterface* aCompressGetImpl(StringView source = {}, StringView target = {});

/// @brief 压缩文件或目录到目标位置
/// @details 自动选择最佳后端进行压缩。根据目标扩展名支持 .tar, .tar.gz, .tgz, .tar.bz2, .tbz2, .tar.xz, .txz, .zip, .7z 等格式。
/// @param source 待压缩的文件或目录路径
/// @param target 压缩目标文件路径
/// @param curdir 工作目录，source 在归档中的路径相对于此目录计算
///              （空串=从 source 父目录推断；等于 source=归档根为".")
/// @return eNoError 表示压缩成功，否则返回错误码
AST_UTIL_API errc_t aCompress(StringView source, StringView target, StringView curdir = {});

AST_NAMESPACE_END
