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
    /// @param minor 次版本号
    /// @param patch 修订版本号，如果<0，则不包含修订版本号
    /// @param prerelease 预发布版本号，如果为空，则不包含预发布版本号
    /// @param build 构建版本号，如果为空，则不包含构建版本号
    Version(int major, int minor, int patch=-1, StringView prerelease = {}, StringView build = {});

public:
    int compare(const Version& other) const;

    bool operator==(const Version& other) const{return compare(other) == 0;}
    bool operator!=(const Version& other) const{return compare(other) != 0;}
    bool operator<(const Version& other) const{return compare(other) < 0;}
    bool operator>(const Version& other) const{return compare(other) > 0;}
    bool operator<=(const Version& other) const{return compare(other) <= 0;}
    bool operator>=(const Version& other) const{return compare(other) >= 0;}

public:
    bool valid() const{return major_ > 0;}

    int major() const{return major_;} 
    void setMajor(int major){major_ = major;}

    int minor() const{return minor_;}
    void setMinor(int minor){minor_ = minor;}

    int patch() const{return patch_;}
    void setPatch(int patch){patch_ = patch;}
    
    const std::string& prerelease() const{return prerelease_;}
    void setPrerelease(StringView prerelease){prerelease_ = std::string(prerelease);}

    const std::string& build() const{return build_;}
    void setBuild(StringView build){build_ = std::string(build);}

private:
    int major_{-1};                 ///< 主版本号
    int minor_{0};                  ///< 次版本号
    int patch_{0};                  ///< 修订版本号
    std::string prerelease_;        ///< 预发布版本号
    std::string build_;             ///< 构建版本号
};

/*! @} */

AST_NAMESPACE_END
