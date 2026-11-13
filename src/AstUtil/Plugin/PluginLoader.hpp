///
/// @file      PluginLoader.hpp
/// @brief     单个插件动态库的加载与元数据读取
/// @details   负责加载插件动态库、读出其元数据，并在需要时获取插件根对象
/// @author    axel
/// @date      2026-10-14
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
#include "AstUtil/StringView.hpp"
#include "PluginInterface.hpp"
#include <string>
#include <mutex>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Plugin
    @{
*/

/// @brief 从动态库文件字节中读取插件元数据记录（不加载动态库）
/// @param filepath 动态库路径
/// @param[out] record 解析出的元数据记录
/// @return 错误码
AST_UTIL_API errc_t aReadPluginMetadataRecord(StringView filepath, PluginMetadataRecord& record);

/// @brief 从动态库文件字节中读取插件元数据（不加载动态库）
/// @param filepath 动态库路径
/// @param[out] metadata 解析出的接口 ID 与索引键
/// @return 错误码
AST_UTIL_API errc_t aReadPluginMetadata(StringView filepath, PluginMetadata& metadata);

/// @brief 单个插件动态库加载器
/// @details
/// 构造时指定目标库；readMetadata() 只读文件中的嵌入元数据，不加载动态库；
/// load() 才真正加载动态库并校验运行期接口 ID。
/// 已加载的插件库常驻到进程退出，不提供卸载：插件根对象及其创建的对象必须始终有效
class AST_UTIL_API PluginLoader
{
public:
    PluginLoader() = default;

    /// @brief 以插件路径构造
    /// @details 路径为空时保持未设置状态，此时 readMetadata()/load() 返回 eErrorNullInput
    PluginLoader(StringView filepath);

    ~PluginLoader() = default;

    A_DISABLE_COPY(PluginLoader)

    /// @brief 动态库文件路径
    const std::string& filepath() const { return filepath_; }

    /// @brief 读取文件中的嵌入元数据（不加载动态库）
    /// @return eNoError 表示这是一个合法插件；其他错误码表示读取失败或不是插件
    errc_t readMetadata();

    /// @brief 是否为已验证的插件库（元数据读取成功）
    bool isPlugin() const { return !metadata_.iid.empty(); }

    /// @brief 已解析的插件元数据
    const PluginMetadata& metadata() const { return metadata_; }

    /// @brief 加载动态库并校验运行期接口 ID
    /// @details 已加载时直接返回 eNoError；尚未读取元数据时会先读取；失败结果会被缓存
    /// @return 错误码
    errc_t load();

    /// @brief 动态库是否已加载
    bool isLoaded() const { return handle_ != nullptr; }

    /// @brief 获取插件根对象（首次调用时加载动态库）
    /// @return 根对象指针；不是插件、动态库加载失败或入口缺失时返回 nullptr
    PluginInstance* instance();

private:
    /// @note 以下函数不加锁，调用方必须已持有 mutex_
    errc_t readMetadataNoLock();
    errc_t loadNoLock();

    void* handle_{nullptr};                  ///< 动态库句柄（加载后常驻进程退出）
    std::string filepath_{};                 ///< 动态库路径
    PluginMetadata metadata_{};              ///< 插件元数据（iid 为空表示尚未读取）
    PluginInstance* instance_{nullptr};      ///< 根对象缓存
    bool loadAttempted_{false};              ///< 是否已尝试加载，失败后不反复重试
    errc_t loadError_{eNoError};             ///< 上次加载的错误码
    std::mutex mutex_{};                     ///< 保护加载状态
};

/*! @} */

AST_NAMESPACE_END
