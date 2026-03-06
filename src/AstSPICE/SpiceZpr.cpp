///
/// @file      SpiceZpr.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-02
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

#include "SpiceZpr.hpp"
#include "AstCore/AxesTransform.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstMath/AngleAxis.hpp"
#include "AstMath/Euler.hpp"
#include "AstMath/Quaternion.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/Math.hpp"
#include "AstUtil/StringView.hpp"
#include "AstSPICE/SpiceRunTime.hpp"

AST_NAMESPACE_BEGIN

using SpiceDouble = double;
using SpiceInt = int;

void axisar(const Vector3d & axis, double angle, Matrix3d & r)
{
    // @todo
    aAngleAxisToMatrix(AngleAxis(-angle, axis), r);
}

void azlrec(double range, double az, double el, bool azccw, bool elplsz, Vector3d &rectan)
{
    if ( !azccw )
        az = -az;

    if ( !elplsz )
        el = -el;

    latrec(range, az, el, rectan);
}

double b1900()
{
    return kB1900Epoch;
}

double b1950()
{
    return kB1950Epoch;
}

// -----------------------------------------------------------


double dpr()
{
    return kRadToDeg;
}


void latrec(double radius, double lon, double lat, Vector3d &rectan)
{
    double cos_lon, sin_lon, cos_lat, sin_lat;
    sincos(lon, &sin_lon, &cos_lon);
    sincos(lat, &sin_lat, &cos_lat);

    rectan[0] = radius * cos_lon * cos_lat;
    rectan[1] = radius * sin_lon * cos_lat;
    rectan[2] = radius * sin_lat;
}

void m2eul(const Matrix3d &r, int axis3, int axis2, int axis1, double &angle3, double &angle2, double &angle1)
{
    int seq = axis1 * 100 + axis2 * 10 + axis3;
    Euler euler;
    err_t rc = aMatrixToEuler(r, seq, euler);
    A_UNUSED(rc);
    angle3 = euler.angle3();
    angle2 = euler.angle2();
    angle1 = euler.angle1();
}

void m2q(const Matrix3d &r, Quaternion &q)
{
    // @todo
    aMatrixToQuat(r.transpose(), q);
}

void mxm(const Matrix3d &m1, const Matrix3d &m2, Matrix3d &mout)
{
    mout = m1 * m2;
}

void mxvg(const void *m1, const void *v2, int nr1, int nc1r2, void *vout)
{
    double *mtx1 = (double *) m1;
    double *vec2 = (double *) v2;
    double *vecout = (double *) vout;

    for(int row=0; row<nr1; row++ )
    {
        double innerProduct = 0.0;
        for(int i=0; i<nc1r2; i++ )
        {
           innerProduct += mtx1[row*nc1r2 + i] * vec2[i];
        }
        vecout[row] = innerProduct;
    }
}

err_t pxform(StringView from, StringView to, const TimePoint &et, Matrix3d &rotate)
{
    Axes *fromAxes = aSpiceFindAxes(from);
    if ( fromAxes == nullptr )
        return -1;
    
    Axes *toAxes = aSpiceFindAxes(to);
    if ( toAxes == nullptr )
        return -1;
    return aAxesTransform(fromAxes, toAxes, et, rotate);
}

AST_SPICE_CAPI
err_t pxform(
    const char   * from,
    const char   * to,
    double         et,
    Matrix3d&      rotate
)
{
    return pxform(from, to, aSpiceEtToTimePoint(et), rotate);
}

void q2m(const Quaternion &q, Matrix3d &r)
{
    // @todo
    aQuatToMatrix(q.conjugate(), r);
}

void radrec(double range, double ra, double dec, Vector3d &rectan)
{
    latrec(range, ra, dec, rectan);
}

void rav2xf(const Matrix3d &rot, const Vector3d &av, Matrix6d &xform)
{
    Matrix3d omegat {
        0,      av[2], -av[1],
        -av[2], 0,      av[0],
        av[1],  -av[0], 0
    };

    Matrix3d drdt = rot * omegat;

    for(int i=0; i<3; i++ )
    {
        for(int j=0; j<3; j++ )
        {
            xform(i  ,j  ) = rot(i,j);
            xform(i+3,j+3) = rot(i,j);
            xform(i  ,j+3) = 0.;
            xform(i+3, j)  = drdt(i, j);
        }
    }
}

err_t tipbod(Axes *ref, CelestialBody *body, const TimePoint &et, Matrix3d &tipm)
{
    if ( ref == nullptr || body == nullptr )
        return -1;
    auto bodyFixed = body->getAxesFixed();
    if ( bodyFixed == nullptr )
        return -1;
    return aAxesTransform(ref, bodyFixed, et, tipm);
}

AST_SPICE_CAPI
err_t tipbod(
    const char      * ref,
    int               body,
    double            et,
    Matrix3d&         tipm
)
{
    auto refAxes = aSpiceFindAxes(ref);
    if ( refAxes == nullptr )
        return -1;
    auto bodyIns = aSpiceFindBody(body);
    if ( bodyIns == nullptr )
        return -1;
    return tipbod(refAxes, bodyIns, aSpiceEtToTimePoint(et), tipm);
}



void eul2m(double angle3, double angle2, double angle1, int axis3, int axis2, int axis1, Matrix3d &r)
{
    int seq = axis1 * 100 + axis2 * 10 + axis3;
    Euler euler{angle1, angle2, angle3};
    err_t rc = aEulerToMatrix(euler, seq, r);
    A_UNUSED(rc);
}

void ident(Matrix3d &matrix)
{
    matrix.setIdentity();
}

double j1900()
{
    return kJ1900Epoch;
}

double j1950()
{
    return kJ1950Epoch;
}

double j2000()
{
    return kJ2000Epoch;
}

double j2100()
{
    return kJ2100Epoch;
}

double jyear()
{
    return kSecondsPerJulianYear;
}

AST_NAMESPACE_END
