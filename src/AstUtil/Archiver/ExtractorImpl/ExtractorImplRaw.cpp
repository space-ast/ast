#include "ExtractorImplRaw.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "../ArchiverUtils.hpp"

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

    fs::path srcPath(source.data());
    fs::path dstPath(target.data());

    if (!fs::exists(srcPath))
    {
        aError("ExtractorImplRaw: source does not exist: %s", source.data());
        return eErrorInvalidFile;
    }

    // 确保目标目录存在
    if (!fs::exists(dstPath))
    {
        if (!fs::create_directories(dstPath))
        {
            aError("ExtractorImplRaw: cannot create target directory: %s", target.data());
            return eErrorInvalidFile;
        }
    }

    if (fs::is_directory(srcPath))
        return aCopyDirectoryRecursive(srcPath, dstPath, "ExtractorImplRaw");
    else
    {
        auto srcFilename = srcPath.filename();
        auto dstFilePath = dstPath / srcFilename;
        std::string dstFileStr = dstFilePath.string();
        return aCopyFile(source, dstFileStr.c_str(), "ExtractorImplRaw");
    }
}

AST_NAMESPACE_END
