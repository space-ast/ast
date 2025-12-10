///
/// @file      ReflectAPI.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
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

AST_NAMESPACE_BEGIN

typedef err_t (*FPropertyGet)(void* obj, void* value);              ///< 获取属性值的函数指针
typedef err_t (*FPropertySet)(void* obj, void const* value);        ///< 设置属性值的函数指针


class Property;

/// @brief 创建一个布尔属性
/// @warning 本函数为内部函数，不建议直接调用。
/// @param getter 获取属性值的函数指针
/// @param setter 设置属性值的函数指针
AST_UTIL_CAPI Property* _aNewPropertyBool(FPropertyGet getter, FPropertySet setter);


/// @brief 创建一个整数属性
/// @warning 本函数为内部函数，不建议直接调用。
/// @param getter 获取属性值的函数指针
/// @param setter 设置属性值的函数指针
AST_UTIL_CAPI Property* _aNewPropertyInt(FPropertyGet getter, FPropertySet setter);


/// @brief 创建一个双精度属性
/// @warning 本函数为内部函数，不建议直接调用。
/// @param getter 获取属性值的函数指针
/// @param setter 设置属性值的函数指针
/// @return Property* 属性指针
AST_UTIL_CAPI Property* _aNewPropertyDouble(FPropertyGet getter, FPropertySet setter);


/// @brief 创建一个字符串属性
/// @warning 本函数为内部函数，不建议直接调用。
/// @param getter 获取属性值的函数指针
/// @param setter 设置属性值的函数指针
/// @return Property* 属性指针
AST_UTIL_CAPI Property* _aNewPropertyString(FPropertyGet getter, FPropertySet setter);



/// @brief 创建一个双精度属性
/// @details 本函数创建一个双精度属性，属性值通过访问对象的成员变量获取。
/// @tparam T 类类型
/// @param member 双精度属性成员指针
/// @return Property* 属性指针
template<typename T, double T::* Member>
A_ALWAYS_INLINE Property* aNewPropertyDouble()
{
    return _aNewPropertyDouble(
        [](void* obj, void* value) -> err_t
        {
            *((double*)value) = ((T*)obj)->*Member;
            return 0;
        },
        [](void* obj, const void* value) -> err_t
        {
            ((T*)obj)->*Member = *((double*)value);
            return 0;
        }
    );
}


/// @brief 创建一个双精度属性
/// @details 本函数创建一个双精度属性，属性值通过调用对象的成员函数获取。
/// @tparam T 类类型
/// @tparam Getter 双精度属性获取函数指针
/// @return Property* 属性指针
template<typename T, double (T::* Getter) () const>
A_ALWAYS_INLINE Property* aNewPropertyDouble()
{
    return _aNewPropertyDouble(
        [](void* obj, void* value) -> err_t
        {
            static_assert(Getter!=nullptr, "invalid getter");
             *((double*)value) = (((T*)obj)->*Getter)();
             return 0;
        }, 
        nullptr
    );
}

/// @brief 创建一个双精度属性
/// @details 本函数创建一个双精度属性，属性值通过调用对象的成员函数获取。
/// @tparam T 类类型
/// @tparam Getter 双精度属性获取函数指针
/// @tparam Setter 双精度属性设置函数指针
/// @return Property* 属性指针
template<typename T, double (T::* Getter) () const, void (T::* Setter)(double)>
A_ALWAYS_INLINE Property* aNewPropertyDouble()
{
    return _aNewPropertyDouble(
        [](void* obj, void* value) -> err_t
    {
        static_assert(Getter!=nullptr, "invalid getter");
        *((double*)value) = (((T*)obj)->*Getter)();
        return 0;
    },
        [](void* obj, const void* value) -> err_t
    {
        static_assert(Setter!=nullptr, "invalid setter");
        (((T*)obj)->*Setter)(*((double*)value));
        return 0;
    }
    );
}

/// @brief 创建一个双精度属性
/// @details 本函数创建一个双精度属性，属性值通过调用对象的成员函数获取。
/// @tparam T 类类型
/// @tparam Getter 双精度属性获取函数指针
/// @tparam Setter 双精度属性设置函数指针
/// @return Property* 属性指针
template<typename T, double (T::* Getter) () const, err_t (T::* Setter)(double)>
A_ALWAYS_INLINE Property* aNewPropertyDouble()
{
    return _aNewPropertyDouble(
        [](void* obj, void* value) -> err_t
    {
        static_assert(Getter != nullptr, "invalid getter");
        *((double*)value) = (((T*)obj)->*Getter)();
        return 0;
    },
        [](void* obj, const void* value) -> err_t
    {
        static_assert(Setter != nullptr, "invalid setter");
        return (((T*)obj)->*Setter)(*((double*)value));
    }
    );
}



AST_NAMESPACE_END
