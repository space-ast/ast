///
/// @file      CompressorImplRaw.hpp
/// @brief     原始复制后端
/// @details   将文件或目录直接复制到目标位置，始终可用，作为通用兜底方案。
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
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/// 原始复制后端
/// @details 将文件或目录直接复制到目标位置，不依赖任何外部工具。
/// 始终可用，作为回退链的最终兜底方案。
class AST_UTIL_API CompressorImplRaw : public CompressorInterface
{
public:
    static CompressorImplRaw& Instance();

    CompressorImplRaw() = default;
    virtual ~CompressorImplRaw() = default;

    virtual errc_t compress(StringView source, StringView target, StringView curdir = {}) const override;
};

AST_NAMESPACE_END
