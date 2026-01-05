///
/// @file      SharedPtr.hpp
/// @brief     共享指针
/// @details   与std::shared_ptr功能类似
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


template<typename _Object>
class SharedPtr
{
public:
    SharedPtr()
        :m_object{}
    {}
    SharedPtr(_Object* obj)
        :m_object{ obj }
    {
        _incRef();
    }
    SharedPtr(const SharedPtr& ptr)
        :SharedPtr{ptr.get()}
    {

    }
    ~SharedPtr()
    {
        _decRef();
    }
    SharedPtr& operator=(const SharedPtr& ptr)
    {
        return this->operator=(ptr.get());
    }

    SharedPtr& operator=(_Object* obj)
    {
        if(obj != m_object){ 
            if(obj)
                obj->incRef();
            _decRef();
            m_object = obj;
        }
        return *this;
    }
    // 这里还是设置为支持隐式转换，因为在很多情况下，我们需要将SharedPtr转换为_Object*
    // explicit 
    operator _Object*() const
    {
        return m_object;
    }
    _Object* operator->() const
    {
        return m_object;
    }
    _Object* get() const
    {
        return m_object;
    }
    _Object* take()
    {
        _Object* obj = m_object;
        if(obj){
            obj->decRefNoDelete();
            m_object = nullptr;
        }
        return obj;
    }
protected:
    void _incRef()
    {
        if(m_object)
            m_object->incRef();
    }
    void _decRef()
    {
        if(m_object)
            m_object->decRef();
    }
protected:
    _Object* m_object;
};


 
AST_NAMESPACE_END
 
