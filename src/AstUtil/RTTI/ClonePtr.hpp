///
/// @file      ClonePtr.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-22
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
#include "ScopedPtr.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup RTTI
    @{
*/

/// @brief 克隆指针类
/// @details 克隆指针类，用于管理可克隆对象的指针
/// @tparam T 可克隆对象的类型
template<typename T>
class ClonePtr: public ScopedPtr<T>
{
public:
    ClonePtr() = default;
    ClonePtr(const ClonePtr& other)
    {
        if(other.get())
            this->m_pointer = other.get()->clone();
    }
    ClonePtr(ClonePtr&& other)
    {
        this->m_pointer = other.m_pointer;
        other.m_pointer = nullptr;
    }
    ClonePtr& operator=(const ClonePtr& other)
    {
        if(other.get())
            this->reset(other.get()->clone());
        else
            this->reset(nullptr);
        return *this;
    }
    ClonePtr& operator=(ClonePtr&& other)
    {
        std::swap(this->m_pointer, other.m_pointer);
        return *this;
    }
    ClonePtr& operator=(T* ptr)
    {
        this->reset(ptr);
        return *this;
    }
};

/*! @} */

AST_NAMESPACE_END
