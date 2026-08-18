#include "CompressorImplRaw.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/ArchiverUtils.hpp"

#include <string>

AST_NAMESPACE_BEGIN

CompressorImplRaw& CompressorImplRaw::Instance()
{
    static CompressorImplRaw instance;
    return instance;
}

bool CompressorImplRaw::canCompress(StringView /*source*/, StringView target) const
{
    EArchiveFormat fmt = aDetectArchiveFormat(target);
    switch (fmt)
    {
    case EArchiveFormat::eTar:
    case EArchiveFormat::eTarGz:
    case EArchiveFormat::eTarBz2:
    case EArchiveFormat::eTarXz:
    case EArchiveFormat::eZip:
    case EArchiveFormat::e7z:
    case EArchiveFormat::eRar:
    case EArchiveFormat::eGz:
        return false;
    default:
        return true;
    }
}

errc_t CompressorImplRaw::compress(StringView source, StringView target, StringView /*curdir*/) const
{
    if (source.empty() || target.empty())
    {
        aError("CompressorImplRaw: source or target is empty");
        return eErrorInvalidParam;
    }

    fs::path srcPath = std::string(source);
    fs::path dstPath = std::string(target);

    if (!fs::exists(srcPath))
    {
        aError("source does not exist: %s", srcPath.string().c_str());
        return eErrorInvalidFile;
    }

    if (fs::is_directory(srcPath))
        return aCopyDirectoryRecursive(srcPath, dstPath, "CompressorImplRaw");
    else
        return aCopyFile(source, target, "CompressorImplRaw");
}

AST_NAMESPACE_END
