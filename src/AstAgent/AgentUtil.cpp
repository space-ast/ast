///
/// @file      AgentUtils.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-27
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

#include "AgentUtil.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/PropertyVisitor.hpp"
#include "AstUtil/PropertyAll.hpp"
#include "AstCore/PropertyTimePoint.hpp"
#include <cmath>
#include <climits>


AST_NAMESPACE_BEGIN





namespace{
    class PropertyVisitorImplForObjectJson : public PropertyVisitor 
    {
    public:
        explicit PropertyVisitorImplForObjectJson(int maxDepth)
            : internalJson_()
            , json_(internalJson_)
            , maxDepth_(maxDepth)
        {
        }
        PropertyVisitorImplForObjectJson(JsonValue& json, int maxDepth)
            : internalJson_()
            , json_(json)
            , maxDepth_(maxDepth)
        {
        }
        ~PropertyVisitorImplForObjectJson() override = default;
        errc_t visit(Property& property, const void* container) override
        {

            json()[property.name()] = property.getValue<std::string>(container);
            return 0;
        }
        errc_t visit(PropertyBool& property, const void* container) override
        {
            json()[property.name()] = property.getValue<bool>(container);
            return 0;
        }
        errc_t visit(PropertyDouble& property, const void* container) override
        {
            json()[property.name()] = property.getValue<double>(container);
            return 0;
        }
        errc_t visit(PropertyInt& property, const void* container) override
        {
            json()[property.name()] = property.getValue<int>(container);
            return 0;
        }
        errc_t visit(PropertyString& property, const void* container) override
        {
            json()[property.name()] = property.getValue<std::string>(container);
            return 0;
        }

        errc_t visit(PropertyObject& property, const void* container) override
        {
            Object* obj = nullptr;
            property.getValue(container, &obj);
            if(maxDepth_ > 0)
                json()[property.name()] = aObjectToJson(obj, maxDepth_ - 1);
            return 0;
        }
        errc_t visit(PropertyStruct& property, const void* container) override
        {
            // @todo
            aWarning(_("不支持 PropertyStruct"));
            return 0;
        }
        errc_t visit(PropertyQuantity& property, const void* container) override
        {
            json()[property.name()] = property.getValue<std::string>(container);
            return 0;
        }
        errc_t visit(PropertyPOD& property, const void* container) override
        {
            // @todo
            aWarning(_("不支持 PropertyPOD"));
            return 0;
        }
        errc_t visit(PropertyTimePoint& property, const void* container) override
        {
            json()[property.name()] = property.getValue<std::string>(container);
            return 0;
        }
        JsonValue& json() { return json_; }
    private:
        JsonValue internalJson_{}; 
        JsonValue& json_;
        int maxDepth_{0};
    };
    
    class PropertyVisitorImplForClassJson : public PropertyVisitor 
    {
    public:
        explicit PropertyVisitorImplForClassJson(int maxDepth)
            : internalJson_()
            , json_(internalJson_)
            , maxDepth_(maxDepth)
        {
        }
        PropertyVisitorImplForClassJson(JsonValue& json, int maxDepth)
            : json_(json)
            , maxDepth_(maxDepth)
        {
        }
        ~PropertyVisitorImplForClassJson() override = default;
        errc_t visitAny(Property& property) 
        {
            auto& propJson = json()[property.name()];
            if(!property.desc().empty())
                propJson["description"] = property.desc();
            if(property.readOnly())
                propJson["readOnly"] = true;
            if(property.writeOnly())
                propJson["writeOnly"] = true;
            return 0;
        }
        errc_t visit(Property& property, const void* container) override
        {
            aWarning(_("不支持 Property"));
            return 0;
        }
        errc_t visit(PropertyBool& property, const void* container) override
        {
            auto& propJson = json()[property.name()];
            propJson["type"] = "boolean";
            this->visitAny(property);
            return 0;
        }
        errc_t visit(PropertyDouble& property, const void* container) override
        {
            auto& propJson = json()[property.name()];
            propJson["type"] = "number";
            this->visitAny(property);
            return 0;
        }
        errc_t visit(PropertyInt& property, const void* container) override
        {
            auto& propJson = json()[property.name()];
            propJson["type"] = "integer";
            this->visitAny(property);
            return 0;
        }
        errc_t visit(PropertyString& property, const void* container) override
        {
            auto& propJson = json()[property.name()];
            propJson["type"] = "string";
            this->visitAny(property);
            return 0;
        }

        errc_t visit(PropertyObject& property, const void* container) override
        {
            auto cls = property.getClass();
            auto& propJson = json()[property.name()];
            if(cls && maxDepth_ > 0)
            {
                propJson = aClassJsonSchema(cls, maxDepth_ - 1);
            }
            else
                propJson["type"] = "object";
            this->visitAny(property);
            return 0;
        }
        errc_t visit(PropertyStruct& property, const void* container) override
        {
            // @todo
            aWarning(_("不支持 PropertyStruct"));
            return 0;
        }
        errc_t visit(PropertyQuantity& property, const void* container) override
        {
            auto& propJson = json()[property.name()];
            propJson["type"] = "string";
            this->visitAny(property);
            return 0;
        }
        errc_t visit(PropertyPOD& property, const void* container) override
        {
            // @todo
            aWarning(_("不支持 PropertyPOD"));
            return 0;
        }
        errc_t visit(PropertyTimePoint& property, const void* container) override
        {
            auto& propJson = json()[property.name()];
            propJson["type"] = "string";
            propJson["format"] = "date-time";
            this->visitAny(property);
            return 0;
        }
        JsonValue& json() { return json_; }
    private:
        JsonValue internalJson_{}; 
        JsonValue& json_;
        int maxDepth_{0};
    };
}




JsonValue aObjectToBriefJson(Object* obj)
{
    JsonValue json;
    json["name"] = obj->getName();
    json["_type"] = obj->getType()->name();
    json["_id"] = static_cast<int>(obj->getID());
    if(auto parent = obj->getParentScope())
        json["_parent_id"] = static_cast<int>(parent->getID());
    return json;
}

JsonValue aObjectToJson(Object* obj, int maxDepth)
{
    if(!obj){return JsonValue();}
    JsonValue json = aObjectToBriefJson(obj);
    auto clazz = obj->getType();
    PropertyVisitorImplForObjectJson visitor(json, maxDepth);
    while(clazz)
    {
        auto& properties = clazz->getProperties();
        for (auto property : properties) {
            if (!property) continue;
            property->accept(visitor, obj);
        }
        clazz = clazz->getParent();
    }
    return visitor.json();
}


JsonValue aClassJsonSchema(Class* cls, int maxDepth)
{
    if(!cls)
        return JsonValue();
    JsonValue json;
    json["type"] = "object";
    json["description"] = cls->desc();
    PropertyVisitorImplForClassJson visitor(json["properties"], maxDepth);
    while(cls)
    {
        auto& properties = cls->getProperties();
        for (auto property : properties) {
            if (!property) continue;
            property->accept(visitor, nullptr);
        }
        cls = cls->getParent();
    }
    return json;
}


errc_t aObject_SetAttribute(Object& object, StringView attr, const JsonValue& valueJson)
{
    Property* prop = object.getProperty(attr);
    if(!prop)
    {
        // 特殊处理名称属性
        if(attr == "name")
        {
            object.setName(valueJson.toString());
            return eNoError;
        }
        aError(_("未找到属性 '%.*s'"), attr.size(), attr.data());
        return eErrorNotFound;
    }
    errc_t rc = 0;
    if(valueJson.isNumber()){
        double value = valueJson.toDouble();
        // 使用floor判断是否为整数值，比fmod更可靠
        double intPart;
        if(std::modf(value, &intPart) == 0.0
            && intPart >= INT_MIN && intPart <= INT_MAX)
        {
            rc = prop->setValueInt(&object, static_cast<int>(intPart));
        }
        else{
            rc = prop->setValueDouble(&object, value);
        }
    }
    else if(valueJson.isBool())
    {
        rc = prop->setValueBool(&object, valueJson.toBool());
    }
    else //if(valueJson.isString())
    {
        rc = prop->setValueString(&object, valueJson.toString());
        // 如果属性是对象类型，尝试设置为对象ID
        if (rc && prop->isObject())
        {
            rc = prop->setValueInt(&object, valueJson.toInt());
        }
    }
    return rc;
}





AST_NAMESPACE_END
