///
/// @file      SOFA.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-06
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

#include "SOFA.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/Logger.hpp"
#include "AstCore/RunTime.hpp"
#include <math.h>

AST_NAMESPACE_BEGIN


#define DAS2R (kArcSecToRad)
#define DJC (kDaysPerJulianCentury)
#define DJ00 (kJ2000Epoch)
#define D2PI (kTwoPI)
#define DPI (kPI)
#define DAYSEC (kSecondsPerDay)
#define DS2R (kTimeSecToRad)
#define dsign(A,B) ((B)<0.0?-fabs(A):fabs(A))


static double astAnpm(double a)
{
    /*
    参考sofa库的 iauAnpm 函数
    根据sofa库的许可证要求，衍生作品不能使用iau或sofa前缀作为函数名，故更改了函数前缀
    */
    double w;
    w = fmod(a, D2PI);
    if (fabs(w) >= DPI) w -= dsign(D2PI, a);

    return w;
}


static double astAnp(double a)
{
    /*
    参考sofa库的 iauAnpm 函数
    根据sofa库的许可证要求，衍生作品不能使用iau或sofa前缀作为函数名，故更改了函数前缀
    */
    double w;

    w = fmod(a, D2PI);
    if (w < 0) w += D2PI;

    return w;
}

/// @brief IAU Resolution C7 
/// @param om 
/// @return 
inline double aIAUResolutionC7(double om)
{
    /*
    参考SOFA函数 iauEqeq94
    */

    return DAS2R*(0.00264*sin(om) + 0.000063*sin(om+om));
}


void aPrecession_IAU1976(double t, double &zeta, double &z, double &theta)
{

    /*
    参考sofa库的 iauPrec76 函数
    */

    double t0, tas2r, w;

/* Interval between fundamental epoch J2000.0 and start date (JC). */
    t0 = 0; // ((date01 - DJ00) + date02) / DJC;

/* Interval over which precession required (JC). */
    // t = tp.julianCenturyFromJ2000TT(); //((date11 - date01) + (date12 - date02)) / DJC;

/* Euler angles. */
    tas2r = t * DAS2R;
    w = 2306.2181 + (1.39656 - 0.000139 * t0) * t0;

    zeta = (w + ((0.30188 - 0.000344 * t0) + 0.017998 * t) * t) * tas2r;

    z = (w + ((1.09468 + 0.000066 * t0) + 0.018203 * t) * t) * tas2r;

    theta = ((2004.3109 + (-0.85330 - 0.000217 * t0) * t0)
          + ((-0.42665 - 0.000217 * t0) - 0.041833 * t) * t) * tas2r;
}

double aMeanObliquity_IAU1980(double t)
{
    /*
    参考sofa库的 iauObl80 函数
    */
   
    double eps0;

/* Interval between fundamental epoch J2000.0 and given date (JC). */
    // t = tp.julianCenturyFromJ2000TT(); //((date1 - DJ00) + date2) / DJC;

/* Mean obliquity of date. */
    eps0 = DAS2R * (84381.448  +
                  (-46.8150   +
                  (-0.00059   +
                  ( 0.001813) * t) * t) * t);

    return eps0;
}




void aNutation_IERS1996(double t, double &dpsi, double &deps, double* eqecorr)
{

    /*
    参考sofa库的 iauNut80 函数
    */
 
    double el, elp, f, d, om, dp, de, arg, s, c;
    int j;

/* Units of 0.1 milliarcsecond to radians */
    const double U2R = DAS2R / 1e4;

/* ------------------------------------------------ */
/* Table of multiples of arguments and coefficients */
/* ------------------------------------------------ */

/* The units for the sine and cosine coefficients are 0.1 mas and */
/* the same per Julian century */

    static const struct {
        int nl,nlp,nf,nd,nom; /* coefficients of l,l',F,D,Om */
        double sp,spt;        /* longitude sine, 1 and t coefficients */
        double ce,cet;        /* obliquity cosine, 1 and t coefficients */
    } x[] = {

    /* 1-10 */
      {  0,  0,  0,  0,  1, -171996.0, -174.2,  92025.0,    8.9 },
      {  0,  0,  0,  0,  2,    2062.0,    0.2,   -895.0,    0.5 },
      { -2,  0,  2,  0,  1,      46.0,    0.0,    -24.0,    0.0 },
      {  2,  0, -2,  0,  0,      11.0,    0.0,      0.0,    0.0 },
      { -2,  0,  2,  0,  2,      -3.0,    0.0,      1.0,    0.0 },
      {  1, -1,  0, -1,  0,      -3.0,    0.0,      0.0,    0.0 },
      {  0, -2,  2, -2,  1,      -2.0,    0.0,      1.0,    0.0 },
      {  2,  0, -2,  0,  1,       1.0,    0.0,      0.0,    0.0 },
      {  0,  0,  2, -2,  2,  -13187.0,   -1.6,   5736.0,   -3.1 },
      {  0,  1,  0,  0,  0,    1426.0,   -3.4,     54.0,   -0.1 },

    /* 11-20 */
      {  0,  1,  2, -2,  2,    -517.0,    1.2,    224.0,   -0.6 },
      {  0, -1,  2, -2,  2,     217.0,   -0.5,    -95.0,    0.3 },
      {  0,  0,  2, -2,  1,     129.0,    0.1,    -70.0,    0.0 },
      {  2,  0,  0, -2,  0,      48.0,    0.0,      1.0,    0.0 },
      {  0,  0,  2, -2,  0,     -22.0,    0.0,      0.0,    0.0 },
      {  0,  2,  0,  0,  0,      17.0,   -0.1,      0.0,    0.0 },
      {  0,  1,  0,  0,  1,     -15.0,    0.0,      9.0,    0.0 },
      {  0,  2,  2, -2,  2,     -16.0,    0.1,      7.0,    0.0 },
      {  0, -1,  0,  0,  1,     -12.0,    0.0,      6.0,    0.0 },
      { -2,  0,  0,  2,  1,      -6.0,    0.0,      3.0,    0.0 },

    /* 21-30 */
      {  0, -1,  2, -2,  1,      -5.0,    0.0,      3.0,    0.0 },
      {  2,  0,  0, -2,  1,       4.0,    0.0,     -2.0,    0.0 },
      {  0,  1,  2, -2,  1,       4.0,    0.0,     -2.0,    0.0 },
      {  1,  0,  0, -1,  0,      -4.0,    0.0,      0.0,    0.0 },
      {  2,  1,  0, -2,  0,       1.0,    0.0,      0.0,    0.0 },
      {  0,  0, -2,  2,  1,       1.0,    0.0,      0.0,    0.0 },
      {  0,  1, -2,  2,  0,      -1.0,    0.0,      0.0,    0.0 },
      {  0,  1,  0,  0,  2,       1.0,    0.0,      0.0,    0.0 },
      { -1,  0,  0,  1,  1,       1.0,    0.0,      0.0,    0.0 },
      {  0,  1,  2, -2,  0,      -1.0,    0.0,      0.0,    0.0 },

    /* 31-40 */
      {  0,  0,  2,  0,  2,   -2274.0,   -0.2,    977.0,   -0.5 },
      {  1,  0,  0,  0,  0,     712.0,    0.1,     -7.0,    0.0 },
      {  0,  0,  2,  0,  1,    -386.0,   -0.4,    200.0,    0.0 },
      {  1,  0,  2,  0,  2,    -301.0,    0.0,    129.0,   -0.1 },
      {  1,  0,  0, -2,  0,    -158.0,    0.0,     -1.0,    0.0 },
      { -1,  0,  2,  0,  2,     123.0,    0.0,    -53.0,    0.0 },
      {  0,  0,  0,  2,  0,      63.0,    0.0,     -2.0,    0.0 },
      {  1,  0,  0,  0,  1,      63.0,    0.1,    -33.0,    0.0 },
      { -1,  0,  0,  0,  1,     -58.0,   -0.1,     32.0,    0.0 },
      { -1,  0,  2,  2,  2,     -59.0,    0.0,     26.0,    0.0 },

    /* 41-50 */
      {  1,  0,  2,  0,  1,     -51.0,    0.0,     27.0,    0.0 },
      {  0,  0,  2,  2,  2,     -38.0,    0.0,     16.0,    0.0 },
      {  2,  0,  0,  0,  0,      29.0,    0.0,     -1.0,    0.0 },
      {  1,  0,  2, -2,  2,      29.0,    0.0,    -12.0,    0.0 },
      {  2,  0,  2,  0,  2,     -31.0,    0.0,     13.0,    0.0 },
      {  0,  0,  2,  0,  0,      26.0,    0.0,     -1.0,    0.0 },
      { -1,  0,  2,  0,  1,      21.0,    0.0,    -10.0,    0.0 },
      { -1,  0,  0,  2,  1,      16.0,    0.0,     -8.0,    0.0 },
      {  1,  0,  0, -2,  1,     -13.0,    0.0,      7.0,    0.0 },
      { -1,  0,  2,  2,  1,     -10.0,    0.0,      5.0,    0.0 },

    /* 51-60 */
      {  1,  1,  0, -2,  0,      -7.0,    0.0,      0.0,    0.0 },
      {  0,  1,  2,  0,  2,       7.0,    0.0,     -3.0,    0.0 },
      {  0, -1,  2,  0,  2,      -7.0,    0.0,      3.0,    0.0 },
      {  1,  0,  2,  2,  2,      -8.0,    0.0,      3.0,    0.0 },
      {  1,  0,  0,  2,  0,       6.0,    0.0,      0.0,    0.0 },
      {  2,  0,  2, -2,  2,       6.0,    0.0,     -3.0,    0.0 },
      {  0,  0,  0,  2,  1,      -6.0,    0.0,      3.0,    0.0 },
      {  0,  0,  2,  2,  1,      -7.0,    0.0,      3.0,    0.0 },
      {  1,  0,  2, -2,  1,       6.0,    0.0,     -3.0,    0.0 },
      {  0,  0,  0, -2,  1,      -5.0,    0.0,      3.0,    0.0 },

    /* 61-70 */
      {  1, -1,  0,  0,  0,       5.0,    0.0,      0.0,    0.0 },
      {  2,  0,  2,  0,  1,      -5.0,    0.0,      3.0,    0.0 },
      {  0,  1,  0, -2,  0,      -4.0,    0.0,      0.0,    0.0 },
      {  1,  0, -2,  0,  0,       4.0,    0.0,      0.0,    0.0 },
      {  0,  0,  0,  1,  0,      -4.0,    0.0,      0.0,    0.0 },
      {  1,  1,  0,  0,  0,      -3.0,    0.0,      0.0,    0.0 },
      {  1,  0,  2,  0,  0,       3.0,    0.0,      0.0,    0.0 },
      {  1, -1,  2,  0,  2,      -3.0,    0.0,      1.0,    0.0 },
      { -1, -1,  2,  2,  2,      -3.0,    0.0,      1.0,    0.0 },
      { -2,  0,  0,  0,  1,      -2.0,    0.0,      1.0,    0.0 },

    /* 71-80 */
      {  3,  0,  2,  0,  2,      -3.0,    0.0,      1.0,    0.0 },
      {  0, -1,  2,  2,  2,      -3.0,    0.0,      1.0,    0.0 },
      {  1,  1,  2,  0,  2,       2.0,    0.0,     -1.0,    0.0 },
      { -1,  0,  2, -2,  1,      -2.0,    0.0,      1.0,    0.0 },
      {  2,  0,  0,  0,  1,       2.0,    0.0,     -1.0,    0.0 },
      {  1,  0,  0,  0,  2,      -2.0,    0.0,      1.0,    0.0 },
      {  3,  0,  0,  0,  0,       2.0,    0.0,      0.0,    0.0 },
      {  0,  0,  2,  1,  2,       2.0,    0.0,     -1.0,    0.0 },
      { -1,  0,  0,  0,  2,       1.0,    0.0,     -1.0,    0.0 },
      {  1,  0,  0, -4,  0,      -1.0,    0.0,      0.0,    0.0 },

    /* 81-90 */
      { -2,  0,  2,  2,  2,       1.0,    0.0,     -1.0,    0.0 },
      { -1,  0,  2,  4,  2,      -2.0,    0.0,      1.0,    0.0 },
      {  2,  0,  0, -4,  0,      -1.0,    0.0,      0.0,    0.0 },
      {  1,  1,  2, -2,  2,       1.0,    0.0,     -1.0,    0.0 },
      {  1,  0,  2,  2,  1,      -1.0,    0.0,      1.0,    0.0 },
      { -2,  0,  2,  4,  2,      -1.0,    0.0,      1.0,    0.0 },
      { -1,  0,  4,  0,  2,       1.0,    0.0,      0.0,    0.0 },
      {  1, -1,  0, -2,  0,       1.0,    0.0,      0.0,    0.0 },
      {  2,  0,  2, -2,  1,       1.0,    0.0,     -1.0,    0.0 },
      {  2,  0,  2,  2,  2,      -1.0,    0.0,      0.0,    0.0 },

    /* 91-100 */
      {  1,  0,  0,  2,  1,      -1.0,    0.0,      0.0,    0.0 },
      {  0,  0,  4, -2,  2,       1.0,    0.0,      0.0,    0.0 },
      {  3,  0,  2, -2,  2,       1.0,    0.0,      0.0,    0.0 },
      {  1,  0,  2, -2,  0,      -1.0,    0.0,      0.0,    0.0 },
      {  0,  1,  2,  0,  1,       1.0,    0.0,      0.0,    0.0 },
      { -1, -1,  0,  2,  1,       1.0,    0.0,      0.0,    0.0 },
      {  0,  0, -2,  0,  1,      -1.0,    0.0,      0.0,    0.0 },
      {  0,  0,  2, -1,  2,      -1.0,    0.0,      0.0,    0.0 },
      {  0,  1,  0,  2,  0,      -1.0,    0.0,      0.0,    0.0 },
      {  1,  0, -2, -2,  0,      -1.0,    0.0,      0.0,    0.0 },

    /* 101-106 */
      {  0, -1,  2,  0,  1,      -1.0,    0.0,      0.0,    0.0 },
      {  1,  1,  0, -2,  1,      -1.0,    0.0,      0.0,    0.0 },
      {  1,  0, -2,  2,  0,      -1.0,    0.0,      0.0,    0.0 },
      {  2,  0,  0,  2,  0,       1.0,    0.0,      0.0,    0.0 },
      {  0,  0,  2,  4,  2,      -1.0,    0.0,      0.0,    0.0 },
      {  0,  1,  0,  1,  0,       1.0,    0.0,      0.0,    0.0 }
   };

/* Number of terms in the series */
    const int NT = (int) (sizeof x / sizeof x[0]);

/* ------------------------------------------------------------------ */

/* Interval between fundamental epoch J2000.0 and given date (JC). */
    // t = tp.julianCenturyFromJ2000TT(); // ((date1 - DJ00) + date2) / DJC;

/* --------------------- */
/* Fundamental arguments */
/* --------------------- */

/* Mean longitude of Moon minus mean longitude of Moon's perigee. */
    el = astAnpm(
        (485866.733 + (715922.633 + (31.310 + 0.064 * t) * t) * t)
        * DAS2R + fmod(1325.0 * t, 1.0) * D2PI);

/* Mean longitude of Sun minus mean longitude of Sun's perigee. */
    elp = astAnpm(
         (1287099.804 + (1292581.224 + (-0.577 - 0.012 * t) * t) * t)
         * DAS2R + fmod(99.0 * t, 1.0) * D2PI);

/* Mean longitude of Moon minus mean longitude of Moon's node. */
    f = astAnpm(
       (335778.877 + (295263.137 + (-13.257 + 0.011 * t) * t) * t)
       * DAS2R + fmod(1342.0 * t, 1.0) * D2PI);

/* Mean elongation of Moon from Sun. */
    d = astAnpm(
       (1072261.307 + (1105601.328 + (-6.891 + 0.019 * t) * t) * t)
       * DAS2R + fmod(1236.0 * t, 1.0) * D2PI);

/* Longitude of the mean ascending node of the lunar orbit on the */
/* ecliptic, measured from the mean equinox of date. */
    om = astAnpm(
        (450160.280 + (-482890.539 + (7.455 + 0.008 * t) * t) * t)
        * DAS2R + fmod(-5.0 * t, 1.0) * D2PI);

/* --------------- */
/* Nutation series */
/* --------------- */

/* Initialize nutation components. */
    dp = 0.0;
    de = 0.0;

/* Sum the nutation terms, ending with the biggest. */
    for (j = NT-1; j >= 0; j--) {

   /* Form argument for current term. */
        arg = (double)x[j].nl  * el
          + (double)x[j].nlp * elp
          + (double)x[j].nf  * f
          + (double)x[j].nd  * d
          + (double)x[j].nom * om;

   /* Accumulate current nutation term. */
        s = x[j].sp + x[j].spt * t;
        c = x[j].ce + x[j].cet * t;
        if (s != 0.0) dp += s * sin(arg);
        if (c != 0.0) de += c * cos(arg);
    }

/* Convert results from 0.1 mas units to radians. */
    dpsi = dp * U2R;
    deps = de * U2R;
    if(eqecorr){
        *eqecorr = aIAUResolutionC7(om);
    }
}


/// @brief 计算章动角（IAU1980模型）
/// @details 计算给定时间点的章动角，使用IAU1980模型
/// @param tp 时间点
/// @param dpsi 输出的章动角（经度）
/// @param deps 输出的章动角（纬度）
/// @return err_t 错误码，0表示成功，其他值表示失败
static err_t aNutation_IAU1980_Impl(const TimePoint& tp, double &dpsi, double &deps)
{
    aNutation_IAU1980(tp.julianCenturyFromJ2000TT(), dpsi, deps);
    return 0;
}

/// @brief 计算章动角（JPL DE星历）
/// @details 计算给定时间点的章动角，使用JPL DE星历
/// @param tp 时间点
/// @param dpsi 输出的章动角（经度）
/// @param deps 输出的章动角（纬度）
/// @return err_t 错误码，0表示成功，其他值表示失败
static err_t aNutation_JplDe_Impl(const TimePoint& tp, double &dpsi, double &deps)
{
    err_t err = aJplDeGetNutation(tp, dpsi, deps);
    if(err){
        // 失败时，尝试使用IAU1980模型
        aInfo("Failed to get nutation from JPL DE, fallback to IAU1980 model.");
        return aNutation_IAU1980_Impl(tp, dpsi, deps);
    }
    return 0;
}


//@fixme: 全局变量，需要考虑多DataContext互相冲突的问题，考虑移动到GlobalContext?
NutationFunc aNutation = &aNutation_IAU1980_Impl;  
// NutationFunc aNutation = &aNutation_JplDe_Impl;

err_t aNutationMethodSet(ENutationMethod method)
{
    switch (method)
    {
    case ENutationMethod::eIAU1980:
        aNutation = &aNutation_IAU1980_Impl;
        break;
    case ENutationMethod::eJplDe:
        aNutation = &aNutation_JplDe_Impl;
        break;
    default:
        return -1;
    }
    return 0;
}

ENutationMethod aNutationMethodGet()
{
    if(aNutation == &aNutation_IAU1980_Impl){
        return ENutationMethod::eIAU1980;
    }
    else if(aNutation == &aNutation_JplDe_Impl){
        return ENutationMethod::eJplDe;
    }
    else{
        return ENutationMethod::eUser;
    }
}


double aGMST_IAU1982(const TimePoint& tp)
{
    JulianDate jdUT1;
    aTimePointToUT1(tp, jdUT1);
    return aGMST_UT1_IAU1982(jdUT1);
}

double aGMST_UT1_IAU1982(const JulianDate &jdUT1)
{
    /*
    参考SOFA函数 iauGmst82
    */

/* Coefficients of IAU 1982 GMST-UT1 model */
    double A = 24110.54841  -  DAYSEC / 2.0;
    double B = 8640184.812866;
    double C = 0.093104;
    double D =  -6.2e-6;

/* The first constant, A, has to be adjusted by 12 hours because the */
/* UT1 is supplied as a Julian date, which begins at noon.           */

    double t, f, gmst;

/* Julian centuries since fundamental epoch. */
    t = jdUT1.julianCenturyFromJ2000(); //(d1 + (d2 - DJ00)) / DJC;

/* Fractional part of JD(UT1), in seconds. */
    f = jdUT1.second();

/* GMST at this UT1. */
    gmst = astAnp(DS2R * ((A + (B + (C + D * t) * t) * t) + f));

    return gmst;
}


double aEquationOfEquinoxes_IAU1994(double t)
{
    /*
    参考SOFA函数 iauEqeq94
    */
    double dpsi,  deps,  eps0, ee, eqecorr;

/* Interval between fundamental epoch J2000.0 and given date (JC). */
   // t = ((date1 - DJ00) + date2) / DJC;

/* Nutation components and mean obliquity. */
   aNutation_IERS1996(t, dpsi, deps, &eqecorr);
   eps0 = aMeanObliquity_IAU1980(t);

/* Equation of the equinoxes. */
   ee = dpsi*cos(eps0) + eqecorr;

   return ee;

}

double aGAST_IAU1994(const TimePoint &tp)
{
    /*
    参考SOFA函数 iauGst94
    */
    double gmst82, eqeq94, gst;

    gmst82 = aGMST_IAU1982(tp);
    eqeq94 = aEquationOfEquinoxes_IAU1994(tp);
    gst = astAnp(gmst82  + eqeq94);

    return gst;
}

AST_NAMESPACE_END
