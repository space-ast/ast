///
/// @file      ExtractorImplTar.hpp
/// @brief     纯 C++ TAR 格式解析后端
/// @details   从头实现 TAR 归档文件解析器，零依赖。
///           支持 USTAR 格式和 GNU 长文件名扩展。
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

#include "AstUtil/ExtractorInterface.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/ArchiverUtils.hpp"

#include <string>

AST_NAMESPACE_BEGIN

/// 纯 C++ TAR 解析后端
/// @details 从头实现 TAR 归档文件读取，不依赖任何第三方库。
/// 支持 USTAR 格式和 GNU 长文件名扩展。仅处理未压缩的 .tar 文件。
class AST_UTIL_API ExtractorImplTar : public ExtractorInterface
{
public:
    static ExtractorImplTar& Instance();

    ExtractorImplTar() = default;
    virtual ~ExtractorImplTar() = default;

    virtual errc_t extract(StringView source, StringView target) const override;
    virtual bool canExtract(StringView source) const override;

private:
    /// 创建一个目录条目（typeflag '5'）
    static errc_t createDirectory(const std::string& entryName, const std::string& targetDir);

    /// 创建一个普通文件条目（typeflag '0' 或 '\0'）
    static errc_t createFile(FILE* src, size_t fileSize, const std::string& entryName, const std::string& targetDir);
};

AST_NAMESPACE_END
