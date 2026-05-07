///
/// @file      RTTIAPI.hpp
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
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/StringView.hpp"
#include <map>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup RTTI
    @{
*/

// -----------
// 类型元信息
// -----------


/// @brief 获取所有已注册的类名
/// @details names 输出参数，用于存储所有已注册的类名
/// @return 无
AST_UTIL_CAPI void aGetAllClassNames(std::vector<std::string>& names);

/// @brief 获取所有已注册的类
/// @details 返回所有已注册的类的指针
/// @return 所有已注册的类的指针
AST_UTIL_API const std::unordered_map<std::string, Class*>& aGetAllClasses();

/// @brief 获取类的元数据
/// @details 根据类名获取类的元数据
/// @param name 类名
/// @return 类的元数据指针
AST_UTIL_CAPI Class* aGetClass(StringView name);

/// @brief 检查类是否为虚类
/// @param cls 类指针
/// @return 是否为虚类
AST_UTIL_CAPI bool aIsVirtualClass(StringView name);

/// @brief 注册类
/// @details 注册类的元数据，用于后续的反射操作
/// @param cls 类指针
/// @return 类的元数据指针
AST_UTIL_CAPI void aRegisterClass(Class* cls, StringView name=StringView());

/// @brief 根据类名获取类的默认对象(CDO)
/// @details CDO是类的默认构造函数创建的对象，用于存储类的默认属性值等信息，参考了Unreal Engine的设计
/// @param name 类名
/// @return 类的默认对象指针
AST_UTIL_API Object* aGetClassDefaultObject(StringView name);

/// @brief 根据类指针获取类的默认对象(CDO)
/// @details CDO是类的默认构造函数创建的对象，用于存储类的默认属性值等信息，参考了Unreal Engine的设计
/// @param cls 类指针
/// @return 类的默认对象指针
AST_UTIL_API Object* aGetClassDefaultObject(Class* cls);

/// @brief 创建对象，返回裸指针
/// @details 根据类名创建对象
/// @warning 创建的对象需要手动管理生命周期（手动释放内存）
/// @param name 类名
/// @param parentScope 父作用域指针
/// @return 对象指针
AST_UTIL_CAPI Object* aNewObject(StringView name, Object* parentScope=nullptr);


/// @brief 创建对象，返回裸指针
/// @details 根据类名创建对象
/// @param parentScope 父作用域指针
/// @return 对象指针
template<typename T>
T* aNewObject(Object* parentScope=nullptr)
{
    T* obj = new T();
    aSetParentScope(obj, parentScope);
    return obj;
}

/// @brief 删除对象
/// @details 用于释放对象裸指针对应的内存
/// @warning 不要用于智能指针管理的对象，否则会导致未定义行为
/// @param obj 对象指针
AST_UTIL_CAPI void aDeleteObject(Object* obj);

/// @brief 创建对象，返回智能指针
/// @details 根据类名创建对象
/// @param name 类名
/// @param parentScope 父作用域指针
/// @return 对象智能指针
AST_UTIL_API SharedPtr<Object> aMakeObject(StringView name, Object* parentScope=nullptr);



/// @brief 解析对象
/// @details 根据对象路径/名称/表达式解析对象
/// @warning 要求value为对象路径（带有类型名称），或者cls不为空，否则只查找类型对象
/// @param value 对象路径/名称/表达式
/// @param cls 对象类型，默认 nullptr 表示不指定类型
/// @return 解析后的对象指针
AST_UTIL_API Object* aResolveObject(StringView value, Class* cls = nullptr);


// -----------
// 对象管理
// -----------

/// @brief 根据索引获取对象
/// @details 根据对象的索引获取对象
/// @param id 对象索引/对象ID
/// @return 对象指针
AST_UTIL_CAPI Object* aGetObject(uint32_t id);

/// @brief 获取对象的索引/对象ID
/// @details obj 对象指针
/// @return 对象的索引/对象ID
AST_UTIL_CAPI uint32_t aGetObjectID(Object* obj);

/// @brief 添加对象到对象管理器
/// @details 
/// 将对象添加到对象管理器中，返回对象的索引/对象ID，
/// 如果对象已存在则返回已存在的索引/对象ID
/// @param object 对象指针
/// @return 对象的索引/对象ID
AST_UTIL_CAPI uint32_t aAddObject(Object* object);


/// @brief 移除对象
/// @details 从对象管理器中移除对象，包括子对象
/// @param object 对象指针
/// @return 错误码
AST_UTIL_CAPI errc_t aRemoveObject(Object* object);

/// @brief 移除所有对象
/// @details 从对象管理器中移除所有对象，包括子对象
/// @return 无
AST_UTIL_CAPI void aRemoveAllObjects();

/// @brief 获取对象管理器中已添加的对象数量
/// @details 获取对象管理器中已添加的对象数量
/// @return 对象数量
AST_UTIL_CAPI int aGetObjectCount();


/// @brief 查找对象
/// @details 根据类指针和对象名查找对象
/// @param cls 类指针，如果为空则匹配所有类型
/// @param name 对象名，如果为空则匹配所有名称
/// @return 对象指针向量
AST_UTIL_API std::vector<Object*> aFindObjects(Class* cls, StringView name=StringView());


/// @brief 查找对象
/// @details 根据类指针和对象名查找对象
/// @param cls 类指针，如果为空则匹配所有类型
/// @param name 对象名，如果为空则匹配所有名称
/// @return 对象指针
AST_UTIL_API Object* aFindObject(Class* cls, StringView name=StringView());

template<typename T>
T aFindObject(StringView name=StringView())
{
    // @todo: 这里逻辑和aobject_cast<T>的重复了，考虑怎么重构
    using ObjectType = typename std::decay<typename std::remove_pointer<T>::type>::type;
    static_assert(has_own_getType<ObjectType>::value, "aFindObject requires the type to has a AST_OBJECT macro");
    return static_cast<T>(aFindObject(ObjectType::StaticType(), name));
}

/// @brief 设置对象的父作用域
/// @details obj 对象指针
/// @param parentScope 父作用域指针
/// @return 错误码
AST_UTIL_CAPI errc_t aSetParentScope(Object* obj, Object* parentScope);


/// @brief 获取对象的父作用域
/// @details obj 对象指针
/// @return 父作用域指针
AST_UTIL_CAPI Object* aGetParentScope(Object* obj);


/// @brief 获取对象的祖先作用域
/// @details obj 对象指针
/// @param cls 类指针，如果为空则匹配所有类型
/// @return 祖先作用域指针
AST_UTIL_CAPI Object* aGetAncestorScope(Object* obj, Class* cls);

template<typename T>
T aGetAncestorScope(Object* obj)
{
    // @todo: 这里逻辑和aobject_cast<T>的重复了，考虑怎么重构
    using ObjectType = typename std::decay<typename std::remove_pointer<T>::type>::type;
    static_assert(has_own_getType<ObjectType>::value, "aGetAncestorScope requires the type to has a AST_OBJECT macro");
    return static_cast<T>(aGetAncestorScope(obj, ObjectType::StaticType()));
}

/// @brief 获取所有对象
/// @details 获取所有已添加的对象
/// @return 所有对象指针向量
AST_UTIL_API std::vector<Object*> aGetAllObjects();


/// @brief 打印所有对象
/// @details 打印所有已添加的对象，包括对象名称、类型等信息
/// @return 无
AST_UTIL_API void aPrintAllObjects();


/// @brief 查找对象的子对象
/// @details 根据父对象、类指针和子对象名查找子对象
/// @param parentScope 父对象指针
/// @param cls 类指针，如果为空则匹配所有类型
/// @param name 子对象名，如果为空则匹配所有名称
/// @return 子对象指针
AST_UTIL_API Object* aFindChild(Object* parentScope, Class* cls=nullptr, StringView name=StringView());


template<typename T>
T aFindChild(Object* parentScope, StringView name=StringView())
{
    // @todo: 这里逻辑和aobject_cast<T>的重复了，考虑怎么重构
    using ObjectType = typename std::decay<typename std::remove_pointer<T>::type>::type;
    static_assert(has_own_getType<ObjectType>::value, "aFindChild requires the type to has a AST_OBJECT macro");
    return static_cast<T>(aFindChild(parentScope, ObjectType::StaticType(), name));
}

/// @brief 查找对象的子对象
/// @details 根据父对象、类指针和子对象名查找子对象
/// @param parentScope 父对象指针
/// @param cls 类指针，如果为空则匹配所有类型
/// @param name 子对象名，如果为空则匹配所有名称
/// @param children 输出参数，用于存储子对象指针
/// @return 错误码
AST_UTIL_API errc_t aFindChildren(Object* parentScope, Class* cls, StringView name, std::vector<Object*>& children);


/// @brief 查找对象的子对象
/// @details 根据父对象、类指针和子对象名查找子对象
/// @param parentScope 父对象指针
/// @param cls 类指针，如果为空则匹配所有类型
/// @param name 子对象名，如果为空则匹配所有名称
/// @return 子对象指针向量
AST_UTIL_API std::vector<Object*> aFindChildren(Object* parentScope, Class* cls=nullptr, StringView name=StringView());

/// @brief 打印对象树的配置结构体
struct ObjectPrintConfig {
    bool printRefCount = false;  ///< 是否打印强引用计数
    bool printWeakRefCount = false;  ///< 是否打印弱引用计数
};


/// @brief 打印对象信息
/// @details 打印对象的名称、类型、引用计数等信息
/// @param obj 对象指针
/// @param indent 缩进级别，默认为0
/// @param config 配置结构体，默认为数
AST_UTIL_API void aPrintObject(Object* obj, int indent=0, const ObjectPrintConfig& config = ObjectPrintConfig());


/// @brief 打印对象树的结构
/// @details 递归打印对象树的结构，包括对象名称、类型等信息
/// @param root 根对象指针
/// @param indent 缩进级别，默认为0
/// @param config 配置结构体，默认为不打印引用计数
AST_UTIL_API void aPrintObjectTree(Object* root, int indent=0);
AST_UTIL_API void aPrintObjectTree(Object* root, int indent, const ObjectPrintConfig& config = ObjectPrintConfig());

/*! @} */

AST_NAMESPACE_END