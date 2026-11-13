///
/// @file      AgentTools.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-11-05
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

#include "AgentTools.hpp"
#include "AstAgent/AgentUtil.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/Object.hpp"
#include <string>

AST_NAMESPACE_BEGIN


//--------------
// 智能体工具函数
//--------------

// 初始化类别名（在首次调用aFindClasses时自动执行）
static bool aInitClassAliases()
{
    aRegisterClass(aGetClass("MotionMissionCommand"), "MotionAstrogator");
    return true;
}

JsonValue aFindClassesParamSchema()
{
    return u8R"(
        {
            "type": "object",
            "properties": {},
            "description": "查找所有可用的对象类型，无需参数"
        }
    )"_json;
}

std::string aFindClasses(const JsonValue& arguments)
{
    static bool aliasesInitialized = aInitClassAliases();
    (void)aliasesInitialized;
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
            aError(_("父对象 %d 未找到"), arguments["parent_id"].toInt());
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
            aError(_("不支持虚类 %s"), className.c_str());
            return u8"虚类" + className + u8"不支持创建对象";
        }
        aError(_("创建对象失败: %s"), className.c_str());
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
        aError(_("获取对象失败: %d"), id);
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
        aError(_("未找到对象 '%d'"), id);
        return u8"未找到对象，可能是相应id的对象不存在或已被删除";
    }
    std::string attr = arguments["attribute"].toString();
    const JsonValue& valueJson = arguments["value"];
    
    errc_t rc = aObject_SetAttribute(*obj, attr, valueJson);

    if(rc != 0)
    {
        aError(_("设置对象属性 '%s' 为 '%s' 失败"), attr.c_str(), valueJson.toString().c_str());
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
        aError(_("未找到对象 '%d'"), id);
        return u8"未找到对象，可能是相应id的对象不存在或已被删除";
    }
    errc_t rc = obj->showEditDialog();
    if(rc != 0)
    {
        aError(_("显示对象 '%d' 的编辑对话框失败"), id);
        return u8"显示编辑对话框失败，可能是当前环境不支持显示对话框";
    }
    else
    {
        return u8"显示编辑对话框成功";
    }
}


AST_NAMESPACE_END
