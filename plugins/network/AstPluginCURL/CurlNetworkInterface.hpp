///
/// @file      CurlNetworkInterface.hpp
/// @brief     基于 libcurl 的网络实现
/// @details   插件根对象同时继承 PluginInstance 与 NetworkInterface，
///            主程序通过 aPluginCast<NetworkInterface> 获取本实现。
/// @author    axel
/// @date      2026-11-04
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/PluginInterface.hpp"
#include "AstUtil/NetworkInterface.hpp"

AST_NAMESPACE_BEGIN

/// @brief 基于 libcurl 的网络实现
/// @details 实例无状态，每次请求创建独立的 CURL easy handle，
///          因此可由多个线程并发调用。
class CurlNetworkInterface : public PluginInstance, public NetworkInterface
{
public:
    CurlNetworkInterface() = default;
    ~CurlNetworkInterface() override = default;

    errc_t requestStream(const NetworkRequest& request,
                         NetworkStreamReceiver& receiver) override;

    bool isSupported() const override;
};

AST_NAMESPACE_END
