///
/// @file      PluginInterface.hpp
/// @brief     通用插件接口定义
/// @details   定义插件根对象、接口 ID 声明机制、元数据导出宏与实例入口约定
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
#include <string>
#include <vector>
#include <cstdint>

// 插件元数据所在的动态库段名
// PE 镜像的段头只有 8 字节用于段名
// 更长的名字在 .obj 里走 COFF 字符串表（段头写 "/偏移"，例如 "/29"），但链接进 PE 镜像时会被截断
// 因此这里取 ".astplug" 这个恰好 8 字符的名字，避免依赖长名机制
#if defined(_MSC_VER) && !defined(__clang__)
#   pragma section(".astplug", read)
#   define AST_PLUGIN_METADATA_SECTION __declspec(allocate(".astplug"))
#elif defined(__clang__) && defined(_WIN32)
#   define AST_PLUGIN_METADATA_SECTION __attribute__((used, section(".astplug")))
#elif defined(__APPLE__)
#   define AST_PLUGIN_METADATA_SECTION __attribute__((used, section("__DATA,astplug")))
#elif defined(__GNUC__) || defined(__clang__)
#   define AST_PLUGIN_METADATA_SECTION __attribute__((used, section(".astplug")))
#else
#   define AST_PLUGIN_METADATA_SECTION
#endif

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Plugin
    @{
*/

/// @brief 插件库导出符号声明
#define AST_PLUGIN_EXPORT A_DECL_EXPORT

/// @brief 插件根对象基类
/// @details 
/// 每个插件动态库通过入口函数导出唯一根对象
/// 根对象可同时实现多个接口，可使用 aPluginCast 转换为所需的接口类型
class AST_UTIL_API PluginInstance
{
public:
    virtual ~PluginInstance() = default;
};

/// @brief 插件入口函数类型
typedef PluginInstance* (*FPluginInstance)();


// 插件动态库导出符号名称

#define AST_PLUGIN_IID_NAME          ast_plugin_iid
#define AST_PLUGIN_INSTANCE_NAME     ast_plugin_instance
#define AST_PLUGIN_METADATA_NAME     ast_plugin_metadata

#define AST_PLUGIN_IID_NAMESTR       A_STRINGIZE(AST_PLUGIN_IID_NAME)
#define AST_PLUGIN_INSTANCE_NAMESTR  A_STRINGIZE(AST_PLUGIN_INSTANCE_NAME)

#define AST_PLUGIN_INSTANCE(Type)   extern "C" AST_PLUGIN_EXPORT PluginInstance* AST_PLUGIN_INSTANCE_NAME(){static Type instance; return &instance;}

// 嵌入到动态库 .astplug 段的元数据记录。
// 主程序在 LoadLibrary/dlopen 之前直接扫描文件字节读取该记录，因此它必须是常量初始化的。
#define AST_PLUGIN_METADATA_MAGIC_LEN   16
#define AST_PLUGIN_METADATA_MAGIC       "ASTPLUGINMETA:1"
#define AST_PLUGIN_METADATA_VERSION     1u
#define AST_PLUGIN_IID_MAX_LEN          128
#define AST_PLUGIN_KEYS_MAX_LEN         128

struct PluginMetadataRecord
{
    char     magic_[AST_PLUGIN_METADATA_MAGIC_LEN];     ///< AST_PLUGIN_METADATA_MAGIC
    uint32_t version_;                                  ///< AST_PLUGIN_METADATA_VERSION
    uint32_t size_;                                     ///< sizeof(PluginMetadataRecord)
    char     iid_[AST_PLUGIN_IID_MAX_LEN];              ///< NUL 结尾，其后全 0
    char     keys_[AST_PLUGIN_KEYS_MAX_LEN];            ///< 逗号分隔的索引键，NUL 结尾，其后全 0
};

/// @brief 编译期字符串相等比较（C++11 constexpr 只允许单条 return）
constexpr bool aPluginStrEqual(const char* lhs, const char* rhs)
{
    return (*lhs == *rhs) && (*lhs == '\0' || aPluginStrEqual(lhs + 1, rhs + 1));
}

/// @brief 由接口类型名取出接口头文件里的 IID 字符串字面量
/// @details 约定：每个接口头都必须定义 AST_IID_<IFace> 宏，且 IFace 使用未限定的简单类型名
#define AST_PLUGIN_IID_LITERAL(IFace)    AST_PLUGIN_IID_LITERAL_I(IFace)
#define AST_PLUGIN_IID_LITERAL_I(IFace)  AST_IID_##IFace

#define AST_PLUGIN_STATIC_ASSERT_IID(IFace)                                      \
    static_assert(aPluginStrEqual(AST_PLUGIN_IID_LITERAL(IFace),                 \
                                  aInterfaceId<IFace>()),                        \
                  "AST_IID_<IFace> must be defined and equal to aInterfaceId<IFace>()")


/// @brief 声明插件元数据：接口 ID + 逗号分隔的索引键
/// @param IFace 接口类型（接口头必须定义同名的 AST_IID_<IFace> 宏）
/// @param KEYS  索引键字符串字面量，多个键用逗号分隔，如 "agg" 或 "a,b"
/// @note iid 不超过 AST_PLUGIN_IID_MAX_LEN 字节，keys 不超过 AST_PLUGIN_KEYS_MAX_LEN 字节；键中不能出现逗号
/// @note 元数据放在动态库的 .astplug 段，主程序无需加载动态库即可读取
#define AST_PLUGIN_METADATA(IFace, KEYS)                                             \
    extern "C" { AST_PLUGIN_EXPORT const char* AST_PLUGIN_IID_NAME =                 \
                     aInterfaceId<IFace>(); }                                        \
    extern AST_PLUGIN_METADATA_SECTION const _AST PluginMetadataRecord               \
        AST_PLUGIN_METADATA_NAME = {                                                 \
            AST_PLUGIN_METADATA_MAGIC,                                               \
            AST_PLUGIN_METADATA_VERSION,                                             \
            static_cast<uint32_t>(sizeof(_AST PluginMetadataRecord)),                \
            AST_PLUGIN_IID_LITERAL(IFace),                                           \
            KEYS };                                                                  \
    AST_PLUGIN_STATIC_ASSERT_IID(IFace);

/// @brief 接口 ID 特化模板
template<class IFace>
struct InterfaceId;

/// @brief 声明一个插件接口及其唯一 ID
/// @param IFace 接口类型
/// @param IID   接口唯一标识字符串，插件元数据里的 "iid" 必须与此一致
#define AST_DECLARE_INTERFACE(IFace, IID)                    \
    template<> struct InterfaceId<IFace> {                   \
        static constexpr const char* value() { return IID; } \
    };

/// @brief 获取接口 ID
template<class IFace>
constexpr const char* aInterfaceId()
{
    return InterfaceId<IFace>::value();
}

/// @brief 把插件根对象转换为指定接口类型
/// @details 依赖 C++ RTTI，转换失败返回 nullptr。
template<class IFace>
IFace* aPluginCast(PluginInstance* instance)
{
    return dynamic_cast<IFace*>(instance);
}

/// @brief 解析后的插件元数据：接口 ID 与该接口下的索引键
struct PluginMetadata
{
    std::string iid{};                ///< 接口 ID
    std::vector<std::string> keys{};  ///< 该接口下的索引键
};

/*! @} */

AST_NAMESPACE_END
