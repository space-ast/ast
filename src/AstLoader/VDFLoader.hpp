///
/// @file      VDFLoader.hpp
/// @brief     VDF (Visual Data File) 加载器
/// @details   将 STK VDF 打包文件解包还原为目录结构，然后加载场景。
/// @author    axel
/// @date      2026-07-26
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

AST_NAMESPACE_BEGIN

class Scenario;

/// @brief 从 VDF 打包文件加载场景
/// @details 自动解压 gzip、解包还原目录结构、然后加载场景。
///          支持 .vdf（gzip 压缩）和已解压的 .vdf 文件。
/// @param filepath VDF 文件路径
/// @param scenario 输出的场景对象
/// @return eNoError 成功，否则返回错误码
AST_LOADER_API errc_t aLoadVDF(StringView filepath, Scenario& scenario);

/// @brief 将 VDF 打包文件解包到指定目录
/// @details 解压 gzip 后将 VDF 内所有文件还原到 outputDir 目录。
/// @param filepath VDF 文件路径
/// @param outputDir 输出目录路径
/// @return eNoError 成功，否则返回错误码
AST_LOADER_API errc_t aExtractVDF(StringView filepath, StringView outputDir);

AST_NAMESPACE_END
