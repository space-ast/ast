///
/// @file      PluginRegistry.hpp
/// @brief     插件注册表
/// @details   负责插件的索引与查找，扫描目录、根据插件元数据建立索引，
///            并可以按 (接口, 索引键) 构造插件根对象
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
#include "AstUtil/Span.hpp"
#include "AstUtil/StringView.hpp"
#include "PluginInterface.hpp"
#include "PluginLoader.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <utility>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Plugin
    @{
*/

/// @brief 插件注册表（进程级单例）
/// @details 维护 (接口 ID, 索引键) → 插件库 的索引。
///          扫描阶段只读取文件中的嵌入元数据，不加载动态库；
///          实际加载推迟到首次 find() 命中，加载后的库常驻进程
class AST_UTIL_API PluginRegistry
{
public:
    /// @brief 获取全局单例
    static PluginRegistry* Instance();

    /// @brief 扫描目录并建立索引
    /// @param dir 目录路径
    /// @param namePrefix 只处理文件名以此前缀开头的库；为空表示不筛选
    /// @return 错误码
    /// @note 只读取插件文件中的元数据，不加载动态库；同一个目录只会被扫描一次，重复调用是幂等的
    errc_t scanDirectory(StringView dir, StringView namePrefix = StringView());

    /// @brief 扫描目录并建立索引，只处理文件名以任一前缀开头的库
    /// @param dir 目录路径
    /// @param namePrefixes 前缀列表；为空表示不筛选。不持有视图指向的存储，调用期间需保持有效
    /// @return 错误码
    /// @note 同一目录仍只扫描一次；需要多前缀时必须一次传入，不能分多次调用
    errc_t scanDirectory(StringView dir, Span<const StringView> namePrefixes);

    /// @brief 按 (接口 ID, 索引键) 查找插件根对象
    /// @return 根对象指针；未命中或动态库加载失败返回 nullptr。命中时才会加载动态库并构造根对象
    PluginInstance* find(StringView iid, StringView key);

    template<class IFace>
    IFace* find(StringView key)
    {
        PluginInstance* instance = find(aInterfaceId<IFace>(), key);
        return aPluginCast<IFace>(instance);
    }

    /// @brief 列出某接口下已注册的全部索引键
    void keys(StringView iid, std::vector<std::string>& keys) const;
    std::vector<std::string> keys(StringView iid) const;

    template<class IFace>
    std::vector<std::string> keys() const
    {
        return this->keys(aInterfaceId<IFace>());
    }

    template<class IFace>
    void keys(std::vector<std::string>& keys) const
    {
        this->keys(aInterfaceId<IFace>(), keys);
    }

    /// @brief 已发现的插件库数量（元数据读取成功，不代表已加载）
    size_t pluginCount() const;

    /// @brief 索引条目总数
    size_t entryCount() const;

    /// @brief 清空索引
    /// @warning 已加载的动态库不会立即卸载（卸载推迟到进程退出）
    void clear();

private:
    PluginRegistry() = default;
    ~PluginRegistry() = default;

    A_DISABLE_COPY(PluginRegistry)

    /// @brief 读取单个插件库的元数据并将其并入索引（不加载动态库）
    void loadPluginMetadata(const std::string& filepath);

private:
    std::vector<std::unique_ptr<PluginLoader>> loaders_{};                ///< 已发现的插件库
    std::map<std::pair<std::string, std::string>, PluginLoader*> index_{};///< (iid, key) → 插件
    std::set<std::string> scannedDirs_{};                                 ///< 已扫描过的目录
};

/// @brief 获取插件注册表全局单例
A_ALWAYS_INLINE PluginRegistry& aPluginRegistry()
{
    return *PluginRegistry::Instance();
}

/// @brief 扫描可用插件并建立索引
/// @details
/// 探测顺序：`<可执行文件目录>/plugins`、`<库目录>/plugins`；
/// 两处都不存在时回退到库目录，按产物的命名习惯筛选插件：
/// MSVC 产物没有 lib 前缀，只匹配 "AstPlugin"
/// GCC / Clang / MinGW 的共享库可能带 lib 前缀，同时接受 "AstPlugin" 与 "libAstPlugin"
/// @return 错误码
/// @note 扫描只读取元数据，不加载动态库；重复调用是幂等的。
AST_UTIL_API errc_t aScanPlugins();


AST_UTIL_API void aEnsurePluginsScanned();

/*! @} */

AST_NAMESPACE_END
