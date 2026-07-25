#include "CompressorImplRaw.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "../ArchiverUtils.hpp"

#include <string>

AST_NAMESPACE_BEGIN

CompressorImplRaw& CompressorImplRaw::Instance()
{
    static CompressorImplRaw instance;
    return instance;
}

errc_t CompressorImplRaw::compress(StringView source, StringView target, StringView /*curdir*/) const
{
    if (source.empty() || target.empty())
    {
        aError("CompressorImplRaw: source or target is empty");
        return eErrorInvalidParam;
    }

    fs::path srcPath(source.data());
    fs::path dstPath(target.data());

    if (!fs::exists(srcPath))
    {
        aError("CompressorImplRaw: source does not exist: %s", source.data());
        return eErrorInvalidFile;
    }

    if (fs::is_directory(srcPath))
        return aCopyDirectoryRecursive(srcPath, dstPath, "CompressorImplRaw");
    else
        return aCopyFile(source, target, "CompressorImplRaw");
}

AST_NAMESPACE_END
