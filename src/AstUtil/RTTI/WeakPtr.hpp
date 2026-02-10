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
        :m_object{ nullptr }
    {
    }

    WeakPtr(_Object* obj)
        :m_object{ obj }
    {
        _incWeakRef();
    }
    WeakPtr(const WeakPtr& ptr)
        :WeakPtr{ ptr.m_object }
    {
    }
    ~WeakPtr()
    {
        _decWeakRef();
    }
    WeakPtr& operator=(_Object* obj)
    {
        if (obj != m_object) {
            _decWeakRef();
            m_object = obj;
            _incWeakRef();
        }
        return *this;
    }
    WeakPtr& operator=(const WeakPtr& ptr)
    {
        if (this != &ptr) {
            this->operator=(ptr.m_object);
        }
        return *this;
    }
    _Object* lock() const
    {
        return get();
    }
    _Object* get() const
    {
        if (!m_object || m_object->isDestructed()) {
            return nullptr;
        }
        return m_object;
    }
private:
    void _incWeakRef()
    {
        if(m_object)
            m_object->incWeakRef();
    }
    void _decWeakRef()
    {
        if(m_object)
            m_object->decWeakRef();
    }
protected:
    _Object* m_object;
};


 
AST_NAMESPACE_END
 



