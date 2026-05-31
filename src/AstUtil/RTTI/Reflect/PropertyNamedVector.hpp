///
/// @file      PropertyNamedVector.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-05
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
#include "AstUtil/Property.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

#ifndef SWIG

AST_UTIL_API errc_t aFakeGetByString(const void* container, StringView key, Object*& value);
AST_UTIL_API errc_t aFakeGetByIndex(const void* container, size_t key, Object*& value);


/// @brief 命名向量属性
class AST_UTIL_API PropertyNamedVector: public Property
{
public:
    /// @brief 构造函数
    /// @param getterByString 获取属性值的函数指针，用于根据属性键获取属性值
    /// @param getterByIndex 获取属性值的函数指针，用于根据属性索引获取属性值
    PropertyNamedVector(FPropertyGetItemByString getterByString, FPropertyGetItemByIndex getterByIndex)
        : getterByString_(getterByString?getterByString:&aFakeGetByString)
        , getterByIndex_(getterByIndex?getterByIndex:&aFakeGetByIndex)
    {
    }

    /// @brief 获取属性值
    /// @param container 容器指针
    /// @param key 属性键
    /// @param value 属性值
    errc_t getItem(const void* container, StringView key, Object*& value);
    
    /// @brief 获取属性值
    /// @param container 容器指针
    /// @param key 属性索引
    /// @param value 属性值
    errc_t getItem(const void* container, size_t key, Object*& value);
protected:
    FPropertyGetItemByString getterByString_;   ///< 获取属性值的函数指针，用于根据属性键获取属性值
    FPropertyGetItemByIndex  getterByIndex_;    ///< 获取属性值的函数指针，用于根据属性索引获取属性值
};

#endif

/*! @} */

AST_NAMESPACE_END

