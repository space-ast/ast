///
/// @file      CompressorImplTar.hpp
/// @brief     纯 C++ TAR 格式写入后端
/// @details   从头实现 TAR 归档文件写入器，零依赖。
///           支持 USTAR 格式。
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
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/StringView.hpp"
#include "ArchiverUtils.hpp"

#include <string>

AST_NAMESPACE_BEGIN

/// 纯 C++ TAR 写入后端
/// @details 从头实现 TAR 归档文件写入，不依赖任何第三方库。
/// 支持 USTAR 格式。仅生成未压缩的 .tar 文件。
class AST_UTIL_API CompressorImplTar : public CompressorInterface
{
public:
    static CompressorImplTar& Instance();

    CompressorImplTar() = default;
    virtual ~CompressorImplTar() = default;

    virtual errc_t compress(StringView source, StringView target, StringView curdir = {}) const override;
    virtual bool canCompress(StringView source, StringView target) const override;

private:
    /// 写入一个文件条目（头部 + 数据 + 填充）
    static errc_t writeFileEntry(FILE* dst, const std::string& name,
                                 const std::string& filePath);

    /// 写入一个目录条目（仅头部）
    static errc_t writeDirectoryEntry(FILE* dst, const std::string& name);

    /// 递归归档目录
    static errc_t archiveDirectory(FILE* dst, const fs::path& dirPath,
                                    const std::string& basePath);
};

AST_NAMESPACE_END
