///
/// @file      VariableListLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-30
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

#include "VariableListLoader.hpp"
#include "AstCore/VariableList.hpp"
#include "AstScript/Variable.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/Logger.hpp"
#include "VariableLoader.hpp"

AST_NAMESPACE_BEGIN


errc_t aLoadParameters(const Value& value, VariableList& list)
{
    list.clear();
    ScopedPtr<Variable> var;
    for(auto& item: value.items())
    {
        std::string name = item.first;
        Value& value = *item.second;
        var.reset(Variable::New());
        var->setName(name);
        errc_t rc = aLoadParameter(value, *var);
        if(rc)
            aWarning("failed to load parameter");
        else
            list.append(var.release());
    }
    return 0;
}


errc_t aLoadAttributes(const Value& value, VariableList& list, Object* scope)
{
    list.clear();
    ScopedPtr<Variable> var;
    for(auto& item: value.items())
    {
        std::string name = item.first;
        Value& value = *item.second;
        var.reset(Variable::New());
        var->setName(name);
        errc_t rc = aLoadAttribute(value, *var, scope);
        if(rc)
            aWarning("failed to load attribute");
        else
            list.append(var.release());
    }
    return 0;
}



errc_t aLoadCalcObjects(const Value& value, VariableList& list, Object* scope)
{
    list.clear();
    ScopedPtr<Variable> var;
    for(auto& item: value.items())
    {
        std::string name = item.first;
        Value& value = *item.second;
        var.reset(Variable::New());
        var->setName(name);
        errc_t rc = aLoadCalcObject(value, *var, scope);
        if(rc)
            aWarning("failed to load calc object");
        else
            list.append(var.release());
    }
    return 0;
}



AST_NAMESPACE_END
