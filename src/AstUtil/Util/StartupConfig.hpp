///
/// @file      StartupConfig.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-20
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include <unordered_map>
#include "AstUtil/ValueView.hpp"
#include "AstUtil/GenericValue.hpp"
#include <cstdio>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief  启动配置类
class AST_UTIL_API StartupConfig
{
public:
    StartupConfig() = default;
    ~StartupConfig() = default;

    /// @brief  加载启动配置文件
    /// @param filepath 
    /// @return 
    errc_t load(StringView filepath);

    /// @brief  获取加载的配置文件路径
    const std::string& filepath() const{return filepath_;}

    /// @brief  获取加载的配置文件所在目录
    StringView dirpath() const;

    /// @brief  检查配置是否为空
    /// @return 
    bool empty() const{return configMap_.empty();}

    /// @brief  获取配置
    /// @param key 
    /// @return 
    ValueView getConfig(StringView key) const;

    /// @brief  获取配置，返回字符串向量
    /// @return 
    std::vector<std::string> getStringVector(StringView key) const;


    /// @brief  获取配置，返回字符串视图向量
    /// @return 
    std::vector<StringView> getStringViewVector(StringView key) const;

    
    /// @brief  获取配置
    /// @param key 
    /// @param value 
    /// @return 
    errc_t getConfig(StringView key, ValueView &value) const;

public: // 修改配置

    /// @brief  设置配置（进行解码）
    /// @param key 
    /// @param value 
    void setConfig(StringView key, ValueView value);

    /// @brief  设置配置（不进行解码），不追加值
    /// @param key 
    /// @param value 
    void setConfigRaw(StringView key, ValueView value);

    /// @brief  添加配置, 如果配置不存在则创建, 否则追加值
    /// @param key 
    /// @param value 
    void addConfig(StringView key, ValueView value);

    /// @brief  添加配置, 如果配置不存在则创建, 否则追加值, 不进行解码
    /// @param key 
    /// @param value 
    void addConfigRaw(StringView key, ValueView value);

    /// @brief  检查配置是否存在
    /// @param key 
    /// @return 
    bool hasConfig(StringView key) const;

    /// @brief  打印配置到指定文件
    /// @param file 
    /// @return  None
    void printConfig(FILE* file=stdout) const;
protected:
    std::string decodeConfig(StringView value);
protected:
    using ConfigMap = std::unordered_map<std::string, GenericValue>;
    ConfigMap configMap_{};
    std::string filepath_{};
};



/*! @} */

AST_NAMESPACE_END
