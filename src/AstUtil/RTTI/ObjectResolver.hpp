///
/// @file      ObjectResolver.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-17
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 对象解析器
/// @details 用于解析对象的延迟链接
class ObjectResolver
{
public:
    ObjectResolver() = default;

    virtual ~ObjectResolver() = default;

    /// @brief 解析对象
    /// @details 解析对象的延迟链接
    /// @return 解析后的对象指针
    virtual Object* resolve() const = 0;

    /// @brief 获取期望的类型元信息
    /// @details Class* 类型元信息指针
    /// @return Class* 类型元信息指针
    virtual Class* getExpectedType() const {return Object::StaticType();};
};


/*! @} */

AST_NAMESPACE_END
