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
#include "AstUtil/Logger.hpp"
#include <map>
#include <string>
#include <functional>
#include <cstdint>

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
    /// @return 0 继续接收；非 0 表示基于响应头拒绝/中止（不会再读取响应体）
    virtual errc_t onHeaders(int statusCode, const std::map<std::string, std::string>& headers) { return eNoError; }

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

    errc_t onHeaders(int statusCode, const std::map<std::string, std::string>& headers) override;
    errc_t onData(const char* data, size_t size) override;
    void onComplete() override;
    void onError(errc_t /*error*/) override;
private:
    static constexpr size_t kMaxBodySize = 1024 * 1024 * 1024;  // 1 GB
    NetworkResponse& response_;
    std::string body_;
};

/// 下载进度回调：downloaded 为已下载字节数；total 为总字节数，为 0 表示未知
/// @return true 继续下载；false 中止下载（aDownloadFile 将返回 eErrorCancelled）
using DownloadProgressCallback = std::function<bool(uint64_t downloaded, uint64_t total)>;



/// @brief 将流式响应体写入文件并在写盘过程中报告下载进度
/// @details 仅在 HTTP 200 后写入临时文件 <filepath>.part，成功时由 finish() 原子改名到 filepath，
///          出错时由外部调用 discard() 清理临时文件；这样失败的下载不会破坏 filepath 上已存在的文件。
class FileDownloadReceiver : public NetworkStreamReceiver
{
public:
    FileDownloadReceiver(const std::string& filepath, const DownloadProgressCallback& progress);

    ~FileDownloadReceiver() override;

    FileDownloadReceiver(const FileDownloadReceiver& other) = delete;
    FileDownloadReceiver& operator=(const FileDownloadReceiver& other) = delete;

    errc_t onHeaders(int statusCode, const std::map<std::string, std::string>& headers) override;
    errc_t onData(const char* data, size_t size) override;
    void onError(errc_t /*error*/) override{}

    uint64_t downloaded() const { return downloaded_; }
    uint64_t total() const { return total_; }

    /// 成功时关闭文件句柄并把临时文件改名到目标路径（覆盖已存在文件）
    /// @return eNoError 成功；否则保留/清理临时文件并返回错误码
    errc_t finish();

    /// 失败时关闭并删除临时文件
    void discard();

private:
    void close();


    std::string filepath_;
    std::string tempPath_;
    DownloadProgressCallback progress_;
    FILE* fp_ = nullptr;
    uint64_t total_ = 0;
    uint64_t downloaded_ = 0;
};

/*! @} */

AST_NAMESPACE_END
