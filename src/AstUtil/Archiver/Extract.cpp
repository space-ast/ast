#include "Extract.hpp"
#include "ArchiverUtils.hpp"
#include "ExtractorImpl/ExtractorImplRaw.hpp"
#include "ExtractorImpl/ExtractorImplTar.hpp"
#include "ExtractorImpl/ExtractorImplSystem.hpp"

#ifdef _WIN32
#include "ExtractorImpl/ExtractorImplShellCOM.hpp"
#include "ExtractorImpl/ExtractorImplPowerShell.hpp"
#endif

#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

ExtractorInterface* aExtractGetImpl(StringView source)
{
    ExtractorInterface* impl = nullptr;

    // 1. Shell COM（仅 Windows）
#ifdef _WIN32
    impl = &ExtractorImplShellCOM::Instance();
    if (impl->isSupported() && impl->canExtract(source))
        return impl;
    // 2. PowerShell（仅 Windows，.gz 单文件）
    impl = &ExtractorImplPowerShell::Instance();
    if (impl->isSupported() && impl->canExtract(source))
        return impl;
#endif
    // 3. 系统命令（tar/unzip/7z）
    impl = &ExtractorImplSystem::Instance();
    if (impl->isSupported() && impl->canExtract(source))
        return impl;

    // 4. 纯 C++ TAR 解析器
    impl = &ExtractorImplTar::Instance();
    if (impl->isSupported() && impl->canExtract(source))
        return impl;

    // 5. 原始复制（通用兜底）
    impl = &ExtractorImplRaw::Instance();
    if (impl->isSupported())
        return impl;

    aError("aExtractGetImpl: no supported extractor implementation found");
    return nullptr;
}

errc_t aExtract(StringView source, StringView target)
{
    ExtractorInterface* impl = aExtractGetImpl(source);
    if (!impl)
    {
        aError("aExtract: no extractor available for: %s", source.data());
        return eErrorNotImplemented;
    }
    return impl->extract(source, target);
}

AST_NAMESPACE_END
