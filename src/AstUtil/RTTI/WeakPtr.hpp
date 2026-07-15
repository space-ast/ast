///
/// @file      WeakPtr.hpp
/// @brief     弱引用指针
/// @details   与std::weak_ptr功能类型
/// @author    axel
/// @date      19.11.2025
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
#include "AstUtil/Object.hpp"

AST_NAMESPACE_BEGIN
 

/// @brief 弱引用指针
/// @tparam _Object 指向的对象类型
/// @details   与std::weak_ptr功能类型
/// @ingroup RTTI
template<typename _Object>
class WeakPtr
{
public:
    WeakPtr()
        :object_{ nullptr }
    {
    }
    WeakPtr(std::nullptr_t)
        :object_{ nullptr }
    {
    }
    WeakPtr(_Object* obj)
        :object_{ obj }
    {
        _incWeakRef();
    }
    WeakPtr(const WeakPtr& ptr)
        :WeakPtr{ ptr.object_ }
    {
    }
    ~WeakPtr()
    {
        _decWeakRef();
    }
    WeakPtr& operator=(_Object* obj)
    {
        if (obj != object_) {
            _decWeakRef();
            object_ = obj;
            _incWeakRef();
        }
        return *this;
    }
    WeakPtr& operator=(std::nullptr_t)
    {
        reset();
        return *this;
    }
    WeakPtr& operator=(const WeakPtr& ptr)
    {
        if (this != &ptr) {
            this->operator=(ptr.object_);
        }
        return *this;
    }
    _Object* lock() const
    {
        return get();
    }
    _Object* get() const
    {
        if (expired()) {
            return nullptr;
        }
        return object_;
    }
    bool expired() const
    {
        return !object_ || aObject_IsDestructed(object_);
    }
    void reset()
    {
        _decWeakRef();
        object_ = nullptr;
    }
    _Object* operator->() const
    {
        return object_;
    }
    // operator _Object*() const
    // {
    //     return get();
    // }
    explicit operator bool() const
    {
        return !expired();
    }
    bool operator==(const WeakPtr& other) const
    {
        return object_ == other.object_;
    }
    bool operator!=(const WeakPtr& other) const
    {
        return object_ != other.object_;
    }
    bool operator<(const WeakPtr& other) const
    {
        return object_ < other.object_;
    }
private:
    void _incWeakRef()
    {
        if(object_)
            aObject_IncWeakRef(object_);
    }
    void _decWeakRef()
    {
        if(object_)
            aObject_DecWeakRef(object_);
    }
protected:
    _Object* object_{nullptr};
};


 
AST_NAMESPACE_END

namespace std
{
    template<typename _Object>
    struct hash<_AST WeakPtr<_Object>>
    {
        size_t operator()(const _AST WeakPtr<_Object>& ptr) const
        {
            return hash<_Object*>()(ptr.get());
        }
    };
}