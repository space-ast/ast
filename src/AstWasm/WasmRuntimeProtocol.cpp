///
/// @file      ObjectProtocol.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
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

#include "WasmRuntimeProtocol.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstAI/AgentUtil.hpp"
#include "AstAI/AgentInit.hpp"

AST_NAMESPACE_BEGIN

errc_t WasmRuntimeProtocol::setAttrString(ObjectId id, const std::string& name, const std::string& value)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return eErrorNullPtr;
    return obj->setAttrString(name, value);
}

errc_t WasmRuntimeProtocol::setAttrDouble(ObjectId id, const std::string& name, double value)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return eErrorNullPtr;
    return obj->setAttrDouble(name, value);
}

errc_t WasmRuntimeProtocol::setAttrInt(ObjectId id, const std::string& name, int value)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return eErrorNullPtr;
    return obj->setAttrInt(name, value);
}


errc_t WasmRuntimeProtocol::setAttrBool(ObjectId id, const std::string& name, bool value)
{
    Object* obj = aGetObject(id);
    if(obj == nullptr)
        return eErrorNullPtr;
    return obj->setAttrBool(name, value);
}


errc_t WasmRuntimeProtocol::setAttrObject(ObjectId id, const std::string& name, ObjectId value)
{
    Object* obj = aGetObject(id);
    if(obj == nullptr)
        return eErrorNullPtr;
    Object* valueObj = aGetObject(value);
    if(valueObj == nullptr)
        return eErrorNullPtr;
    return obj->setAttrObject(name, valueObj);
}



std::optional<std::string> WasmRuntimeProtocol::getAttrString(ObjectId id, const std::string& name)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return std::nullopt;
    std::string value;
    errc_t ret = obj->getAttrString(name, value);
    if (ret != 0)
        return std::nullopt;
    return value;
}


std::optional<double> WasmRuntimeProtocol::getAttrDouble(ObjectId id, const std::string& name)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return std::nullopt;
    double value;
    errc_t ret = obj->getAttrDouble(name, value);
    if (ret != 0)
        return std::nullopt;
    return value;
}

std::optional<int> WasmRuntimeProtocol::getAttrInt(ObjectId id, const std::string& name)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return std::nullopt;
    int value;
    errc_t ret = obj->getAttrInt(name, value);
    if (ret != 0)
        return std::nullopt;
    return value;
}



std::optional<bool> WasmRuntimeProtocol::getAttrBool(ObjectId id, const std::string& name)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return std::nullopt;
    bool value;
    errc_t ret = obj->getAttrBool(name, value);
    if (ret != 0)
        return std::nullopt;
    return value;
}

std::optional<ObjectId> WasmRuntimeProtocol::getAttrObject(ObjectId id, const std::string& name)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return std::nullopt;
    Object* valueObj = nullptr;
    errc_t ret = obj->getAttrObject(name, valueObj);
    if (ret != 0 || valueObj == nullptr)
        return std::nullopt;
    return valueObj->getID();
}

std::optional<std::string> WasmRuntimeProtocol::objectType(ObjectId id)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return std::nullopt;
    return obj->getType()->name();
}

static WasmObjectInfo aObjectInfo(Object& obj)
{
    WasmObjectInfo info{};
    info.id = obj.getID();
    info.type = obj.getType()->name();
    info.name = obj.getName();
    if(auto parent = obj.getParentScope())
        info.parent = parent->getID();
    std::vector<Object*> children = aFindChildren(&obj);
    for(auto child: children)
    {
        if(child)
            info.children.push_back(child->getID());
    }
    return info;
}

std::optional<WasmObjectInfo>  WasmRuntimeProtocol::objectInfo(ObjectId id)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return std::nullopt;
    return aObjectInfo(*obj);
}


std::vector<ObjectId> WasmRuntimeProtocol::allObjects()
{
    std::vector<ObjectId> ids;
    std::vector<Object*> objects = aGetAllObjects();
    for(auto object: objects)
    {
        if(object)
            ids.push_back(object->getID());
    }
    return ids;
}

std::vector<WasmObjectInfo> WasmRuntimeProtocol::allObjectInfoList()
{
    std::vector<WasmObjectInfo> infoList;
    std::vector<Object*> objects = aGetAllObjects();
    for(auto object: objects)
    {
        if(object)
            infoList.push_back( aObjectInfo(*object));
    }
    return infoList;
}



std::optional<ObjectId> WasmRuntimeProtocol::newObject(const std::string& typeName, std::optional<ObjectId> parentId)
{
    Object* parent = nullptr;
    if(parentId.has_value())
        parent = aGetObject(parentId.value());
    auto obj = aNewObject(typeName, parent);
    if (obj == nullptr)
        return std::nullopt;
    return obj->getID();
}


errc_t WasmRuntimeProtocol::removeObject(ObjectId id)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return eErrorNullPtr;
    return aRemoveObject(obj);
}




std::optional<std::string> WasmRuntimeProtocol::objectToJsonStr(ObjectId id)
{
    Object* obj = aGetObject(id);
    if (obj == nullptr)
        return std::nullopt;
    return aObjectToJson(obj).toJsonString();
}

std::optional<std::string> WasmRuntimeProtocol::classJsonSchemaStr(const std::string& typeName)
{
    Class* cls = aGetClass(typeName);
    if (cls == nullptr)
        return std::nullopt;
    JsonValue schema = aClassJsonSchema(cls);
    return schema.toJsonString();
}

std::vector<std::string> WasmRuntimeProtocol::getAllClassNames()
{
    std::vector<std::string> classNames;
    aGetAllClassNames(classNames);
    return classNames;
}


std::string WasmRuntimeProtocol::handleToolCall(const std::string& toolCallJsonStr)
{
    return aAgentToolsHandleToolCall(toolCallJsonStr);
}



AST_NAMESPACE_END
