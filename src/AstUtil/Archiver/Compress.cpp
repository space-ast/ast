#include "Compress.hpp"
#include "ArchiverUtils.hpp"
#include "CompressorImpl/CompressorImplRaw.hpp"
#include "CompressorImpl/CompressorImplTar.hpp"
#include "CompressorImpl/CompressorImplSystem.hpp"

#ifdef _WIN32
#include "CompressorImpl/CompressorImplShellCOM.hpp"
#endif

#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

CompressorInterface* aCompressGetImpl(StringView source, StringView target)
{
    CompressorInterface* impl = nullptr;

    // 1. Shell COM（仅 Windows）
#ifdef _WIN32
    impl = &CompressorImplShellCOM::Instance();
    if (impl->isSupported() && impl->canCompress(source, target))
        return impl;
#endif

    // 2. 系统命令（tar/zip/7z）
    impl = &CompressorImplSystem::Instance();
    if (impl->isSupported() && impl->canCompress(source, target))
        return impl;

    // 3. 纯 C++ TAR 写入器
    impl = &CompressorImplTar::Instance();
    if (impl->isSupported() && impl->canCompress(source, target))
        return impl;

    // 4. 原始复制（通用兜底）
    impl = &CompressorImplRaw::Instance();
    if (impl->isSupported())
        return impl;

    aError("aCompressGetImpl: no supported compressor implementation found");
    return nullptr;
}

errc_t aCompress(StringView source, StringView target, StringView curdir)
{
    CompressorInterface* impl = aCompressGetImpl(source, target);
    if (!impl)
    {
        aError("aCompress: no compressor available for: %s", source.data());
        return eErrorNotImplemented;
    }
    return impl->compress(source, target, curdir);
}

AST_NAMESPACE_END
