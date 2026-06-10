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
#include "AstCore/BuiltinFrame.hpp"
#include "AstMath/KinematicTransform.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

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

// @todo 从 vrotv 函数复制的函数实现，需要考虑删减重复代码

void aVectorRotation(const Vector3d& v, const Vector3d& axis, double theta, Vector3d& r)
{
    double axisNorm = axis.norm();
    if (axisNorm == 0.0) {
        r = v;
        return;
    }

    // 单位轴
    Vector3d x = axis / axisNorm;

    // 投影到轴上的分量
    Vector3d p = v.dot(x) * x;

    // 垂直轴的分量
    Vector3d v1 = v - p;

    // 将垂直分量绕轴旋转 90° 所得向量
    Vector3d v2 = x.cross(v1);

    double c = std::cos(theta);
    double s = std::sin(theta);

    // 旋转后的垂直分量
    Vector3d rplane = c * v1 + s * v2;

    // 最终结果 = 旋转后的垂直分量 + 轴向分量
    r = rplane + p;
}

// @todo 从 stelab 函数复制的函数实现，需要考虑删减重复代码

errc_t aStellarAberrationForReception(const Vector3d& pobj, const Vector3d& vobs, Vector3d& appobj)
{
    // 计算目标方向的单位向量
    double normPobj = pobj.norm();
    if (normPobj == 0.0) {
        // 目标恰好位于观测者处，无需校正
        appobj = pobj;
        return eNoError;
    }
    Vector3d u = pobj / normPobj;

    // 速度与光速的比值 v/c
    Vector3d vbyc = vobs / aLightSpeed();

    // 检查观测者速度是否超光速
    double lensqr = vbyc.squaredNorm();
    if (lensqr >= 1.0) {
        aError("observer speed >= speed of light");
        return -1;
    }

    // 计算叉积 H = U × (v/c)
    Vector3d h = u.cross(vbyc);

    double sinphi = h.norm();
    if (sinphi != 0.0) {
        // 像差角 φ = arcsin(|h|)
        double phi = std::asin(sinphi);
        // 绕旋转轴 h 旋转 phi(φ) 弧度
        aVectorRotation(pobj, h, phi, appobj);
    } else {
        // 运动方向与视线重合，无需校正
        appobj = pobj;
    }
    return eNoError;
}

errc_t aStellarAberrationForTransmission(const Vector3d& pobj, const Vector3d& vobs, Vector3d& appobj)
{
    // 发射情况的校正是接收情况的逆校正，只需将速度取反
    Vector3d negVel = -vobs;
    return aStellarAberrationForReception(pobj, negVel, appobj);
}

errc_t aStellarAberration(const Vector3d& relPosition, const Vector3d& velObserver, EAberrationFlags aberrationFlags, Vector3d& relPositionCorrected)
{
    // 如果指定了发射情况的校正
    if(!!(aberrationFlags & EAberrationFlags::eTransmission))
    {
        return aStellarAberrationForTransmission(relPosition, velObserver, relPositionCorrected);
    }
    // 如果指定了接收情况的校正
    else
    {
        return aStellarAberrationForReception(relPosition, velObserver, relPositionCorrected);
    }
    return eNoError;
}

// @todo 从 spkapo 函数复制的函数实现，需要考虑删减重复代码

errc_t aApparentPositionICRF(
    Point* point, const TimePoint& time, const Vector3d& observerPosInSSBICRF, const Vector3d& observerVelInSSBICRF, EAberrationFlags aberrationFlags,
    Vector3d& apparentRelPosition, double* lightTime
)
{
    if(!point)
        return eErrorNullInput;
    const double lightSpeed = aLightSpeed();
    Frame* icrf = aFrameICRF();
    Vector3d posTarg;
    errc_t rc = point->getPosIn(icrf, time, posTarg);
    if ( rc != 0 )
        return rc;
    apparentRelPosition = posTarg - observerPosInSSBICRF;
    if(aberrationFlags == EAberrationFlags::eNone)
    {
        if(lightTime != nullptr)
            *lightTime = apparentRelPosition.norm() / lightSpeed;
        return eNoError;
    }
    else
    {
        double lt = apparentRelPosition.norm() / lightSpeed;
        int sign = !!(aberrationFlags & EAberrationFlags::eTransmission)? 1 : -1;
        int maxIter = 3;
        for(int i=0; i<maxIter; i++)
        {
            double dt = sign * lt;
            errc_t rc = point->getPosIn(icrf, time.shiftedBySecond(dt), posTarg);
            if ( rc != 0 )
                return rc;
            apparentRelPosition = posTarg - observerPosInSSBICRF;
            lt = apparentRelPosition.norm() / lightSpeed;
        }
        if(lightTime != nullptr)
            *lightTime = lt;
        if(!!(aberrationFlags & EAberrationFlags::eStellarAberration))
        {
            return aStellarAberration(apparentRelPosition, observerVelInSSBICRF, aberrationFlags, apparentRelPosition);
        }
    }
    return eNoError;
}


errc_t aApparentPositionInFrame(
    Point* point, const TimePoint& time, Frame* frame, const Vector3d& observerPos, const Vector3d& observerVel, EAberrationFlags aberrationFlags,
    Vector3d& apparentRelPosition, double* lightTime
)
{
    // 1. 计算观察者在太阳系质心ICRF下的位置
    KinematicTransform transform;
    auto icrf = aFrameICRF();
    errc_t rc = icrf->getTransformFrom(frame, time, transform);
    if ( rc != 0 ) return rc;
    Vector3d icrfPos;
    Vector3d icrfVel;
    transform.transformPositionVelocity(observerPos, observerVel, icrfPos, icrfVel);
    // 2. 计算目标点在太阳系质心ICRF下的视位置
    rc = aApparentPositionICRF(point, time, icrfPos, icrfVel, aberrationFlags, apparentRelPosition, lightTime);
    if ( rc != 0 )  return rc;
    // 3. 将视向量转换回原坐标系
    transform.getRotation().transformVectorInv(apparentRelPosition, apparentRelPosition);
    return eNoError;
}
AST_NAMESPACE_END

