///
/// @file      ReflectAPI.hpp
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/Dimension.hpp"
#include <string>
#include <type_traits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup RTTI
    @{
*/

// ============================================================================
// 基础类型特征与函数指针
// ============================================================================

template<typename T>
struct property_trait
{
    using output_type = T;
    using input_type  = T;
};

template<>
struct property_trait<std::string>
{
    using output_type = std::string;
    using input_type  = StringView;
};

template<>
struct property_trait<TimePoint>
{
    using output_type = TimePoint;
    using input_type  = TimePoint;
};

using FPropertyGet = errc_t (*)(const void* obj, void* value);      ///< 获取属性值
using FPropertySet = errc_t (*)(void* obj, const void* value);      ///< 设置属性值
using FPropertyGetItemByString = errc_t (*)(const void* obj, StringView key, Object*& value);
using FPropertyGetItemByIndex  = errc_t (*)(const void* obj, size_t key, Object*& value);

// ----------------------------------------------------------------------------
// 底层工厂函数
// ----------------------------------------------------------------------------
AST_UTIL_CAPI Property* _aNewPropertyBool(FPropertyGet getter, FPropertySet setter);
AST_UTIL_CAPI Property* _aNewPropertyInt(FPropertyGet getter, FPropertySet setter);
AST_UTIL_CAPI Property* _aNewPropertyDouble(FPropertyGet getter, FPropertySet setter);
AST_UTIL_CAPI Property* _aNewPropertyString(FPropertyGet getter, FPropertySet setter);
AST_UTIL_CAPI Property* _aNewPropertyQuantity(FPropertyGet getter, FPropertySet setter, Dimension dimension);
AST_UTIL_CAPI Property* _aNewPropertyObject(FPropertyGet getter, FPropertySet setter, Class* cls);

// ----------------------------------------------------------------------------
// 类型到工厂函数的分发
// ----------------------------------------------------------------------------
template<typename T> Property* _aNewProperty(FPropertyGet getter, FPropertySet setter);

template<>
inline Property* _aNewProperty<bool>(FPropertyGet getter, FPropertySet setter)
{
    return _aNewPropertyBool(getter, setter);
}

template<>
inline Property* _aNewProperty<int>(FPropertyGet getter, FPropertySet setter)
{
    return _aNewPropertyInt(getter, setter);
}

template<>
inline Property* _aNewProperty<double>(FPropertyGet getter, FPropertySet setter)
{
    return _aNewPropertyDouble(getter, setter);
}

template<>
inline Property* _aNewProperty<std::string>(FPropertyGet getter, FPropertySet setter)
{
    return _aNewPropertyString(getter, setter);
}

// ============================================================================
// 内部辅助模板：生成 Getter/Setter Lambda
// ============================================================================
namespace detail
{
    // 访问模式标签
    struct MemberPtrTag {};          // 成员指针
    struct GetterOnlyTag {};         // 只读 getter
    struct GetterVoidSetterTag {};   // getter + void setter
    struct GetterErrorSetterTag {};  // getter + errc_t setter

    // 基础 Builder：由标签派发到具体实现
    template<typename T, typename ValueType, typename Tag>
    struct PropertyBuilder;

    // ---------- 1. 成员指针访问 ----------
    template<typename T, typename ValueType>
    struct PropertyBuilder<T, ValueType, MemberPtrTag>
    {
        template<ValueType T::*Member>
        static FPropertyGet makeGetter()
        {
            return [](const void* obj, void* value) -> errc_t
            {
                *static_cast<ValueType*>(value) = static_cast<const T*>(obj)->*Member;
                return 0;
            };
        }

        template<ValueType T::*Member>
        static FPropertySet makeSetter()
        {
            return [](void* obj, const void* value) -> errc_t
            {
                using InputType = typename property_trait<ValueType>::input_type;
                static_cast<T*>(obj)->*Member = ValueType(*static_cast<const InputType*>(value));
                return 0;
            };
        }
    };

    // ---------- 2. 只读 getter ----------
    template<typename T, typename ValueType>
    struct PropertyBuilder<T, ValueType, GetterOnlyTag>
    {
        template<ValueType (T::*Getter)() const>
        static FPropertyGet makeGetter()
        {
            return [](const void* obj, void* value) -> errc_t
            {
                *static_cast<ValueType*>(value) = (static_cast<const T*>(obj)->*Getter)();
                return 0;
            };
        }

        static FPropertySet makeSetter() { return nullptr; }
    };

    // ---------- 3. getter + void setter ----------
    template<typename T, typename ValueType>
    struct PropertyBuilder<T, ValueType, GetterVoidSetterTag>
    {
        template<ValueType (T::*Getter)() const>
        static FPropertyGet makeGetter()
        {
            return [](const void* obj, void* value) -> errc_t
            {
                *static_cast<ValueType*>(value) = (static_cast<const T*>(obj)->*Getter)();
                return 0;
            };
        }

        template<void (T::*Setter)(ValueType)>
        static FPropertySet makeSetter()
        {
            return [](void* obj, const void* value) -> errc_t
            {
                (static_cast<T*>(obj)->*Setter)(*static_cast<const ValueType*>(value));
                return 0;
            };
        }
    };

    // ---------- 4. getter + errc_t setter ----------
    template<typename T, typename ValueType>
    struct PropertyBuilder<T, ValueType, GetterErrorSetterTag>
    {
        template<ValueType (T::*Getter)() const>
        static FPropertyGet makeGetter()
        {
            return [](const void* obj, void* value) -> errc_t
            {
                *static_cast<ValueType*>(value) = (static_cast<const T*>(obj)->*Getter)();
                return 0;
            };
        }

        template<errc_t (T::*Setter)(ValueType)>
        static FPropertySet makeSetter()
        {
            return [](void* obj, const void* value) -> errc_t
            {
                return (static_cast<T*>(obj)->*Setter)(*static_cast<const ValueType*>(value));
            };
        }
    };

    // ---------- std::string 特殊处理：setter 输入类型为 StringView ----------
    template<typename T>
    struct PropertyBuilder<T, std::string, GetterVoidSetterTag>
    {
        template<const std::string& (T::*Getter)() const>
        static FPropertyGet makeGetter()
        {
            return [](const void* obj, void* value) -> errc_t
            {
                *static_cast<std::string*>(value) = (static_cast<const T*>(obj)->*Getter)();
                return 0;
            };
        }

        template<void (T::*Setter)(StringView)>
        static FPropertySet makeSetter()
        {
            return [](void* obj, const void* value) -> errc_t
            {
                (static_cast<T*>(obj)->*Setter)(*static_cast<const StringView*>(value));
                return 0;
            };
        }
    };

    template<typename T>
    struct PropertyBuilder<T, std::string, GetterErrorSetterTag>
    {
        template<const std::string& (T::*Getter)() const>
        static FPropertyGet makeGetter()
        {
            return [](const void* obj, void* value) -> errc_t
            {
                *static_cast<std::string*>(value) = (static_cast<const T*>(obj)->*Getter)();
                return 0;
            };
        }

        template<errc_t (T::*Setter)(StringView)>
        static FPropertySet makeSetter()
        {
            return [](void* obj, const void* value) -> errc_t
            {
                return (static_cast<T*>(obj)->*Setter)(*static_cast<const StringView*>(value));
            };
        }
    };

    // ---------- std::string 值返回版本的辅助函数 ----------
    template<typename T>
    struct PropertyBuilderValueString
    {
        template<std::string (T::*Getter)() const>
        static FPropertyGet makeGetter()
        {
            return [](const void* obj, void* value) -> errc_t
            {
                *static_cast<std::string*>(value) = (static_cast<const T*>(obj)->*Getter)();
                return 0;
            };
        }

        template<void (T::*Setter)(StringView)>
        static FPropertySet makeVoidSetter()
        {
            return [](void* obj, const void* value) -> errc_t
            {
                (static_cast<T*>(obj)->*Setter)(*static_cast<const StringView*>(value));
                return 0;
            };
        }

        template<errc_t (T::*Setter)(StringView)>
        static FPropertySet makeErrorSetter()
        {
            return [](void* obj, const void* value) -> errc_t
            {
                return (static_cast<T*>(obj)->*Setter)(*static_cast<const StringView*>(value));
            };
        }
    };

} // namespace detail

// ============================================================================
// 统一的公共 API
// ============================================================================

// ----------------------------------------------------------------------------
// 1. 成员指针版本
// ----------------------------------------------------------------------------
template<typename T, typename MemberType, MemberType T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyMem()
{
    using ValueType = typename std::decay<MemberType>::type;
    using Builder = detail::PropertyBuilder<T, ValueType, detail::MemberPtrTag>;
    return _aNewProperty<ValueType>(
        Builder::template makeGetter<Member>(),
        Builder::template makeSetter<Member>()
    );
}

// ----------------------------------------------------------------------------
// 2. 只读 getter 版本
// ----------------------------------------------------------------------------
template<typename T, typename Ret, Ret (T::*Getter)() const>
A_ALWAYS_INLINE Property* aNewPropertyReadOnly()
{
    using ValueType = typename std::decay<Ret>::type;
    using Builder = detail::PropertyBuilder<T, ValueType, detail::GetterOnlyTag>;
    return _aNewProperty<ValueType>(
        Builder::template makeGetter<Getter>(),
        nullptr
    );
}

// ----------------------------------------------------------------------------
// 3. getter + void setter 版本
// ----------------------------------------------------------------------------
template<typename T,
         typename Ret,
         Ret (T::*Getter)() const,
         void (T::*Setter)(Ret)>
A_ALWAYS_INLINE Property* aNewProperty()
{
    using ValueType = typename std::decay<Ret>::type;
    using Builder = detail::PropertyBuilder<T, ValueType, detail::GetterVoidSetterTag>;
    return _aNewProperty<ValueType>(
        Builder::template makeGetter<Getter>(),
        Builder::template makeSetter<Setter>()
    );
}

// ----------------------------------------------------------------------------
// 4. getter + errc_t setter 版本
// ----------------------------------------------------------------------------
template<typename T,
         typename Ret,
         Ret (T::*Getter)() const,
         errc_t (T::*Setter)(Ret)>
A_ALWAYS_INLINE Property* aNewPropertyWithError()
{
    using ValueType = typename std::decay<Ret>::type;
    using Builder = detail::PropertyBuilder<T, ValueType, detail::GetterErrorSetterTag>;
    return _aNewProperty<ValueType>(
        Builder::template makeGetter<Getter>(),
        Builder::template makeSetter<Setter>()
    );
}


// 成员指针版本
template<typename T, double T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyQuantity(Dimension dim)
{
    using Builder = detail::PropertyBuilder<T, double, detail::MemberPtrTag>;
    return _aNewPropertyQuantity(
        Builder::template makeGetter<Member>(),
        Builder::template makeSetter<Member>(),
        dim
    );
}

// 只读 getter 版本
template<typename T, double (T::*Getter)() const>
A_ALWAYS_INLINE Property* aNewPropertyQuantity(Dimension dim)
{
    using Builder = detail::PropertyBuilder<T, double, detail::GetterOnlyTag>;
    return _aNewPropertyQuantity(
        Builder::template makeGetter<Getter>(),
        nullptr,
        dim
    );
}

// getter + void setter 版本
template<typename T,
         double (T::*Getter)() const,
         void (T::*Setter)(double)>
A_ALWAYS_INLINE Property* aNewPropertyQuantity(Dimension dim)
{
    using Builder = detail::PropertyBuilder<T, double, detail::GetterVoidSetterTag>;
    return _aNewPropertyQuantity(
        Builder::template makeGetter<Getter>(),
        Builder::template makeSetter<Setter>(),
        dim
    );
}

// getter + errc_t setter 版本
template<typename T,
         double (T::*Getter)() const,
         errc_t (T::*Setter)(double)>
A_ALWAYS_INLINE Property* aNewPropertyQuantityWithError(Dimension dim)
{
    using Builder = detail::PropertyBuilder<T, double, detail::GetterErrorSetterTag>;
    return _aNewPropertyQuantity(
        Builder::template makeGetter<Getter>(),
        Builder::template makeSetter<Setter>(),
        dim
    );
}

// ============================================================================
// 对象类型属性
// ============================================================================

// 成员指针版本
template<typename T, typename ObjectType, ObjectType* T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyObject()
{
    using Builder = detail::PropertyBuilder<T, ObjectType*, detail::MemberPtrTag>;
    return _aNewPropertyObject(
        Builder::template makeGetter<Member>(),
        Builder::template makeSetter<Member>(),
        &ObjectType::staticType
    );
}

// 只读 getter 版本
template<typename T, typename ObjectType, ObjectType* (T::*Getter)() const>
A_ALWAYS_INLINE Property* aNewPropertyObject()
{
    using Builder = detail::PropertyBuilder<T, ObjectType*, detail::GetterOnlyTag>;
    return _aNewPropertyObject(
        Builder::template makeGetter<Getter>(),
        nullptr,
        &ObjectType::staticType
    );
}

// getter + void setter 版本
template<typename T, typename ObjectType, ObjectType* (T::*Getter)() const, void (T::*Setter)(ObjectType*)>
A_ALWAYS_INLINE Property* aNewPropertyObject()
{
    using Builder = detail::PropertyBuilder<T, ObjectType*, detail::GetterVoidSetterTag>;
    return _aNewPropertyObject(
        Builder::template makeGetter<Getter>(),
        Builder::template makeSetter<Setter>(),
        &ObjectType::staticType
    );
}

// getter + errc_t setter 版本
template<typename T, typename ObjectType, ObjectType* (T::*Getter)() const, errc_t (T::*Setter)(ObjectType*)>
A_ALWAYS_INLINE Property* aNewPropertyObjectWithError()
{
    using Builder = detail::PropertyBuilder<T, ObjectType*, detail::GetterErrorSetterTag>;
    return _aNewPropertyObject(
        Builder::template makeGetter<Getter>(),
        Builder::template makeSetter<Setter>(),
        &ObjectType::staticType
    );
}

// ============================================================================
// 兼容旧 API（内联转发）
// ============================================================================

// ---- 成员指针旧接口 ----
template<typename T, bool T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyBoolMem()   { return aNewPropertyMem<T, bool, Member>(); }
template<typename T, int T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyIntMem()    { return aNewPropertyMem<T, int, Member>(); }
template<typename T, double T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyDoubleMem() { return aNewPropertyMem<T, double, Member>(); }
template<typename T, std::string T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyStringMem() { return aNewPropertyMem<T, std::string, Member>(); }

template<typename T, bool T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyBool()      { return aNewPropertyBoolMem<T, Member>(); }
template<typename T, int T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyInt()       { return aNewPropertyIntMem<T, Member>(); }
template<typename T, double T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyDouble()    { return aNewPropertyDoubleMem<T, Member>(); }
template<typename T, std::string T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyString()    { return aNewPropertyStringMem<T, Member>(); }

template<typename T, double T::*Member>
A_ALWAYS_INLINE Property* aNewPropertyQuantityMem(Dimension dim) {
    return aNewPropertyQuantity<T, Member>(dim);
}

// ---- 只读 getter 旧接口 ----
template<typename T, bool (T::*Getter)() const>
A_ALWAYS_INLINE Property* aNewPropertyBool()      { return aNewPropertyReadOnly<T, bool, Getter>(); }
template<typename T, int (T::*Getter)() const>
A_ALWAYS_INLINE Property* aNewPropertyInt()       { return aNewPropertyReadOnly<T, int, Getter>(); }
template<typename T, double (T::*Getter)() const>
A_ALWAYS_INLINE Property* aNewPropertyDouble()    { return aNewPropertyReadOnly<T, double, Getter>(); }
template<typename T, std::string (T::*Getter)() const>
A_ALWAYS_INLINE Property* aNewPropertyString()    { return aNewPropertyReadOnly<T, std::string, Getter>(); }



// ---- getter + void setter 旧接口 ----
template<typename T, bool (T::*Getter)() const, void (T::*Setter)(bool)>
A_ALWAYS_INLINE Property* aNewPropertyBool()      { return aNewProperty<T, bool, Getter, Setter>(); }
template<typename T, int (T::*Getter)() const, void (T::*Setter)(int)>
A_ALWAYS_INLINE Property* aNewPropertyInt()       { return aNewProperty<T, int, Getter, Setter>(); }
template<typename T, double (T::*Getter)() const, void (T::*Setter)(double)>
A_ALWAYS_INLINE Property* aNewPropertyDouble()    { return aNewProperty<T, double, Getter, Setter>(); }
template<typename T,
         const std::string& (T::*Getter)() const,
         void (T::*Setter)(StringView)>
A_ALWAYS_INLINE Property* aNewPropertyString()
{
    using Builder = detail::PropertyBuilder<T, std::string, detail::GetterVoidSetterTag>;
    return _aNewProperty<std::string>(
        Builder::template makeGetter<Getter>(),
        Builder::template makeSetter<Setter>()
    );
}

// std::string 值返回版本：支持 std::string value() const;
template<typename T,
         std::string (T::*Getter)() const,
         void (T::*Setter)(StringView)>
A_ALWAYS_INLINE Property* aNewPropertyString()
{
    using Builder = detail::PropertyBuilderValueString<T>;
    return _aNewProperty<std::string>(
        Builder::template makeGetter<Getter>(),
        Builder::template makeVoidSetter<Setter>()
    );
}


// ---- getter + errc_t setter 旧接口 ----
template<typename T, bool (T::*Getter)() const, errc_t (T::*Setter)(bool)>
A_ALWAYS_INLINE Property* aNewPropertyBool()      { return aNewPropertyWithError<T, bool, Getter, Setter>(); }
template<typename T, int (T::*Getter)() const, errc_t (T::*Setter)(int)>
A_ALWAYS_INLINE Property* aNewPropertyInt()       { return aNewPropertyWithError<T, int, Getter, Setter>(); }
template<typename T, double (T::*Getter)() const, errc_t (T::*Setter)(double)>
A_ALWAYS_INLINE Property* aNewPropertyDouble()    { return aNewPropertyWithError<T, double, Getter, Setter>(); }
template<typename T,
         const std::string& (T::*Getter)() const,
         errc_t (T::*Setter)(StringView)>
A_ALWAYS_INLINE Property* aNewPropertyString()
{
    using Builder = detail::PropertyBuilder<T, std::string, detail::GetterErrorSetterTag>;
    return _aNewProperty<std::string>(
        Builder::template makeGetter<Getter>(),
        Builder::template makeSetter<Setter>()
    );
}

// std::string 值返回版本：支持 std::string value() const;
template<typename T,
         std::string (T::*Getter)() const,
         errc_t (T::*Setter)(StringView)>
A_ALWAYS_INLINE Property* aNewPropertyString()
{
    using Builder = detail::PropertyBuilderValueString<T>;
    return _aNewProperty<std::string>(
        Builder::template makeGetter<Getter>(),
        Builder::template makeErrorSetter<Setter>()
    );
}

template<typename T, double (T::*Getter)() const, errc_t (T::*Setter)(double)>
A_ALWAYS_INLINE Property* aNewPropertyQuantity(Dimension dim) {
    return aNewPropertyQuantityWithError<T, Getter, Setter>(dim);
}

/*! @} */

AST_NAMESPACE_END