///
/// @file      Class.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-09
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
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Struct.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/Object.hpp"
#include <type_traits>

AST_NAMESPACE_BEGIN

class Object;



/// @brief 类元信息
/// @details 类元信息，包含类的名称、属性、方法等
/// @todo 实现类的反射机制
/// @ingroup RTTI
class AST_UTIL_API Class: public Struct
{
    typedef Object* (*FConstructor)(Object* parentScope);
    typedef Object* (*FResolve)(StringView value);

public:
    AST_OBJECT(Class);

    using Struct::Struct;

    /// @brief 构造函数
    /// @param parent 父类指针
    explicit Class(Class* parent = nullptr);

    
    ~Class();

    /// @brief 类型转换
    /// @param obj 输入对象指针
    /// @return Object* 转换后的对象指针
    Object* cast(Object* obj) const;

    /// @brief 检查类是否为虚类
    /// @return 是否为虚类
    bool isVirtual() const{return constructor_ == nullptr;}

    /// @brief 获取父类
    /// @return Class* 父类指针
    Class* getParent() const{ return parent_; }

    /// @brief 注册类到类注册器
    void addToRegistry() const;

    /// @brief 创建新对象
    /// @return Object* 新对象指针
    Object* newObject(Object* parentScope) const;

    /// @brief 解析字符串
    /// @param value 字符串值
    /// @return Object* 解析结果
    Object* resolve(StringView value) const;

    /// @brief 获取默认对象
    /// @return Object* 默认对象指针
    Object* getDefaultObject() const;
public:

    /// @brief 设置父类
    /// @warning 内部使用，不建议外部调用，否则可能导致未定义行为
    /// @param parent 父类指针
    void setParent(Class* parent){ parent_ = parent; }
    
    /// @brief 设置构造函数
    /// @warning 内部使用，不建议外部调用，否则可能导致未定义行为
    /// @param constructor 构造函数指针
    void setConstructor(FConstructor constructor){ constructor_ = constructor; }

    /// @brief 设置解析函数
    /// @param resolve 解析函数指针
    void setResolve(FResolve resolve){ resolve_ = resolve; }

    /// @brief 设置父类
    /// @warning 内部使用，不建议外部调用，否则可能导致未定义行为
    /// @param T 父类类型
    template<typename T>
    void setParent()
    {
        //setParent(T::StaticType());
        setParent(&T::staticType);
    }

    /// @brief 设置构造函数
    /// @warning 内部使用，不建议外部调用，否则可能导致未定义行为
    /// @param T 构造函数类型
    template<typename T>
    typename std::enable_if<!std::is_abstract<T>::value>::type setConstructor() {
        setConstructor([](Object* parentScope) -> Object* {
            auto obj = new T();
            obj->setParentScope(parentScope);
            return obj;
        });
    }

    template<typename T>
    typename std::enable_if<std::is_abstract<T>::value>::type setConstructor() {
        setConstructor(nullptr);
    }

    template<typename T>
    void setResolve()
    {
        typedef T* (*FResolveClsSpec)(StringView value);
        static_assert(std::is_same<decltype(&T::Resolve), FResolveClsSpec>::value, "T must have Resolve method with signature T* (StringView value)");
        setResolve(reinterpret_cast<FResolve>(&T::Resolve));
    }
    
protected:
    Class* parent_{nullptr};                       ///< 父类
    FConstructor constructor_{nullptr};            ///< 构造函数
    FResolve resolve_{nullptr};                    ///< 解析函数
    SharedPtr<Object> defaultObject_{nullptr};     ///< 默认对象
};


AST_NAMESPACE_END