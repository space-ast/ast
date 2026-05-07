///
/// @file      NetworkImplCurlCmd.hpp
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

#include "AstGlobal.h"
#include "AstUtil/NetworkInterface.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// 网络接口实现，通过curl命令与网络进行交互
/// @details 该实现使用curl命令行工具与网络进行交互，支持GET、POST、PUT、DELETE等HTTP方法
/// @note 该实现需要已安装curl命令行工具，一般操作系统已默认安装
/// @warning 注意不要通过该实现发送敏感数据，如密码、API密钥等
/// 因为该实现使用了临时文件与curl命令行工具交互
class NetworkImplCurlCmd : public NetworkInterface
{
public:
    static NetworkImplCurlCmd* Instance();

    NetworkImplCurlCmd() = default;
    
    virtual ~NetworkImplCurlCmd() = default;
    
    virtual errc_t request(const NetworkRequest& request, NetworkResponse& response) override;

    /// @brief 检查是否支持该网络实现
    /// @return true 如果支持，false 否则
    virtual bool isSupported() const override;
};


/*! @} */

AST_NAMESPACE_END
