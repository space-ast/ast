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
    // 同 aCompress：按优先级依次尝试，而不是只认第一个可用的实现。
    // Shell COM 解压同样走异步的 CopyHere，部分环境下会静默超时后失败，
    // 此时换个后端仍能解出同样的内容。
    ExtractorInterface* impls[5];
    size_t count = 0;
#ifdef _WIN32
    impls[count++] = &ExtractorImplShellCOM::Instance();
    impls[count++] = &ExtractorImplPowerShell::Instance();
#endif
    impls[count++] = &ExtractorImplTar::Instance();
    impls[count++] = &ExtractorImplSystem::Instance();
    impls[count++] = &ExtractorImplRaw::Instance();

    bool found = false;
    for (size_t i = 0; i < count; ++i)
    {
        ExtractorInterface* impl = impls[i];
        if (!impl->isSupported() || !impl->canExtract(source))
            continue;
        found = true;
        errc_t ret = impl->extract(source, target);
        if (ret == eNoError)
            return eNoError;
        aWarning(_("第 %d 个解压器实现失败(错误码 %d)，尝试下一个"), (int)(i + 1), (int)ret);
    }

    if (!found)
    {
        aError(_("没有可用于 %s 的解压器"), source.data());
        return eErrorNotImplemented;
    }
    aError(_("所有解压器实现均失败: %s"), source.data());
    return eError;
}

AST_NAMESPACE_END
