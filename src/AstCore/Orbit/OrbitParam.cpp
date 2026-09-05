/// @file      OrbitParam.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      19.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
 
#include "OrbitParam.hpp" 
#include "AstCore/MathOperator.hpp"
#include "AstUtil/Constants.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/Axes.hpp"
#include "AstUtil/Math.hpp"
#include "AstUtil/Logger.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Rotation.hpp"
#include <cmath>

#define PI kPI
#define PI2 kTwoPI
 
AST_NAMESPACE_BEGIN

using namespace math;


// #define INVALID_PARAM(val) val
#define INVALID_PARAM(val) NAN



double aApoAltToMeanMotion(double apogeeAlt, double eccentricity, double bodyRadius, double gm)
{
    double apoRad = apogeeAlt + bodyRadius;
    double smajAx = apoRad / (1 + eccentricity);
    return sqrt(gm / (smajAx * smajAx * smajAx));
}

double aApoAltToPeriAlt(double apogeeAlt, double eccentricity, double bodyRadius)
{
    return (apogeeAlt + bodyRadius) / (1 + eccentricity) * (1 - eccentricity) - bodyRadius;
}

double aApoAltToPeriRad(double apogeeAlt, double eccentricity, double bodyRadius)
{
    return (apogeeAlt + bodyRadius) * (1 - eccentricity) / (1 + eccentricity);
}

double aApoAltToPeriod(double apogeeAlt, double eccentricity, double bodyRadius, double gm)
{
    double apoRad = apogeeAlt + bodyRadius;
    double smajAx = apoRad / (1 + eccentricity);
    return  kTwoPI * sqrt((smajAx * smajAx * smajAx) / gm);
}

double aApoAltToSMA(double apogeeAlt, double eccentricity, double bodyRadius)
{
    double apoRad = apogeeAlt + bodyRadius;
    return apoRad / (1 + eccentricity);
}

double aApoRadToApoAlt(double apogeeRad, double bodyRadius)
{
    return apogeeRad - bodyRadius;
}

double aApoRadToMeanMotion(double apogeeRad, double eccentricity, double gm)
{
    double smajAx = apogeeRad / (1 + eccentricity);
    return sqrt(gm / (smajAx * smajAx * smajAx));
}

double aApoRadToPeriAlt(double apogeeRad, double eccentricity, double bodyRadius)
{
    return apogeeRad * (1 - eccentricity) / (1 + eccentricity) - bodyRadius;
}


double	aApoRadToPeriod(double apogeeRad, double eccentricity, double gm)
{
    double smajAx = apogeeRad / (1 + eccentricity);
    return  kTwoPI * sqrt((smajAx * smajAx * smajAx) / gm);
}

double	aApoRadToPeriRad(double apogeeRad, double eccentricity)
{
    assert(eccentricity >=0);
    return apogeeRad * (1 - eccentricity) / (1 + eccentricity);
}

double aApoRadToSMA(double apogeeRad, double eccentricity)
{
    return apogeeRad / (1 + eccentricity);
}


double	aEccToMean(double E, double e)
{
    if (e < 0.0) {
        return INVALID_PARAM(E);
    }
    double M = 0.0;
    if (e >= 0.0 && e < 1.0)//圆和椭圆轨道
    {
        M = E - e * sin(E);
    }
    else if (e > 1.0)       //双曲线轨道
        M = e * sinh(E) - E;
    else
    {
        // E = tan(f/2);
        M = E/2 + E * E * E / 6;
    }
    return M;
}

double	aEccToTrue(double E, double e)
{
    if (e < 0.0) {
        return INVALID_PARAM(E);
    }
    double f = 0.0;
    if (e >= 0.0 && e < 1.0)     //圆和椭圆轨道
    {
        double E0 = fmod(E, PI2);
        f = 2.0 * atan(sqrt((1.0 + e) / (1.0 - e)) * tan(0.5 * E)) + (E - E0);
    }
    else if (e > 1.0)    //双曲线轨道
        f = 2.0 * atan(sqrt((e + 1.0) / (e - 1.0)) * tanh(0.5 * E));
    else // 抛物线轨道
    {
        f = atan(E) * 2; 
    }
    return f;
}
// time pass ascn node 过升交点时间
double	aEccToTimePastAscNode(double E, double argPeri, double semiMajorAxis, double e, double gm)
{
    return aEccToTimePastPeri(E, semiMajorAxis, e, gm) + aTrueToTimePastPeri(argPeri, semiMajorAxis, e, gm);
}
// time pass perigee 过近心点时间
double	aEccToTimePastPeri(double E, double semiMajorAxis, double e, double gm)
{
    if (e < 0) {
        return INVALID_PARAM(E);
    }
    if (e >= 0 && e < 1) {
        double M = E - e * sin(E);
        //return M / sqrt(gm / pow(semiMajorAxis, 3));
        return M * sqrt((semiMajorAxis * semiMajorAxis * semiMajorAxis) / gm);
    }
    else if (e > 1.) {
        double M = e * sinh(E) - E;
        return M * sqrt(pow(std::abs(semiMajorAxis), 3) / gm);  // 双曲线a<0
    }
    else {
        aError("暂不支持抛物线轨道.");
        // @todo
        return NAN;
    }
}

double	aMeanMotionToApoAlt(double meanMotion, double eccentricity, double bodyRadius, double gm)
{
    double smjax = cbrt(gm / (meanMotion * meanMotion));
    return smjax * (1 + eccentricity) - bodyRadius;
}

double	aMeanMotionToApoRad(double meanMotion, double eccentricity, double gm)
{
    double smjax = cbrt(gm / (meanMotion * meanMotion));
    return smjax * (1 + eccentricity);
}
double	aMeanMotionToPeriAlt(double meanMotion, double eccentricity, double bodyRadius, double gm)
{
    double smjax = cbrt(gm / (meanMotion * meanMotion));
    return smjax * (1 - eccentricity) - bodyRadius;
}

double	aMeanMotionToPeriRad(double meanMotion, double eccentricity, double gm)
{
    double smjax = cbrt(gm / (meanMotion * meanMotion));
    return smjax * (1 - eccentricity);
}
double	aMeanMotionToPeriod(double meanMotn)
{
    return PI2 / meanMotn;
}
double	aMeanMotionToSMA(double meanMotn, double gm)
{
    return cbrt(gm / (meanMotn * meanMotn));
}


double	aMeanToEcc(double M, double e, double eps, int maxIter)
{
    // static double eps = 1e-14;
    // static int maxIter = 100;
    if (e < 0.0) {
        return M;
    }
    //迭代方法:《Solar System Dynamics》Chapter2,Carl D.Murray and Stanley F.Dermott著
    double E = 0.0, Minus = 0.0, DeMinus = 0.0, DeDeMinus = 0.0, DeDeDeMinus = 0.0, Delta1 = 0.0, Delta2 = 0.0, Delta3 = 0.0;
    int N = 0;
    if (e >= 0.0 && e < 1.0)//圆和椭圆轨道
    {
        double RM = fmod(M, PI2);
        if (RM < 0.0)
            RM += PI2;
        double sinRM = sin(RM);
        E = RM + 0.85 * e * sign(sinRM);
        N = 0;
        Delta3 = 1.0;
        while (fabs(Delta3) >= eps && N < maxIter)
        {
            Minus = E - e * sin(E) - RM;
            DeMinus = 1.0 - e * cos(E);
            DeDeMinus = e * sin(E);
            DeDeDeMinus = e * cos(E);
            Delta1 = -Minus / DeMinus;
            Delta2 = -Minus / (DeMinus + 0.5 * Delta1 * DeDeMinus);
            Delta3 = -Minus / (DeMinus + 0.5 * Delta2 * DeDeMinus + 1.0 / 6.0 * Delta2 * Delta2 * DeDeDeMinus);
            E = E + Delta3;
            N = N + 1;
        }
        E = E + M - RM;
    }
    else if (e > 1.0)//双曲线轨道
    {
        E = asinh(M / e);
        Delta3 = 1.0;
        N = 0;
        while (fabs(Delta3) >= eps && N < maxIter)
        {
            Minus = e * sinh(E) - E - M;
            DeMinus = e * cosh(E) - 1.0;
            DeDeMinus = e * sinh(E);
            DeDeDeMinus = e * cosh(E);
            Delta1 = -Minus / DeMinus;
            Delta2 = -Minus / (DeMinus + 0.5 * Delta1 * DeDeMinus);
            Delta3 = -Minus / (DeMinus + 0.5 * Delta2 * DeDeMinus + 1.0 / 6.0 * Delta2 * Delta2 * DeDeDeMinus);
            E = E + Delta3;
            N = N + 1;
        }
    }
    else // 抛物线轨道 
    {
        // 巴克方程
        double B = 3 * M;
        double temp = cbrt(B + sqrt(B*B + 1));
        E = temp - 1 / temp;
    }
    if (((e >= 0.0 && e < 1.0) || (e > 1.0)) && fabs(Delta3) >= 5.0 * eps && N >= maxIter)
    {
        aError("mean2ecc迭代不收敛，请降低精度epsilon或增加迭代次数限制.");
        return M;
    }
    return E;
}
double	aMeanToTimePastAscNode(double meanAnomaly, double argPeri, double semiMajorAxis, double eccentricity, double gm)
{
    return aMeanToTimePastPeri(meanAnomaly, semiMajorAxis, gm) + aTrueToTimePastPeri(argPeri, semiMajorAxis, eccentricity, gm);
}
double	aMeanToTimePastPeri(double meanAnomaly, double semiMajorAxis, double gm)
{
    return meanAnomaly * sqrt(pow(std::abs(semiMajorAxis), 3) / gm);
}
double	aMeanToTrue(double meanAnomaly, double eccentricity, double eps, int maxIter)
{
    double E = aMeanToEcc(meanAnomaly, eccentricity, eps, maxIter);
    return aEccToTrue(E, eccentricity);
}
double	aPeriAltToApoAlt(double perigeeAlt, double eccentricity, double bodyRadius)
{
    return (perigeeAlt + bodyRadius) * (1 + eccentricity) / (1 - eccentricity) - bodyRadius;
}
double	aPeriAltToApoRad(double perigeeAlt, double eccentricity, double bodyRadius)
{
    return (perigeeAlt + bodyRadius) * (1 + eccentricity) / (1 - eccentricity);
}
double	aPeriAltToMeanMotion(double perigeeAlt, double eccentricity, double bodyRadius, double gm)
{
    double smjax = (perigeeAlt + bodyRadius) / (1 - eccentricity);
    return sqrt(gm / (smjax * smjax * smjax));
}
double	aPeriAltToPeriRad(double perigeeAlt, double bodyRadius)
{
    return perigeeAlt + bodyRadius;
}
double	aPeriAltToPeriod(double perigeeAlt, double eccentricity, double bodyRadius, double gm)
{
    double smjax = (perigeeAlt + bodyRadius) / (1 - eccentricity);
    return PI2 * sqrt((smjax * smjax * smjax) / gm);
}
double	aPeriAltToSMA(double perigeeAlt, double eccentricity, double bodyRadius)
{
    return (perigeeAlt + bodyRadius) / (1 - eccentricity);
}
double	aPeriRadToApoAlt(double perigeeRad, double eccentricity, double bodyRadius)
{
    return perigeeRad * (1 + eccentricity) / (1 - eccentricity) - bodyRadius;
}
double	aPeriRadToApoRad(double perigeeRad, double eccentricity)
{
    return perigeeRad * (1 + eccentricity) / (1 - eccentricity);

}
double	aPeriRadToMeanMotion(double perigeeRad, double eccentricity, double gm)
{
    double smjax = perigeeRad / (1 - eccentricity);
    return sqrt(gm / (smjax * smjax * smjax));
}
double	aPeriRadToPeriAlt(double perigeeRad, double bodyRadius)
{
    return perigeeRad - bodyRadius;
}
double	aPeriRadToPeriod(double perigeeRad, double eccentricity, double gm)
{
    double smjax = perigeeRad / (1 - eccentricity);
    return PI2 * sqrt((smjax * smjax * smjax) / gm);
}
double	aPeriRadToSMA(double perigeeRad, double eccentricity)
{
    return perigeeRad / (1 - eccentricity);
}
double	aPeriodToApoAlt(double period, double eccentricity, double bodyRadius, double gm)
{
    double smajax = pow(period * sqrt(gm) / PI2, 2. / 3.);
    return smajax * (1 + eccentricity) - bodyRadius;
}
double	aPeriodToApoRad(double period, double eccentricity, double gm)
{
    double smajax = pow(period * sqrt(gm) / PI2, 2. / 3.);
    return smajax * (1 + eccentricity);
}
double	aPeriodToMeanMotion(double period)
{
    return PI2 / period;
}
double	aPeriodToPeriAlt(double period, double eccentricity, double bodyRadius, double gm)
{
    double smajax = pow(period * sqrt(gm) / PI2, 2. / 3.);
    return smajax * (1 - eccentricity) - bodyRadius;
}
double	aPeriodToPeriRad(double period, double eccentricity, double gm)
{
    double smajax = pow(period * sqrt(gm) / PI2, 2. / 3.);
    return smajax * (1 - eccentricity);
}
double	aPeriodToSMA(double period, double gm)
{
    return  pow(period * sqrt(gm) / PI2, 2. / 3.);
}
double	aPeriRadApoRadToEcc(double perigeeRad, double apogeeRad)
{
    return std::abs(apogeeRad - perigeeRad) / (perigeeRad + apogeeRad);
}
double aPeriAltApoAltToEcc(double perigeeAlt, double apogeeAlt, double bodyRadius)
{
    return std::abs(apogeeAlt - perigeeAlt) / (perigeeAlt + apogeeAlt + 2*bodyRadius);
}

double aSMAToApoAlt(double semiMajorAxis, double eccentricity, double bodyRadius)
{
    return semiMajorAxis * (1 + eccentricity) - bodyRadius;
}
double	aSMAToApoRad(double semiMajorAxis, double eccentricity)
{
    return semiMajorAxis * (1 + eccentricity);
}
double	aSMAToMeanMotion(double semiMajorAxis, double gm)
{
    return sqrt(gm / (semiMajorAxis * semiMajorAxis * semiMajorAxis));
}
double	aSMAToPeriAlt(double semiMajorAxis, double eccentricity, double bodyRadius)
{
    return semiMajorAxis * (1 - eccentricity) - bodyRadius;
}

double	aSMAToPeriod(double semiMajorAxis, double gm)
{
    return PI2 * sqrt((semiMajorAxis * semiMajorAxis * semiMajorAxis) / gm);
}
// 半短轴
double	aSMAToSMinAx(double semiMajorAxis, double eccentricity)
{
    return semiMajorAxis * sqrt(std::abs(1 - eccentricity * eccentricity));
}
// p? 半通径
double	aSMAToSParam(double semiMajorAxis, double eccentricity)
{
    return  semiMajorAxis * (1 - eccentricity * eccentricity);
}

double	aSMinAxToSMA(double semiminorAxis, double eccentricity)
{
    return semiminorAxis / sqrt(std::abs(1 - eccentricity * eccentricity));
}
double	aTimePastAscNodeToEcc(double TimePastAscNode, double argPeri, double semiMajorAxis, double eccentricity, double gm, double eps, int maxIter)
{
    double TTP = TimePastAscNode - aTrueToTimePastPeri(argPeri, semiMajorAxis, eccentricity, gm);
    return aTimePastPeriToEcc(TTP, semiMajorAxis, eccentricity, gm, eps, maxIter);
}

double	aTimePastAscNodeToMean(double TimePastAscNode, double argPeri, double semiMajorAxis,
    double eccentricity, double gm)
{
    double TTP = TimePastAscNode - aTrueToTimePastPeri(argPeri, semiMajorAxis, eccentricity, gm);
    return aTimePastPeriToMean(TTP, semiMajorAxis, gm);
}
double	aTimePastAscNodeToTimePastPeri(double TimePastAscNode, double argPeri, double semiMajorAxis,
    double eccentricity, double gm)
{
    return TimePastAscNode - aTrueToTimePastPeri(argPeri, semiMajorAxis, eccentricity, gm);
}

double	aTimePastAscNodeToTrue(double TimePastAscNode, double argPeri, double semiMajorAxis,
    double eccentricity, double gm, double eps, int maxIter)
{
    double TTP = TimePastAscNode - aTrueToTimePastPeri(argPeri, semiMajorAxis, eccentricity, gm);
    return aTimePastPeriToTrue(TTP, semiMajorAxis, eccentricity, gm, eps, maxIter);
}

double	aTimePastPeriToEcc(double TimePastPeri, double semiMajorAxis, double eccentricity, double gm, double eps, int maxIter)
{
    double M = TimePastPeri * sqrt(gm / pow(std::abs(semiMajorAxis), 3));
    return aMeanToEcc(M, eccentricity, eps, maxIter);
}
double	aTimePastPeriToMean(double TimePastPeri, double semiMajorAxis, double gm)
{
    return TimePastPeri * sqrt(gm / pow(std::abs(semiMajorAxis), 3));
}
double	aTimePastPeriToTrue(double TimePastPeri, double semiMajorAxis, double eccentricity, double gm, double eps, int maxIter)
{
    double M = TimePastPeri * sqrt(gm / pow(std::abs(semiMajorAxis), 3));
    return aMeanToTrue(M, eccentricity, eps, maxIter);
}
double	aTimePastPeriToTimePastAscNode(double TimePastPeri, double argPeri, double semiMajorAxis,
    double eccentricity, double gm)
{
    return TimePastPeri + aTrueToTimePastPeri(argPeri, semiMajorAxis, eccentricity, gm);
}

double	aTrueToEcc(double f, double e)
{
    if (e < 0.0) {
        return INVALID_PARAM(f);
    }
    double E = 0.0;
    if (e >= 0.0 && e < 1.0)//圆和椭圆轨道
    {
        double f0 = fmod(f, PI2);
        E = 2.0 * atan(sqrt((1.0 - e) / (1.0 + e)) * tan(0.5 * f)) + (f - f0);
    }
    else if (e > 1.0)//双曲线轨道
    {
        if (f > PI - acos(1.0 / e) || f < -PI + acos(1.0 / e))
        {
            aError("不可能达到的双曲轨道.");
            return INVALID_PARAM(f);
        }
        else
            E = 2.0 * atanh(sqrt((e - 1.0) / (1.0 + e)) * tan(0.5 * f));
    }
    else // 抛物线轨道
    {
        E = tan(f/2);
    }
    return E;
}
double	aTrueToMean(double trueAnomaly, double eccentricity)
{
    double E = aTrueToEcc(trueAnomaly, eccentricity);
    return aEccToMean(E, eccentricity);
}
double	aTrueToTimePastAscNode(double trueAnomaly, double argPeri, double semiMajorAxis,
    double eccentricity, double gm)
{
    return aTrueToTimePastPeri(trueAnomaly, semiMajorAxis, eccentricity, gm) + aTrueToTimePastPeri(argPeri, semiMajorAxis, eccentricity, gm);
}
double	aTrueToTimePastPeri(double trueAnomaly, double semiMajorAxis, double eccentricity, double gm)
{
    double E = aTrueToEcc(trueAnomaly, eccentricity);
    return aEccToTimePastPeri(E, semiMajorAxis, eccentricity, gm);
}



double aRAANToLAN(double raan, Axes* inertialAxes, const TimePoint& timeOfAscNodePassage, Axes* bodyFixedAxes)
{
    double cosRAAN, sinRAAN;
    sincos(raan, &sinRAAN, &cosRAAN);
    Vector3d vecBodyFixed{ cosRAAN, sinRAAN, 0.0 };
    Rotation rotation;
    aAxesTransform(inertialAxes, bodyFixedAxes, timeOfAscNodePassage, rotation);
    vecBodyFixed = rotation.transformVector(vecBodyFixed);
    return atan2(vecBodyFixed.y(), vecBodyFixed.x());
}

double aLANToRAAN(double lan, Axes *bodyFixedAxes, const TimePoint &timeOfAscNodePassage, Axes *inertialAxes)
{
    double cosLAN, sinLAN;
    sincos(lan, &sinLAN, &cosLAN);
    Vector3d vecBodyFixed{ cosLAN, sinLAN, 0.0 };
    Rotation rotation;
    aAxesTransform(bodyFixedAxes, inertialAxes, timeOfAscNodePassage, rotation);
    vecBodyFixed = rotation.transformVector(vecBodyFixed);
    return atan2(vecBodyFixed.y(), vecBodyFixed.x());
}

double	aRepeatGroundTrackSMA(int daysToRepeat, int revsToRepeat, double gm, double bodyRotRate)
{
    double meanMotn = revsToRepeat * bodyRotRate / daysToRepeat;
    return aMeanMotionToSMA(meanMotn, gm);
}

double  aEccToFlat(double eccentricity)
{
    return 1 - sqrt(1 - eccentricity * eccentricity);
}

double  aFlatToEcc(double flatFactor)
{
    return sqrt(flatFactor * (2 - flatFactor));
}


AST_NAMESPACE_END


