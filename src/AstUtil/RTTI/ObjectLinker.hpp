///
/// @file      ObjectLinker.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-01
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
#include "AstUtil/WeakPtr.hpp"
#include <type_traits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 对象链接器
/// @details 用于解析对象的延迟链接
class ObjectLinker
{
public:
    ObjectLinker(Object* object)
        : object_(object)
    {
    }

    virtual ~ObjectLinker() = default;

    /// @brief 解析对象的延迟链接
    /// @return errc_t 错误码
    virtual errc_t resolve() = 0;

    /// @brief 获取相关对象
    /// @return Object* 对象指针
    Object* object() const {return object_.get();}
private:
    WeakPtr<Object> object_;
};


/// @brief 通用对象链接器
/// @details 用于解析对象的延迟链接
/// @tparam Func 延迟链接函数类型
template <typename Func>
class ObjectLinkerGeneric : public ObjectLinker
{
    Func func_;

    // 非 void 版本
    template <typename F>
    static errc_t invoke(F& f,
        typename std::enable_if<!std::is_void<
            decltype(std::declval<F>()())>::value>::type* = nullptr)
    {
        return f();
    }

    // void 版本
    template <typename F>
    static errc_t invoke(F& f,
        typename std::enable_if<std::is_void<
            decltype(std::declval<F>()())>::value>::type* = nullptr)
    {
        f();
        return 0;   
    }

public:
    ObjectLinkerGeneric(Object* object, Func func)
        : ObjectLinker(object), func_(func)
    {
    }

    virtual errc_t resolve() override
    {
        return invoke(func_);
    }
};



/// @brief 添加对象延迟链接
/// @details linker 对象延迟链接指针
/// @return void
AST_UTIL_CAPI void aAddObjectLinker(ObjectLinker* linker);


/// @brief 解析对象的延迟链接
/// @details object 对象指针
/// @return errc_t 错误码
AST_UTIL_CAPI errc_t aObject_ResolveLinks(Object* object);


/// @brief 解析所有对象的延迟链接
/// @details 解析所有对象的延迟链接
/// @return errc_t 错误码
AST_UTIL_CAPI errc_t aResolveAllLinks();


/// @brief 添加对象延迟链接
/// @tparam Func 延迟链接函数类型
/// @param[in] object 对象指针
/// @param[in] func 延迟链接函数
/// @return void
template <typename Func>
inline void aObject_AddDelayedLink(Object* object, Func func)
{
    aAddObjectLinker(new ObjectLinkerGeneric<Func>(object, func));
}


// object methods

template<typename Func>
inline void Object::addDelayedLink(Func &&link)
{
    aObject_AddDelayedLink(this, std::forward<Func>(link));
}

template<typename Func>
inline void Object::addDelayedLinkIfFailed(Func &&link)
{
    errc_t rc = link();
    if(rc != eNoError)
        aObject_AddDelayedLink(this, std::forward<Func>(link));
}


A_ALWAYS_INLINE void Object::resolveLinks()
{
    aObject_ResolveLinks(this);
}


/*! @} */

AST_NAMESPACE_END

