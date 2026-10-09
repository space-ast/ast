///
/// @file      ObjectComponent.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-09
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class ObjectAccessConstraints;


/// @tparam ComponentType 组件类型
/// @param object 对象
/// @return 对象的组件
template<typename ComponentType>
ComponentType& aObject_EnsureComponent(Object& object)
{
    ComponentType* component = aFindChild<ComponentType*>(&object);
    if(component == nullptr)
        component = aNewObject<ComponentType>(&object);
    return *component;
}


/// @param object 对象
/// @return 对象的访问约束组件
AST_SIM_CAPI ObjectAccessConstraints& aObject_EnsureAccessConstraints(Object& object);



/*! @} */

AST_NAMESPACE_END
