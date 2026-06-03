///
/// @file      BodyPosition.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-02
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

#include "BodyPosition.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/AER.hpp"

AST_NAMESPACE_BEGIN

errc_t aBodyInBodyInertial(const TimePoint& time, Body* body, Body* referenceBody, Vector3d& posInReferenceBodyInertial)
{
    if(!body || !referenceBody)
        return eErrorNullInput;
    return body->getPosIn(referenceBody->frameInertial(), time, posInReferenceBodyInertial);
}

errc_t aBodyInBodyFixed(const TimePoint& time, Body* body, Body* referenceBody, Vector3d& posInReferenceBodyFixed)
{
    if(!body || !referenceBody)
        return eErrorNullInput;
    return body->getPosIn(referenceBody->frameFixed(), time, posInReferenceBodyFixed);
}


errc_t aBodyAERInTopocentric(const TimePoint& time, Body* body, Body* referenceBody, const GeodeticPoint& observerPosition, AER& aer)
{
    AST_CHECK_NULLPTR(referenceBody);
    auto shape = referenceBody->getShape();    AST_CHECK_NULLPTR(shape);

    // 计算天体在参考天体固连系下的位置
    Vector3d bodyInReferenceBodyFixed{};
    errc_t rc = aBodyInBodyFixed(time, body, referenceBody, bodyInReferenceBodyFixed);
    if (rc) return rc;

    // 计算天体相对于参考点的AER
    aGeodeticToAER(bodyInReferenceBodyFixed, observerPosition, aer, shape);

    return eNoError;
}


errc_t aSunAERInTopocentric(const TimePoint& time, Body* observerBody, const GeodeticPoint& observerPosition, AER& aer)
{
    return aBodyAERInTopocentric(time, aGetSun(), observerBody, observerPosition, aer);
}


AST_NAMESPACE_END

