/// @file      OrbitElement.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      20.11.2025
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
 
 
#include "OrbitElement.hpp"
#include "OrbitParam.hpp"
#include "AstUtil/Constants.hpp"            // for pi, std::array
#include "AstCore/MathOperator.hpp"         // for mod
#include "AstCore/CelestialBody.hpp"        // for aBodyOrbitNormalIn
#include "AstCore/RunTimeSolarSystem.hpp"   // for aGetSun
#include "AstCore/BodyShape.hpp"            // for BodyShape::transform
#include "AstCore/GeodeticPoint.hpp"        // for GeodeticPoint
#include "AstMath/Rotation.hpp"             // for Rotation::transformVector
#include "AstUtil/Logger.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/Constants.hpp"
#include <cmath>

#define PI kPI
#define PI2 kTwoPI
 
AST_NAMESPACE_BEGIN

using namespace math;

std::string ModOrbElem::toString() const
{
    return std::string(
        "ModOrbElem{rp: " + aFormatDouble(rp_) + "m" + 
        ", e: " + aFormatDouble(e_) + 
        ", i: " + aFormatDouble(i_ * kRadToDeg) + "deg" + 
        ", raan: " + aFormatDouble(raan_ * kRadToDeg) + "deg" + 
        ", argper: " + aFormatDouble(argper_ * kRadToDeg) + "deg" + 
        ", true: " + aFormatDouble(trueA_ * kRadToDeg) + "deg" + 
        "}");
}

std::string OrbElem::toString() const
{
    return std::string(
        "OrbElem{a: " + aFormatDouble(a_) + "m" + 
        ", e: " + aFormatDouble(e_) + 
        ", i: " + aFormatDouble(i_ * kRadToDeg) + "deg" + 
        ", raan: " + aFormatDouble(raan_ * kRadToDeg) + "deg" + 
        ", argper: " + aFormatDouble(argper_ * kRadToDeg) + "deg" + 
        ", true: " + aFormatDouble(trueA_ * kRadToDeg) + "deg" + 
        "}");
}

std::string CartState::toString() const
{
    return std::string(
        "CartState{pos: " + pos_.toString() + 
        ", vel: " + vel_.toString() + 
        "}");
}

errc_t coe2rv(const double* coe, double gm, double* pos, double* vel)
{
    double sma = coe[0], ecc = coe[1], inc = coe[2], raan = coe[3], argper = coe[4], tanom = coe[5];
    double slr = sma * (1 - ecc * ecc);         // p  semi-latus rectum
    double rm = slr / (1 + ecc * cos(tanom));   // 当前距离
    double arglat = argper + tanom;             // 纬度辐角
    double sarglat = sin(arglat);
    double carglat = cos(arglat);

    double c4 = sqrt(gm / slr);
    double c5 = ecc * cos(argper) + carglat;
    double c6 = ecc * sin(argper) + sarglat;

    double sinc = sin(inc);
    double cinc = cos(inc);

    double sraan = sin(raan);
    double craan = cos(raan);

    // position vector
    pos[0] = rm * (craan * carglat - sraan * cinc * sarglat);
    pos[1] = rm * (sraan * carglat + cinc * sarglat * craan);
    pos[2] = rm * sinc * sarglat;

    // velocity vector
    vel[0] = -c4 * (craan * c6 + sraan * cinc * c5);
    vel[1] = -c4 * (sraan * c6 - craan * cinc * c5);
    vel[2] = c4 * c5 * sinc;
    if (ecc == 1) {
        aWarning(_("偏心率为1"));
        return eErrorInvalidParam;
    }
    return eNoError;
}

errc_t coe2mee(const double* coe, double* mee)
{
    double a = coe[0], e = coe[1], i = coe[2], raan = coe[3], argper = coe[4], trueAnom = coe[5];
    double& p = mee[0], & f = mee[1], & g = mee[2], & h = mee[3], & k = mee[4], & L = mee[5];

    // if ( e * f < -1.0)
    //     return false;
    p = a * (1.0 - e * e);             // 半通径p
    // if (e == 1.0)                   // 抛物线轨道 
    //     p = 2.0 * a;                // 对于抛物线根数a用perirad代替?
    double perlon = raan + argper;
    f = e * cos(perlon);               // f
    g = e * sin(perlon);               // g
    double temp = tan(i / 2.0);
    h = temp * cos(raan);              // h
    k = temp * sin(raan);              // k
    L = mod(raan + argper + trueAnom, PI2);
    if (e == 1) {
        aWarning(_("偏心率为1"));
        return eErrorInvalidParam;
    }
    return eNoError;
}
void ee2rv(const double* ee, double gm, double* pos, double* vel)
{
    /*
     // ee
     double	m_SMajAx;	///< semimajor axis length
     double	m_h;		///< e*sin(argper + raan)   perlon = argper + raan
     double	m_k;		///< e*cos(argper + raan)
     double	m_p;		///< tan(i/2)*sin(raan)
     double	m_q;		///< tan(i/2)*cos(raan)
     double	m_MeanLon;	///< mean longitude = M + raan + argper
     bool	m_Dir;		///< Retrograde=false, Posigrade=true
     // mee
     double m_p;        // p = a(1-e^2)
     double m_f;        // f = e*cos(argper+RAAN)
     double m_g;        // g = e*sin(argper+RAAN)
     double m_h;        // h = tan(i/2)cos(RAAN)
     double m_k;	    // k = tan(i/2)sin(RAAN)
     double m_L;	    // L = RAAN + argper + f
    */
    // 从ee中读取mee数据
    double sma = ee[0], mee_g = ee[1], mee_f = ee[2], k = ee[3], h = ee[4], meanlon = ee[5];  // 取值防止覆盖
    double f = mee_f;
    double g = mee_g;

    double ecc = sqrt(f * f + g * g);
    double L;
    if (ecc == 0) { // argper = 0
        L = meanlon;
    }
    else {
        double perlon = atan2(g, f);
        L = aMeanToTrue(meanlon - perlon, ecc) + perlon;
    }
    double p = sma * (1 - ecc * ecc);

    double cosL = cos(L);
    double sinL = sin(L);

    //double h_ = sqrt(p / gm);     // p = h^2 / gm
    double smovrp = sqrt(gm / p);   // = h/p
    double r = p / (1.0 + f * cosL + g * sinL);
    double s2 = 1.0 + h * h + k * k;
    double hh_kk = h * h - k * k;
    double hk2 = 2.0 * h * k;

    pos[0] = r / s2 * (cosL + hh_kk * cosL + hk2 * sinL);
    pos[1] = r / s2 * (sinL - hh_kk * sinL + hk2 * cosL);
    pos[2] = 2.0 * r / s2 * (h * sinL - k * cosL);

    vel[0] = -smovrp / s2 * (sinL + hh_kk * sinL - hk2 * cosL + g - hk2 * f + hh_kk * g);
    vel[1] = -smovrp / s2 * (-cosL + hh_kk * cosL + hk2 * sinL - f + hk2 * g + hh_kk * f);
    vel[2] = 2.0 * smovrp / s2 * (h * cosL + k * sinL + f * h + g * k);
}

void mee2rv(const double* mee, double gm, double* pos, double* vel)
{
    double p = mee[0], f = mee[1], g = mee[2], h = mee[3], k = mee[4], L = mee[5];  // 取值防止覆盖

    double cosL = cos(L);
    double sinL = sin(L);

    //double h_ = sqrt(p / gm);     // p = h^2 / gm
    double smovrp = sqrt(gm / p);   // = h/p
    double r = p / (1.0 + f * cosL + g * sinL);
    double s2 = 1.0 + h * h + k * k;
    double hh_kk = h * h - k * k;
    double hk2 = 2.0 * h * k;

    pos[0] = r / s2 * (cosL + hh_kk * cosL + hk2 * sinL);
    pos[1] = r / s2 * (sinL - hh_kk * sinL + hk2 * cosL);
    pos[2] = 2.0 * r / s2 * (h * sinL - k * cosL);

    vel[0] = -smovrp / s2 * (sinL + hh_kk * sinL - hk2 * cosL + g - hk2 * f + hh_kk * g);
    vel[1] = -smovrp / s2 * (-cosL + hh_kk * cosL + hk2 * sinL - f + hk2 * g + hh_kk * f);
    vel[2] = 2.0 * smovrp / s2 * (h * cosL + k * sinL + f * h + g * k);
}

errc_t rv2mee(const double* pos_, const double* vel_, double gm, double* mee)
{
    const array3d& vel = *reinterpret_cast<const array3d*>(vel_);
    const array3d& pos = *reinterpret_cast<const array3d*>(pos_);


    double& p = mee[0], & f = mee[1], & g = mee[2], & h_ = mee[3], & k = mee[4], & L = mee[5];
    double radius = norm(pos);                   // 距离
    double velocity = norm(vel);                 // 速度	
    auto unitR = normalized(pos);                // 径向单位矢量  
    auto unitV = normalized(vel);                // 切向单位矢量
    auto hvector = cross(unitR, unitV);             // 角动量方向
    double h = radius * velocity * norm(hvector);   // 角动量值
    auto unith = normalized(hvector);               //轨道面法向单位矢量

    // unith=[sin(i)*sin(OMEGA),
    //       -sin(i)*cos(OMEGA),
    //       cos(i)];
    //偏心率矢量	
    auto evector = (velocity * h / gm) * cross(unitV, unith) - unitR;
    //半能径方向矢量,模为e
    auto qvector = cross(unith, evector);
    //定义的一个矢量
    auto unitA = h/gm * vel - qvector;
    // 基本关系式
    // evector=e*[cos(omega)*cos(OMEGA)-sin(omega)*sin(OMEGA)*cos(i),
    //            cos(omega)*sin(OMEGA)+sin(omega)*cos(OMEGA)*cos(i),
    //            sin(omega)*sin(i)];
    // qvector=e*[-sin(omega)*cos(OMEGA)-cos(omega)*sin(OMEGA)*cos(i),
    //            -sin(omega)*sin(OMEGA)+cos(omega)*cos(OMEGA)*cos(i),
    //            cos(omega)*sin(i)];
    // unitR=[cos(OMEGA)*cos(omega+f)-cos(i)*sin(OMEGA)*sin(omega+f),
    //        sin(OMEGA)*cos(omega+f)+cos(i)*cos(OMEGA)*sin(omega+f),
    //        sin(i)*sin(omega+f)];
    // unitA=[-cos(OMEGA)*sin(omega+f)-cos(i)*sin(OMEGA)*cos(omega+f),
    //        -sin(OMEGA)*sin(omega+f)+cos(i)*cos(OMEGA)*cos(omega+f),
    //         sin(i)*cos(omega+f)];
    // 推导得出
    // evector[0]+qvector[1]=(1+cos(i))*e*cos(omega+OMEGA);
    // evector[1]-qvector[0]=(1+cos(i))*e*sin(omega+OMEGA);
    // unith[0]=(1+cos(i))*tan(i/2)*sin(OMEGA);
    // unith[1]=-(1+cos(i))*tan(i/2)*cos(OMEGA);
    // unitR[0]+unitA[1]=(1+cos(i))*cos(omega+OMEGA+f);
    // unitR[1]-unitA[0]=(1+cos(i))*sin(omega+OMEGA+f);


    if (unith[2] + 1.0 <= 0.0)
    {
        aWarning(_("轨道倾角接近180度，不适合用春分点轨道根数描述."));
        return eErrorInvalidParam;
    }
    double cosiadd1 = 1.0 + unith[2];
    p = h * h / gm;//p
    f = (evector[0] + qvector[1]) / cosiadd1;//f
    g = (evector[1] - qvector[0]) / cosiadd1;//g
    h_ = -unith[1] / cosiadd1;//h
    k = unith[0] / cosiadd1;//k
    L = atan2(unitR[1] - unitA[0], unitR[0] + unitA[1]);//L
    //转换到[0,2pi)中
    L = mod(L, PI2);
    return eNoError;
}

errc_t mee2coe(const double* mee, double* coe)
{
    double& a = coe[0], & e = coe[1], & i = coe[2], & raan = coe[3], & argper = coe[4], & trueAnom = coe[5];
    double p = mee[0], f = mee[1], g = mee[2], h = mee[3], k = mee[4], L = mee[5];

    // if (p <= 0.0)
    //     return false;
    e = sqrt(f * f + g * g);
    //if (e == 1.0)
    //    a = 0.5 * p;                 // p: 抛物线轨道的半通径
    //else
    a = p / (1.0 - e * e);   //半长轴
    double temp = sqrt(h * h + k * k);
    i = 2.0 * atan(temp);
    if (temp <= 0.0)
    {
        raan = 0.0;//升交点赤经
        //    cout<<"轨道倾角接近0或180度,升交点赤经容易奇异.在此将其置为零."<<endl;
        if (e <= 0.0)
        {
            argper = 0.0;//近拱点幅角
            //	cout<<"偏心率接近0,近星点幅角容易奇异.在此将其置为零."<<endl;        
            trueAnom = L;  //真近点角
        }
        else
        {
            argper = atan2(g, f); //近拱点幅角       
            trueAnom = L - argper;
        }
    }
    else
    {
        raan = atan2(k, h);
        trueAnom = L - raan;
        if (e <= 0.0)
        {
            argper = 0.0;
            //			cout<<"偏心率接近0,近星点幅角容易奇异.在此将其置为零."<<endl;
        }
        else
        {
            argper = atan2(g * h - f * k, f * h + g * k);
            trueAnom = trueAnom - argper;
        }
    }
    //转换到[0,2pi)中
    raan = mod(raan, PI2);
    argper = mod(argper, PI2);
    trueAnom = mod(trueAnom, PI2);
    if (e >= 1.0)
    {
        if (trueAnom > PI - acos(1.0 / e))
            trueAnom -= PI2;
        else if (trueAnom < -PI + acos(1.0 / e))
            trueAnom += PI2;
    }
    if (fabs(e - 1.0) < 1e-15) {
        return eErrorInvalidParam;
    }
    return eNoError;
}

void rv2ee(const double* pos, const double* vel, double gm, double* ee)
{
    //double& sma = coe[0], & eccm = coe[1], & inc = coe[2], & raan = coe[3], & argper = coe[4], & tanom = coe[5];
    double& sma = ee[0], & h = ee[1], & xk = ee[2], & p = ee[3], & q = ee[4], & meanlon = ee[5];

    const array3d& r = *reinterpret_cast<const array3d*>(pos);
    const array3d& v = *reinterpret_cast<const array3d*>(vel);

    // position and velocity magnitude
    double rmag = norm(r);
    double vmag = norm(v);

    // position unit vector
    auto rhat = r / rmag;

    // angular momentum vectors
    auto hv = cross(r, v);
    auto hhat = normalized(hv);

    // eccentricity vector
    auto vtmp = v / gm;
    auto ecc = cross(vtmp, hv) - rhat;

    // semimajor axis
    sma = 1.0 / (2.0 / rmag - vmag * vmag / gm);  // 如果是抛物线, a -> nan

    p = hhat[0] / (1.0 + hhat[2]);
    q = -hhat[1] / (1.0 + hhat[2]);

    double const1 = 1.0 / (1.0 + p * p + q * q);

    array3d fhat, ghat;
    fhat[0] = const1 * (1.0 - p * p + q * q);
    fhat[1] = const1 * 2.0 * p * q;
    fhat[2] = -const1 * 2.0 * p;

    ghat[0] = const1 * 2.0 * p * q;
    ghat[1] = const1 * (1.0 + p * p - q * q);
    ghat[2] = const1 * 2.0 * q;

    h = dot(ecc, ghat);
    xk = dot(ecc, fhat);
    double x1 = dot(r, fhat);
    double y1 = dot(r, ghat);

    // orbital eccentricity
    double eccm = sqrt(h * h + xk * xk);
    // orbital inclination
    double inc = 2.0 * atan(sqrt(p * p + q * q));
    // true longitude
    double xlambdat = atan2(y1, x1);
    // check for equatorial orbit
    double raan, argper;
    if (inc > 0.00000001)
        raan = atan2(p, q);
    else
        raan = 0.0;
    // check for circular orbit
    if (eccm > 0.00000001)
        argper = atan2(h, xk) - raan;
    else
        argper = 0.0;
    // true anomaly
    double tanom = xlambdat - raan - argper;
    double manom = aTrueToMean(tanom, eccm);
    meanlon = mod(manom + argper + raan, PI2);
}

errc_t rv2moe(const double* pos, const double* vel, double gm, double* moe)
{
    double& perirad = moe[0], & eccm = moe[1], & inc = moe[2], & raan = moe[3], & argper = moe[4], & tanom = moe[5];
    const array3d& r = *reinterpret_cast<const array3d*>(pos);
    const array3d& v = *reinterpret_cast<const array3d*>(vel);

    // position and velocity magnitude
    double rmag = norm(r);
    // double vmag = norm(v);

    // position unit vector
    auto rhat = r / rmag;

    // angular momentum vectors
    auto hv = cross(r, v);
    auto hmag = norm(hv);
    auto hhat = hv / hmag;

    // eccentricity vector
    auto vtmp = v / gm;
    auto ecc = cross(vtmp, hv) - rhat;

    double p = hhat[0] / (1.0 + hhat[2]);
    double q = -hhat[1] / (1.0 + hhat[2]);

    double const1 = 1.0 / (1.0 + p * p + q * q);

    array3d fhat, ghat;
    fhat[0] = const1 * (1.0 - p * p + q * q);
    fhat[1] = const1 * 2.0 * p * q;
    fhat[2] = -const1 * 2.0 * p;

    ghat[0] = const1 * 2.0 * p * q;
    ghat[1] = const1 * (1.0 + p * p - q * q);
    ghat[2] = const1 * 2.0 * q;

    double h = dot(ecc, ghat);
    double xk = dot(ecc, fhat);
    double x1 = dot(r, fhat);
    double y1 = dot(r, ghat);

    // orbital eccentricity
    eccm = sqrt(h * h + xk * xk);
    // perigee radius
    perirad = hmag * hmag / gm / (1 + eccm);      // perirad = p/(1+e) = a (1-e)
    // orbital inclination
    inc = 2.0 * atan(sqrt(p * p + q * q));
    // true longitude
    double xlambdat = atan2(y1, x1);
    // check for equatorial orbit
    if (inc > 0.00000001)
        raan = mod(atan2(p, q), PI2);
    else
        raan = 0.0;
    // check for circular orbit
    if (eccm > 0.00000001)
        argper = mod(atan2(h, xk) - raan, PI2);
    else
        argper = 0.0;
    // true anomaly
    tanom = mod(xlambdat - raan - argper, PI2);
    return eNoError;
}

errc_t rv2coe(const double* pos, const double* vel, double gm, double* coe)
{

    /*
      // ee
      double	SMajAx;	    ///< semimajor axis length
      double	h;		    ///< e*sin(argper + raan)   omegabar=argper + raan
      double	k;		    ///< e*cos(argper + raan)
      double	p;		    ///< tan(i/2)*sin(raan)
      double	q;		    ///< tan(i/2)*cos(raan)
      double	MeanLon;	///< mean longitude = M + raan + argper
      bool	    Dir;		///< Retrograde=false, Posigrade=true
      // mee
      double p;             // p = a(1-e^2)
      double f;             // f = e*cos(argper+RAAN)
      double g;             // g = e*sin(argper+RAAN)
      double h;             // h = tan(i/2)cos(RAAN)
      double k;	            // g = tan(i/2)sin(RAAN)
      double L;	            // L = RAAN + argper + f
     */
    const array3d& r = *reinterpret_cast<const array3d*>(pos);
    const array3d& v = *reinterpret_cast<const array3d*>(vel);

    double& sma = coe[0], & eccm = coe[1], & inc = coe[2], & raan = coe[3], & argper = coe[4], & tanom = coe[5];

    // position and velocity magnitude
    double rmag = norm(r);
    double vmag = norm(v);

    // position unit vector
    auto rhat = r / rmag;   // rhat = r / rmag;

    // angular momentum vectors
    auto hv = cross(r, v);  //hv = cross(r, v);
    auto hhat = hv / norm(hv); // hhat = hv / norm(hv);

    // eccentricity vector
    auto vtmp = v / gm; // vtmp = v / gm;
    auto ecc = cross(vtmp, hv) - rhat; // ecc = vtmp x hv - rhat;

    // semimajor axis
    sma = 1.0 / (2.0 / rmag - vmag * vmag / gm);

    double p = hhat[0] / (1.0 + hhat[2]);
    double q = -hhat[1] / (1.0 + hhat[2]);

    double const1 = 1.0 / (1.0 + p * p + q * q);

    double fhat[3], ghat[3];
    fhat[0] = const1 * (1.0 - p * p + q * q);
    fhat[1] = const1 * 2.0 * p * q;
    fhat[2] = -const1 * 2.0 * p;

    ghat[0] = const1 * 2.0 * p * q;
    ghat[1] = const1 * (1.0 + p * p - q * q);
    ghat[2] = const1 * 2.0 * q;

    double h = dot(ecc, ghat);
    double xk = dot(ecc, fhat);  // ee.k
    double x1 = dot(r, fhat);
    double y1 = dot(r, ghat);

    // orbital eccentricity
    eccm = sqrt(h * h + xk * xk);
    // orbital inclination
    inc = 2.0 * atan(sqrt(p * p + q * q));
    // true longitude
    double xlambdat = atan2(y1, x1);
    // check for equatorial orbit
    if (inc > 0.00000001)
        raan = mod(atan2(p, q), PI2);
    else
        raan = 0.0;
    // check for circular orbit
    if (eccm > 0.00000001)
        argper = mod(atan2(h, xk) - raan, PI2);

    else
        argper = 0.0;
    // true anomaly
    tanom = mod(xlambdat - raan - argper, PI2);
    return eNoError;
}

errc_t ee2moe(const double* ee, double* moe)
{
    double& perirad = moe[0], & ecc = moe[1], & inc = moe[2], & raan = moe[3], & argper = moe[4], & tanom = moe[5];
    double sma = ee[0], h = ee[1], k = ee[2], p = ee[3], q = ee[4], meanlon = ee[5];
    ecc = sqrt(h * h + k * k);
    perirad = sma * (1 - ecc);        // @todo: a<0, 同时e>1如何处理？
    /*
     // ee
     double	SMajAx;	    ///< semimajor axis length
     double	h;		    ///< e*sin(argper + raan)   omegabar=argper + raan
     double	k;		    ///< e*cos(argper + raan)
     double	p;		    ///< tan(i/2)*sin(raan)
     double	q;		    ///< tan(i/2)*cos(raan)
     double	MeanLon;	///< mean longitude = M + raan + argper
     bool	Dir;		///< Retrograde=false, Posigrade=true
     // mee
     double p;          // p = a(1-e^2)
     double f;          // f = e*cos(argper+RAAN)
     double g;          // g = e*sin(argper+RAAN)
     double h;          // h = tan(i/2)cos(RAAN)
     double k;	        // g = tan(i/2)sin(RAAN)
     double L;	        // L = RAAN + argper + f
    */

    double temp = sqrt(p * p + q * q);
    inc = 2.0 * atan(temp);
    if (temp <= 0.0)  // inc == 0
    {
        raan = 0.0;//升交点赤经
        //  cout<<"轨道倾角接近0或180度,升交点赤经容易奇异.在此将其置为零."<<endl;
        double manom;
        if (ecc <= 0.0) // ecc == 0
        {
            argper = 0.0;//近星点幅角
            //	cout<<"偏心率接近0,近星点幅角容易奇异.在此将其置为零."<<endl;      
            manom = meanlon;
        }
        else
        {
            argper = atan2(h, k); //近星点幅角       
            manom = meanlon - argper;
        }
        tanom = aMeanToTrue(manom, ecc);   //真近点角
    }
    else  // inc != 0
    {
        raan = atan2(p, q);
        double manom = meanlon - raan;  // mean lat
        if (ecc <= 0.0)
        {
            argper = 0.0;
            // cout<<"偏心率接近0,近星点幅角容易奇异.在此将其置为零."<<endl;
        }
        else
        {
            argper = atan2(h * q - k * p, k * q + h * p);
            manom = manom - argper;
        }
        tanom = aMeanToTrue(manom, ecc);   //真近点角
    }
    //转换到[0,2pi)中
    raan = mod(raan, PI2);
    argper = mod(argper, PI2);
    tanom = mod(tanom, PI2);
    if (ecc >= 1.0)
    {
        if (tanom > PI - acos(1.0 / ecc))
            tanom -= PI2;
        else if (tanom < -PI + acos(1.0 / ecc))
            tanom += PI2;
    }
    if (ecc == 1) {
        return eErrorInvalidParam;
    }
    return eNoError;
}

errc_t moe2ee(const double* moe, double* ee)
{
    /*
     double	SMajAx;	    ///< semimajor axis length
     double	h;		    ///< e*sin(argper+-raan)   omegabar=argper+-raan
     double	k;		    ///< e*cos(argper+-raan)
     double	p;		    ///< tan(i/2)*sin(raan)
     double	q;		    ///< tan(i/2)*cos(raan)
     double	MeanLon;	///< mean longitude = M + raan + argper
     bool	Dir;		///< Retrograde=false, Posigrade=true

     double p;          // p = a(1-e^2)
     double f;          // f = e*cos(argper+RAAN)
     double g;          // g = e*sin(argper+RAAN)
     double h;          // h = tan(i/2)cos(RAAN)
     double k;	        // g = tan(i/2)sin(RAAN)
     double L;	        // L = RAAN + argper + f
    */
    double perirad = moe[0], ecc = moe[1], inc = moe[2], raan = moe[3], argper = moe[4], tanom = moe[5];
    double& sma = ee[0], & h = ee[1], & k = ee[2], & p = ee[3], & q = ee[4], & meanlon = ee[5];
    if (inc == PI) {
        return eErrorInvalidParam;
    }
    sma = perirad / (1 - ecc);
    double lonper = raan + argper;
    h = ecc * sin(lonper);
    k = ecc * cos(lonper);
    double temp = tan(inc / 2);
    p = temp * sin(raan);
    q = temp * cos(raan);
    double manom = aTrueToMean(tanom, ecc);
    meanlon = mod(raan + argper + manom, PI2);
    return eNoError;
}
errc_t moe2coe(const double* moe_, double* coe_)
{
    const array6d& moe = (const array6d&) *moe_;
    array6d& coe = (array6d&)*coe_;

    double perirad = moe[0], ecc = moe[1]/*, inc = moe[2], raan = moe[3], argper = moe[4], tanom = moe[5]*/;
    double& a = coe[0];
    if(ecc == 1)
        return eErrorInvalidParam; //抛物线轨道
    coe = moe;
    a = perirad / (1 - ecc);
    return eNoError;
}
void coe2moe(const double* coe_, double* moe_)
{
    array6d& moe = (array6d&)*moe_;
    const array6d& coe = (const array6d&)*coe_;

    const double a = coe[0], e = coe[1]/*, i = coe[2], raan = coe[3], argper = coe[4], trueAnom = coe[5]*/;
    double& perirad = moe[0];
    moe = coe;
    perirad = a * (1 - e);
}

errc_t moe2rv(const double* moe, double gm, double* pos, double* vel)
{
    double perirad = moe[0], ecc = moe[1], inc = moe[2], raan = moe[3], argper = moe[4], tanom = moe[5];

    if(ecc == 1)
        return eErrorInvalidParam;
    double slr = perirad * (1 + ecc);         // p  semi-latus rectum  // perirad = p/(1+e) = a (1-e)
    double rm = slr / (1 + ecc * cos(tanom));  // 当前距离
    double arglat = argper + tanom;           // 纬度辐角
    double sarglat = sin(arglat);
    double carglat = cos(arglat);

    double c4 = sqrt(gm / slr);
    double c5 = ecc * cos(argper) + carglat;
    double c6 = ecc * sin(argper) + sarglat;

    double sinc = sin(inc);
    double cinc = cos(inc);

    double sraan = sin(raan);
    double craan = cos(raan);

    // position vector
    pos[0] = rm * (craan * carglat - sraan * cinc * sarglat);
    pos[1] = rm * (sraan * carglat + cinc * sarglat * craan);
    pos[2] = rm * sinc * sarglat;

    // velocity vector
    vel[0] = -c4 * (craan * c6 + sraan * cinc * c5);
    vel[1] = -c4 * (sraan * c6 - craan * cinc * c5);
    vel[2] = c4 * c5 * sinc;
    return eNoError;
}
void moe2mee(const double* moe, double* mee)
{
    const double perirad = moe[0], ecc = moe[1], inc = moe[2], raan = moe[3], argper = moe[4], tanom = moe[5];
    double& p = mee[0], & f = mee[1], & g = mee[2], & h = mee[3], & k = mee[4], & L = mee[5];
    // if ( e * f < -1.0)
   //     return false;
    p = perirad * (1.0 + ecc);          // 半通径p
    // if (e == 1.0)                   // 抛物线轨道 
    //     p = 2.0 * a;                // 对于抛物线根数a用perirad代替?
    double perlon = raan + argper;
    f = ecc * cos(perlon);               // f
    g = ecc * sin(perlon);               // g
    double temp = tan(inc / 2.0);
    h = temp * cos(raan);              // h
    k = temp * sin(raan);              // k
    L = mod(raan + argper + tanom, PI2);
}
void coe2ee(const double* coe, double* ee)
{
    double a = coe[0], e = coe[1], i = coe[2], raan = coe[3], argper = coe[4], trueAnom = coe[5];
    double& sma = ee[0], & h = ee[1], & k = ee[2], & p = ee[3], & q = ee[4], & meanlon = ee[5];

    // if ( e * f < -1.0)
    //     return false;
    sma = a;
    double perlon = raan + argper;
    h = e * sin(perlon);               // f
    k = e * cos(perlon);               // g
    double temp = tan(i / 2.0);
    p = temp * sin(raan);              // h
    q = temp * cos(raan);              // k
    double manom = aTrueToMean(trueAnom, e);
    meanlon = mod(raan + argper + manom, PI2);
}

void ee2coe(const double* ee, double* coe)
{
    double& a = coe[0], & ecc = coe[1], & inc = coe[2], & raan = coe[3], & argper = coe[4], & tanom = coe[5];
    double sma = ee[0], h = ee[1], k = ee[2], p = ee[3], q = ee[4], meanlon = ee[5];
    ecc = sqrt(h * h + k * k);
    a = sma;
    /*
     // ee
     double	m_SMajAx;	///< semimajor axis length
     double	m_h;		///< e*sin(argper + raan)   omegabar=argper + raan
     double	m_k;		///< e*cos(argper + raan)
     double	m_p;		///< tan(i/2)*sin(raan)
     double	m_q;		///< tan(i/2)*cos(raan)
     double	m_MeanLon;	///< mean longitude = M + raan + argper
     bool	m_Dir;		///< Retrograde=false, Posigrade=true
     // mee
     double m_p;        // p = a(1-e^2)
     double m_f;        // f = e*cos(argper+RAAN)
     double m_g;        // g = e*sin(argper+RAAN)
     double m_h;        // h = tan(i/2)cos(RAAN)
     double m_k;	    // g = tan(i/2)sin(RAAN)
     double m_L;	    // L = RAAN + argper + f
    */

    double temp = sqrt(p * p + q * q);
    inc = 2.0 * atan(temp);
    if (temp <= 0.0)  // inc == 0
    {
        raan = 0.0;//升交点赤经
        //  cout<<"轨道倾角接近0或180度,升交点赤经容易奇异.在此将其置为零."<<endl;
        double manom;
        if (ecc <= 0.0) // ecc == 0
        {
            argper = 0.0;//近星点幅角
            //	cout<<"偏心率接近0,近星点幅角容易奇异.在此将其置为零."<<endl;      
            manom = meanlon;
        }
        else
        {
            argper = atan2(h, k); //近星点幅角       
            manom = meanlon - argper;
        }
        tanom = aMeanToTrue(manom, ecc);   //真近点角
    }
    else  // inc != 0
    {
        raan = atan2(p, q);
        double manom = meanlon - raan;  // mean lat
        if (ecc <= 0.0)
        {
            argper = 0.0;
            // cout<<"偏心率接近0,近星点幅角容易奇异.在此将其置为零."<<endl;
        }
        else
        {
            argper = atan2(h * q - k * p, k * q + h * p);
            manom = manom - argper;
        }
        tanom = aMeanToTrue(manom, ecc);   //真近点角
    }
    //转换到[0,2pi)中
    raan = mod(raan, PI2);
    argper = mod(argper, PI2);
    tanom = mod(tanom, PI2);
    if (ecc >= 1.0)
    {
        if (tanom > PI - acos(1.0 / ecc))
            tanom -= PI2;
        else if (tanom < -PI + acos(1.0 / ecc))
            tanom += PI2;
    }
}

void ee2mee(const double* ee, double* mee)
{
    const double sma = ee[0], ee_h = ee[1], ee_k = ee[2], ee_p = ee[3], ee_q = ee[4], meanlon = ee[5];
    double& mee_p = mee[0], & mee_f = mee[1], & mee_g = mee[2], & mee_h = mee[3], & mee_k = mee[4], & L = mee[5];
    /*
    // ee
    double	m_SMajAx;	///< semimajor axis length
    double	m_h;		///< e*sin(argper + raan)   omegabar=argper + raan
    double	m_k;		///< e*cos(argper + raan)
    double	m_p;		///< tan(i/2)*sin(raan)
    double	m_q;		///< tan(i/2)*cos(raan)
    double	m_MeanLon;	///< mean longitude = M + raan + argper
    bool	m_Dir;		///< Retrograde=false, Posigrade=true
    // mee
    double m_p;        // p = a(1-e^2)
    double m_f;        // f = e*cos(argper+RAAN)
    double m_g;        // g = e*sin(argper+RAAN)
    double m_h;        // h = tan(i/2)cos(RAAN)
    double m_k;	    // g = tan(i/2)sin(RAAN)
    double m_L;	    // L = RAAN + argper + f
   */
    double ecc = sqrt(ee_h * ee_h + ee_k * ee_k);
    mee_p = sma * (1 - ecc * ecc);
    mee_f = ee_k;
    mee_g = ee_h;
    mee_h = ee_q;
    mee_k = ee_p;
    if (ecc == 0) {
        L = meanlon;
    }
    else {
        double perlon = atan2(ee_h, ee_k);
        L = mod(aMeanToTrue(meanlon - perlon, ecc) + perlon, PI2);
    }
}

void mee2ee(const double* mee, double* ee)
{
    const double mee_p = mee[0], mee_f = mee[1], mee_g = mee[2], mee_h = mee[3], mee_k = mee[4], L = mee[5];
    double& sma = ee[0], & ee_h = ee[1], & ee_k = ee[2], & ee_p = ee[3], & ee_q = ee[4], & meanlon = ee[5];
    double ecc = sqrt(mee_f * mee_f + mee_g * mee_g);
    sma = mee_p / (1 - ecc * ecc);
    ee_h = mee_g;
    ee_k = mee_f;
    ee_p = mee_k;
    ee_q = mee_h;
    if (ecc == 0) {
        meanlon = L;
    }
    else {
        double perlon = atan2(mee_g, mee_f);
        meanlon = mod(aTrueToMean(L - perlon, ecc) + perlon, PI2);
    }
}

void mee2moe(const double* mee, double* moe)
{
    const double p = mee[0], f = mee[1], g = mee[2], h = mee[3], k = mee[4], L = mee[5];
    double& perirad = moe[0], & eccm = moe[1], & inc = moe[2], & raan = moe[3], & argper = moe[4], & tanom = moe[5];
    eccm = sqrt(f * f + g * g);
    //if (e == 1.0)
    //    a = 0.5 * p;                 // p: 抛物线轨道的半通径
    //else
    perirad = p / (1 + eccm);   //半长轴
    double temp = sqrt(h * h + k * k);
    inc = 2.0 * atan(temp);
    if (temp <= 0.0)
    {
        raan = 0.0;//升交点赤经
        //    cout<<"轨道倾角接近0或180度,升交点赤经容易奇异.在此将其置为零."<<endl;
        if (eccm <= 0.0)
        {
            argper = 0.0;//近星点幅角
            //	cout<<"偏心率接近0,近星点幅角容易奇异.在此将其置为零."<<endl;        
            tanom = L;  //真近点角
        }
        else
        {
            argper = atan2(g, f); //近星点幅角       
            tanom = L - argper;
        }
    }
    else
    {
        raan = atan2(k, h);
        tanom = L - raan;
        if (eccm <= 0.0)
        {
            argper = 0.0;
            //			cout<<"偏心率接近0,近星点幅角容易奇异.在此将其置为零."<<endl;
        }
        else
        {
            argper = atan2(g * h - f * k, f * h + g * k);
            tanom = tanom - argper;
        }
    }
    //转换到[0,2pi)中
    raan = mod(raan, PI2);
    argper = mod(argper, PI2);
    tanom = mod(tanom, PI2);
    if (eccm >= 1.0)
    {
        if (tanom > PI - acos(1.0 / eccm))
            tanom -= PI2;
        else if (tanom < -PI + acos(1.0 / eccm))
            tanom += PI2;
    }
}

errc_t coe2dela(const double *coeIn, double gm, double *delaOut)
{
    const OrbElem* keplerian = (const OrbElem*)coeIn;
    DelaunayElem* dela = (DelaunayElem*)delaOut;
	double sma= keplerian->a();
	double ecc= keplerian->e();
	double inc= keplerian->i();
	double ta= keplerian->trueA();
    if(ecc < 0.0)
    {
        aWarning(_("ecc 小于 0.0，不支持"));
        ecc = -ecc;
    }
    if ((sma > 0.0) && (ecc > 1.0))
    {
        aWarning(_("sma 大于 0 而 ecc 大于 1.0，不支持"));
        sma = -sma;
    }
   
    if ((sma < 0.0) && (ecc < 1.0))
    {
        aWarning(_("sma 小于 0 而 ecc 小于 1.0，不支持"));
        sma = -sma;
    }
   
    if (gm < 1e-15)
    {
        aWarning(_("引力常数 (gm=%e) 太小，无法从 Keplerian 转换到 Delaunay"), gm);
        return eErrorInvalidParam;
    }
   
    if ( ecc >= 1.0)
    {
        aWarning(_("ecc 不小于 1.0，不支持"));
        return eErrorInvalidParam;
    }
   
    double L_dela= sqrt(gm * sma);
    double G_dela= L_dela * sqrt(1 - ecc*ecc);
    double H_dela= G_dela * cos(inc);
    double ll_dela= aTrueToMean(ta, ecc);
    double gg_dela= keplerian->argper();
    double hh_dela= keplerian->raan();

    dela->L() = L_dela;
    dela->G() = G_dela;
    dela->H() = H_dela;
    dela->l() = ll_dela;
    dela->g() = gg_dela;
    dela->h() = hh_dela;
   
    return eNoError;
}

errc_t dela2coe(const double *delaIn, double gm, double *coeOut)
{
    const DelaunayElem* dela = (const DelaunayElem*)delaIn;
    OrbElem* coe = (OrbElem*) coeOut;
    
    double L_dela= dela->L();
    double G_dela= dela->G();
    double H_dela= dela->H();
    double ll_dela= dela->l();
    
    if (std::abs(H_dela) > std::abs(G_dela))
    {
        aWarning(_("DelaunayH 的模必须小于或等于 DelaunayG 的模"));
        return eErrorInvalidParam;
    }
    if ((G_dela / L_dela) > 1.0)
    {
        aWarning(_("要求 (DelaunayG / DelaunayL) 不大于 1"));
        return eErrorInvalidParam;
    }
 
    double GdL = G_dela / L_dela;
    double sma= L_dela*L_dela / gm;
    double ecc= sqrt ( 1 - GdL*GdL );
    double inc= acos(H_dela / G_dela) ;
    double aop= dela->g();
    double raan= dela->h();
    double ta= aMeanToTrue(ll_dela, ecc) ;

    coe->a() = sma;
    coe->e() = ecc;
    coe->i() = inc;
    coe->argper() = aop;
    coe->raan() = raan;
    coe->trueA() = ta;

    return eNoError;
}

void aModEquinElemToCart(
    const ModEquinElem& mee,
    double gm,
    Vector3d& pos,
    Vector3d& vel
)
{
    return mee2rv(mee.data(), gm, pos.data(), vel.data());
}



errc_t aCartToModEquinElem(
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    ModEquinElem& mee
)
{
    return rv2mee(pos.data(), vel.data(), gm, mee.data());
}



errc_t aOrbElemToModEquinElem(
    const OrbElem& elem,
    ModEquinElem& mee
)
{
    return coe2mee(elem.data(), mee.data());
}


errc_t aModEquinElemToOrbElem(
    const ModEquinElem& mee,
    OrbElem& elem
)
{
    return mee2coe(mee.data(), elem.data());
}


errc_t	aCartToModOrbElem(
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    ModOrbElem& modOrb)
{
    return rv2moe(pos.data(), vel.data(), gm, modOrb.data());
}



errc_t aCartToOrbElem     (
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    OrbElem& elem)
{
    return rv2coe(pos.data(), vel.data(), gm, elem.data());
}




errc_t aEquinElemToModOrb (
    const EquinElem& equinElem,
    ModOrbElem& modOrb)
{
    return ee2moe(equinElem.data(), modOrb.data());
}



errc_t aModOrbToEquinElem (
    const ModOrbElem& modOrb,
    EquinElem& equinElem)
{
    return moe2ee(modOrb.data(), equinElem.data());
}



errc_t aModOrbElemToCart  (
    const ModOrbElem& modOrb,
    double gm,
    Vector3d& pos,
    Vector3d& vel)
{
    return moe2rv(modOrb.data(), gm, pos.data(), vel.data());
}



errc_t aOrbElemToCart     (
    const OrbElem& elem,
    double gm,
    Vector3d& pos,
    Vector3d& vel)
{
    return coe2rv(elem.data(), gm, pos.data(), vel.data());
}



void	aCartToEquinElem(
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    EquinElem& equinElem)
{
    return rv2ee(pos.data(), vel.data(), gm, equinElem.data());
}


void aEquinElemToCart(
    const EquinElem& equinElem,
    double gm,
    Vector3d& pos,
    Vector3d& vel)
{
    return ee2rv(equinElem.data(), gm, pos.data(), vel.data());
}

errc_t aOrbElemToDelaunay(const OrbElem &elem, double gm, DelaunayElem &delaunay)
{
    return coe2dela(elem.data(), gm, delaunay.data());
}

errc_t aDelaunayToOrbElem(const DelaunayElem &delaunay, double gm, OrbElem &elem)
{
    return dela2coe(delaunay.data(), gm, elem.data());
}


// -------------
// B平面相关计算
// -------------


/// 偏心率小于该值时认为轨道为圆轨道, 此时偏心率矢量的方向不再有意义
constexpr double kEccCircularTol = 1e-6;

/// 矢量模/矢量间夹角正弦的判定阈值
constexpr double kMagTol = 1e-12;

std::string BPlaneElem::toString() const
{
    return std::string(
        "BPlaneElem{ra: " + aFormatDouble(ra_ * kRadToDeg) + "deg" +
        ", dec: " + aFormatDouble(dec_ * kRadToDeg) + "deg" +
        ", B.R: " + aFormatDouble(bDotR_) + "m" +
        ", B.T: " + aFormatDouble(bDotT_) + "m" +
        ", C3: " + aFormatDouble(c3_) + "m^2/s^2" +
        ", true: " + aFormatDouble(trueA_ * kRadToDeg) + "deg" +
        "}");
}

/// 取与给定法向垂直的一个单位矢量, 优先取+x轴, 退化时取+y轴
static Vector3d aPerpendicularPlusX(const Vector3d& hhat)
{
    Vector3d xhat{1.0, 0.0, 0.0};
    auto xv = xhat - dot(xhat, hhat) * hhat;
    double xmag = norm(xv);
    if (xmag > kMagTol)
        return xv / xmag;
    Vector3d yhat{0.0, 1.0, 0.0};
    auto yv = yhat - dot(yhat, hhat) * hhat;
    double ymag = norm(yv);
    if (ymag > kMagTol)
        return yv / ymag;
    aWarning(_("无法确定与轨道面法向垂直的矢量"));
    return xhat;
}

/// 计算B平面内的R、T轴: T = unit(S x N), R = S x T
/// @param shat 入渐近线方向(单位矢量)
/// @param refVector 参考向量
/// @param rhat 输出R轴(单位矢量)
/// @param that 输出T轴(单位矢量)
/// @return 错误码，成功返回eNoError
static errc_t aBPlaneRT(const Vector3d& shat, const Vector3d& refVector, Vector3d& rhat, Vector3d& that)
{
    double nmag = norm(refVector);
    AST_CHECK_INVALID(nmag <= 0);

    auto nhat = refVector / nmag;
    auto tv = cross(shat, nhat);
    double tmag = norm(tv);
    if (tmag <= kMagTol)            // 参考向量与入渐近线平行时, T轴无定义
    {
        aWarning(_("参考向量与入渐近线平行, 无法确定B平面的R、T轴"));
        return eErrorInvalidParam;
    }
    that = tv / tmag;
    rhat = cross(shat, that);
    return eNoError;
}

errc_t aCartToBPlane(
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    const Vector3d& refVector,
    BPlaneElem& bPlane)
{
    double rmag = norm(pos);
    double vmag = norm(vel);
    AST_CHECK_INVALID(rmag <= 0 || gm <= 0);

    // 角动量矢量(轨道面法向)
    auto hv = cross(pos, vel);
    double hmag = norm(hv);
    if (hmag <= 0)                  // 直线运动, 轨道面不确定
    {
        aWarning(_("角动量为零, 无法确定轨道面"));
        return eErrorInvalidParam;
    }
    auto hhat = hv / hmag;

    // 偏心率矢量
    auto evec = cross(vel / gm, hv) - pos / rmag;
    double ecc = norm(evec);

    // 圆轨道(偏心率数值上为零)时偏心率矢量方向无意义, 取与轨道面垂直的+x方向
    auto ehat = (ecc > kEccCircularTol) ? evec / ecc : aPerpendicularPlusX(hhat);

    // 半长轴与特征能量
    double sma = 1.0 / (2.0 / rmag - vmag * vmag / gm);
    bPlane.c3_ = vmag * vmag - 2.0 * gm / rmag;

    // 半短轴, 即B矢量的模: 双曲线为 |a|*sqrt(e^2-1), 椭圆为 a*sqrt(1-e^2)
    double bmag = fabs(sma) * sqrt(fabs(ecc * ecc - 1.0));

    // 半张角: cos(alpha) = 1/e, 椭圆(e<1)时按0处理
    double alpha = (ecc > 1.0) ? acos(1.0 / ecc) : 0.0;

    // 入渐近线方向, 即B-Plane Normal
    auto sv = cos(alpha) * ehat + sin(alpha) * cross(hhat, ehat);
    double smag = norm(sv);
    AST_CHECK_INVALID(smag <= 0);
    auto shat = sv / smag;

    bPlane.ra_ = atan2(shat[1], shat[0]);
    bPlane.dec_ = asin(shat[2] > 1.0 ? 1.0 : (shat[2] < -1.0 ? -1.0 : shat[2]));

    // B矢量: 位于B平面内, 垂直于入渐近线
    auto bvec = bmag * cross(shat, hhat);

    // R、T轴
    Vector3d rhat, that;
    errc_t rc = aBPlaneRT(shat, refVector, rhat, that);
    if (rc != eNoError)
        return rc;

    bPlane.bDotR_ = dot(bvec, rhat);
    bPlane.bDotT_ = dot(bvec, that);

    // 真近点角: 从偏心率矢量到位置矢量, 沿运动方向为正
    bPlane.trueA_ = atan2(dot(pos, cross(hhat, ehat)), dot(pos, ehat));
    return eNoError;
}

errc_t aBPlaneToCart(
    const BPlaneElem& bPlane,
    double gm,
    const Vector3d& refVector,
    Vector3d& pos,
    Vector3d& vel)
{
    AST_CHECK_INVALID(gm <= 0);

    // 入渐近线方向
    double sdec = sin(bPlane.dec_);
    Vector3d shat{cos(bPlane.dec_) * cos(bPlane.ra_), cos(bPlane.dec_) * sin(bPlane.ra_), sdec};
    double smag = norm(shat);
    AST_CHECK_INVALID(smag <= 0);
    shat = shat / smag;

    // B矢量模(半短轴)
    double bmag = hypot(bPlane.bDotR_, bPlane.bDotT_);
    AST_CHECK_INVALID(bmag <= 0);

    // 半长轴与偏心率: b = |a|*sqrt(|e^2-1|)
    double c3 = bPlane.c3_;
    AST_CHECK_INVALID(c3 == 0.0);   // 抛物线情形半长轴无穷, 暂不支持
    double sma = -gm / c3;
    double tmp = bmag / fabs(sma);
    double ecc2 = (c3 > 0) ? (1.0 + tmp * tmp) : (1.0 - tmp * tmp);
    if (ecc2 < 0)                   // 椭圆轨道下 |B| 不能大于半长轴
    {
        aWarning(_("B矢量模大于半长轴, 参数不合法"));
        return eErrorInvalidParam;
    }
    double ecc = sqrt(ecc2);

    // R、T轴
    Vector3d rhat, that;
    errc_t rc = aBPlaneRT(shat, refVector, rhat, that);
    if (rc != eNoError)
        return rc;

    // B矢量与轨道面法向: B = b*(S x h) => h = B x S
    auto bhat = (bPlane.bDotR_ * rhat + bPlane.bDotT_ * that) / bmag;
    auto hv = cross(bhat, shat);
    double hmag = norm(hv);
    if (hmag <= 0)
    {
        aWarning(_("B矢量为零, 无法确定轨道面"));
        return eErrorInvalidParam;
    }
    auto hhat = hv / hmag;

    // 偏心率矢量: S = cos(alpha)*e + sin(alpha)*(h x e)
    double alpha = (ecc > 1.0) ? acos(1.0 / ecc) : 0.0;
    auto ev = cos(alpha) * shat - sin(alpha) * cross(hhat, shat);
    double emag = norm(ev);
    AST_CHECK_INVALID(emag <= 0);
    auto ehat = ev / emag;
    auto qhat = cross(hhat, ehat);

    // 由真近点角给出位置和速度
    double slr = sma * (1.0 - ecc * ecc);   // 半通径 p = a(1-e^2)
    AST_CHECK_INVALID(slr <= 0);
    double nu = bPlane.trueA_;
    double rmag = slr / (1.0 + ecc * cos(nu));
    double hmom = sqrt(gm * slr);

    pos = rmag * (cos(nu) * ehat + sin(nu) * qhat);
    vel = (gm / hmom) * (-sin(nu) * ehat + (ecc + cos(nu)) * qhat);
    return eNoError;
}


// -------------
// 球坐标根数相关计算
// -------------

std::string SphericalElem::toString() const
{
    return std::string(
        "SphericalElem{ra: " + aFormatDouble(ra_ * kRadToDeg) + "deg" +
        ", dec: " + aFormatDouble(dec_ * kRadToDeg) + "deg" +
        ", r: " + aFormatDouble(r_) + "m" +
        ", fpa: " + aFormatDouble(fpa_ * kRadToDeg) + "deg" +
        ", azi: " + aFormatDouble(azi_ * kRadToDeg) + "deg" +
        ", v: " + aFormatDouble(v_) + "m/s" +
        "}");
}

/// 计算当地东、北单位矢量: 东 = (-sin(ra), cos(ra), 0), 北 = rhat x 东
/// @param ra 赤经 [rad]
/// @param rhat 位置方向单位矢量
/// @param ehat 输出当地东向单位矢量
/// @param nhat 输出当地北向单位矢量
static void aLocalEastNorth(double ra, const Vector3d& rhat, Vector3d& ehat, Vector3d& nhat)
{
    ehat = Vector3d{-sin(ra), cos(ra), 0.0};
    nhat = cross(rhat, ehat);
}

errc_t aCartToSpherical(
    const Vector3d& pos,
    const Vector3d& vel,
    SphericalElem& sph)
{
    double rmag = norm(pos);
    double vmag = norm(vel);
    if(rmag <= 0 || vmag <= 0)
    {
        sph = SphericalElem{};
        aWarning(_("位置或速度为零矢量, 无法确定赤经、赤纬、航迹角与航迹方位角"));
        return eErrorInvalidParam;
    }

    auto rhat = pos / rmag;

    // 赤经与赤纬: 极点处atan2(0, 0)的取值为0, 该取值是一个约定, 仍可精确往返
    double ra = atan2(pos[1], pos[0]);
    double dec = atan2(pos[2], hypot(pos[0], pos[1]));

    // 当地东、北单位矢量
    Vector3d ehat, nhat;
    aLocalEastNorth(ra, rhat, ehat, nhat);

    // 航迹角: 速度的径向分量与横向分量之比, 向上(远离中心天体)为正
    double vRadial = dot(vel, rhat);
    double vTransversal = norm(vel - vRadial * rhat);

    sph.ra_  = ra;
    sph.dec_ = dec;
    sph.r_   = rmag;
    sph.fpa_ = atan2(vRadial, vTransversal);
    // 航迹方位角: 自当地北向东为正
    sph.azi_ = atan2(dot(vel, ehat), dot(vel, nhat));
    sph.v_   = vmag;
    return eNoError;
}

errc_t aSphericalToCart(
    const SphericalElem& sph,
    Vector3d& pos,
    Vector3d& vel)
{
    if(sph.r_ <= 0 || sph.v_ < 0)
    {
        aWarning(_("地心距或速度大小为负, 无法确定位置和速度"));
        pos = Vector3d::Zero();
        vel = Vector3d::Zero();
        return eErrorInvalidParam;
    }

    double cosDec = cos(sph.dec_);
    Vector3d rhat{cosDec * cos(sph.ra_), cosDec * sin(sph.ra_), sin(sph.dec_)};

    // 当地东、北单位矢量
    Vector3d ehat, nhat;
    aLocalEastNorth(sph.ra_, rhat, ehat, nhat);

    // 速度: 径向分量 + 当地水平面内的横向分量(方位角自当地北向东量)
    Vector3d horizontal = cos(sph.azi_) * nhat + sin(sph.azi_) * ehat;

    pos = sph.r_ * rhat;
    vel = sph.v_ * (sin(sph.fpa_) * rhat + cos(sph.fpa_) * horizontal);
    return eNoError;
}


// -------------
// 混合球坐标根数相关计算
// -------------

std::string MixedSphericalElem::toString() const
{
    return std::string(
        "MixedSphericalElem{lon: " + aFormatDouble(lon_ * kRadToDeg) + "deg" +
        ", lat: " + aFormatDouble(lat_ * kRadToDeg) + "deg" +
        ", alt: " + aFormatDouble(alt_) + "m" +
        ", fpa: " + aFormatDouble(fpa_ * kRadToDeg) + "deg" +
        ", azi: " + aFormatDouble(azi_ * kRadToDeg) + "deg" +
        ", v: " + aFormatDouble(v_) + "m/s" +
        "}");
}

errc_t aCartToMixedSpherical(
    const Vector3d& pos,
    const Vector3d& vel,
    const Rotation& frameToFixed,
    const BodyShape& shape,
    MixedSphericalElem& mixedSph)
{
    double rmag = norm(pos);
    double vmag = norm(vel);
    if(rmag <= 0 || vmag <= 0)
    {
        mixedSph = MixedSphericalElem{};
        aWarning(_("位置或速度为零矢量, 无法确定经度、纬度、航迹角与航迹方位角"));
        return eErrorInvalidParam;
    }

    // 位置: 转换到天体固连系后反解大地坐标
    Vector3d posFixed = frameToFixed.transformVector(pos);
    GeodeticPoint detic;
    shape.transform(posFixed, detic);

    // 速度: 在参考系(惯性系)下分解, 与椭球和固连系无关
    // 当地东、北单位矢量由位置矢量在参考系下的赤经确定, 与 SphericalElem 的约定一致
    Vector3d rhat = pos / rmag;
    double ra = atan2(pos[1], pos[0]);
    Vector3d ehat, nhat;
    aLocalEastNorth(ra, rhat, ehat, nhat);

    double vRadial = dot(vel, rhat);
    double vTransversal = norm(vel - vRadial * rhat);

    mixedSph.lon_ = detic.longitude();
    mixedSph.lat_ = detic.latitude();
    mixedSph.alt_ = detic.altitude();
    // 航迹角: 速度的径向分量与横向分量之比, 向上(远离中心天体)为正
    mixedSph.fpa_ = atan2(vRadial, vTransversal);
    // 航迹方位角: 自当地北向东为正
    mixedSph.azi_ = atan2(dot(vel, ehat), dot(vel, nhat));
    mixedSph.v_   = vmag;
    return eNoError;
}

errc_t aMixedSphericalToCart(
    const MixedSphericalElem& mixedSph,
    const Rotation& fixedToFrame,
    const BodyShape& shape,
    Vector3d& pos,
    Vector3d& vel)
{
    if(mixedSph.v_ < 0)
    {
        aWarning(_("速度大小为负, 无法确定位置和速度"));
        pos = Vector3d::Zero();
        vel = Vector3d::Zero();
        return eErrorInvalidParam;
    }

    // 位置: 由大地坐标正算天体固连系位置
    GeodeticPoint detic(mixedSph.lat_, mixedSph.lon_, mixedSph.alt_);
    Vector3d posFixed;
    shape.transform(detic, posFixed);

    double rmag = norm(posFixed);
    if(rmag <= 0)
    {
        aWarning(_("位置退化到参考椭球中心, 无法确定航迹角与航迹方位角"));
        pos = Vector3d::Zero();
        vel = Vector3d::Zero();
        return eErrorInvalidParam;
    }

    // 位置换算到参考系, 速度在参考系(惯性系)下分解, 与椭球和固连系无关
    pos = fixedToFrame.transformVector(posFixed);
    Vector3d rhat = pos / rmag;
    double ra = atan2(pos[1], pos[0]);
    Vector3d ehat, nhat;
    aLocalEastNorth(ra, rhat, ehat, nhat);

    // 速度: 径向分量 + 当地水平面内的横向分量(方位角自当地北向东量)
    Vector3d horizontal = cos(mixedSph.azi_) * nhat + sin(mixedSph.azi_) * ehat;
    vel = mixedSph.v_ * (sin(mixedSph.fpa_) * rhat + cos(mixedSph.fpa_) * horizontal);
    return eNoError;
}


// -------------
// 大地坐标根数相关计算
// -------------

std::string GeodeticElem::toString() const
{
    return std::string(
        "GeodeticElem{lon: " + aFormatDouble(lon_ * kRadToDeg) + "deg" +
        ", lat: " + aFormatDouble(lat_ * kRadToDeg) + "deg" +
        ", alt: " + aFormatDouble(alt_) + "m" +
        ", lonRate: " + aFormatDouble(lonRate_ * kRadToDeg) + "deg/s" +
        ", latRate: " + aFormatDouble(latRate_ * kRadToDeg) + "deg/s" +
        ", altRate: " + aFormatDouble(altRate_) + "m/s" +
        "}");
}

errc_t aCartToGeodetic(
    const Vector3d& pos,
    const Vector3d& vel,
    const BodyShape& shape,
    GeodeticElem& geodetic)
{
    // 位置与速度同在天体固连系下, 由参考形状统一完成换算
    // 注意: 速度为零是合法的(如地球同步轨道), 此时三个变化率均为零, 故不校验速度
    GeodeticPoint detic;
    LatLonAlt rate;     // 三个分量依次为纬度率、经度率、高度率
    errc_t rc = shape.transform(pos, vel, detic, rate);
    if (rc != eNoError)
    {
        geodetic = GeodeticElem{};
        return rc;
    }

    geodetic.lon_     = detic.longitude();
    geodetic.lat_     = detic.latitude();
    geodetic.alt_     = detic.altitude();
    geodetic.lonRate_ = rate.longitude();
    geodetic.latRate_ = rate.latitude();
    geodetic.altRate_ = rate.altitude();
    return eNoError;
}

errc_t aGeodeticToCart(
    const GeodeticElem& geodetic,
    const BodyShape& shape,
    Vector3d& pos,
    Vector3d& vel)
{
    GeodeticPoint detic(geodetic.lat_, geodetic.lon_, geodetic.alt_);
    LatLonAlt rate;     // 三个分量依次为纬度率、经度率、高度率
    rate.latitude()  = geodetic.latRate_;
    rate.longitude() = geodetic.lonRate_;
    rate.altitude()  = geodetic.altRate_;
    errc_t rc = shape.transform(detic, rate, pos, vel);
    if (rc != eNoError)
    {
        pos = Vector3d::Zero();
        vel = Vector3d::Zero();
    }
    return rc;
}

AST_NAMESPACE_END

