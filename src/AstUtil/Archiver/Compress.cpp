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
    // 按优先级依次尝试，而不是只认第一个可用的实现。
    // Shell COM 依赖系统 Shell 的异步复制：CopyHere 会立刻返回成功，真正干活的是
    // 后台的副本引擎，部分环境下它始终不落地（表现为轮询超时 30s 后才失败）。
    // 这种情况下换个后端仍能产出同样的归档，不该让整个压缩直接失败。
    CompressorInterface* impls[4];
    size_t count = 0;
#ifdef _WIN32
    impls[count++] = &CompressorImplShellCOM::Instance();
#endif
    impls[count++] = &CompressorImplTar::Instance();
    impls[count++] = &CompressorImplSystem::Instance();
    impls[count++] = &CompressorImplRaw::Instance();

    bool found = false;
    for (size_t i = 0; i < count; ++i)
    {
        CompressorInterface* impl = impls[i];
        if (!impl->isSupported() || !impl->canCompress(source, target))
            continue;
        found = true;
        errc_t ret = impl->compress(source, target, curdir);
        if (ret == eNoError)
            return eNoError;
        aWarning(_("第 %d 个压缩器实现失败(错误码 %d)，尝试下一个"), (int)(i + 1), (int)ret);
    }

    if (!found)
    {
        aError(_("没有可用于 %s 的压缩器"), source.data());
        return eErrorNotImplemented;
    }
    aError(_("所有压缩器实现均失败: %s"), target.data());
    return eError;
}

AST_NAMESPACE_END
