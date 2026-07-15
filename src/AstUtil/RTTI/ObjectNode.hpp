///
/// @file      ObjectNode.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Object.hpp"
#include <memory>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 对象节点
/// @details 对象节点用于表示RTTI中的对象，每个对象都有一个父对象和多个子对象。
class AST_UTIL_API ObjectNode
{
public:
    ObjectNode() = default;

    ObjectNode(const ObjectNode&) = delete;
    ObjectNode& operator=(const ObjectNode&) = delete;

    explicit ObjectNode(Object* object)
        : object_(object)
        , children_()
    {}
    friend class ObjectManager;
    ~ObjectNode() = default;

    /// @brief 获取对象节点的对象
    /// @return Object* 对象指针
    Object* getObject() const {return object_.get();}

    /// @brief 检查节点的对象是否过期
    /// @return bool 是否过期
    bool expired() const {return object_.expired();}

    /// @brief 获取对象节点的父对象
    /// @return Object* 父对象指针
    Object* getParentScope() const {return parentNode_?parentNode_->getObject():nullptr;}

    /// @brief 获取对象节点的父对象的父节点
    /// @return ObjectNode* 父对象的父节点指针
    ObjectNode* getParentNode() const {return parentNode_;}

    /// @brief 获取对象节点的子节点
    /// @return std::vector<ObjectNode*>& 子节点列表
    std::vector<ObjectNode*>& getChildren(){return children_;}

    /// @brief 添加子节点
    /// @param child 子节点指针
    errc_t addChild(ObjectNode* child);

    /// @brief 设置对象节点的父节点
    /// @param parent 父节点指针
    errc_t setParent(ObjectNode* parent);

    /// @brief 移除子节点
    /// @param child 子节点指针
    errc_t removeChild(ObjectNode* child);
    
    /// @brief 清除对象节点的所有子节点
    void clearChildren();

    /// @brief 清除对象节点的所有子节点和引用计数
    void clear();

protected:
    void incRef();
    void decRef();
protected:
    WeakPtr<Object> object_;
    ObjectNode* parentNode_{nullptr};
    std::vector<ObjectNode*> children_;
};

/*! @} */

AST_NAMESPACE_END
