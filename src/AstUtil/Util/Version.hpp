///
/// @file      Version.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-30
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
#include <string>
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 版本号
/// @details 
/// 根据 Semantic Versioning 2.0.0 规范定义的版本号
/// 版本号的格式为：主版本号.次版本号.修订版本号[-预发布版本号+构建版本号]
/// 本实现在规范之外额外允许：省略修订版本号（主版本号.次版本号）、可选的 v/V 前缀；
/// @see https://semver.org/lang/zh-CN/
class AST_UTIL_API Version
{
public:
    /// @brief 默认构造（无效版本，major_ = -1）
    Version() = default;

    /// @brief 解析版本号字符串
    /// @param version 版本号字符串
    /// @return 版本号
    static Version Parse(StringView version);

    /// @param version 版本号字符串
    explicit Version(StringView version);

    /// @param major 主版本号
    /// @param minor 次版本号，必须 >= 0，否则构造出的版本无效
    /// @param patch 修订版本号，如果<0，则不包含修订版本号
    /// @param prerelease 预发布版本号，如果为空，则不包含预发布版本号；格式非法时版本无效
    /// @param build 构建版本号，如果为空，则不包含构建版本号；格式非法时版本无效
    Version(int major, int minor, int patch=-1, StringView prerelease = {}, StringView build = {});

public:
    /// @brief 比较两个版本的优先层级
    /// @note 仅在两个版本均有效时具有 SemVer 语义；无效版本的比较结果不作保证
    int compare(const Version& other) const;

    bool operator==(const Version& other) const{return compare(other) == 0;}
    bool operator!=(const Version& other) const{return compare(other) != 0;}
    bool operator<(const Version& other) const{return compare(other) < 0;}
    bool operator>(const Version& other) const{return compare(other) > 0;}
    bool operator<=(const Version& other) const{return compare(other) <= 0;}
    bool operator>=(const Version& other) const{return compare(other) >= 0;}

public:
    /// @brief 版本是否有效
    /// @details 主版本号、次版本号非负，且预发布版本号、构建版本号格式合法时才有效；
    bool valid() const{return major_ >= 0 && minor_ >= 0 && prereleaseValid_ && buildValid_;}

    int major() const{return major_;} 
    void setMajor(int major){major_ = major;}

    int minor() const{return minor_;}
    /// @note 次版本号必须 >= 0，传入负值会使版本变为无效
    void setMinor(int minor){minor_ = minor;}

    int patch() const{return patch_;}
    /// @note 修订版本号 < 0 表示无修订版本号
    void setPatch(int patch){patch_ = patch < 0 ? -1 : patch;}
    
    const std::string& prerelease() const{return prerelease_;}
    /// @note 传入的字符串格式非法时，版本会变为无效
    void setPrerelease(StringView prerelease);

    const std::string& build() const{return build_;}
    /// @note 传入的字符串格式非法时，版本会变为无效
    void setBuild(StringView build);
public:
    /// @brief 将版本号转换为字符串表示
    /// @return 版本号字符串
    std::string toString() const;
private:
    /// @brief 依据 prerelease_ / build_ 重新计算两个字符串合法性缓存
    void updateStringValid();
private:
    int major_{-1};                  ///< 主版本号
    int minor_{0};                   ///< 次版本号
    int patch_{0};                   ///< 修订版本号
    bool prereleaseValid_{true};     ///< 预发布版本号合法性
    bool buildValid_{true};          ///< 构建版本号合法性
    std::string prerelease_{};       ///< 预发布版本号
    std::string build_{};            ///< 构建版本号
};

/// @brief 获取本库的版本号
AST_UTIL_API const Version& aVersion();

/// @brief 获取本库的版本文本
AST_UTIL_API const char* aVersionStr();

/*! @} */

AST_NAMESPACE_END
