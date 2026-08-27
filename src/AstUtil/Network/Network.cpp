///
/// @file      NetworkAPI.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-24
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "Network.hpp"
#include "NetworkInterface.hpp"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
#include <io.h>
#include <conio.h>
#else
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#endif
#include "NetworkStreamReceiver.hpp"
#include "AstUtil/NetworkImplWinHTTP.hpp"
#include "AstUtil/NetworkImplWinINet.hpp"
#include "AstUtil/NetworkImplCurlCmd.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/IO.hpp"

AST_NAMESPACE_BEGIN

static std::atomic<NetworkInterface*> s_interface{nullptr};




NetworkInterface* aNetworkGetImplDefault()
{
    NetworkInterface* impl = nullptr;
    impl = NetworkImplWinHTTP::Instance();
    if (impl != nullptr && impl->isSupported())
        return impl;
    impl = NetworkImplWinINet::Instance();
    if (impl != nullptr && impl->isSupported())
        return impl;
    impl = NetworkImplCurlCmd::Instance();
    if (impl != nullptr && impl->isSupported())
        return impl;
    aError("no supported network implementation has been found");
    return nullptr;
}

errc_t aNetworkSetImpl(ENetworkImplType impltype)
{
    NetworkInterface* impl = nullptr;
    switch (impltype)
    {
    case ENetworkImplType::eWinHTTP:
        impl = NetworkImplWinHTTP::Instance();
        break;
    case ENetworkImplType::eWinINet:
        impl = NetworkImplWinINet::Instance();
        break;
    case ENetworkImplType::eCurlCmd:
        impl = NetworkImplCurlCmd::Instance();
        break;
    case ENetworkImplType::eDefault:
        impl = aNetworkGetImplDefault();
        break;
    }
    if (impl == nullptr || !impl->isSupported())
    {
        aError("given network implementation not supported, try to use default implementation instead");
        impl = aNetworkGetImplDefault();
    }
    s_interface.store(impl, std::memory_order_release);
    return eNoError;
}

void aNetworkSetImpl(NetworkInterface* impl)
{
    if (impl == nullptr)
        return;
    s_interface.store(impl, std::memory_order_release);
}



errc_t aNetworkRequest(const NetworkRequest& request, NetworkResponse& response)
{
    auto* impl = s_interface.load(std::memory_order_acquire);
    if (impl == nullptr)
    {
        impl = aNetworkGetImplDefault();
        if (impl == nullptr)
            return eErrorNullPtr;
        s_interface.store(impl, std::memory_order_release);
    }
    return impl->request(request, response);
}


errc_t aNetworkRequestStream(const NetworkRequest& request, NetworkStreamReceiver& receiver)
{
    auto* impl = s_interface.load(std::memory_order_acquire);
    if (impl == nullptr)
    {
        impl = aNetworkGetImplDefault();
        if (impl == nullptr)
            return eErrorNullPtr;
        s_interface.store(impl, std::memory_order_release);
    }
    return impl->requestStream(request, receiver);
}


errc_t aDownloadFile(const std::string& url, const std::string& filepath)
{
    // 默认带上控制台进度条；若调用方希望无进度显示，可使用 3 参版本传空回调
    return aDownloadFile(url, filepath, ConsoleProgressBar());
}

errc_t aDownloadFile(const std::string& url, const std::string& filepath, const DownloadProgressCallback& progress)
{
    NetworkRequest request;
    request.setMethod(ENetworkRequestMethod::eGet);
    request.setUrl(url);

    FileDownloadReceiver receiver(filepath, progress);
    errc_t err = aNetworkRequestStream(request, receiver);

    if (err != eNoError)
    {
        receiver.discard();
        if (err == eErrorCancelled)
            aWarning("download cancelled for %s", url.c_str());
        else
            aError("download failed for %s (err=%d)", url.c_str(), err);
        return err;
    }

    if (receiver.downloaded() == 0)
    {
        receiver.discard();
        aError("empty body for %s", url.c_str());
        return eErrorInvalidFile;
    }

    return receiver.finish();
}

ConsoleProgressBar::ConsoleProgressBar(const std::string& label)
    : label_(label.empty() ? "downloading" : label)
{
#ifdef _WIN32
    interactive_ = _isatty(_fileno(stdin)) != 0;
#else
    interactive_ = isatty(STDIN_FILENO) != 0;
#endif
}

bool ConsoleProgressBar::cancelKeyPressed() const
{
    if (!interactive_)
        return false;
#ifdef _WIN32
    if (_kbhit())
    {
        int ch = _getch();
        return ch == 'q' || ch == 'Q' || ch == 0x1B;  // q / ESC
    }
#else
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    timeval tv = {0, 0};
    if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0)
    {
        char ch = 0;
        if (read(STDIN_FILENO, &ch, 1) == 1)
            return ch == 'q' || ch == 'Q' || ch == 0x1B;  // q / ESC
    }
#endif
    return false;
}

bool ConsoleProgressBar::operator()(uint64_t downloaded, uint64_t total)
{
    const int barWidth = 30;
    double frac = (total > 0) ? static_cast<double>(downloaded) / static_cast<double>(total) : 0.0;
    if (total > 0 && frac > 1.0)
        frac = 1.0;
    int filled = static_cast<int>(frac * barWidth);
    if (filled > barWidth)
        filled = barWidth;

    std::string bar(barWidth, '-');
    for (int i = 0; i < filled; ++i)
        bar[i] = '#';

    std::string line = "\r" + label_ + " [" + bar + "] ";
    if (total > 0)
    {
        char tail[64];
        snprintf(tail, sizeof(tail), "%3d%%  %s/%s",
                 static_cast<int>(frac * 100.0 + 0.5),
                 aFormatBytes(downloaded).c_str(),
                 aFormatBytes(total).c_str());
        line += tail;
    }
    else
    {
        line += aFormatBytes(downloaded);
    }
    // 仅在可交互取消时提示用户，避免非交互场景产生误导
    if (interactive_)
        line += u8"  (按 q/ESC 取消)";

    // 若新行比上一行短，补空格清除上一行残留
    if (lastLen_ > static_cast<int>(line.size()))
        line += std::string(lastLen_ - line.size(), ' ');

    // 用 posix::fprintf 而非 fwrite：Windows 下首次调用会经 aUTF8Locale() 把控制台设为 CP_UTF8，
    // 使进度条里的中文提示能正确渲染（Linux 上等价于 std::fprintf）。
    posix::fprintf(stderr, "%s", line.c_str());
    fflush(stderr);
    lastLen_ = static_cast<int>(line.size());

    // 下载完成（已知总大小）：换行收尾
    if (total > 0 && downloaded >= total)
    {
        fputc('\n', stderr);
        fflush(stderr);
        lastLen_ = 0;
        return true;
    }

    // 用户按下取消键（q / ESC）：换行并中止下载
    if (cancelKeyPressed())
    {
        fputc('\n', stderr);
        fflush(stderr);
        lastLen_ = 0;
        return false;
    }

    return true;
}

AST_NAMESPACE_END
