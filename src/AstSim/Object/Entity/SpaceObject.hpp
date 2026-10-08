///
/// @file      SpaceObject.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-26
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "AstSim/Mover.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_SIM_API SpaceObject: public Mover
{
public:
    AST_OBJECT(SpaceObject)
    SpaceObject() = default;
    ~SpaceObject() override = default;
};

class ObjectLinkTo;

/// @brief 获取参考航天器链接
AST_SIM_API ObjectLinkTo* aSpaceObject_GetReferenceVehicleLink(SpaceObject* spaceObject);

/// @brief 获取参考航天器
AST_SIM_API SpaceObject* aSpaceObject_GetReferenceVehicle(SpaceObject* spaceObject);

/// @brief 设置参考航天器
AST_SIM_API void aSpaceObject_SetReferenceVehicle(SpaceObject* spaceObject, SpaceObject* referenceVehicle);

/// @brief 设置参考航天器(通过路径)
AST_SIM_API void aSpaceObject_SetReferenceVehicle(SpaceObject* spaceObject, StringView referenceVehiclePath);


/*! @} */

AST_NAMESPACE_END