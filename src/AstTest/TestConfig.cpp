///
/// @file      TestConfig.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-21
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

#include "TestConfig.hpp"
#include "AstCore/RunTime.hpp"
#include <memory>

AST_NAMESPACE_BEGIN


#define AST_DEFAULT_TEST_CONFIG_PATH AST_PROJECT_NAME "_testconfig_file.txt"


std::shared_ptr<StartupConfig> aTestLoadConfig()
{
    auto config = std::make_shared<StartupConfig>();
    errc_t rc = config->load(AST_DEFAULT_TEST_CONFIG_PATH);
    if(rc){
        std::string configfile = aDataDir() + "/Config/" + AST_DEFAULT_TEST_CONFIG_PATH;
        aInfo(_("正在使用测试配置文件: %s"), configfile.c_str());
        rc = config->load(configfile);
    }
    return config;
}

StartupConfig *aTestGetConfig()
{
    static std::shared_ptr<StartupConfig> config = aTestLoadConfig();
    return config.get();
}

ValueView aTestGetConfigValue(StringView key)
{
    return aTestGetConfig()->getConfig(key);
}

std::vector<std::string> aTestGetConfigStringVector(StringView key)
{
    return aTestGetConfig()->getStringVector(key);
}

std::vector<StringView> aTestGetConfigStringViewVector(StringView key)
{
    return aTestGetConfig()->getStringViewVector(key);
}

AST_NAMESPACE_END

