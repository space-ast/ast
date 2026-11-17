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
#include "AstUtil/JsonValue.hpp"
#include <string>
#include <vector>
#include <cstddef>
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
#define AST_PLUGIN_METADATA_MAGIC       "AST_PLUGIN_META"
#define AST_PLUGIN_METADATA_VERSION     2u
#define AST_PLUGIN_IID_MAX_LEN          128

// 数据区类型
#define AST_PLUGIN_DATA_TEXT            0   ///< 纯文本：逗号分隔的索引键
#define AST_PLUGIN_DATA_JSON            1   ///< JSON 字符串

// 数据区长度上限。扫描时据此拒绝异常记录，避免按文件中的长度分配任意大的缓冲区
#define AST_PLUGIN_DATA_MAX_LEN         (64 * 1024)

#ifndef SWIG

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4200)     // 末尾柔性数组是刻意为之的布局约定
#endif

/// @brief 元数据记录的定长头
/// @details 记录在文件中形如「定长头 + size_ 字节的数据区」，数据区紧接 data_ 存放
struct PluginMetadataRecord
{
    char     magic_[AST_PLUGIN_METADATA_MAGIC_LEN];     ///< AST_PLUGIN_METADATA_MAGIC
    uint32_t version_;                                  ///< AST_PLUGIN_METADATA_VERSION
    char     iid_[AST_PLUGIN_IID_MAX_LEN];              ///< NUL 结尾，其后全 0
    uint8_t  dataType_;                                 ///< 数据类型: AST_PLUGIN_DATA_TEXT / AST_PLUGIN_DATA_JSON
    uint32_t size_;                                     ///< 数据区字节数（不含结尾 NUL）
    char     data_[];                                   ///< 数据区，长度为 size_
};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif

/// @brief 带定长数据区的完整记录对象
/// @details 仅用于在插件里定义常量：与 PluginMetadataRecord 的定长头逐字段一致，data_ 紧随其后
/// @note 不能直接对 PluginMetadataRecord 的柔性数组做聚合初始化，因此用这个模板生成完整字节布局
template<size_t N>
struct PluginMetadataRecordStorage
{
    char     magic_[AST_PLUGIN_METADATA_MAGIC_LEN];
    uint32_t version_;
    char     iid_[AST_PLUGIN_IID_MAX_LEN];
    uint8_t  dataType_;
    uint32_t size_;
    char     data_[N];
};


/// @brief 编译期字符串相等比较（C++11 constexpr 只允许单条 return）
constexpr bool aPluginStrEqual(const char* lhs, const char* rhs)
{
    return (*lhs == *rhs) && ((*lhs == '\0') ? true : aPluginStrEqual(lhs + 1, rhs + 1));
}

/// @brief 由接口类型名取出接口头文件里的 IID 字符串字面量
/// @details 约定：每个接口头都必须定义 AST_IID_<IFace> 宏，且 IFace 使用未限定的简单类型名
#define AST_PLUGIN_IID_LITERAL(IFace)    AST_PLUGIN_IID_LITERAL_I(IFace)
#define AST_PLUGIN_IID_LITERAL_I(IFace)  AST_IID_##IFace

#define AST_PLUGIN_STATIC_ASSERT_IID(IFace)                                      \
    static_assert(aPluginStrEqual(AST_PLUGIN_IID_LITERAL(IFace),                 \
                                  aInterfaceId<IFace>()),                        \
                  "AST_IID_<IFace> must be defined and equal to aInterfaceId<IFace>()")


/// @brief 声明插件元数据：接口 ID + 数据区
/// @param IFace     接口类型（接口头必须定义同名的 AST_IID_<IFace> 宏）
/// @param DATA_TYPE AST_PLUGIN_DATA_TEXT / AST_PLUGIN_DATA_JSON
/// @param DATA      数据区字符串字面量，长度不得超过 AST_PLUGIN_DATA_MAX_LEN
#define _AST_PLUGIN_METADATA_STORAGE(IFace, DATA_TYPE, DATA)                             \
    extern "C" { AST_PLUGIN_EXPORT const char* AST_PLUGIN_IID_NAME =                 \
                     aInterfaceId<IFace>(); }                                        \
    extern AST_PLUGIN_METADATA_SECTION const _AST PluginMetadataRecordStorage<       \
        sizeof(DATA)> AST_PLUGIN_METADATA_NAME = {                                   \
            AST_PLUGIN_METADATA_MAGIC,                                               \
            AST_PLUGIN_METADATA_VERSION,                                             \
            AST_PLUGIN_IID_LITERAL(IFace),                                           \
            DATA_TYPE,                                                               \
            static_cast<uint32_t>(sizeof(DATA) - 1),                                 \
            DATA };                                                                  \
    AST_PLUGIN_STATIC_ASSERT_IID(IFace);                                             \
    static_assert(sizeof(DATA) - 1 <= AST_PLUGIN_DATA_MAX_LEN,                       \
                  "plugin metadata data must not exceed AST_PLUGIN_DATA_MAX_LEN");   \
    static_assert(sizeof(AST_PLUGIN_IID_LITERAL(IFace)) <= AST_PLUGIN_IID_MAX_LEN,   \
                  "plugin interface id must not exceed AST_PLUGIN_IID_MAX_LEN");

/// @brief 声明插件元数据：接口 ID + 逗号分隔的索引键
/// @param IFace 接口类型（接口头必须定义同名的 AST_IID_<IFace> 宏）
/// @param KEYS  索引键字符串字面量，多个键用逗号分隔，如 "agg" 或 "a,b"；键中不能出现逗号
/// @note 元数据放在动态库的 .astplug 段，主程序无需加载动态库即可读取
#define AST_PLUGIN_METADATA_TEXT(IFace, KEYS)                                        \
    _AST_PLUGIN_METADATA_STORAGE(IFace, AST_PLUGIN_DATA_TEXT, KEYS)

/// @brief 声明插件元数据：接口 ID + JSON 元数据
/// @param IFace 接口类型（接口头必须定义同名的 AST_IID_<IFace> 宏）
/// @param JSON  JSON 字符串字面量；
/// @note 整个 JSON 会解析到 PluginMetadata::json_，供插件自行解释
#define AST_PLUGIN_METADATA_JSON(IFace, JSON)                                        \
    _AST_PLUGIN_METADATA_STORAGE(IFace, AST_PLUGIN_DATA_JSON, JSON)


#define AST_PLUGIN_METADATA(IFace, KEYS) AST_PLUGIN_METADATA_TEXT(IFace, KEYS)

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

/// @brief 解析后的插件元数据：接口 ID、索引键与 JSON 数据
struct PluginMetadata
{
    std::string iid_{};                ///< 接口 ID
    std::vector<std::string> keys_{};  ///< 该接口下的索引键
    JsonValue json_{};                 ///< JSON 元数据解析后的值；纯文本元数据时为 null
};

/*! @} */

AST_NAMESPACE_END
