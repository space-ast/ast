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
#include <string>

AST_NAMESPACE_BEGIN

template<typename T>
struct property_trait
{
    using output_type = T;
    using input_type = T;
};

template<>
struct property_trait<std::string>
{
    using output_type = std::string;
    using input_type = StringView;
};



typedef err_t (*FPropertyGet)(const void* obj, void* value);              ///< 获取属性值的函数指针
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



template<typename T>
A_ALWAYS_INLINE Property* _aNewProperty(FPropertyGet getter, FPropertySet setter);



/// @brief 创建一个布尔属性
/// @warning 本函数为内部函数，不建议直接调用。
/// @param getter 获取属性值的函数指针
/// @param setter 设置属性值的函数指针
/// @return Property* 属性指针
template<>
A_ALWAYS_INLINE Property* _aNewProperty<bool>(FPropertyGet getter, FPropertySet setter)
{
    return _aNewPropertyBool(getter, setter);
}

/// @brief 创建一个整数属性
/// @warning 本函数为内部函数，不建议直接调用。
/// @param getter 获取属性值的函数指针
/// @param setter 设置属性值的函数指针
/// @return Property* 属性指针
template<>
A_ALWAYS_INLINE Property* _aNewProperty<int>(FPropertyGet getter, FPropertySet setter)
{
    return _aNewPropertyInt(getter, setter);
}

/// @brief 创建一个双精度属性
/// @warning 本函数为内部函数，不建议直接调用。
/// @param getter 获取属性值的函数指针
/// @param setter 设置属性值的函数指针
/// @return Property* 属性指针
template<>
A_ALWAYS_INLINE Property* _aNewProperty<double>(FPropertyGet getter, FPropertySet setter)
{
    return _aNewPropertyDouble(getter, setter);
}


/// @brief 创建一个字符串属性
/// @warning 本函数为内部函数，不建议直接调用。
/// @param getter 获取属性值的函数指针
/// @param setter 设置属性值的函数指针
/// @return Property* 属性指针
template<>
A_ALWAYS_INLINE Property* _aNewProperty<std::string>(FPropertyGet getter, FPropertySet setter)
{
    return _aNewPropertyString(getter, setter);
}




/// @brief 创建一个布尔属性
/// @details 本函数创建一个布尔属性，属性值通过访问对象的成员变量获取。
/// @tparam T 类类型
/// @param member 布尔属性成员指针
/// @return Property* 属性指针
template<typename T, bool T::* Member>
A_ALWAYS_INLINE Property* aNewPropertyBool()
{
    return _aNewPropertyBool(
        [](const void* obj, void* value) -> err_t
        {
            *((bool*)value) = ((T*)obj)->*Member;
            return 0;
        },
        [](void* obj, const void* value) -> err_t
        {
            ((T*)obj)->*Member = *((bool*)value);
            return 0;
        }
    );
}


template<typename T, int T::* Member>
A_ALWAYS_INLINE Property* aNewPropertyInt()
{
    return _aNewPropertyInt(
        [](const void* obj, void* value) -> err_t
        {
            *((int*)value) = ((T*)obj)->*Member;
            return 0;
        },
        [](void* obj, const void* value) -> err_t
        {
            ((T*)obj)->*Member = *((int*)value);
            return 0;
        }
    );
}



/// @brief 创建一个双精度属性
/// @details 本函数创建一个双精度属性，属性值通过访问对象的成员变量获取。
/// @tparam T 类类型
/// @param member 双精度属性成员指针
/// @return Property* 属性指针
template<typename T, double T::* Member>
A_ALWAYS_INLINE Property* aNewPropertyDouble()
{
    return _aNewPropertyDouble(
        [](const void* obj, void* value) -> err_t
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

template<typename T, std::string T::* Member>
A_ALWAYS_INLINE Property* aNewPropertyString()
{
    return _aNewPropertyString(
        [](const void* obj, void* value) -> err_t
        {
            *((std::string*)value) = ((T*)obj)->*Member;
            return 0;
        },
        [](void* obj, const void* value) -> err_t
        {
            ((T*)obj)->*Member = *((typename property_trait<std::string>::input_type*)value);
            return 0;
        }
    );
}



/// @brief 创建一个布尔属性
/// @details 本函数创建一个布尔属性，属性值通过调用对象的成员函数获取。
/// @tparam T 类类型
/// @tparam Getter 布尔属性获取函数指针
/// @return Property* 属性指针
template<typename T, bool(T::* Getter) () const>
A_ALWAYS_INLINE Property* aNewPropertyBool()
{
    return _aNewPropertyBool(
        [](const void* obj, void* value) -> err_t
        {
            *((bool*)value) = (((T*)obj)->*Getter)();
            return 0;
        },
        nullptr
    );
}


template<typename T, int(T::* Getter) () const>
A_ALWAYS_INLINE Property* aNewPropertyInt()
{
    return _aNewPropertyInt(
        [](const void* obj, void* value) -> err_t
        {
            *((int*)value) = (((T*)obj)->*Getter)();
            return 0;
        },
        nullptr
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
        [](const void* obj, void* value) -> err_t
        {
            static_assert(Getter!=nullptr, "invalid getter");
             *((double*)value) = (((T*)obj)->*Getter)();
             return 0;
        }, 
        nullptr
    );
}

/// @brief 创建一个字符串属性
/// @details 本函数创建一个字符串属性，属性值通过调用对象的成员函数获取。
/// @tparam T 类类型
/// @tparam Getter 字符串属性获取函数指针
/// @return Property* 属性指针
template<typename T, std::string(T::*Getter)() const>
A_ALWAYS_INLINE Property* aNewPropertyString()
{
    return _aNewPropertyString(
        [](const void* obj, void* value) -> err_t
        {
            *((std::string*)value) = (((T*)obj)->*Getter)();
            return 0;
        },
        nullptr
    );
}


template<typename T, bool (T::* Getter) () const, void (T::* Setter)(bool)>
A_ALWAYS_INLINE Property* aNewPropertyBool()
{
    return _aNewPropertyBool(
        [](const void* obj, void* value) -> err_t
        {
            *((bool*)value) = (((T*)obj)->*Getter)();
            return 0;
        },
        [](void* obj, const void* value) -> err_t
        {
            static_assert(Setter!=nullptr, "invalid setter");
            (((T*)obj)->*Setter)(*((bool*)value));
            return 0;
        }
    );
}



/// @brief 创建一个整数属性
/// @details 本函数创建一个整数属性，属性值通过调用对象的成员函数获取。
/// @tparam T 类类型
/// @tparam Getter 整数属性获取函数指针
/// @tparam Setter 整数属性设置函数指针
/// @return Property* 属性指针
template<typename T, int(T::* Getter) () const, void (T::* Setter)(int)>
A_ALWAYS_INLINE Property* aNewPropertyInt()
{
    return _aNewPropertyInt(
        [](const void* obj, void* value) -> err_t
    {
        static_assert(Getter!=nullptr, "invalid getter");
        *((int*)value) = (((T*)obj)->*Getter)();
        return 0;
    },
        [](void* obj, const void* value) -> err_t
    {
        static_assert(Setter!=nullptr, "invalid setter");
        (((T*)obj)->*Setter)(*((int*)value));
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
template<typename T, double (T::* Getter) () const, void (T::* Setter)(double)>
A_ALWAYS_INLINE Property* aNewPropertyDouble()
{
    return _aNewPropertyDouble(
        [](const void* obj, void* value) -> err_t
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



template<typename T, const std::string& (T::* Getter) () const, void (T::* Setter)(StringView)>
A_ALWAYS_INLINE Property* aNewPropertyString()
{
    return _aNewPropertyString(
        [](const void* obj, void* value) -> err_t
    {
        static_assert(Getter!=nullptr, "invalid getter");
        *((std::string*)value) = (((T*)obj)->*Getter)();
        return 0;
    },
        [](void* obj, const void* value) -> err_t
    {
        static_assert(Setter!=nullptr, "invalid setter");
        (((T*)obj)->*Setter)(*((typename property_trait<std::string>::input_type*)value));
        return 0;
    }
    );
}



/// @brief 创建一个布尔属性
/// @details 本函数创建一个布尔属性，属性值通过调用对象的成员函数获取。
/// @tparam T 类类型
/// @tparam Getter 布尔属性获取函数指针
/// @tparam Setter 布尔属性设置函数指针
/// @return Property* 属性指针
template<typename T, err_t (T::* Getter) () const, err_t (T::* Setter)(bool)>
A_ALWAYS_INLINE Property* aNewPropertyBool()
{
    return _aNewPropertyBool(
        [](const void* obj, void* value) -> err_t
        {
            *((bool*)value) = (((T*)obj)->*Getter)();
            return 0;
        },
        [](void* obj, const void* value) -> err_t
        {
            static_assert(Setter!=nullptr, "invalid setter");
            return (((T*)obj)->*Setter)(*((bool*)value));
        }
    );
}



/// @brief 创建一个整数属性
/// @details 本函数创建一个整数属性，属性值通过调用对象的成员函数获取。
/// @tparam T 类类型
/// @tparam Getter 整数属性获取函数指针
/// @tparam Setter 整数属性设置函数指针
/// @return Property* 属性指针
template<typename T, int (T::* Getter) () const, err_t (T::* Setter)(int)>
A_ALWAYS_INLINE Property* aNewPropertyInt()
{
    return _aNewPropertyInt(
        [](const void* obj, void* value) -> err_t
    {
        static_assert(Getter!=nullptr, "invalid getter");
        *((int*)value) = (((T*)obj)->*Getter)();
        return 0;
    },
        [](void* obj, const void* value) -> err_t
    {
        static_assert(Setter!=nullptr, "invalid setter");
        return (((T*)obj)->*Setter)(*((int*)value));
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
        [](const void* obj, void* value) -> err_t
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

/// @brief 创建一个字符串属性
/// @details 本函数创建一个字符串属性，属性值通过调用对象的成员函数获取。
/// @tparam T 类类型
/// @tparam Getter 字符串属性获取函数指针
/// @tparam Setter 字符串属性设置函数指针
/// @return Property* 属性指针
template<typename T, const std::string& (T::* Getter) () const, err_t (T::* Setter)(StringView)>
A_ALWAYS_INLINE Property* aNewPropertyString()
{
    return _aNewPropertyString(
        [](const void* obj, void* value) -> err_t
    {
        static_assert(Getter!=nullptr, "invalid getter");
        *((std::string*)value) = (((T*)obj)->*Getter)();
        return 0;
    },
        [](void* obj, const void* value) -> err_t
    {
        static_assert(Setter!=nullptr, "invalid setter");
        return (((T*)obj)->*Setter)(*((typename property_trait<std::string>::input_type*)value));
    }
    );
}



AST_NAMESPACE_END
