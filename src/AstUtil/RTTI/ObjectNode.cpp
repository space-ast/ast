///
/// @file      ObjectNode.cpp
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

#include "ObjectNode.hpp"

AST_NAMESPACE_BEGIN

Object* ObjectNode::getParentScope() const
{
    // 节点的 object_ 为 nullptr 时，获取其 parentNode_ 的操作是未定义的
    assert(this->object_.get());
    return parentNode_?parentNode_->getObject():nullptr;
}

errc_t ObjectNode::addChild(ObjectNode *child)
{
    if(!child)
        return eErrorNullInput;
    return child->setParent(this);
}

errc_t ObjectNode::removeChild(ObjectNode *child)
{
    if(!child)
        return -1;
    child->decRef();
    child->parentNode_ = nullptr;
    children_.erase(std::remove(children_.begin(), children_.end(), child), children_.end());
    return 0;
}

void ObjectNode::clearChildren()
{
    // 先置空 children_，避免在遍历中修改容器的迭代器失效
    auto children = std::move(children_);
    for(auto child : children){
        child->decRef();
        child->parentNode_ = nullptr;
    }
}

void ObjectNode::clear()
{
    clearChildren();
    
    /*
    父节点对本节点持有一个强引用，必须在这里显式释放
    这种情况应该只会在调用 removeAllObjects 以及显式移除指定对象时出现

    而在对象具有父节点时，因为父节点拥有子节点的一个强引用
    所以子对象不会进入析构逻辑，从而不会进入这里的 if 分支逻辑
    */
    
    if(parentNode_)
    {
        parentNode_->removeChild(this);
    }
    object_ = nullptr;
}

errc_t ObjectNode::setParent(ObjectNode *parent)
{
    if(this->parentNode_ == parent)
        return eNoError;
    if(!parent)
    {
        if(this->parentNode_){
            this->parentNode_->removeChild(this);
        }
        return eNoError;
    }
    this->incRef();
    if(this->parentNode_){
        this->parentNode_->removeChild(this);
    }
    this->parentNode_ = parent;
    parent->children_.push_back(this);
    return eNoError;
}

void ObjectNode::incRef()
{
    if(auto object = getObject()){
        object->incRef();
    }else{
        object_ = nullptr;
    }
}

void ObjectNode::decRef()
{
    if(auto object = getObject()){
        object->decRef();
    }else{
        object_ = nullptr;
    }
}

AST_NAMESPACE_END

