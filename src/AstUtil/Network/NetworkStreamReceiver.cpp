///
/// @file      NetworkStreamReceiver.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-08-27
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

#include "NetworkStreamReceiver.hpp"
#include "AstUtil/Posix.hpp"
#include "AstUtil/FileSystem.hpp"

AST_NAMESPACE_BEGIN

// ------------------------
// CollectingStreamReceiver
// ------------------------


errc_t CollectingStreamReceiver::onHeaders(int statusCode,
                                    const std::map<std::string, std::string>& headers)
{
    response_.setStatusCode(statusCode);
    response_.setHeaders(headers);
    return eNoError;
}

errc_t CollectingStreamReceiver::onData(const char* data, size_t size)
{
    // 防止无限制响应体耗尽内存
    if (body_.size() + size > kMaxBodySize)
    {
        aError("response body size %zu exceeds max limit %zu", body_.size() + size, kMaxBodySize);
        return eErrorOutOfRange;
    }
    body_.append(data, size);
    return 0;
}

void CollectingStreamReceiver::onComplete()
{
    response_.setBody(body_);
}


void CollectingStreamReceiver::onError(errc_t /*error*/)
{
    // 保留已累积的部分体数据（用于诊断）
    if (!body_.empty())
        response_.setBody(body_);
}

// ------------------------
// FileDownloadReceiver
// ------------------------

FileDownloadReceiver::FileDownloadReceiver(const std::string &filepath, const DownloadProgressCallback &progress)
    : filepath_(filepath), tempPath_(filepath + ".part"), progress_(progress)
{
}

FileDownloadReceiver::~FileDownloadReceiver()
{
    close();
}

errc_t FileDownloadReceiver::onHeaders(int statusCode, const std::map<std::string, std::string>& headers)
{
    auto it = headers.find("Content-Length");
    if (it != headers.end())
        total_ = static_cast<uint64_t>(std::strtoull(it->second.c_str(), nullptr, 10));
    // 基于响应头拒绝：非 200 立即中止，不读取响应体
    if (statusCode != 200)
        return (eErrorInvalidFile);
    // 写入临时文件，成功后再改名到目标路径，避免失败下载破坏已存在的目标文件
    fp_ = posix::fopen(tempPath_.c_str(), "wb");
    if (fp_ == nullptr)
    {
        aError("cannot open %s", tempPath_.c_str());
        return (eErrorInvalidFile);
    }
    return eNoError;
}


errc_t FileDownloadReceiver::onData(const char* data, size_t size)
{
    if (fp_ == nullptr)
        return (eErrorInvalidFile);
    if (fwrite(data, 1, size, fp_) != size)
    {
        aError("write incomplete for %s", tempPath_.c_str());
        return (eErrorInvalidFile);
    }
    downloaded_ += size;
    if (progress_ && !progress_(downloaded_, total_))
        return (eErrorCancelled);
    return eNoError;
}

void FileDownloadReceiver::discard()
{
    close();
    std::error_code ec;
    fs::remove(tempPath_, ec);
}


errc_t FileDownloadReceiver::finish()
{
    close();
    std::error_code ec;
    // 覆盖已存在的目标文件：Windows 上 rename 不会覆盖，需先移除目标
    if (fs::exists(filepath_, ec))
    {
        if (!fs::remove(filepath_, ec))
        {
            aError("cannot replace existing %s", filepath_.c_str());
            fs::remove(tempPath_, ec);
            return eErrorInvalidFile;
        }
    }
    if (!fs::rename(tempPath_, filepath_))
    {
        aError("rename %s -> %s failed", tempPath_.c_str(), filepath_.c_str());
        fs::remove(tempPath_, ec);
        return eErrorInvalidFile;
    }
    return eNoError;
}


void FileDownloadReceiver::close()
{
    if (fp_ != nullptr)
    {
        fclose(fp_);
        fp_ = nullptr;
    }
}




AST_NAMESPACE_END
