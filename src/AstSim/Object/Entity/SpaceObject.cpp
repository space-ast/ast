///
/// @file      SpaceObject.cpp
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

#include "SpaceObject.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/ObjectLinkTo.hpp"
#include "AstUtil/RTTIAPI.hpp"

AST_NAMESPACE_BEGIN

ObjectLinkTo* aSpaceObject_GetReferenceVehicleLink(SpaceObject* spaceObject)
{
    if(!spaceObject)
        return nullptr;
    auto linkTo = aFindChild<ObjectLinkTo*>(spaceObject, "ReferenceVehicle");
    if(!linkTo)
    {
        linkTo = aNewObject<ObjectLinkTo>(spaceObject);
        linkTo->setName("ReferenceVehicle");
    }
    return linkTo;
}

SpaceObject* aSpaceObject_GetReferenceVehicle(SpaceObject* spaceObject)
{
    auto linkTo = aSpaceObject_GetReferenceVehicleLink(spaceObject);
    if(!linkTo)
        return nullptr;
    return aobject_cast<SpaceObject*>(linkTo->resolve());
}


void aSpaceObject_SetReferenceVehicle(SpaceObject* spaceObject, SpaceObject* referenceVehicle)
{
    auto linkTo = aSpaceObject_GetReferenceVehicleLink(spaceObject);
    if(!linkTo)
        return;
    if(referenceVehicle){
        linkTo->setResolvedName(referenceVehicle->getName());
        linkTo->setResolvedType(referenceVehicle->getType());
    }
    else
    {
        linkTo->setResolvedName({});
        linkTo->setResolvedType(nullptr);
    }
    linkTo->setResolvedObject(referenceVehicle);
}

void aSpaceObject_SetReferenceVehicle(SpaceObject* spaceObject, StringView referenceVehiclePath)
{
    auto referenceVehicle = aResolveObject<SpaceObject*>(referenceVehiclePath);
    if(!referenceVehicle)
        return;
    return aSpaceObject_SetReferenceVehicle(spaceObject, referenceVehicle);
}

AST_NAMESPACE_END