///
/// @file      NetworkStreamReceiver.hpp
/// @brief     流式数据接收器 — 纯虚接口
/// @details   NetworkInterface::requestStream() 通过此接口逐块回调响应体数据。
///            用于实现 SSE (Server-Sent Events) 等流式 HTTP 响应场景。
/// @author    axel
/// @date      2026-06-14
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
#include "NetworkResponse.hpp"
#include <map>
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Network
    @{
*/

class NetworkResponse;

/// @brief 流式数据接收器
/// @details 纯虚接口，子类需实现 onData() 方法
class AST_UTIL_API NetworkStreamReceiver
{
public:
    NetworkStreamReceiver() = default;
    virtual ~NetworkStreamReceiver() = default;

    /// @brief 收到 HTTP 响应头（在 onData 之前触发一次）
    /// @param statusCode HTTP 状态码
    /// @param headers 响应头（键值对）
    virtual void onHeaders(int statusCode,
                           const std::map<std::string, std::string>& headers)
    {
    }

    /// @brief 收到一块响应体数据（可能被多次调用）
    /// @param data 数据指针
    /// @param size 数据长度
    /// @return 0 继续接收，非 0 取消流式接收
    virtual errc_t onData(const char* data, size_t size) = 0;

    /// @brief 发生网络错误
    /// @param error 错误码
    virtual void onError(errc_t error) {}

    /// @brief 数据接收完毕（正常完成时触发）
    virtual void onComplete() {}
};


/// @brief 将流式数据收集到 NetworkResponse 的适配器
/// @details 实现 NetworkStreamReceiver，将 onHeaders/onData/onDone 回调
///          累积为 NetworkResponse 对象。供 NetworkInterface 各实现
///          的 request() 方法使用，使其可委托给 requestStream()。
class AST_UTIL_API CollectingStreamReceiver : public NetworkStreamReceiver
{
public:
    explicit CollectingStreamReceiver(NetworkResponse& response)
        : response_(response), body_() {}

    void onHeaders(int statusCode,
                   const std::map<std::string, std::string>& headers) override
    {
        response_.setStatusCode(statusCode);
        response_.setHeaders(headers);
    }

    errc_t onData(const char* data, size_t size) override
    {
        // 防止无限制响应体耗尽内存（上限 100 MB）
        if (body_.size() + size > kMaxBodySize)
            return eErrorOutOfRange;
        body_.append(data, size);
        return 0;
    }

    void onComplete() override
    {
        response_.setBody(body_);
    }

    void onError(errc_t /*error*/) override
    {
        // 保留已累积的部分体数据（用于诊断）
        if (!body_.empty())
            response_.setBody(body_);
    }

private:
    static constexpr size_t kMaxBodySize = 100 * 1024 * 1024;  // 100 MB
    NetworkResponse& response_;
    std::string body_;
};

/*! @} */

AST_NAMESPACE_END
