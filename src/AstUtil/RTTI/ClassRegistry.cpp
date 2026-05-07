///
/// @file      ClassRegistry.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-02
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

#include "ClassRegistry.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

ClassRegistry *ClassRegistry::Instance()
{
    static ClassRegistry instance;
    return &instance;
}

Class *ClassRegistry::getClass(StringView name) const
{
    std::string nameStr(name); // = aAsciiStrToLower(name);
    auto it = classMap_.find(nameStr);
    if(it != classMap_.end())
        return it->second;
    return nullptr;
}

void ClassRegistry::getAllClassNames(std::vector<std::string> &names) const
{
    names.clear();
    names.reserve(classMap_.size());
    for(auto &it : classMap_){
        if(!it.first.empty())
            names.push_back(it.first);
    }
}

void ClassRegistry::registerClass(Class *cls)
{
    registerClass(cls, cls->name());
}

void ClassRegistry::registerClass(Class *cls, StringView name)
{
    if(!cls)
    {
        aWarning("failed to register class, cls is nullptr");
        return;
    }
    //std::string nameStr = aAsciiStrToLower(name);
    std::string nameStr = std::string(name);
    classMap_[nameStr] = cls;
}

AST_NAMESPACE_END


