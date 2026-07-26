///
/// @file      ExtractorImplPowerShell.hpp
/// @brief     PowerShell .gz 解压后端
/// @details   通过 PowerShell + .NET System.IO.Compression.GZipStream 解压
///           单文件 .gz。在 Windows 7（无 tar/7z）环境下始终可用。
///           兼容 PowerShell 2.0 (.NET 3.5 SP1)。
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

#if defined(_WIN32) && !defined(SWIG)

#include "AstUtil/ExtractorInterface.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/// PowerShell .gz 解压后端
/// @details 通过 PowerShell + .NET System.IO.Compression.GZipStream 解压
/// 单文件 .gz。在 Windows 上始终可用（PowerShell 自 Windows 7 起内置）。
/// 兼容 PowerShell 2.0 (.NET 3.5 SP1)。
class AST_UTIL_API ExtractorImplPowerShell : public ExtractorInterface
{
public:
    static ExtractorImplPowerShell& Instance();

    ExtractorImplPowerShell() = default;
    virtual ~ExtractorImplPowerShell() = default;

    virtual errc_t extract(StringView source, StringView target) const override;
    virtual bool canExtract(StringView source) const override;
    virtual bool isSupported() const override;
};

AST_NAMESPACE_END

#endif // _WIN32
