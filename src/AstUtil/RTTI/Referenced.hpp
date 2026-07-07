///
/// @file      Referenced.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-26
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
#include <atomic>       // for std::atomic
#include <cstdint>      // for uint32_t
#include <cstdio>       // for printf
#include <cassert>      // for assert

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


struct initial_strong_ref_t {};                      ///< 初始化强引用计数的标记
constexpr initial_strong_ref_t initial_strong_ref{}; ///< 初始化强引用计数的标记值


class AST_UTIL_API Referenced
{
public:
    Referenced()
        : refcnt_{0}
        , weakrefcnt_{1}
    {}
    /// @brief 构造函数，用于初始化对象的强引用计数为1
    /// @param initial_strong_ref_t 初始化强引用计数的标记
    explicit Referenced(initial_strong_ref_t)
        : refcnt_{1}
        , weakrefcnt_{1}
    {}
    virtual ~Referenced()
    {
        setDestructed();
    }
public: // 引用计数
    /// @brief 获取强引用计数
    /// @return uint32_t 强引用计数
    uint32_t refCount() const{return refcnt_;}

    /// @brief 获取弱引用计数
    /// @return uint32_t 弱引用计数
    uint32_t weakRefCount() const{return weakrefcnt_;}

    /// @brief 判断对象是否被析构
    /// @return bool 是否已析构
    bool     isDestructed() const{return aObject_IsDestructed(this);}

    /// @brief 析构对象，仅当强引用计数为0时才会被调用
    /// @details 析构对象时，会先将弱引用计数减1，若弱引用计数为0，则会调用析构函数
    /// @warning 对于栈上的对象，不要调用该函数，避免对栈内存调用 delete 导致崩溃
    void     destruct()
    {
        #ifndef NDEBUG
        if(refcnt_ != 0)
        {
            std::printf("object ref count is not 0, can not destruct");
        }
        #endif
        assert(refcnt_ == 0);  // 只能直接删除不采用共享引用计数管理的对象
        this->_destruct();
    }

    /// @brief 增加弱引用计数
    /// @return uint32_t 新的弱引用计数
    uint32_t incWeakRef()
    {
        return aObject_IncWeakRef(this);
    }

    /// @brief 减少弱引用计数
    /// @return uint32_t 新的弱引用计数
    uint32_t decWeakRef()
    {
        return aObject_DecWeakRef(this);
    }

    /// @brief 增加强引用计数
    /// @return uint32_t 新的强引用计数
    uint32_t incRef()
    {
        return aObject_IncRef(this);
    }

    /// @brief 减少强引用计数
    /// @return uint32_t 新的强引用计数
    uint32_t decRef()
    {
        return aObject_DecRef(this);
    }

    /// @brief 减少强引用计数，不删除对象
    /// @return uint32_t 新的强引用计数
    uint32_t decRefNoDelete()
    {
        return --refcnt_;
    }

public: // 实参依赖查找（ADL）
    A_ALWAYS_INLINE 
    friend uint32_t aObject_IncRef(Referenced* obj)
    {
        return ++(obj->refcnt_);
    }
    A_ALWAYS_INLINE 
    friend uint32_t aObject_DecRef(Referenced* obj)
    {
        if (obj->refcnt_ == 1) {
            obj->_destruct();
            return 0;
        }
        return --(obj->refcnt_);
    }
    A_ALWAYS_INLINE 
    friend uint32_t aObject_IncWeakRef(Referenced* obj)
    {
        return ++(obj->weakrefcnt_);
    }
    A_ALWAYS_INLINE 
    friend uint32_t aObject_DecWeakRef(Referenced* obj)
    {
        if (obj->weakrefcnt_ == 1) {
            operator delete(obj);
            return 0;
        }
        else {
            return --(obj->weakrefcnt_);
        }
    }
    A_ALWAYS_INLINE 
    friend bool aObject_IsDestructed(const Referenced *obj)
    {
        return obj->refcnt_ == static_cast<uint32_t>(-1);
    }
protected:
    void setDestructed()
    {
        this->refcnt_ = static_cast<uint32_t>(-1); // 标识对象是否被析构. bit mask indicate whether object is destructed.
    }
private:
    /// @brief 析构对象，仅当强引用计数为0时才会被调用
    /// @warning 对于栈上的对象，不要调用该函数，避免对栈内存调用 delete 导致崩溃
    void    _destruct()
    {
        this->~Referenced();
        this->decWeakRef();
    }
private:
    std::atomic<uint32_t>    refcnt_{0};                                ///< 强引用计数，给SharedPtr使用（是否考虑废弃共享引用计数，全面采用父子对象进行内存管理？）
    std::atomic<uint32_t>    weakrefcnt_{1};                            ///< 弱引用计数，给WeakPtr使用
};



/*! @} */

AST_NAMESPACE_END
