///
/// @file      TempFileGuard.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-08-27
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include <string>
#include <system_error>
#include <utility>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Util
    @{
*/

/// @brief 临时文件守卫
/// @details 用于在其作用域结束时自动删除指定的临时文件，保证即使提前返回或抛出异常也能完成清理。
///          支持主动删除（remove()）、释放文件所有权（release()）以及移动转移所有权等语义。
///          以"路径是否为空"表示是否持有待删除的文件：为空即不删除任何文件。
class TempFileGuard
{
public:
    /// @brief 构造为空的守卫，不持有任何文件
    TempFileGuard() = default;

    /// @brief 构造并接管指定临时文件
    /// @param path 临时文件的路径
    explicit TempFileGuard(StringView path)
        : path_(std::string(path))
    {}

    explicit TempFileGuard(const fs::path& path)
        : path_(path)
    {}

    explicit TempFileGuard(const std::string& path)
        : path_(path)
    {}

    /// @brief 析构时删除临时文件
    ~TempFileGuard()
    {
        remove();
    }

    /// @brief 立即删除临时文件并解除析构时的删除
    void remove()
    {
        if (path_.empty())
            return;
        std::error_code ec;
        fs::remove(path_, ec);
        if (ec) {
            aError("failed to remove temp file '%s': %s", path_.c_str(), ec.message().c_str());
        }
        path_ = fs::path();
    }

    /// @brief 释放文件所有权，保留文件并解除析构时的删除
    void release()
    {
        path_ = fs::path();
    }

    /// @brief 是否仍将在析构时删除文件（即是否持有待删除的文件）
    bool isActive() const { return !path_.empty(); }

    /// @brief 文件当前是否存在
    bool exists() const
    {
        if (path_.empty())
            return false;
        std::error_code ec;
        return fs::exists(path_, ec);
    }

    /// @brief 获取文件路径（释放或移除后为空）
    fs::path& path() { return path_; }

    /// @brief 布尔转换：仍在自动删除状态
    operator bool() const { return isActive(); }

protected:
    A_DISABLE_COPY(TempFileGuard);

    fs::path path_{};    ///< 临时文件路径，为空表示不持有任何文件
};


/*! @} */

AST_NAMESPACE_END
