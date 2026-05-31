///
/// @file      StartupConfig.cpp
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

#include "StartupConfig.hpp"
#include "AstUtil/KVParser.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/FileSystem.hpp"

AST_NAMESPACE_BEGIN

errc_t StartupConfig::load(StringView filepath)
{
    KVParser parser;
    parser.open(filepath);
    if(!parser.isOpen())
        return eErrorInvalidFile;
    this->filepath_ = std::string(filepath);
    this->configMap_.clear();
    BKVItemView item;
    while(1)
    {
        auto token = parser.getNext(item);
        if(token == KVParser::eEqual){
            this->setConfig(item.key(), item.value());
        }else if(token == KVParser::eAddEqual){
            this->addConfig(item.key(), item.value());
        }else if(token == KVParser::eEOF){
            break;
        }else if(token == KVParser::eError){
            return eErrorInvalidFile;
        }
    }
    return eNoError;
}


StringView StartupConfig::dirpath() const
{
    #ifdef _WIN32
    auto pos = filepath_.find_last_of("/\\");
    #else
    auto pos = filepath_.find_last_of('/');
    #endif
    if(pos == std::string::npos) return ".";
    return StringView(filepath_).substr(0, pos);
}

ValueView StartupConfig::getConfig(StringView key) const
{
    ValueView value;
    errc_t rc = this->getConfig(key, value);
    A_UNUSED(rc);
    return value;
}

std::vector<std::string> StartupConfig::getStringVector(StringView key) const
{
    return this->getConfig(key).split(',');
}

std::vector<StringView> StartupConfig::getStringViewVector(StringView key) const
{
    return this->getConfig(key).split(',');
}

errc_t StartupConfig::getConfig(StringView key, ValueView &value) const
{
    auto it = this->configMap_.find(std::string(key));
    if(it == this->configMap_.end())
        return eErrorNotFound;
    value = it->second;
    return eNoError;
}

void StartupConfig::setConfig(StringView key, ValueView value)
{
    this->setConfigRaw(key, this->decodeConfig(value));
}

void StartupConfig::setConfigRaw(StringView key, ValueView value)
{
    this->configMap_[std::string(key)] = value.toString();
}

void StartupConfig::addConfig(StringView key, ValueView value)
{
    std::string valuestr = this->decodeConfig(value);
    this->addConfigRaw(key, valuestr);
}

void StartupConfig::addConfigRaw(StringView key, ValueView value)
{
    ValueView existsValue;
    if(this->getConfig(key, existsValue) == eNoError)
    {
        std::string valuestr = existsValue.toString() + "," + value.toString();
        this->setConfigRaw(key, valuestr);
    }else{
        this->setConfigRaw(key, value);
    }
}

bool StartupConfig::hasConfig(StringView key) const
{
    auto it = this->configMap_.find(std::string(key));
    return it != this->configMap_.end();
}

std::string StartupConfig::decodeConfig(StringView value)
{
    auto pos = value.find('/');
    {
        StringView prefix = value.substr(0, pos);
        StringView suffix;
        if(pos != std::string::npos)
            suffix = value.substr(pos + 1);
        
        if(prefix.ends_with("_PATH") || prefix.ends_with("_DIR") || prefix.ends_with("_FILE"))
        {
            prefix = this->getConfig(prefix).toStringView();
            if(suffix.size() == 0){
                return std::string(prefix);
            }else{
                if(suffix.starts_with('/') || prefix.ends_with('/')){
                    return std::string(prefix) + std::string(suffix);
                }else{
                    return std::string(prefix) + "/" + std::string(suffix);
                }
            }
        }
    }
    return std::string(value);
}

void StartupConfig::printConfig(FILE* file) const
{
    for(auto it = this->configMap_.begin(); it != this->configMap_.end(); ++it)
    {
        posix::fprintf(file, "%20s=%s\n", it->first.c_str(), it->second.value().c_str());
    }
}



AST_NAMESPACE_END


