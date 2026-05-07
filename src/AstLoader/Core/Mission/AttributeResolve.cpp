///
/// @file      AttributeResolve.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-06
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

#include "AttributeResolve.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/Return.hpp"
#include "AstCore/TargeterSequence.hpp"
#include "AstCore/DifferentialCorrectorProfile.hpp"
#include "AstCore/ScriptingToolProfile.hpp"
#include "AstCore/BurnImpulsive.hpp"
#include "AstCore/EventDetector.hpp"
#include "AstCore/TargeterProfile.hpp"

AST_NAMESPACE_BEGIN


Attribute aResolveAttributeByMap(Object* obj, StringView attrpath)
{
    if(obj->isOfType<Maneuver>())
    {
        if(attrpath == "ImpulsiveMnvr")
        {
            attrpath = aPropertyName(Maneuver, burn);
            return obj->attr(attrpath);
        }
    }
    else if(obj->isOfType<Return>())
    {
        if(attrpath == "RetState")
        {
            attrpath = aPropertyName(Return, enabled);
            return obj->attr(attrpath);
        }
    }
    else if(obj->isOfType<Variable>())
    {
        if(attrpath == "ParamValue")
        {
            attrpath = aPropertyName(Variable, value);
            return obj->attr(attrpath);
        }
    }
    aWarning("failed to resolve attribute '%.*s' for object '%s<%s>'", 
        attrpath.size(), attrpath.data(), 
        obj->getName().c_str(), obj->typeName().c_str()
    );
    return Attribute();
}

Attribute aResolveAttributeByMap(Object* obj, StringView subAttrPath, StringView remainingAttrPath)
{
    if(obj->isOfType<BurnImpulsive>())
    {
        if(subAttrPath == "Cartesian")
        {
            return obj->attr(remainingAttrPath);
        }
    }
    else if(Propagate* prop = aobject_cast<Propagate*>(obj))
    {
        if(subAttrPath == "StoppingConditions")
        {
            auto pos = remainingAttrPath.find('.');
            if(pos == String::npos)
            {
                aError("unsupported attribute path %.*s", remainingAttrPath.size(), remainingAttrPath.data());
                return Attribute();
            }
            else
            {
                StringView eventDetectorName = remainingAttrPath.substr(0, pos);
                EventDetector* eventDetector = prop->getEventDetector(eventDetectorName);
                if(eventDetector)
                    return aResolveAttribute(eventDetector, remainingAttrPath.substr(pos + 1));
                else
                    return Attribute();
            }
        }
    }
    else if(TargeterSequence* seq = aobject_cast<TargeterSequence*>(obj))
    {
        if(subAttrPath == "Profiles")
        {
            auto pos = remainingAttrPath.find('.');
            if(pos == String::npos)
            {
                aError("unsupported attribute path %.*s", remainingAttrPath.size(), remainingAttrPath.data());
                return Attribute();
            }
            else
            {
                StringView profileName = remainingAttrPath.substr(0, pos);
                TargeterProfile* profile = seq->getTargeterProfile(profileName);
                if(profile)
                    return aResolveAttribute(profile, remainingAttrPath.substr(pos + 1));
                else
                    return Attribute();
            }
        }
    }
    else if(ScriptingToolProfile* profile = aobject_cast<ScriptingToolProfile*>(obj))
    {
        if(subAttrPath == "Parameters")
        {
            auto pos = remainingAttrPath.find('.');
            if(pos == String::npos)
            {
                aError("unsupported attribute path %.*s", remainingAttrPath.size(), remainingAttrPath.data());
                return Attribute();
            }
            else
            {
                StringView parameterName = remainingAttrPath.substr(0, pos);
                Variable* parameter = profile->getParameter(parameterName);
                if(parameter)
                    return aResolveAttribute(parameter, remainingAttrPath.substr(pos + 1));
                else
                    return Attribute();
            }
        }
    }
    else if(DifferentialCorrectorProfile* profile = aobject_cast<DifferentialCorrectorProfile*>(obj))
    {
        if(subAttrPath == "Results")
        {
            auto pos = remainingAttrPath.find('.');
            if(pos == String::npos)
            {
                aError("unsupported attribute path %.*s", remainingAttrPath.size(), remainingAttrPath.data());
                return Attribute();
            }
            else
            {
                StringView resultName = remainingAttrPath.substr(0, pos);
                ShooterResult* result = profile->getResult(resultName);
                if(result)
                    return aResolveAttribute(result, remainingAttrPath.substr(pos + 1));
                else
                    return Attribute();
            }
        }
    }

    // 如果前面没有处理，则转发到另一个函数处理
    Attribute subattr = aResolveAttributeByMap(obj, subAttrPath);
    if(subattr.isValid())
        return aResolveAttribute(subattr, remainingAttrPath);
    aWarning("failed to resolve attribute '%.*s.%.*s' for object '%s<%s>'", 
        subAttrPath.size(), subAttrPath.data(),
        remainingAttrPath.size(), remainingAttrPath.data(), 
        obj->getName().c_str(), obj->typeName().c_str()
    );
    return Attribute();
}


Attribute aResolveAttribute(const Attribute& attr, StringView attrpath)
{
    Object* object = attr.getValueObject();
    if(!object)
    {
        aError("resolve attribute %.*s failed, object is null", attrpath.size(), attrpath.data());
        return Attribute();
    }
    return aResolveAttribute(object, attrpath);
}


Attribute aResolveAttribute(Object* obj, StringView attrpath)
{
    if(!obj)
        return Attribute();
    auto pos = attrpath.find('.');
    if(pos == String::npos)
    {
        auto attr = obj->attr(attrpath);
        if(attr.isValid())
            return attr;
        return aResolveAttributeByMap(obj, attrpath);
    }
    else
    {
        StringView subAttrPath = attrpath.substr(0, pos);
        StringView remainingAttrPath = attrpath.substr(pos + 1);
        Attribute subattr = obj->attr(subAttrPath);
        if(subattr.isValid())
            return aResolveAttribute(subattr, remainingAttrPath);
        return aResolveAttributeByMap(obj, subAttrPath, remainingAttrPath);
    }
}


AST_NAMESPACE_END
