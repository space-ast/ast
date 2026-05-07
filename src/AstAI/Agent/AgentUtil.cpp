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
            : json_(json)
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
            aWarning("PropertyStruct is not supported in JsonVisitorImplForJson");
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
            aWarning("PropertyPOD is not supported in JsonVisitorImplForJson");
            return 0;
        }
        errc_t visit(PropertyTimePoint& property, const void* container) override
        {
            json()[property.name()] = property.getValue<std::string>(container);
            return 0;
        }
        JsonValue& json() { return json_; }
    private:
        JsonValue internalJson_;
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
            aWarning("Property is not supported in JsonVisitorImplForClassJson");
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
            aWarning("PropertyStruct is not supported in JsonVisitorImplForJson");
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
            aWarning("PropertyPOD is not supported in JsonVisitorImplForJson");
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
        JsonValue internalJson_;
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

//--------------
// 智能体工具函数
//--------------

// 初始化类别名
bool aInitClassAliases()
{
    aRegisterClass(aGetClass("MotionMissionCommand"), "MotionAstrogator");
    return true;
}

JsonValue aFindClassesParamSchema()
{
    // static bool initialized = aInitClassAliases();
    return JsonValue();
}

std::string aFindClasses(const JsonValue& arguments)
{
    auto& classMap = aGetAllClasses();
    JsonValue json;
    for(auto& item : classMap)
    {
        auto& name = item.first;
        Class* cls = item.second;
        JsonValue classJson;
        classJson["name"] = cls->name();
        classJson["description"] = cls->desc();
        classJson["is_virtual"] = cls->isVirtual();
        classJson["schema"] = aClassJsonSchema(cls);
        if(name != cls->name())
            classJson["alias"] = name;
        if(auto parent = cls->getParent())
            classJson["parent_class"] = parent->name();
        json.append(classJson);
    }
    return json.toJsonString();
}

JsonValue aCreateObjectParamSchema()
{
    return u8R"(
        {
            "type": "object",
            "properties": {
                "class": {
                    "type": "string",
                    "description": "对象的类型，必须是通过find_classes返回的类型"
                },
                "name": {
                    "type": "string",
                    "description": "对象的名称，可选"
                },
                "parent_id": {
                    "type": "integer",
                    "description": "对象的父作用域ID，对象的生命周期将与其父作用域绑定，可选"
                }
            }
        }
    )"_json;
}

std::string aCreateObject(const JsonValue& arguments)
{
    std::string className = arguments["class"];
    Object* parentObj = nullptr;
    if(!arguments["parent_id"].isNull())
    {
        parentObj = aGetObject(arguments["parent_id"].toInt());
        if(!parentObj)
        {
            aError("parent object %d not found", arguments["parent_id"].toInt());
            return u8"父对象不存在";
        }
    }
    Object* obj = aNewObject(className, parentObj);
    if(obj)
    {
        obj->setName(arguments["name"].toString());
        return aObjectToJson(obj).toJsonString();
    }
    else
    {
        if(aIsVirtualClass(className))
        {
            aError("virtual class %s is not supported", className.c_str());
            return u8"虚类" + className + u8"不支持创建对象";
        }
        aError("create object failed: %s", className.c_str());
        return u8"创建对象失败";
    }
}


JsonValue aFindObjectsParamSchema()
{
    return u8R"(
        {
            "type": "object",
            "properties": {
                "class": {
                    "type": "string",
                    "description": "对象的类型，必须是通过find_classes返回的类型，如果没有指定类型，默认查找所有对象"
                }
            }
        }
    )"_json;
}

std::string aFindObjects(const JsonValue& arguments)
{
    std::vector<Object*> objectsMatched;
    std::vector<Object*> objects = aGetAllObjects();
    if(!arguments["class"].isNull())
    {
        Class* cls = aGetClass(arguments["class"].toString());
        if(cls)
        {
            for(auto& obj : objects)
            {
                if(obj->isOfType(cls))
                {
                    objectsMatched.push_back(obj);
                }
            }
        }
    }
    else{
        objectsMatched = objects;
    }
    JsonValue json;
    for(auto& obj : objectsMatched)
    {
        JsonValue objJson = aObjectToBriefJson(obj);
        json.append(objJson);
    }
    return json.toJsonString();
}


JsonValue aGetObjectAttributesParamSchema()
{
    return u8R"(
        {
            "type": "object",
            "properties": {
                "id": {
                    "type": "integer",
                    "description": "对象的ID"
                }
            }
        }
    )"_json;
}

std::string aGetObjectAttributes(const JsonValue& arguments)
{
    int id = arguments["id"].toInt();
    Object* obj = aGetObject(id);
    if(obj)
    {
        JsonValue json = aObjectToJson(obj);
        std::string str = json.toJsonString();
        return str;
    }
    else
    {
        aError("get object failed: %d", id);
        return u8"获取对象失败，可能是相应id的对象不存在或已被删除";
    }
}

JsonValue aSetObjectAttributeParamSchema()
{
    return u8R"(
        {
            "type": "object",
            "properties": {
                "id": {
                    "type": "integer",
                    "description": "对象的ID"
                },
                "attribute": {
                    "type": "string",
                    "description": "要设置的属性名"
                },
                "value": {
                    "description": "要设置的属性值"
                }
            }
        }
    )"_json;
}

std::string aSetObjectAttribute(const JsonValue& arguments)
{
    int id = arguments["id"].toInt();
    Object* obj = aGetObject(id);
    if(!obj)
    {
        aError("object '%d' not found", id);
        return u8"未找到对象，可能是相应id的对象不存在或已被删除";
    }
    std::string attr = arguments["attribute"].toString();
    Property* prop = obj->getProperty(attr);
    if(!prop)
    {
        // 特殊处理名称属性
        if(attr == "name")
        {
            obj->setName(arguments["value"].toString());
            return u8"设置对象名称成功";
        }
        aError("property '%s' not found", attr.c_str());
        return u8"未找到属性" + attr;
    }
    auto& valueJson = arguments["value"];
    errc_t rc = 0;
    if(valueJson.isNumber()){
        double value = valueJson.toDouble();
        if(fmod(value, 1.0) == 0.0)
        {
            rc = prop->setValueInt(obj, static_cast<int>(value));
        }
        else{
            rc = prop->setValueDouble(obj, value);
        }
    }
    else if(valueJson.isBool())
    {
        rc = prop->setValueBool(obj, valueJson.toBool());
    }
    else //if(valueJson.isString())
    {
        rc = prop->setValueString(obj, valueJson.toString());
        // 如果属性是对象类型，尝试设置为对象ID
        if (rc && prop->isObject())
        {
            rc = prop->setValueInt(obj, valueJson.toInt());
        }
    }
    
    if(rc != 0)
    {
        aError("failed to set object attribute '%s' to '%s'", attr.c_str(), valueJson.toString().c_str());
        return u8"设置对象属性失败，可能是数值不合法或者属性是只读的";
    }
    else
    {
        return u8"设置对象属性成功";
    }
}

JsonValue aShowEditDialogParamSchema()
{
    return u8R"(
        {
            "type": "object",
            "properties": {
                "id": {
                    "type": "integer",
                    "description": "对象的ID"
                }
            }
        }
    )"_json;
}

std::string aShowEditDialog(const JsonValue &arguments)
{
    int id = arguments["id"].toInt();
    Object* obj = aGetObject(id);
    if(!obj)
    {
        aError("object '%d' not found", id);
        return u8"未找到对象，可能是相应id的对象不存在或已被删除";
    }
    errc_t rc = obj->showEditDialog();
    if(rc != 0)
    {
        aError("failed to show edit dialog for object '%d'", id);
        return u8"显示编辑对话框失败，可能是当前环境不支持显示对话框";
    }
    else
    {
        return u8"显示编辑对话框成功";
    }
}

AST_NAMESPACE_END
