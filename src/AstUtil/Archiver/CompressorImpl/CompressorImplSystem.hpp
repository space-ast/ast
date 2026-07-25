///
/// @file      CompressorImplSystem.hpp
/// @brief     系统命令后端
/// @details   通过 popen() 调用系统安装的 tar/zip/7z 命令进行压缩。
///           对标 ExtractorImplSystem 模式。
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

#include "AstUtil/CompressorInterface.hpp"
#include "AstUtil/ArchiverUtils.hpp"

#include <string>

AST_NAMESPACE_BEGIN

/// 系统命令后端
/// @details 通过调用系统安装的命令行工具进行压缩：
/// - tar    → .tar, .tar.gz, .tar.bz2, .tar.xz
/// - zip    → .zip
/// - 7z     → .7z
class AST_UTIL_API CompressorImplSystem : public CompressorInterface
{
public:
    static CompressorImplSystem& Instance();

    CompressorImplSystem() = default;
    virtual ~CompressorImplSystem() = default;

    virtual errc_t compress(StringView source, StringView target, StringView curdir = {}) const override;
    virtual bool isSupported() const override;
    virtual bool canCompress(StringView source, StringView target) const override;
};

AST_NAMESPACE_END
