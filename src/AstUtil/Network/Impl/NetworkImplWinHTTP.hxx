///
/// @file      NetworkImplWinHTTP.hpp
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

#pragma once

#include "AstUtil/NetworkInterface.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// 网络接口实现，通过WinHTTP API与网络进行交互
/// @details   网络实现，使用 WinHTTP API 与网络进行交互。
/// @warning   该实现仅在 Windows 平台上可用
class NetworkImplWinHTTP : public NetworkInterface
{
public:
    static NetworkImplWinHTTP* Instance();
    
    NetworkImplWinHTTP();
    
    virtual ~NetworkImplWinHTTP();
    
    virtual errc_t request(const NetworkRequest& request, NetworkResponse& response) override;

    /// @brief 检查是否支持该网络实现
    /// @return true 如果支持，false 否则
    bool isSupported() const override;

private:
    class Impl;
    Impl* impl_;
};


/*! @} */

AST_NAMESPACE_END