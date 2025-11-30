///
/// @file      Object.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
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
#include <string>       // for std::string
#include <stdint.h>     // for uint32_t
#include <assert.h>     // for assert
#include <atomic>       // for std::atomic

AST_NAMESPACE_BEGIN
 
class Type;


/// @brief 对象基类，实现运行时元信息、强弱引用计数
class AST_UTIL_API Object
{
public:
    Object(Type* tp)
        :m_type{tp}
        ,m_refcnt{0}
        ,m_weakrefcnt{1}
    {
        assert(tp);
    }
    err_t getAttrString(const std::string& path, std::string& value) const;
    
    uint32_t refCount() const{return m_refcnt;}
    uint32_t weakRefCount() const{return m_weakrefcnt;}
    bool     isDestructed() const{return m_type ==nullptr;}
    void     destruct() ;
    uint32_t incWeakRef();
    uint32_t decWeakRef();
    uint32_t incRef();
    uint32_t decRef(); 
private:
    void    _destruct();
protected:
    virtual ~Object(){}

protected:
    Type*                    m_type;                 ///< 类型元信息，同时用于标识对象是否被析构
    std::atomic<uint32_t>    m_refcnt;               ///< 强引用计数，给SharedPtr使用
    std::atomic<uint32_t>    m_weakrefcnt;           ///< 弱引用计数，给WeakPtr使用
};

inline void Object::destruct() 
{
    assert(m_refcnt == 0);  // 只能直接删除不采用共享引用计数管理的对象
    this->_destruct();
}

inline uint32_t Object::incWeakRef()
{
    return ++m_weakrefcnt;
}

inline uint32_t Object::decWeakRef()
{
    if (m_weakrefcnt == 1) {
        operator delete(this);
        return 0;
    }
    else {
        return --m_weakrefcnt;
    }
}

inline uint32_t Object::incRef()
{
    return ++m_refcnt;
}

inline uint32_t Object::decRef()
{
    if (m_refcnt == 1) {
        this->_destruct();
        return 0;
    }
    return --m_refcnt;
}

inline void Object::_destruct()
{
    this->~Object();
    this->m_type = nullptr; // 标识对象是否被析构. bit mask indicate whether object is destructed.
    this->decWeakRef();
}



AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Object)


