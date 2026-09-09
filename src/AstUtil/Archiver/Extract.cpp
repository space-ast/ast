#include "Extract.hpp"
#include "ArchiverUtils.hpp"
#include "AstUtil/ExtractorImplRaw.hpp"
#include "AstUtil/ExtractorImplTar.hpp"
#include "AstUtil/ExtractorImplSystem.hpp"

#ifdef _WIN32
#include "AstUtil/ExtractorImplShellCOM.hpp"
#include "AstUtil/ExtractorImplPowerShell.hpp"
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
    // 3. 纯 C++ TAR 解析器（始终可用，优先于系统命令）
    impl = &ExtractorImplTar::Instance();
    if (impl->isSupported() && impl->canExtract(source))
        return impl;

    // 4. 系统命令（tar/unzip/7z）
    impl = &ExtractorImplSystem::Instance();
    if (impl->isSupported() && impl->canExtract(source))
        return impl;

    // 5. 原始复制（通用兜底）
    impl = &ExtractorImplRaw::Instance();
    if (impl->isSupported() && impl->canExtract(source))
        return impl;

    aError(_("未找到受支持的解压器实现"));
    return nullptr;
}

errc_t aExtract(StringView source, StringView target)
{
    ExtractorInterface* impl = aExtractGetImpl(source);
    if (!impl)
    {
        aError(_("没有可用于 %s 的解压器"), source.data());
        return eErrorNotImplemented;
    }
    return impl->extract(source, target);
}

AST_NAMESPACE_END
