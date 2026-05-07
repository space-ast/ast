///
/// @file      NetworkAPI.hpp
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
#include "NetworkCookieJar.hpp"
#include "NetworkCookie.hpp"
#include "NetworkInterface.hpp"
#include "NetworkSession.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class NetworkRequest;
class NetworkResponse;
class NetworkInterface;

/// 网络实现
enum class ENetworkImplType {
    eDefault,    // 默认选择实现
    eWinHTTP,    // WinHTTP
    eWinINet,    // WinINet
    eCurlCmd,    // curl命令实现
};

/// 发送网络请求
/// @details   发送网络请求，返回网络响应
/// @param request 网络请求
/// @param response 网络响应
/// @note 该接口会阻塞调用线程，直到网络请求完成或超时为止
/// @warning 注意不要通过该接口发送敏感数据，如密码、API密钥等，
/// 因为该接口的其中一个实现使用了临时文件与curl命令行工具交互，存在安全风险
/// @return 错误码
AST_UTIL_CAPI errc_t aNetworkRequest(const NetworkRequest& request, NetworkResponse& response);


/// 设置网络实现
/// @details   设置网络实现，用于在不同接口实现之间切换
/// @param impl 网络实现
/// @return 错误码
AST_UTIL_API errc_t aNetworkSetImpl(ENetworkImplType impltype);


/// 设置网络实现
/// @details   设置网络实现，用于在不同接口实现之间切换
/// @param impl 网络实现
/// @return 错误码
AST_UTIL_API void aNetworkSetImpl(NetworkInterface* impl);


/*! @} */

AST_NAMESPACE_END
