///
/// @file      NetworkInterface.hpp
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
#include "NetworkRequest.hpp"
#include "NetworkResponse.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

// 判断 HTTP 方法是否允许携带请求体
AST_UTIL_API bool aMethodAllowsBody(const std::string& method);

// 简单验证端口字符串并转换为整数，非法则返回默认端口
AST_UTIL_API int aParsePort(const std::string& portStr, int defaultPort);


/// 网络抽象接口
/// @details   网络抽象接口，用于与网络进行交互
class NetworkInterface
{
public:
    NetworkInterface() = default;
    
    virtual ~NetworkInterface() = default;
    
    /// 发送网络请求
    /// @details   发送网络请求，返回网络响应
    /// @param request 网络请求
    /// @param response 网络响应
    /// @note 该接口会阻塞调用线程，直到网络请求完成或超时为止
    /// @return 错误码
    virtual errc_t request(const NetworkRequest& request, NetworkResponse& response) = 0;

    /// @brief 检查是否支持该网络实现
    /// @details 检查该网络实现是否在当前操作系统上受支持，例如：
    /// 基于curl命令的实现会检查当前操作系统是否安装了curl 命令行工具
    /// 基于WinHTTP的实现会检查是否是Windows操作系统，且支持加载WinHTTP库
    /// 基于WinINet的实现会检查是否是Windows操作系统，且支持加载WinINet库
    /// @return true 如果支持，false 否则
    virtual bool isSupported() const = 0;
};


/*! @} */

AST_NAMESPACE_END
