///
/// @file      RTTIAPI.cpp
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

#include "RTTIAPI.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/ClassRegistry.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectManager.hpp"
#include "AstUtil/ColoredPrint.hpp"
#include "AstUtil/Logger.hpp"
#include <cstdio>


AST_NAMESPACE_BEGIN

void aGetAllClassNames(std::vector<std::string> &names)
{
    ClassRegistry::Instance()->getAllClassNames(names);
}

const std::unordered_map<std::string, Class*>& aGetAllClasses()
{
    return ClassRegistry::Instance()->getAllClasses();
}

Class *aGetClass(StringView name)
{
    return ClassRegistry::Instance()->getClass(name);
}

bool aIsVirtualClass(StringView name)
{
    Class *cls = aGetClass(name);
    return cls && cls->isVirtual();
}

void aRegisterClass(Class *cls, StringView name)
{
    if(name.empty())
        return ClassRegistry::Instance()->registerClass(cls);
    return ClassRegistry::Instance()->registerClass(cls, name);
}

Object *aGetClassDefaultObject(StringView name)
{
    Class *cls = aGetClass(name);
    return aGetClassDefaultObject(cls);
}

Object *aGetClassDefaultObject(Class *cls)
{
    if(!cls)
        return nullptr;
    return cls->getDefaultObject();
}

Object *aNewObject(StringView name, Object* parentScope)
{
    Class *cls = aGetClass(name);
    if(!cls)
        return nullptr;
    return cls->newObject(parentScope);
}

void aDeleteObject(Object *obj)
{
    obj->destruct();
}

SharedPtr<Object> aMakeObject(StringView name, Object* parentScope)
{
    return aNewObject(name, parentScope);
}


Object *aResolveObject(StringView value, Class* cls)
{
    auto pos = value.find("/");
    if(pos == StringView::npos)
    {
        if(cls)
        {
            do
            {
                auto obj = cls->resolve(value);
                if(obj && obj->isOfType(cls))
                    return obj;
                cls = cls->getParent();
            }while(cls);
            return aFindObject(cls, value);
        }
        else
            return aGetClass(value);
    }
    else
    {
        StringView className = value.substr(0, pos);
        auto pos2 = value.substr(pos + 1).find("/");
        StringView objName = value.substr(pos + 1, pos2);
        Object* obj = aFindObject(aGetClass(className), objName);
        while(pos2 != StringView::npos)
        {
            value = value.substr(pos2 + 1);
            pos = value.find("/");
            if(pos == StringView::npos)
            {
                aError("invalid object path: '%.*s'", value.size(), value.data());
                return nullptr;
            }
            className = value.substr(0, pos);
            pos2 = value.substr(pos + 1).find("/");
            objName = value.substr(pos + 1, pos2);
            obj = aFindChild(obj, cls, objName);
        }
        return obj;
    }
    return nullptr;
}

Object *aGetObject(uint32_t id)
{
    return ObjectManager::CurrentInstance().getObject(id);
}

uint32_t aAddObject(Object *object)
{
    return ObjectManager::CurrentInstance().addObject(object);
}

errc_t aRemoveObject(Object *object)
{
    return ObjectManager::CurrentInstance().removeObject(object);
}

void aRemoveAllObjects()
{
    ObjectManager::CurrentInstance().removeAllObjects();
}

int aGetObjectCount()
{
    return ObjectManager::CurrentInstance().getObjectCount();
}


std::vector<Object*> aFindObjects(Class* cls, StringView name)
{
    return ObjectManager::CurrentInstance().findObjects(cls, name);
}

Object *aFindObject(Class* cls, StringView name)    
{
    return ObjectManager::CurrentInstance().findObject(cls, name);
}

errc_t aSetParentScope(Object *obj, Object *parentScope)
{
    return ObjectManager::CurrentInstance().setParentScope(obj, parentScope);
}

Object *aGetParentScope(Object *obj)
{
    return ObjectManager::CurrentInstance().getParentScope(obj);
}

Object *aGetAncestorScope(Object *obj, Class *cls)
{
    return ObjectManager::CurrentInstance().getAncestorScope(obj, cls);
}

std::vector<Object*> aGetAllObjects()
{
    return ObjectManager::CurrentInstance().getAllObjects();
}

void aPrintAllObjects()
{
    auto objects = aGetAllObjects();
    for(auto obj : objects)
    {
        aPrintObject(obj);
    }
}

Object *aFindChild(Object *parentScope, Class *cls, StringView name)
{
    if(!parentScope)
        return nullptr;
    
    // 获取父对象对应的对象节点
    auto& objectManager = ObjectManager::CurrentInstance();
    auto parentNode = objectManager.getObjectNode(parentScope);
    if(!parentNode)
        return nullptr;
    
    // 遍历父对象的子节点
    for(auto childNode : parentNode->getChildren())
    {
        if(childNode->expired())
            continue;
        
        auto childObject = childNode->getObject();
        if(!childObject)
            continue;
        
        // 检查类型是否匹配
        if(cls)
        {
            bool typeMatch = false;
            auto childType = childObject->getType();
            while(childType)
            {
                if(childType == cls)
                {
                    typeMatch = true;
                    break;
                }
                childType = childType->getParent();
            }
            if(!typeMatch)
                continue;
        }
        
        // 检查名称是否匹配
        if(name.data())
        {
            if(name != childObject->getName())
                continue;
        }
        
        return childObject;
    }
    
    return nullptr;
}

errc_t aFindChildren(Object *parentScope, Class *cls, StringView name, std::vector<Object*> &children)
{
    if(!parentScope)
        return -1;
    
    children.clear();
    
    // 获取父对象对应的对象节点
    auto& objectManager = ObjectManager::CurrentInstance();
    auto parentNode = objectManager.getObjectNode(parentScope);
    if(!parentNode)
        return -1;
    auto& childrenNodes = parentNode->getChildren();
    children.reserve(childrenNodes.size());
    
    // 遍历父对象的子节点
    for(auto childNode : childrenNodes)
    {
        if(childNode->expired())
            continue;
        
        auto childObject = childNode->getObject();
        if(!childObject)
            continue;
        
        // 检查类型是否匹配
        if(cls)
        {
            bool typeMatch = false;
            auto childType = childObject->getType();
            while(childType)
            {
                if(childType == cls)
                {
                    typeMatch = true;
                    break;
                }
                childType = childType->getParent();
            }
            if(!typeMatch)
                continue;
        }
        
        // 检查名称是否匹配
        if(name.data())
        {
            if(name != childObject->getName())
                continue;
        }
        
        children.push_back(childObject);
    }
    
    return 0;
}

std::vector<Object*> aFindChildren(Object* parentScope, Class* cls, StringView name)
{
    std::vector<Object*> children;
    aFindChildren(parentScope, cls, name, children);
    return children;
}

void aPrintObject(Object* obj, int indent, const ObjectPrintConfig& config)
{
    if(!obj)
        return;
    // 打印缩进
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    // 打印对象信息
    const std::string& name = obj->getName();
    Class* type = obj->getType();
    uint32_t id = obj->getID();
    uint32_t refCount = obj->refCount();
    uint32_t weakRefCount = obj->weakRefCount();

    // 使用不同颜色和符号来区分字段
    cprintf(eGreen, "%s", name.c_str());
    cprintf(eLightGray, "#%u", id);
    cprintf(eCyan, " <%s>", type ? type->name().c_str() : "Unknown");
    
    // 根据配置决定是否打印引用计数
    if (config.printRefCount) {
        cprintf(eLightPink, " R:%u", refCount);
    }
    if (config.printWeakRefCount) {
        cprintf(eLightBlue, " W:%u", weakRefCount);
    }
    printf("\n");
}

void aPrintObjectTree(Object* root, int indent)
{
    aPrintObjectTree(root, indent, ObjectPrintConfig());
}

void aPrintObjectTree(Object* root, int indent, const ObjectPrintConfig& config)
{
    if (!root) {
        return;
    }
    aPrintObject(root, indent, config);

    // 打印子对象
    std::vector<Object*> children = aFindChildren(root);
    for (Object* child : children) {
        aPrintObjectTree(child, indent + 1, config);
    }
}

AST_NAMESPACE_END