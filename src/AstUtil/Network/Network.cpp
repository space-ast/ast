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
#include "Impl/NetworkImplWinHTTP.hxx"
#include "Impl/NetworkImplWinINet.hxx"
#include "Impl/NetworkImplCurlCmd.hxx"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

static NetworkInterface* s_interface = nullptr;



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
    s_interface = impl;
    return eNoError;
}

void aNetworkSetImpl(NetworkInterface* impl)
{
    if (impl == nullptr)
        return;
    s_interface = impl;
}



errc_t aNetworkRequest(const NetworkRequest& request, NetworkResponse& response)
{
    if (s_interface == nullptr)
    {
        s_interface = aNetworkGetImplDefault();
        if (s_interface == nullptr)
            return eErrorNullPtr;
    }
    return s_interface->request(request, response);
}



AST_NAMESPACE_END
