///
/// @file      ObjectManager.hpp
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
#include "ObjectNode.hpp"
#include <vector>
#include <limits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 对象管理器
class AST_UTIL_API ObjectManager
{
public:
    static ObjectManager& CurrentInstance();
    
    ObjectManager() = default;
    ~ObjectManager() = default;
public:
    /// @brief 根据索引获取对象
    /// @param index 对象索引/对象ID
    /// @return 对象指针
    Object* getObject(uint32_t index);

    /// @brief 添加对象到对象管理器
    /// @details 
    /// 将对象添加到对象管理器中，返回对象的索引/对象ID，
    /// 如果对象已存在则返回已存在的索引/对象ID
    /// @param object 对象指针
    /// @return 对象的索引/对象ID
    uint32_t addObject(Object* object);

    /// @brief 移除对象
    /// @details 从对象管理器中移除对象，包括子对象
    /// @param object 对象指针
    /// @return 错误码
    errc_t removeObject(Object* object);

    /// @brief 移除所有对象
    /// @details 从对象管理器中移除所有对象，包括子对象
    void removeAllObjects();

    /// @brief 设置对象的父作用域
    /// @details obj 对象指针
    /// @param parentScope 父作用域指针
    errc_t setParentScope(Object* obj, Object* parentScope);


    /// @brief 获取对象的父作用域
    /// @details obj 对象指针
    /// @return 父作用域指针
    Object* getParentScope(Object* obj);


    /// @brief 获取对象的祖先作用域
    /// @details obj 对象指针
    /// @param cls 类指针，如果为空则匹配所有类型
    /// @return 祖先作用域指针
    Object* getAncestorScope(Object* obj, Class* cls);

    /// @brief 获取对象节点数量
    /// @warning 对象节点数量可能大于对象数量，因为对象节点对应的对象可能已被删除
    /// @return 对象节点数量
    size_t getNodeCount() const {return objects_.size();}

    /// @brief 获取可以管理的最大对象数量
    /// @return 最大对象数量
    uint32_t getMaxObjectCount() const{return maxObjectCount_;}

    /// @brief 设置可以管理的最大对象数量
    /// @details 如果最大对象数量小于当前对象数量，则设置失败
    /// @param maxCount 最大对象数量
    /// @warning 这个函数会限制对象管理器中最大对象数量，请谨慎使用
    errc_t setMaxObjectCount(uint32_t maxCount);
    
    /// @brief 获取当前对象数量
    /// @warning 这个函数会遍历所有对象节点判断对象是否存活，时间复杂度为O(n)，请谨慎使用
    /// @return 当前对象数量
    uint32_t getObjectCount() const;

    /// @brief 查找对象
    /// @details 根据类指针和对象名查找对象
    /// @param cls 类指针，如果为空则匹配所有类型
    /// @param name 对象名，如果为空则匹配所有名称
    /// @return 对象指针向量
    std::vector<Object*> findObjects(Class* cls, StringView name=StringView());

    /// @brief 查找对象
    /// @details 根据类指针和对象名查找对象
    /// @param cls 类指针，如果为空则匹配所有类型
    /// @param name 对象名，如果为空则匹配所有名称
    /// @return 对象指针
    Object* findObject(Class* cls, StringView name=StringView());

    /// @brief 获取对象对应的对象节点
    /// @param obj 对象指针
    /// @return 对象节点指针
    ObjectNode* getObjectNode(Object* obj);

    /// @brief 获取所有对象
    /// @return 所有对象指针向量
    std::vector<Object*> getAllObjects();
protected:
    friend class Object;
    errc_t removeNode(uint32_t index);

    void advanceIndex();
protected:
    constexpr static uint32_t kDefaultMaxObjectCount = static_cast<uint32_t>(-1) - 1;

    std::vector<ObjectNode*> objects_;                      ///< 所有对象节点
    uint32_t nextIndex_{0};                                 ///< 下一个索引
    uint32_t maxObjectCount_{kDefaultMaxObjectCount};       ///< 最大对象数量
};

/*! @} */

AST_NAMESPACE_END