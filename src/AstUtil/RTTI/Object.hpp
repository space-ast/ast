///
/// @file      Object.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/ReflectAPI.hpp"
#include <string>       // for std::string
#include <stdint.h>     // for uint32_t
#include <assert.h>     // for assert
#include <atomic>       // for std::atomic

AST_NAMESPACE_BEGIN


/*!
    @ingroup Util
    @defgroup RTTI 运行时类型信息
    @brief 提供运行时类型信息功能，包括类元信息、属性元信息、对象的强弱引用计数。
*/

/*!
    @addtogroup RTTI
    @{
*/
 
class Class;        // 类元信息
class Property;     // 属性元信息

template<typename _Object>
class WeakPtr;

template<typename ObjectPtrType, typename PropertyType>
class AttributeBasic;

typedef AttributeBasic<WeakPtr<Object>, Property> Attribute;

// AST 对象运行时元信息
#define AST_OBJECT(TYPE) \
    static Class staticType;\
    static inline Class* StaticType(){return &staticType;}\
    Class* getType() const override{return &staticType;} \
    static void ClassInit(Class* cls);\

// 定义属性
#define AST_PROPERT(NAME)   static constexpr const char* _prop_##NAME = #NAME;

// 获取属性名称
#define AST_PROPERT_NAME(CLASS, NAME) CLASS::_prop_##NAME
#define aPropertyName(CLASS, NAME) AST_PROPERT_NAME(CLASS, NAME)

#define _AST_IMPL_OBJECT(TYPE) \
    Class TYPE::staticType;\


enum {
    INVALID_ID = -1, ///< 无效对象ID
};

struct initial_strong_ref_t {};                      ///< 初始化强引用计数的标记
constexpr initial_strong_ref_t initial_strong_ref{}; ///< 初始化强引用计数的标记值


/// @brief 对象基类，继承自该类的对象可以使用运行时类型信息相关功能，实现强弱引用计数、运行时元信息（属性访问、序列化等）等基础功能
/// @details 参考了Qt的QObject类、UE的UObject类、以及Python的PyObject等类的设计和实现
class AST_UTIL_API Object
{
public:
    /// @brief 解析字符串
    /// @param value 字符串值
    /// @return Object* 解析结果
    static Object* Resolve(StringView value);
    
    // friend uint32_t aObject_IncRef(Object* obj);
    // friend uint32_t aObject_DecRef(Object* obj);
    // friend uint32_t aObject_IncWeakRef(Object* obj);
    // friend uint32_t aObject_DecWeakRef(Object* obj);
    // friend bool aObject_IsDestructed(const Object* obj);
    
    Object()
        : refcnt_{0}
        , weakrefcnt_{1}
    {}
    explicit Object(Object* parentScope);
    Object(std::nullptr_t);
    
    /// @brief 构造函数，用于初始化对象的强引用计数为1
    /// @param initial_strong_ref_t 初始化强引用计数的标记
    Object(initial_strong_ref_t)
        : refcnt_{1}
        , weakrefcnt_{1}
    {}
public:
    static Class staticType;
    static void ClassInit(Class* cls);
    static inline Class* StaticType(){return &staticType;}

    /// @brief 获取对象的类型元信息
    /// @return Class* 类型元信息指针
    virtual Class* getType() const;

    /// @brief 获取对象的名称
    virtual const std::string& getName() const;

    /// @brief 设置对象的名称
    /// @param name 对象名称
    virtual void setName(StringView name) = 0;

    /// @brief 获取对象的名称
    const std::string& name() const {return this->getName();}
public: // 编辑属性
    
    /// @brief 显示编辑对话框，用于编辑对象的属性
    /// @return errc_t 错误码
    errc_t showEditDialog();

public: // 通用属性访问
    /// @brief 获取属性，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @return Attribute 属性
    Attribute attr(StringView path);

public: // 获取属性
    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值引用
    /// @return errc_t 错误码
    errc_t getAttrBool(StringView path, bool& value) const;
    
    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值引用
    /// @return errc_t 错误码
    errc_t getAttrInt(StringView path, int& value) const;

    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值引用
    /// @return errc_t 错误码
    errc_t getAttrDouble(StringView path, double& value) const;

    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值引用
    /// @return errc_t 错误码
    errc_t getAttrString(StringView path, std::string& value) const;


    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值引用
    /// @return errc_t 错误码
    errc_t getAttrObject(StringView path, Object*& value) const;

public: // 获取属性值
    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @return double 属性值
    double getAttrDouble(StringView path) const;

    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @return int 属性值
    int getAttrInt(StringView path) const;

    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @return bool 属性值
    bool getAttrBool(StringView path) const;

    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @return std::string 属性值
    std::string getAttrString(StringView path) const;


    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @return Object* 属性值
    Object* getAttrObject(StringView path) const;

public: // 设置属性值
    /// @param path 属性路径
    /// @param value 属性值
    /// @return errc_t 错误码
    errc_t setAttrBool(StringView path, bool value);

    /// @brief 设置属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值
    /// @return errc_t 错误码
    errc_t setAttrInt(StringView path, int value);

    /// @brief 设置属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值
    /// @return errc_t 错误码
    errc_t setAttrDouble(StringView path, double value);

    /// @brief 设置属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值
    /// @return errc_t 错误码
    errc_t setAttrString(StringView path, StringView value);


    /// @brief 设置属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值
    /// @return errc_t 错误码
    errc_t setAttrObject(StringView path, Object* value);

public: // 类型与字段属性
    /// @brief 获取对象类型
    /// @return Class* 类型元信息
    Class* type() const{return getType();} // 转发到新接口getType

    const std::string& typeName() const;

    /// @brief 获取属性元信息
    /// @param fieldName 属性名
    /// @return Property* 属性元信息
    Property* getProperty(StringView fieldName) const;
public: // 对象ID

    /// @brief 获取对象ID
    /// @return ObjectId 对象ID
    ObjectId getID() const;

    /// @brief 设置对象的父作用域
    /// @param parentScope 父作用域指针
    /// @return errc_t 错误码
    errc_t setParentScope(Object* parentScope);

    /// @brief 获取对象的父作用域
    /// @return Object* 父作用域指针
    Object* getParentScope() const;

    /// @brief 判断对象是否为指定类型的实例
    /// @param type 类型元信息
    /// @return bool 是否为指定类型的实例
    bool isOfType(const Class* type) const;

    /// @brief 判断对象是否为指定类型的实例
    /// @param typeName 类型名
    /// @return bool 是否为指定类型的实例   
    bool isOfType(StringView typeName) const;

    /// @brief 判断对象是否为指定类型的实例
    /// @param T 类型参数
    template<typename T>
    bool isOfType() const;

public: // 引用计数
    /// @brief 获取强引用计数
    /// @return uint32_t 强引用计数
    uint32_t refCount() const{return refcnt_;}

    /// @brief 获取弱引用计数
    /// @return uint32_t 弱引用计数
    uint32_t weakRefCount() const{return weakrefcnt_;}

    /// @brief 判断对象是否被析构
    /// @return bool 是否已析构
    bool     isDestructed() const{return aObject_IsDestructed(this);}

    /// @brief 析构对象，仅当强引用计数为0时才会被调用
    /// @details 析构对象时，会先将弱引用计数减1，若弱引用计数为0，则会调用析构函数
    /// @warning 对于栈上的对象，不要调用该函数，避免对栈内存调用 delete 导致崩溃
    void     destruct()
    {
        #ifndef NDEBUG
        if(refcnt_ != 0)
        {
            printf("object ref count is not 0, can not destruct");
        }
        #endif
        assert(refcnt_ == 0);  // 只能直接删除不采用共享引用计数管理的对象
        this->_destruct();
    }

    /// @brief 增加弱引用计数
    /// @return uint32_t 新的弱引用计数
    uint32_t incWeakRef()
    {
        return aObject_IncWeakRef(this);
    }

    /// @brief 减少弱引用计数
    /// @return uint32_t 新的弱引用计数
    uint32_t decWeakRef()
    {
        return aObject_DecWeakRef(this);
    }

    /// @brief 增加强引用计数
    /// @return uint32_t 新的强引用计数
    uint32_t incRef()
    {
        return aObject_IncRef(this);
    }

    /// @brief 减少强引用计数
    /// @return uint32_t 新的强引用计数
    uint32_t decRef()
    {
        return aObject_DecRef(this);
    }

    /// @brief 减少强引用计数，不删除对象
    /// @return uint32_t 新的强引用计数
    uint32_t decRefNoDelete()
    {
        return --refcnt_;
    }
private:
    /// @brief 析构对象，仅当强引用计数为0时才会被调用
    /// @warning 对于栈上的对象，不要调用该函数，避免对栈内存调用 delete 导致崩溃
    /// @details 析构对象时，会先将弱引用计数减1，若弱引用计数为0，则会调用析构函数
    void    _destruct()
    {
        this->~Object();
        this->decWeakRef();
    }
public: // 延迟链接
    /// @brief 添加延迟链接
    /// @param link 延迟链接函数
    /// @warning 必须include ObjectLinker.hpp
    /// @see ObjectLinker.hpp
    template<typename Func>
    inline void addDelayedLink(Func &&link);


    /// @brief 若直接解析失败则添加延迟链接
    /// @param link 延迟链接函数
    /// @warning 必须include ObjectLinker.hpp
    /// @see ObjectLinker.hpp
    template<typename Func>
    inline void addDelayedLinkIfFailed(Func &&link);

    /// @brief 解析延迟链接
    /// @details 解析所有延迟链接
    /// @warning 必须include ObjectLinker.hpp
    /// @see ObjectLinker.hpp
    void resolveLinks();

protected:
    friend class ObjectManager;
    virtual ~Object();
    Object(const Object& obj)
        : Object()
    {}
    Object& operator=(const Object&)
    {
        return *this;
    }
public: // 实参依赖查找（ADL）
    A_ALWAYS_INLINE 
    friend uint32_t aObject_IncRef(Object* obj)
    {
        return ++(obj->refcnt_);
    }
    A_ALWAYS_INLINE 
    friend uint32_t aObject_DecRef(Object* obj)
    {
        if (obj->refcnt_ == 1) {
            obj->_destruct();
            return 0;
        }
        return --(obj->refcnt_);
    }
    A_ALWAYS_INLINE 
    friend uint32_t aObject_IncWeakRef(Object* obj)
    {
        return ++(obj->weakrefcnt_);
    }
    A_ALWAYS_INLINE 
    friend uint32_t aObject_DecWeakRef(Object* obj)
    {
        if (obj->weakrefcnt_ == 1) {
            operator delete(obj);
            return 0;
        }
        else {
            return --(obj->weakrefcnt_);
        }
    }
    A_ALWAYS_INLINE 
    friend bool aObject_IsDestructed(const Object *obj)
    {
        return obj->refcnt_ == static_cast<uint32_t>(-1);
    }

private:
    // Class*                type_;                                     ///< 类型元信息，同时用于标识对象是否被析构(废弃，采用虚函数`getType()`实现)
    std::atomic<uint32_t>    refcnt_{0};                                ///< 强引用计数，给SharedPtr使用（是否考虑废弃共享引用计数，全面采用父子对象进行内存管理？）
    std::atomic<uint32_t>    weakrefcnt_{1};                            ///< 弱引用计数，给WeakPtr使用
    uint32_t                 index_{static_cast<uint32_t>(INVALID_ID)}; ///< 对象索引，用于唯一标识对象
    // uint32_t                 flags_{0};                                 ///< 对象标志位，用于存储对象的额外信息
};

// 主模板：默认 false
template <typename T, typename = void>
struct has_own_getType : std::false_type {};

// 特化：仅当 &T::getType 的类型严格为 Class* (T::*)()const 时匹配
template <typename T>
struct has_own_getType<T, typename std::enable_if<
    std::is_same<decltype(&T::getType), Class* (T::*)()const>::value>::type> : std::true_type {};

template<typename T>
T aobject_cast(Object* obj)
{
    using ObjectType = typename std::decay<typename std::remove_pointer<T>::type>::type;
    static_assert(has_own_getType<ObjectType>::value, "aobject_cast requires the type to has a AST_OBJECT macro");
    return static_cast<T>(ObjectType::StaticType()->cast(obj));
}

template<typename T>
bool Object::isOfType() const
{
    using ObjectType = typename std::decay<typename std::remove_pointer<T>::type>::type;
    static_assert(has_own_getType<ObjectType>::value, "isOfType requires the type to has a AST_OBJECT macro");
    return isOfType(ObjectType::StaticType());
}

/*! @} */



AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Object)

#include "AstUtil/Attribute.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/WeakPtr.hpp"


