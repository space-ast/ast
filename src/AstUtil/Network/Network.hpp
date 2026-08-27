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
#include <functional>
#include <cstdint>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class NetworkRequest;
class NetworkResponse;
class NetworkInterface;
class NetworkStreamReceiver;

/// 网络实现
enum class ENetworkImplType {
    eDefault,    // 默认选择实现
    eWinHTTP,    // WinHTTP
    eWinINet,    // WinINet
    eCurlCmd,    // curl命令实现
};

/// 控制台进度条（可移动，无静态状态），可作为 DownloadProgressCallback 直接传入
/// @details 下载过程中输出到 stderr；若 stdin 为交互式终端，用户可按 q 或 ESC 主动取消下载
///          （operator() 返回 false，aDownloadFile 返回 eErrorCancelled）。
class AST_UTIL_API ConsoleProgressBar
{
public:
    explicit ConsoleProgressBar(const std::string& label = "downloading");
    /// @brief 打印进度，并检测用户是否按下取消键
    /// @return true 继续下载；false 用户已取消（下载中止）
    bool operator()(uint64_t downloaded, uint64_t total);
private:
    /// 检测 stdin 是否有取消键按下（仅在交互式终端时返回 true 可能为真）
    bool cancelKeyPressed() const;
    std::string label_;
    int lastLen_ = 0;     ///< 上一个输出行的长度，用于 \r 覆盖清除残留
    bool interactive_ = false; ///< stdin 是否为交互式终端（决定是否轮询取消键）
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

/// 发送流式网络请求
/// @details 发送网络请求，数据到达时通过 receiver 逐块回调
/// @param request 网络请求
/// @param receiver 流式数据接收器
/// @return 错误码
AST_UTIL_CAPI errc_t aNetworkRequestStream(const NetworkRequest& request, NetworkStreamReceiver& receiver);



/// 下载 URL 内容并直接写入文件，流式写盘并实时上报进度
/// @details 发送 HTTP GET 请求，将响应体以流式方式直接写入文件，
///          避免整文件缓冲在内存中，并通过 progress 逐块上报进度。
/// @param url 下载地址
/// @param filepath 目标文件路径
/// @param progress 进度回调，下载过程中被多次调用；total 为 0 时总大小未知
/// @return eNoError 表示下载成功
AST_UTIL_API errc_t aDownloadFile(
    const std::string& url, const std::string& filepath,
    const DownloadProgressCallback& progress
);


/// @brief 下载 URL 内容并直接写入文件
/// @details 发送 HTTP GET 请求，将响应内容写入指定文件路径。
/// @param url 下载地址
/// @param filepath 目标文件路径
/// @return eNoError 表示下载成功
AST_UTIL_API errc_t aDownloadFile(const std::string& url, const std::string& filepath);


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
