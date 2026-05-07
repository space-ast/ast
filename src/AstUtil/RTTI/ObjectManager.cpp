///
/// @file      ObjectManager.cpp
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

#include "ObjectManager.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

ObjectManager &ObjectManager::CurrentInstance()
{
    static ObjectManager instance;
    return instance;
}

Object* ObjectManager::getObject(uint32_t index)
{
    if(index >= objects_.size())
        return nullptr;
    return objects_[index]->object_.get();
}

uint32_t ObjectManager::addObject(Object *object)
{
    if(!object)
        return INVALID_ID;
    if(object->index_ != static_cast<uint32_t>(INVALID_ID))
        return object->index_;
    uint32_t index;
    uint32_t numTries = 0;
    while(1)
    {
        index = nextIndex_;
        advanceIndex();
        if(index >= objects_.size())
        {
            ObjectNode *node = new ObjectNode(object);
            object->index_ = index;
            objects_.push_back(node);
            break;
        }
        else if(objects_[index]->expired())
        {
            objects_[index]->object_ = object;
            object->index_ = index;
            break;
        }
        else if(numTries++ > maxObjectCount_)
        {
            index = INVALID_ID;
            // @todo 处理对象管理器已满的情况
            aCritical("object manager is full, max object count is %d", maxObjectCount_);
            break;
        }
    }
    return index;
}

errc_t ObjectManager::removeObject(Object *object)
{
    SharedPtr<Object> objectPtr = object;
    if(objectPtr){
        auto objNode = this->getObjectNode(object);
        if(!objNode)
            return eErrorInvalidParam;
        objNode->clear();
        return 0;
    }
    return eErrorNullPtr;
}


void ObjectManager::removeAllObjects()
{
    for(auto objNode : objects_)
    {
        SharedPtr<Object> object = objNode->getObject();
        if(object)
            object->index_ = static_cast<uint32_t>(INVALID_ID);
        objNode->clear();
    }
    nextIndex_ = 0;
}




errc_t ObjectManager::setParentScope(Object *obj, Object *parentScope)
{
    if(!obj || !parentScope)
        return -1;
    uint32_t parentScopeIndex = addObject(parentScope);
    if(parentScopeIndex == static_cast<uint32_t>(INVALID_ID))
        return -1;
    uint32_t objIndex = addObject(obj);
    if(objIndex == static_cast<uint32_t>(INVALID_ID))
        return -1;
    return objects_[objIndex]->setParent(objects_[parentScopeIndex]);
}

Object *ObjectManager::getParentScope(Object *obj)
{
    if(!obj)
        return nullptr;
    if(obj->index_ == static_cast<uint32_t>(INVALID_ID))
        return nullptr;
    auto objNode = objects_[obj->index_];
    if(objNode->parentNode_)
    {
        return objNode->parentNode_->getObject();
    }
    return nullptr;
}

Object *ObjectManager::getAncestorScope(Object *obj, Class*cls)
{
    if(!obj || !cls)
        return nullptr;
    if(obj->index_ == static_cast<uint32_t>(INVALID_ID))
        return nullptr;
    auto objNode = objects_[obj->index_];
    auto parentNode = objNode->parentNode_;
    while(parentNode)
    {
        auto parentObject = parentNode->getObject();
        if(cls->cast(parentObject))
        {
            return parentObject;
        }
        parentNode = parentNode->parentNode_;
    }
    return nullptr;
}

errc_t ObjectManager::setMaxObjectCount(uint32_t maxCount)
{
    uint32_t count = getObjectCount();
    if(count > maxCount)
        return -1;
    maxObjectCount_ = maxCount;
    return 0;
}

uint32_t ObjectManager::getObjectCount() const
{
    uint32_t count = 0;
    for(auto objNode : objects_)
    {
        if(!objNode->expired())
            count++;
    }
    return count;
}

std::vector<Object*> ObjectManager::findObjects(Class* cls, StringView name)
{
    std::vector<Object*> objects;
    for(auto objNode : objects_)
    {
        if(auto object = objNode->getObject())
        {
            if(cls == nullptr || object->isOfType(cls))
            {
                if(name.empty() || name == object->getName())
                {
                    objects.push_back(object);
                }
            }
        }
    }
    return objects;
}

Object* ObjectManager::findObject(Class* cls, StringView name)
{
    for(auto objNode : objects_)
    {
        if(auto object = objNode->getObject())
        {
            if(cls == nullptr || object->isOfType(cls))
            {
                if(name.empty() || name == object->getName())
                {
                    return object;
                }
            }
        }
    }
    return nullptr;
}


ObjectNode* ObjectManager::getObjectNode(Object* obj)
{
    if(!obj)
        return nullptr;
    if(obj->index_ == static_cast<uint32_t>(INVALID_ID))
        return nullptr;
    if(obj->index_ >= objects_.size())
        return nullptr;
    return objects_[obj->index_];
}


std::vector<Object*> ObjectManager::getAllObjects()
{
    std::vector<Object*> objects;
    for(auto objNode : objects_)
    {
        if(!objNode->expired())
        {
            objects.push_back(objNode->object_.get());
        }
    }
    return objects;
}

errc_t ObjectManager::removeNode(uint32_t index)
{
    if(index >= objects_.size())
        return -1;
    objects_[index]->clear();
    return 0;
}

void ObjectManager::advanceIndex()
{
    nextIndex_++;
    nextIndex_ %= maxObjectCount_;
}

AST_NAMESPACE_END