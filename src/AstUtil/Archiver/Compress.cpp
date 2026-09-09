#include "Compress.hpp"
#include "ArchiverUtils.hpp"
#include "AstUtil/CompressorImplRaw.hpp"
#include "AstUtil/CompressorImplTar.hpp"
#include "AstUtil/CompressorImplSystem.hpp"

#ifdef _WIN32
#include "AstUtil/CompressorImplShellCOM.hpp"
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

    // 2. 纯 C++ TAR 写入器（始终可用，优先于系统命令）
    impl = &CompressorImplTar::Instance();
    if (impl->isSupported() && impl->canCompress(source, target))
        return impl;

    // 3. 系统命令（tar/zip/7z）
    impl = &CompressorImplSystem::Instance();
    if (impl->isSupported() && impl->canCompress(source, target))
        return impl;

    // 4. 原始复制（通用兜底，拒绝归档格式）
    impl = &CompressorImplRaw::Instance();
    if (impl->isSupported() && impl->canCompress(source, target))
        return impl;

    aError(_("未找到受支持的压缩器实现"));
    return nullptr;
}

errc_t aCompress(StringView source, StringView target, StringView curdir)
{
    CompressorInterface* impl = aCompressGetImpl(source, target);
    if (!impl)
    {
        aError(_("没有可用于 %s 的压缩器"), source.data());
        return eErrorNotImplemented;
    }
    return impl->compress(source, target, curdir);
}

AST_NAMESPACE_END
