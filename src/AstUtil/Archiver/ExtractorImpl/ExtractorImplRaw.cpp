#include "ExtractorImplRaw.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/ArchiverUtils.hpp"

#include <string>

AST_NAMESPACE_BEGIN

ExtractorImplRaw& ExtractorImplRaw::Instance()
{
    static ExtractorImplRaw instance;
    return instance;
}

errc_t ExtractorImplRaw::extract(StringView source, StringView target) const
{
    if (source.empty() || target.empty())
    {
        aError("ExtractorImplRaw: source or target is empty");
        return eErrorInvalidParam;
    }

    fs::path srcPath = std::string(source);
    fs::path dstPath = std::string(target);

    if (!fs::exists(srcPath))
    {
        aError("source does not exist: '%.*s'", source.size(), source.data());
        return eErrorInvalidFile;
    }

    // 确保目标目录存在
    if (!fs::exists(dstPath))
    {
        if (!fs::create_directories(dstPath))
        {
            aError("cannot create target directory: '%.*s'", target.size(), target.data());
            return eErrorInvalidFile;
        }
    }

    if (fs::is_directory(srcPath))
        return aCopyDirectoryRecursive(srcPath, dstPath, "ExtractorImplRaw");
    else
    {
        auto srcFilename = srcPath.filename();
        auto dstFilePath = dstPath / srcFilename;
        return aCopyFile(srcPath.string(), dstFilePath.string(), "ExtractorImplRaw");
    }
}

AST_NAMESPACE_END
