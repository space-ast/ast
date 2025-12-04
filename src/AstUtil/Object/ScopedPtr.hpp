///
/// @file      ScopedPtr.hpp
/// @brief     作用域指针
/// @details   指针的生命周期限制在一个作用域内，作用域结束指针析构
/// @author    jinke18
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
#include <type_traits>    // for std::enable_if
#include <cstdio>

AST_NAMESPACE_BEGIN


template<typename T, typename = void>
struct ScopedPtrDeleter
{
    static void cleanup(T* ptr)
    {
        delete ptr;
    }
};

template<typename T>
struct ScopedPtrDeleter<T, typename std::enable_if<std::is_base_of<Object, T>::value>::type>
{
    static void cleanup(T* ptr)
    {
        ptr->destruct();
    }
};

template<>
struct ScopedPtrDeleter<std::FILE>
{
    static void cleanup(std::FILE* ptr)
    {
        fclose(ptr);
    }
};

template<typename T>
class ScopedPtr
{
public:
    ScopedPtr(T* ptr)
        :m_pointer{ ptr }
    { }
    ScopedPtr()
        :m_pointer{ nullptr }
    {}
    ScopedPtr& operator=(T* ptr)
    {
        this->_delete();
        m_pointer = ptr;
        return *this;
    }
    ~ScopedPtr()
    {
        this->_delete();
    }
    void reset(T* ptr)
    {
        this->_delete();
        m_pointer = ptr;
    }
    T* get() const
    {
        return m_pointer;
    }
    T* operator->() const{return m_pointer;}
    operator T*() const{return m_pointer;}
private:
    ScopedPtr(const ScopedPtr&) = delete;
    ScopedPtr& operator =(const ScopedPtr&) = delete;
    void _delete()
    {
        if(m_pointer)
            ScopedPtrDeleter<T>::cleanup(m_pointer);
    }

protected:
    T* m_pointer;
};
 
AST_NAMESPACE_END
 
