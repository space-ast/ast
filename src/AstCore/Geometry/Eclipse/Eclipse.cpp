///
/// @file      Eclipse.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-10
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

#include "Eclipse.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Math.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

int aLightingRatio_CylindricalModel(const Vector3d& position, const Vector3d& lightingSourcePos, double occultingBodyRadius)
{
    double d2 = position.squaredNorm();
    if (d2 < occultingBodyRadius * occultingBodyRadius) {
        return 0;
    }
    double SEDistance = lightingSourcePos.norm();                    // 日地距离
    double xSEPos = -position.dot(lightingSourcePos) / SEDistance;   // 日地方向投影
    double ySEPos = sqrtSafe(d2 - xSEPos * xSEPos);                  // 垂直日地方向投影距离
    if (xSEPos <= 0)
    {
        return 1;
    }
    else if (ySEPos < occultingBodyRadius)
    {
        return 0;
    }
    return 1;
}


int _aLightingRatio_CylindricalModel(const TimePoint &tp, const Vector3d &position, Frame *frame, CelestialBody *lightSource, CelestialBody *occultingBody)
{
    if(occultingBody == lightSource)
    {
        aWarning("occultingBody is same as lightSource");
        return 1;
    }

    // 获取光源在观测者坐标系下的位置
    Vector3d lightSourcePos;
    if (lightSource->getPosIn(frame, tp, lightSourcePos) != eNoError)
    {
        return 1; // 获取失败，保守返回全光照
    }
    // 获取遮挡体在观测者坐标系下的位置
    Vector3d occultingBodyPos;
    if (occultingBody->getPosIn(frame, tp, occultingBodyPos) != eNoError)
    {
        return 1;
    }
    // 观测者相对于遮挡体中心的位置
    Vector3d obsRelativeToOcc = position - occultingBodyPos;

    // 光源相对于遮挡体中心的位置
    Vector3d lightRelativeToOcc = lightSourcePos - occultingBodyPos;

    return aLightingRatio_CylindricalModel(obsRelativeToOcc, lightRelativeToOcc, occultingBody->getRadius());
}


int aLightingRatio_CylindricalModel(const TimePoint &tp, const Vector3d &position, Frame *frame, CelestialBody *lightSource, CelestialBody *occultingBody)
{
    if(!lightSource || !occultingBody)
    {
        aWarning("lightSource or occultingBody is null");
        return 1;
    }
    return _aLightingRatio_CylindricalModel(tp, position, frame, lightSource, occultingBody);
}

int aLightingRatio_CylindricalModel(const TimePoint &tp, Point *point, CelestialBody *lightSource, CelestialBody *occultingBody)
{
    if(!lightSource || !occultingBody || !point)
    {
        aWarning("lightSource or occultingBody or point is null");
        return 1;
    }
    auto frame = occultingBody->getFrameInertial();
    Vector3d position;
    if (point->getPosIn(frame, tp, position) != eNoError)
    {
        return 1; // 获取失败，保守视为全光照
    }
    return _aLightingRatio_CylindricalModel(tp, position, frame, lightSource, occultingBody);
}




/// @brief 计算两个圆锥在单位球面上的公共立体角
/// @details 基于 "Solid Angle of Conical Surfaces, Polyhedral Cones, and Intersecting Spherical Caps" 公式45，
///          计算两个半角分别为 ang1 和 ang2、轴线夹角为 dang 的圆锥在单位球面上的相交立体角。
///          该函数处理三种情况：
///          - 两圆锥完全分离 (dang >= ang1 + ang2)：无交叠，返回 0；
///          - 一圆锥完全包含另一圆锥 (dang <= |ang1 - ang2|)：返回较小圆锥的立体角；
///          - 部分交叠：使用文献公式45进行精确计算。
/// @param dang 两圆锥轴线之间的角距离 [rad]
/// @param ang1 第一个圆锥的半角（圆锥半张角）[rad]
/// @param ang2 第二个圆锥的半角（圆锥半张角）[rad]
/// @return 两圆锥在单位球面上的公共立体角 [sr]
double aCommonSolidAngleOfCone(double dang, double ang1, double ang2)
{
    if(dang >= ang1 + ang2)
    {
        return 0;
    }
    else if(dang <= fabs(ang1 - ang2) && dang >= 0)
    {
        if(ang1 > ang2)
        {
            return kTwoPI * (1 - cos(ang2));
        }
        else
        {
            return kTwoPI * (1 - cos(ang1));
        }
    }
    else
    {
        double c1 = cos(ang1);
        double s1 = sin(ang1);
        double t1 = tan(ang1);
        
        double c2 = cos(ang2);
        double s2= sin(ang2);
        double t2 = tan(ang2);

        double cd = cos(dang);
        double sd = sin(dang);

        double tgama1 = (c2 - cd * c1) / (sd * c1);
        double gama1 = atan(tgama1);
        double phi1 = acosSafe(tgama1 / t1);
        double beta1 = acosSafe(sin(gama1) / s1);

        double tgama2 = (c1 - cd * c2) / (sd * c2);
        double gama2 = atan(tgama2);
        double phi2 = acosSafe(tgama2 / t2);
        double beta2 = acosSafe(sin(gama2) / s2);

        return 2 * (beta1 - phi1 * c1 + beta2 - phi2 * c2);
    }
}


double aLightingRatio_DualConeModel(const Vector3d& position, const Vector3d& lightingSourcePos, double lightingSourceRadius, double occultingBodyRadius)
{
    Vector3d s2pVec = position - lightingSourcePos;                         // 光源→观测者 矢量

    double s2pDis = s2pVec.norm();                                          // 光源→观测者 距离
    double s2pAng = asinSafe(lightingSourceRadius / s2pDis);               // 光源视半径张角（半角）[rad]

    double e2pDis = position.norm();                                        // 阴影体中心→观测者 距离
    double e2pAng = asinSafe(occultingBodyRadius / e2pDis);                // 阴影体视半径张角（半角）[rad]

    double ieAng = acosSafe(position.dot(s2pVec) / s2pDis / e2pDis);       // 光源中心与阴影体中心的观测夹角 [rad]


    if(ieAng > s2pAng + e2pAng)                                            // 两视圆盘相离：全光照
    {
        return 1;
    }else if(ieAng < e2pAng - s2pAng)                                      // 光源视圆盘完全在阴影体后方：全阴影
    {
        return 0;
    }
    else{                                                                   // 部分交叠：1 - 被遮挡立体角/光源总立体角
        return 1.0 - aCommonSolidAngleOfCone(ieAng, s2pAng, e2pAng) / (kTwoPI * (1 - cos(s2pAng)));
    }
}


double _aLightingRatio_DualConeModel(const TimePoint &tp, const Vector3d &position, Frame *frame, CelestialBody *lightSource, CelestialBody *occultingBody)
{
    if(occultingBody == lightSource)
    {
        aWarning("occultingBody is same as lightSource");
        return 1.0;
    }
    
    // 获取光源在观测者坐标系下的位置
    Vector3d lightSourcePos;
    if (lightSource->getPosIn(frame, tp, lightSourcePos) != eNoError)
    {
        return 1.0; // 获取失败，保守返回全光照
    }
    // 获取遮挡体在观测者坐标系下的位置
    Vector3d occultingBodyPos;
    if (occultingBody->getPosIn(frame, tp, occultingBodyPos) != eNoError)
    {
        return 1.0;
    }
    // 观测者相对于遮挡体中心的位置
    Vector3d obsRelativeToOcc = position - occultingBodyPos;

    // 光源相对于遮挡体中心的位置
    Vector3d lightRelativeToOcc = lightSourcePos - occultingBodyPos;
    
    return aLightingRatio_DualConeModel(obsRelativeToOcc, lightRelativeToOcc, lightSource->getRadius(), occultingBody->getRadius());
}



double aLightingRatio_DualConeModel(const TimePoint &tp, const Vector3d &position, Frame *frame, CelestialBody *lightSource, CelestialBody *occultingBody)
{
    if(!lightSource || !occultingBody)
    {
        aWarning("lightSource or occultingBody is null");
        return 1.0;
    }
    return _aLightingRatio_DualConeModel(tp, position, frame, lightSource, occultingBody);
}


double aLightingRatio_DualConeModel(const TimePoint &tp, Point *point, CelestialBody *lightSource, CelestialBody *occultingBody)
{
    if(!lightSource || !occultingBody || !point)
    {
        aWarning("lightSource or occultingBody or point is null");
        return 1.0;
    }
    auto frame = occultingBody->getFrameInertial();
    Vector3d position;
    if (point->getPosIn(frame, tp, position) != eNoError)
    {
        return 1.0; // 获取失败，保守视为全光照
    }
    return _aLightingRatio_DualConeModel(tp, position, frame, lightSource, occultingBody);
}

AST_NAMESPACE_END
