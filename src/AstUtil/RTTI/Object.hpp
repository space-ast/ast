///
/// @file      Object.hpp
/// @brief     
/// @details   ~
/// @author    axel
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
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/WeakPtr.hpp"
#include <string>       // for std::string
#include <stdint.h>     // for uint32_t
#include <assert.h>     // for assert
#include <atomic>       // for std::atomic

AST_NAMESPACE_BEGIN


/*!
    @ingroup Util
    @defgroup RTTI 运行时类型信息
    @brief 提供运行时类型信息功能，包括类元信息、属性元信息、对象的强弱引用计数。
*/

/*!
    @addtogroup RTTI
    @{
*/
 
class Class;        // 类元信息
class Property;     // 属性元信息

/// @brief 对象基类，实现运行时元信息、强弱引用计数
class AST_UTIL_API Object
{
public:
    Object():Object(nullptr){}

    Object(Class* tp)
        :m_type{tp}
        ,m_refcnt{0}
        ,m_weakrefcnt{1}
    {
        // assert(tp);
    }

    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值引用
    /// @return err_t 错误码
    err_t getAttrBool(StringView path, bool& value) const;
    
    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值引用
    /// @return err_t 错误码
    err_t getAttrInt(StringView path, int& value) const;

    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值引用
    /// @return err_t 错误码
    err_t getAttrDouble(StringView path, double& value) const;

    /// @brief 获取属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值引用
    /// @return err_t 错误码
    err_t getAttrString(StringView path, std::string& value) const;

    /// @param path 属性路径
    /// @param value 属性值
    /// @return err_t 错误码
    err_t setAttrBool(StringView path, bool value);

    /// @brief 设置属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值
    /// @return err_t 错误码
    err_t setAttrInt(StringView path, int value);

    /// @brief 设置属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值
    /// @return err_t 错误码
    err_t setAttrDouble(StringView path, double value);

    /// @brief 设置属性值，属性路径格式为 "attr1.attr2.attr3"
    /// @param path 属性路径
    /// @param value 属性值
    /// @return err_t 错误码
    err_t setAttrString(StringView path, StringView value);

    /// @brief 获取对象类型
    /// @return Class* 类型元信息
    Class* type() const{return m_type;}

    /// @brief 获取属性元信息
    /// @param fieldName 属性名
    /// @return Property* 属性元信息
    Property* getProperty(StringView fieldName) const;
    
    /// @brief 获取强引用计数
    /// @return uint32_t 强引用计数
    uint32_t refCount() const{return m_refcnt;}

    /// @brief 获取弱引用计数
    /// @return uint32_t 弱引用计数
    uint32_t weakRefCount() const{return m_weakrefcnt;}

    /// @brief 判断对象是否被析构
    /// @return bool 是否已析构
    bool     isDestructed() const{return m_type ==nullptr;}

    /// @brief 析构对象，仅当强引用计数为0时才会被调用
    /// @details 析构对象时，会先将弱引用计数减1，若弱引用计数为0，则会调用析构函数
    void     destruct()
    {
        assert(m_refcnt == 0);  // 只能直接删除不采用共享引用计数管理的对象
        this->_destruct();
    }

    /// @brief 增加弱引用计数
    /// @return uint32_t 新的弱引用计数
    uint32_t incWeakRef()
    {
        return ++m_weakrefcnt;
    }

    /// @brief 减少弱引用计数
    /// @return uint32_t 新的弱引用计数
    uint32_t decWeakRef()
    {
        if (m_weakrefcnt == 1) {
            operator delete(this);
            return 0;
        }
        else {
            return --m_weakrefcnt;
        }
    }

    /// @brief 增加强引用计数
    /// @return uint32_t 新的强引用计数
    uint32_t incRef()
    {
        return ++m_refcnt;
    }

    /// @brief 减少强引用计数
    /// @return uint32_t 新的强引用计数
    uint32_t decRef()
    {
        if (m_refcnt == 1) {
            this->_destruct();
            return 0;
        }
        return --m_refcnt;
    }

    /// @brief 减少强引用计数，不删除对象
    /// @return uint32_t 新的强引用计数
    uint32_t decRefNoDelete()
    {
        return --m_refcnt;
    }
private:
    /// @brief 析构对象，仅当强引用计数为0时才会被调用
    /// @details 析构对象时，会先将弱引用计数减1，若弱引用计数为0，则会调用析构函数
    void    _destruct()
    {
        this->~Object();
        this->m_type = nullptr; // 标识对象是否被析构. bit mask indicate whether object is destructed.
        this->decWeakRef();
    }

protected:
    virtual ~Object() = default;

protected:
    Class*                   m_type;                 ///< 类型元信息，同时用于标识对象是否被析构
    std::atomic<uint32_t>    m_refcnt;               ///< 强引用计数，给SharedPtr使用
    std::atomic<uint32_t>    m_weakrefcnt;           ///< 弱引用计数，给WeakPtr使用
};


/*! @} */



AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Object)


