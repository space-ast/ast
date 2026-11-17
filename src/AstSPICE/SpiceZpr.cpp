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
#include "AstCore/FrameTransform.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/TimeSystem.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstMath/AngleAxis.hpp"
#include "AstMath/Euler.hpp"
#include "AstMath/Quaternion.hpp"
#include "AstMath/MathOperator.hpp"
#include "AstMath/MathUtil.hpp"
#include "AstMath/ParamAdapter.hpp"
#include "AstUtil/Constants.hpp"
#include "AstUtil/Math.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"
#include "AstSPICE/SpiceRunTime.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

using SpiceDouble = double;
using SpiceInt = int;

namespace {

double spiceDet3(const Matrix3d& m)
{
    return m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1))
         - m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0))
         + m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
}


void spiceTranspose6(const Matrix6d& m, Matrix6d& out)
{
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j)
            out(i, j) = m(j, i);
}

void spiceInverse3(const Matrix3d& m, Matrix3d& out)
{
    const double a = m(0, 0), b = m(0, 1), c = m(0, 2);
    const double d = m(1, 0), e = m(1, 1), f = m(1, 2);
    const double g = m(2, 0), h = m(2, 1), i = m(2, 2);
    const double det = spiceDet3(m);
    if (det == 0.0) {
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                out(r, c) = 0.0;
        return;
    }
    const double invDet = 1.0 / det;
    out(0, 0) = (e * i - f * h) * invDet;
    out(0, 1) = (c * h - b * i) * invDet;
    out(0, 2) = (b * f - c * e) * invDet;
    out(1, 0) = (f * g - d * i) * invDet;
    out(1, 1) = (a * i - c * g) * invDet;
    out(1, 2) = (c * d - a * f) * invDet;
    out(2, 0) = (d * h - e * g) * invDet;
    out(2, 1) = (b * g - a * h) * invDet;
    out(2, 2) = (a * e - b * d) * invDet;
}

} // namespace





// appndc

// appndd

// appndi


void axisar(const Vector3d & axis, double angle, Matrix3d & r)
{
    // @todo
    aAngleAxisToMatrix(AngleAxis(-angle, axis), r);
}

AST_SPICE_CAPI
void axisar(
    const double  axis[3],
    double        angle,
    double        r[3][3]
)
{
    axisar(in(axis), angle, out(r));
}


// azlcpo


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


// badkpv

// bltfrm

// bodc2n

// bodc2s

// boddef

// bodfnd

// bodn2c

// bods2c

// bodvar

// bodvcd

// bodvrd

// brcktd

// brckti

// bschoc

// bschoi

// bsrchc

// bsrchd

// bsrchi

// card

// ccifrm

// cgv2el

// chbder

// chbigr

// chbint

// chbval

// chkin

// chkout


// cidfrm

// ckcls

// ckcov

// ckfrot

// ckfxfm

// ckgpav

// ckgp

// ckgr02

// ckgr03

// cklpf

// ckmeta

// cknr02

// cknr03

// ckobj

// ckopn

// ckupf

// ckw01

// ckw02

// ckw03

// ckw05

// clearc

// cleard

// cleari

double clight()
{
    return aLightSpeed();
}

// clpool

// cmprss

// cnmfrm

// conics

// convrt

// copy

// cposr

// cpos

// CSPICE_symbols.txt

// cvpool

// cyllat

// cylrec

// cylsph

// dafac

// dafbbs

// dafbfs

// dafcls

// dafcs

// dafdc

// dafec

// daffna

// daffpa

// dafgda

// dafgh

// dafgn

// dafgsr

// dafgs

// dafhsf

// dafopr

// dafopw

// dafps

// dafrda

// dafrfr

// dafrs

// dafus

// dasac

// dasadc

// dasadd

// dasadi

// dascls

// dasdc

// dasec

// dashfn

// dashfs

// daslla

// dasllc

// dasonw

// dasopr

// dasops

// dasopw

// dasrdc

// dasrdd

// dasrdi

// dasrfr

// dasudc

// dasudd

// dasudi

// daswbr

// dazldr

// dcyldr

// deltet

double det(const Matrix3d& m1) { return spiceDet3(m1); }

AST_SPICE_CAPI double det(const double m1[3][3]) { return det(in(m1)); }

// dgeodr

// diags2

// diff

// dlabbs

// dlabfs

// dlabns

// dlaens

// dlafns

// dlafps

// dlaopn

// dlatdr

// dnearp

// dp2hx

// dpgrdr

// dpmax

// dpmin


double dpr()
{
    return kRadToDeg;
}

// drdazl

// drdcyl

// drdgeo

// drdlat

// drdpgr

// drdsph

// dskb02

// dskcls

// dskd02

// dskgd

// dskgtl

// dski02

// dskmi2

// dskn02

// dskobj

// dskopn

// dskp02

// dskrb2

// dsksrf

// dskstl

// dskv02

// dskw02

// dskx02

// dskxsi

// dskxv

// dskz02

// dsphdr

// dtpool

// ducrss

// dvcrss

// dvdot

// dvhat

// dvnorm

// dvpool

// dvsep

// edlimb

// ednmpt

// edpnt

// edterm

// ekacec

// ekaced

// ekacei

// ekaclc

// ekacld

// ekacli

// ekappr

// ekbseg

// ekccnt

// ekcii

// ekcls

// ekdelr

// ekffld

// ekfind

// ekgc

// ekgd

// ekgi

// ekifld

// ekinsr

// eklef

// eknelt

// eknseg

// ekntab

// ekopn

// ekopr

// ekops

// ekopw

// ekpsel

// ekrcec

// ekrced

// ekrcei

// ekssum

// ektnam

// ekucec

// ekuced

// ekucei

// ekuef

// el2cgv

// elemc

// elemd

// elemi

// eqncpv

// eqstr

// erract

// errch

// errdev

// errdp

// errint

// errprt

// esrchc

// et2lst

// et2utc

// etcal


void eul2m(double angle3, double angle2, double angle1, int axis3, int axis2, int axis1, Matrix3d &r)
{
    int seq = axis1 * 100 + axis2 * 10 + axis3;
    Euler euler{angle1, angle2, angle3};
    errc_t rc = aEulerToMatrix(euler, seq, r);
    A_UNUSED(rc);
}

AST_SPICE_CAPI 
void eul2m(
    double  angle3,
    double  angle2,
    double  angle1,
    int     axis3,
    int     axis2,
    int     axis1,
    double  r [3][3]
)
{
    return eul2m(angle3, angle2, angle1, axis3, axis2, axis1, out(r));
}


// eul2xf

// evsgp4

// exists

// expool

// failed

// filld

// filli

// fovray

// fovtrg

// frame

// frinfo

// frmnam

// ftncls

// furnsh

// gcpool

// gdpool

// georec

// getcml

// getelm

// getfat

// getfov

// getfvn

// getmsg

// gfbail

// gfclrh

// gfdist

// gfevnt

// gffove

// gfilum

// gfinth

// gfocce

// gfoclt

// gfpa

// gfposc

// gfrefn

// gfrepf

// gfrepi

// gfrepu

// gfrfov

// gfrr

// gfsep

// gfsntc

// gfsstp

// gfstep

// gfstol

// gfsubc

// gftfov

// gfudb

// gfuds

// gipool

// gnpool

AST_SPICE_API double halfpi(void) { return kHalfPI; }

// hrmesp

// hrmint

// hx2dp


void ident(Matrix3d &matrix)
{
    matrix.setIdentity();
}

AST_SPICE_CAPI
void ident(double matrix[3][3])
{
    ident(out(matrix));
}

// illumf

// illumg

// illum

// illum_pl02

// illum_plid_pl02

// ilumin

// index

// inedpl

// inelpl

// inrypl

// insrtc

// insrtd

// insrti

// inter

// intmax

// intmin

void invert(const Matrix3d& m1, Matrix3d& mout) { 
    spiceInverse3(m1, mout); 
}

AST_SPICE_CAPI void invert(const double m1[3][3], double mout[3][3]) { invert(in(m1), out(mout)); }
// invort

// invstm

// isordv

// isrchc

// isrchd

// isrchi

bool isrot(const Matrix3d& m, double ntol, double dtol) { 
    const Matrix3d r = m * m.transpose(); 
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j) 
        { 
            const double expected = (i == j) ? 1.0 : 0.0; 
            if (std::fabs(r(i, j) - expected) > ntol) 
            return false; 
        }
    }
    return std::fabs(spiceDet3(m) - 1.0) <= dtol; 
}

AST_SPICE_CAPI int isrot(const double m[3][3], double ntol, double dtol) { return isrot(in(m), ntol, dtol) ? 1 : 0; }

// iswhsp


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


// kclear

// kdata

// kinfo

// kplfrm

// ktotal

// kxtrct

// lastnb

// latcyl



void latrec(double radius, double lon, double lat, Vector3d &rectan)
{
    double cos_lon, sin_lon, cos_lat, sin_lat;
    sincos(lon, &sin_lon, &cos_lon);
    sincos(lat, &sin_lat, &cos_lat);

    rectan[0] = radius * cos_lon * cos_lat;
    rectan[1] = radius * sin_lon * cos_lat;
    rectan[2] = radius * sin_lat;
}

// latsph

// latsrf

// lcase

// ldpool

// lgresp

// lgrind

// lgrint

// limbpt

// limb_pl02

// llgrid_pl02

// lmpool

// lparse

// lparsm

// lparss

// lspcn

// lstlec

// lstled

// lstlei

// lstltc

// lstltd

// lstlti

// ltime

// lx4dec

// lx4num

// lx4sgn

// lx4uns

// lxqstr


void m2eul(const Matrix3d &r, int axis3, int axis2, int axis1, double &angle3, double &angle2, double &angle1)
{
    int seq = axis1 * 100 + axis2 * 10 + axis3;
    Euler euler{};
    errc_t rc = aMatrixToEuler(r, seq, euler);
    A_UNUSED(rc);
    angle3 = euler.angle3();
    angle2 = euler.angle2();
    angle1 = euler.angle1();
}

AST_SPICE_CAPI
void m2eul(
    const double    r[3][3],
    int             axis3,
    int             axis2,
    int             axis1,
    double        * angle3,
    double        * angle2,
    double        * angle1
)
{
    m2eul(in(r), axis3, axis2, axis1, *angle3, *angle2, *angle1);
}

void m2q(const Matrix3d &r, Quaternion &q)
{
    // @todo
    aMatrixToQuat(r.transpose(), q);
}

AST_SPICE_CAPI
void m2q(
    const double  r[3][3],
    double        q[4]
)
{
    m2q(in(r), out(q));
}


// matchi

// matchw

// maxd

// maxi

// mequg

void mequ(const Matrix3d& m1, Matrix3d& mout) { mout = m1; }

AST_SPICE_CAPI void mequ(const double m1[3][3], double mout[3][3]) { mequ(in(m1), out(mout)); }

// mind

// mini

// moved

// mtxmg

void mtxm(const Matrix3d& m1, const Matrix3d& m2, Matrix3d& mout) { 
    mout = m1.transpose() * m2; 
}

AST_SPICE_CAPI void mtxm(const double m1[3][3], const double m2[3][3], double mout[3][3]) { mtxm(in(m1), in(m2), out(mout)); }

// mtxvg

void mtxv(const Matrix3d& m1, const Vector3d& v2, Vector3d& vout) { vout = m1.transpose() * v2; }

AST_SPICE_CAPI void mtxv(const double m1[3][3], const double v2[3], double vout[3]) { mtxv(in(m1), in(v2), out(vout)); }

// mxmg

// mxmtg

void mxmt(const Matrix3d& m1, const Matrix3d& m2, Matrix3d& mout) { mout = m1 * m2.transpose(); }

AST_SPICE_CAPI void mxmt(const double m1[3][3], const double m2[3][3], double mout[3][3]) { mxmt(in(m1), in(m2), out(mout)); }

void mxm(const Matrix3d &m1, const Matrix3d &m2, Matrix3d &mout)
{
    mout = m1 * m2;
}

AST_SPICE_CAPI
void mxm(
    const double m1[3][3],
    const double m2[3][3],
    double       mout[3][3]
)
{
    mxm(in(m1), in(m2), out(mout));
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

void mxv(const Matrix3d& m1, const Vector3d& v2, Vector3d& vout) { vout = m1 * v2; }

AST_SPICE_CAPI void mxv(const double m1[3][3], const double v2[3], double vout[3]) { mxv(in(m1), in(v2), out(vout)); }

// namfrm

// ncposr

// ncpos

// nearpt

// nextwd

// npedln

// npelpt

// nplnpt

// nthwd

// nvc2pl

// nvp2pl

// occult

// ordc

// ordd

// orderc

// orderd

// orderi

// ordi

// oscelt

// oscltx

// pckcls

// pckcov

// pckfrm

// pcklof

// pckopn

// pckuof

// pckw02

// pcpool

// pdpool

// pgrrec

// phaseq

// pipool

AST_SPICE_CAPI double pi(void) { return kPI; }

// pjelpl

// pl2nvc

// pl2nvp

// pl2psv

// pltar

// pltexp

// pltnp

// pltnrm

// pltvol

// polyds

// posr

// pos

// prompt

// prop2b

// prsdp

// prsint

// psv2pl

// putcml



errc_t pxform(StringView from, StringView to, const TimePoint &et, Matrix3d &rotate)
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
errc_t pxform(
    const char   * from,
    const char   * to,
    double         et,
    double         rotate[3][3]
)
{
    return pxform(from, to, aSpiceEtToTimePoint(et), out(rotate));
}

// pxfrm2

void q2m(const Quaternion &q, Matrix3d &r)
{
    // @todo
    aQuatToMatrix(q.conjugate(), r);
}

AST_SPICE_CAPI
void q2m(
    const double  q[4],
    double        r[3][3]
)
{
    q2m(in(q), out(r));
}


// qcktrc

// qderiv

// qdq2av

// qxq

void radrec(double range, double ra, double dec, Vector3d &rectan)
{
    latrec(range, ra, dec, rectan);
}



void rav2xf(const Matrix3d &rot, const Vector3d &av, Matrix6d &xform)
{
    Matrix3d omegat;
    aSetMatrix(omegat,
        0,      av[2], -av[1],
        -av[2], 0,      av[0],
        av[1],  -av[0], 0
    );

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

AST_SPICE_CAPI
void rav2xf(
    const double rot   [3][3],
    const double av    [3],
    double       xform [6][6]
)
{
    rav2xf(in(rot), in(av), out(xform));
}


// raxisa

// rdtext

// recazl

// reccyl

// recgeo

// reclat

// recpgr

// recrad

// recsph

// removc

// removd

// removi

// reordc

// reordd

// reordi

// reordl

// repmct

// repmc

// repmd

// repmf

// repmi

// repml

// repmot

// reset

// return

// rotate

// rotmat

// rotvec

AST_SPICE_CAPI double rpd(void) { return kDegToRad; }

// rquad

// saelgv

// scard

// scdecd

// sce2c

// sce2s

// sce2t

// scencd

// scfmt

// scpart

// scs2e

// sct2e

// sctiks

// sdiff

// setmsg

// set

// shellc

// shelld

// shelli

// sigerr

// sincpt

// size

AST_SPICE_CAPI double spd(void) { return kSecondsPerDay; }

// sphcyl

// sphlat

// sphrec

// spk14a

// spk14b

// spk14e

// spkacs

errc_t spkapo(
    CelestialBody      * targ,
    const TimePoint&     et,
    Axes               * ref,
    const CartState&     sobs,
    StringView           abcorr,
    Vector3d&            ptarg,
    double             * lt       
){
    if ( targ == nullptr || ref == nullptr)
        return eErrorNullInput;
    double temp;
    if(!lt)
        lt = &temp;

    Vector3d posTarg;
    errc_t rc = targ->getPosICRF(et, posTarg);
    if ( rc != 0 )
        return rc;
    ptarg = posTarg - sobs.pos();
    *lt = ptarg.norm() / aLightSpeed();
        
    if(abcorr.empty() || aEqualsIgnoreCase(abcorr, "None"))
    {
        // pass
    }
    else{
        int sign = abcorr[0] == 'X' ? 1 : -1;
        int maxIter=0;

        if(abcorr.starts_with("LT") || abcorr.starts_with("XLT"))
        {
            maxIter = 1;
        }
        else if(abcorr.starts_with("CN") || abcorr.starts_with("XCN"))
        {
            maxIter = 3;
        }

        for(int i=0; i<maxIter; i++)
        {
            double dt = sign * (*lt);
            /*!
            @note SPICE 中的时间系统按照 TDB 时间尺度进行推进
            */
            errc_t rc = targ->getPosICRF(et.shiftedBySecondInTDB(dt), posTarg);
            if ( rc != 0 )
                return rc;
            ptarg = posTarg - sobs.pos();
            *lt = ptarg.norm() / aLightSpeed();
        }
        if(abcorr.ends_with("+S"))
        {
            if(abcorr[0] == 'X' ){
                stlabx(ptarg, sobs.vel(), ptarg);
            }
            else{
                stelab(ptarg, sobs.vel(), ptarg);
            }
        }
    }

    // 转换参考轴系
    {
        auto icrf = aAxesICRF();
        if(ref != icrf && ref != nullptr){
            Rotation rot;
            rc = aAxesTransform(icrf, ref, et, rot);
            if ( rc != 0 )
                return rc;
            ptarg = rot.transformVector(ptarg);
        }
    }
    return eNoError;
}


// spkapp

// spkaps

// spkcls

// spkcov

// spkcpo

// spkcpt

// spkcvo

// spkcvt

// spkezp

// spkezr

// spkez

// spkgeo

// spkgps

// spklef

// spkltc

// spkobj

// spkopa

// spkopn

// spkpds



errc_t spkpos(
    CelestialBody *targ, 
    const TimePoint &et, 
    Axes *ref, 
    StringView abcorr, 
    CelestialBody *obs, 
    Vector3d &ptarg, 
    double *lt
)
{
    if ( targ == nullptr || obs == nullptr )
        return eErrorNullInput;
    CartState staObs{};
    errc_t rc  = obs->getPosVelICRF(et, staObs.pos(), staObs.vel());
    if ( rc != 0 )
        return rc;
    return spkapo(targ, et, ref, staObs, abcorr, ptarg, lt);
}

errc_t spkpos(
    StringView targ,     
    const TimePoint&   et,
    StringView ref, 
    StringView abcorr, 
    StringView obs, 
    Vector3d &ptarg, 
    double *lt)
{
    CelestialBody *targIns = aSpiceFindBody(targ);
    if ( targIns == nullptr )
        return -1;
    CelestialBody *obsIns = aSpiceFindBody(obs);
    if ( obsIns == nullptr )
        return -1;
    Axes *refAxes = aSpiceFindAxes(ref);
    if ( refAxes == nullptr )
        return -1;
    return spkpos(targIns, et, refAxes, abcorr, obsIns, ptarg, lt);
}


AST_SPICE_CAPI
errc_t spkpos(
    const char   * targ,
    double         et,
    const char   * ref,
    const char   * abcorr,
    const char   * obs,
    Vector3d&      ptarg,
    double      *  lt        
)
{
    return spkpos(targ, aSpiceEtToTimePoint(et), ref, abcorr, obs, ptarg, lt);
}


// spkpvn

// spksfs


errc_t spkssb(CelestialBody *targ, const TimePoint &et, Axes *ref, CartState &starg)
{
    if ( targ == nullptr || ref == nullptr )
        return -1;
    errc_t rc = targ->getPosVelICRF(et, starg.pos(), starg.vel());
    if ( rc != 0 )
        return rc;
    auto icrf = aAxesICRF();
    if(ref != icrf && ref != nullptr){
        KinematicRotation rot;
        rc = aAxesTransform(icrf, ref, et, rot);
        if ( rc != 0 )
            return rc;
        rot.transformVectorVelocity(starg.pos(), starg.vel(), starg.pos(), starg.vel());
    }
    return eNoError;
}

AST_SPICE_CAPI
errc_t spkssb(
    int            targ,
    double         et,
    const char   * ref,
    CartState&     starg
)
{
    CelestialBody *targIns = aSpiceFindBody(targ);
    if ( targIns == nullptr )
        return -1;
    Axes *refAxes = aSpiceFindAxes(ref);
    if ( refAxes == nullptr )
        return -1;
    return spkssb(targIns, aSpiceEtToTimePoint(et), refAxes, starg);
}

// spksub

// spkuds

// spkuef

// spkw02

// spkw03

// spkw05

// spkw08

// spkw09

// spkw10

// spkw12

// spkw13

// spkw15

// spkw17

// spkw18

// spkw20

// srfc2s

// srfcss

// srfnrm

// srfrec

// srfs2c

// srfscc

// srfxpt

// ssize

// stelab

errc_t stelab(const Vector3d& pobj, const Vector3d& vobs, Vector3d& appobj)
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
        aError(_("观测者速度不应大于光速"));
        return -1;
    }

    // 计算叉积 H = U × (v/c)
    Vector3d h = u.cross(vbyc);

    double sinphi = h.norm();
    if (sinphi != 0.0) {
        // 像差角 φ = arcsin(|h|)
        double phi = std::asin(sinphi);
        // 绕旋转轴 h 旋转 phi(φ) 弧度
        vrotv(pobj, h, phi, appobj);
    } else {
        // 运动方向与视线重合，无需校正
        appobj = pobj;
    }
    return eNoError;
}

// stlabx

errc_t stlabx(const Vector3d& pobj, const Vector3d& vobs, Vector3d& appobj)
{
    // 发射情况的校正是接收情况的逆校正，只需将速度取反
    Vector3d negVel = -vobs;
    return stelab(pobj, negVel, appobj);
}

// stpool

// str2et

// subpnt

// subpt

// subpt_pl02

// subslr

// subsol

// subsol_pl02

// sumad

// sumai

// surfnm

// surfpt

// surfpv

// swpool

// sxform

// szpool

// tangpt

// termpt

// term_pl02

// timdef

// timout


errc_t tipbod(Axes *ref, CelestialBody *body, const TimePoint &et, Matrix3d &tipm)
{
    if ( ref == nullptr || body == nullptr )
        return -1;
    auto bodyFixed = body->getAxesFixed();
    if ( bodyFixed == nullptr )
        return -1;
    return aAxesTransform(ref, bodyFixed, et, tipm);
}

AST_SPICE_CAPI
errc_t tipbod(
    const char      * ref,
    int               body,
    double            et,
    double            tipm[3][3]
)
{
    auto refAxes = aSpiceFindAxes(ref);
    if ( refAxes == nullptr )
        return -1;
    auto bodyIns = aSpiceFindBody(body);
    if ( bodyIns == nullptr )
        return -1;
    return tipbod(refAxes, bodyIns, aSpiceEtToTimePoint(et), out(tipm));
}


// tisbod

// tkfram

// tkvrsn

// tparch

// tparse

// tpictr

// trace

// trcdep

// trcnam

// trcoff

// trgsep

// tsetyr

AST_SPICE_CAPI double twopi(void) { return kTwoPI; }

// twovec

// twovxf

AST_SPICE_CAPI double tyear(void) { return kSecondsPerTropicalYear; }

// ucase

void ucrss(const Vector3d& v1, const Vector3d& v2, Vector3d& vout) { 
    vout = v1.cross(v2); 
    vout.normalize();
}

AST_SPICE_CAPI void ucrss(const double v1[3], const double v2[3], double vout[3]) { ucrss(in(v1), in(v2), out(vout)); }

// uddc

// uddf

// udf

// union

// unitim

// unload

// unormg

double unorm(const Vector3d& v, Vector3d& u) { 
    const double n = v.norm(); 
    if (n > 0.0) 
        u = v / n; 
    else 
        u.setZero(); 
    return n; 
}

AST_SPICE_CAPI void unorm(const double v[3], double u[3], double* vmag) 
{
    const double mag = unorm(in(v), out(u));
    if (vmag) *vmag = mag; 
}

errc_t utc2et(StringView utcstr, double &et)
{
    DateTime dttm{};
    errc_t rc = aDateTimeParseAny(utcstr, dttm);
    if ( rc != 0 )
        return rc;
    TimePoint tp = TimePoint::FromUTC(dttm);
    et = aTimePointToSpiceEt(tp);
    return eNoError;
}

AST_SPICE_CAPI 
errc_t utc2et(
    const char * utcstr,
    double &et
)
{
    return utc2et(StringView(utcstr), et);
}

// vaddg

void vadd(const Vector3d& v1, const Vector3d& v2, Vector3d& vout) { vout = v1 + v2; }

AST_SPICE_CAPI void vadd(const double v1[3], const double v2[3], double vout[3]) { vadd(in(v1), in(v2), out(vout)); }

// valid

void vcrss(const Vector3d& v1, const Vector3d& v2, Vector3d& vout) { vout = v1.cross(v2); }

AST_SPICE_CAPI void vcrss(const double v1[3], const double v2[3], double vout[3]) { vcrss(in(v1), in(v2), out(vout)); }

// vdistg

double vdist(const Vector3d& v1, const Vector3d& v2) { return (v1 - v2).norm(); }

AST_SPICE_CAPI double vdist(const double v1[3], const double v2[3]) { return vdist(in(v1), in(v2)); }

// vdotg

double vdot(const Vector3d& v1, const Vector3d& v2) { return v1.dot(v2); }

AST_SPICE_CAPI double vdot(const double v1[3], const double v2[3]) { return vdot(in(v1), in(v2)); }

// vequg

void vequ(const Vector3d& vin, Vector3d& vout) { vout = vin; }

AST_SPICE_CAPI void vequ(const double vin[3], double vout[3]) { vequ(in(vin), out(vout)); }

// vhatg

void vhat(const Vector3d& v, Vector3d& vout) { 
    vout = v.normalized(); 
}

AST_SPICE_CAPI void vhat(const double v[3], double vout[3]) { vhat(in(v), out(vout)); }

void vlcom3(double a, const Vector3d& v1, double b, const Vector3d& v2, double c, const Vector3d& v3, Vector3d& sum) { sum = a * v1 + b * v2 + c * v3; }

AST_SPICE_CAPI void vlcom3(double a, const double v1[3], double b, const double v2[3], double c, const double v3[3], double sum[3]) { vlcom3(a, in(v1), b, in(v2), c, in(v3), out(sum)); }

// vlcomg

void vlcom(double a, const Vector3d& v1, double b, const Vector3d& v2, Vector3d& sum) { sum = a * v1 + b * v2; }

AST_SPICE_CAPI void vlcom(double a, const double v1[3], double b, const double v2[3], double sum[3]) { vlcom(a, in(v1), b, in(v2), out(sum)); }

void vminug(const Vector3d& v, Vector3d& vout) { vout = -v; }

AST_SPICE_CAPI void vminug(const double* vin, int ndim, double* vout) 
{
    for (int i = 0; i < ndim; ++i) 
        vout[i] = -vin[i]; 
}

void vminus(const Vector3d& v, Vector3d& vout) { vout = -v; }

AST_SPICE_CAPI void vminus(const double v[3], double vout[3]) { vminus(in(v), out(vout)); }

// vnormg

double vnorm(const Vector3d& v) { return v.norm(); }

AST_SPICE_CAPI double vnorm(const double v[3]) { return vnorm(in(v)); }

void vpack(double x, double y, double z, Vector3d& v) { v.x() = x; v.y() = y; v.z() = z; }

AST_SPICE_CAPI void vpack(double x, double y, double z, double v[3]) { vpack(x, y, z, out(v)); }

void vperp(const Vector3d& a, const Vector3d& b, Vector3d& p) { 
    Vector3d t; 
    vproj(a, b, t); 
    p = a - t; 
}

AST_SPICE_CAPI void vperp(const double a[3], const double b[3], double p[3]) { vperp(in(a), in(b), out(p)); }

// vprjpi

// vprjp

// vprojg

void vproj(const Vector3d& a, const Vector3d& b, Vector3d& p) { 
    const double bb = b.dot(b); 
    if (bb == 0.0) 
        p.setZero(); 
    else 
        p = b * (a.dot(b) / bb); 
}

AST_SPICE_CAPI void vproj(const double a[3], const double b[3], double p[3]) { vproj(in(a), in(b), out(p)); }

// vrelg

// vrel

// vrotv
void vrotv(const Vector3d& v, const Vector3d& axis, double theta, Vector3d& r)
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

// vsclg

void vscl(double s, const Vector3d& v1, Vector3d& vout) { vout = s * v1; }

AST_SPICE_CAPI void vscl(double s, const double v1[3], double vout[3]) { vscl(s, in(v1), out(vout)); }

// vsepg

double vsep(const Vector3d& v1, const Vector3d& v2) { 
    const double n1 = v1.norm(), n2 = v2.norm(); 
    if (n1 == 0.0 || n2 == 0.0) 
        return 0.0; 
    double c = v1.dot(v2) / (n1 * n2); 
    return acosSafe(c); 
}

AST_SPICE_CAPI double vsep(const double v1[3], const double v2[3]) { return vsep(in(v1), in(v2)); }

// vsubg

void vsub(const Vector3d& v1, const Vector3d& v2, Vector3d& vout) { vout = v1 - v2; }

AST_SPICE_CAPI void vsub(const double v1[3], const double v2[3], double vout[3]) { vsub(in(v1), in(v2), out(vout)); }

// vtmvg

// vtmv

void vupack(const Vector3d& v, double& x, double& y, double& z) { x = v.x(); y = v.y(); z = v.z(); }

AST_SPICE_CAPI void vupack(const double v[3], double* x, double* y, double* z) { vupack(in(v), *x, *y, *z); }

// vzerog

bool vzero(const Vector3d& v) { return v.x() == 0.0 && v.y() == 0.0 && v.z() == 0.0; }

AST_SPICE_CAPI int vzero(const double v[3]) { return vzero(in(v)) ? 1 : 0; }

// wncard

// wncomd

// wncond

// wndifd

// wnelmd

// wnexpd

// wnextd

// wnfetd

// wnfild

// wnfltd

// wnincd

// wninsd

// wnintd

// wnreld

// wnsumd

// wnunid

// wnvald

// xf2eul

// xf2rav

// xfmsta

void xpose6(const Matrix6d& m1, Matrix6d& mout) { spiceTranspose6(m1, mout); }

AST_SPICE_CAPI void xpose6(const double m1[6][6], double mout[6][6]) { xpose6(in(m1), out(mout)); }

// xposeg

void xpose(const Matrix3d& m1, Matrix3d& mout) { mout = m1.transpose(); }

AST_SPICE_CAPI void xpose(const double m1[3][3], double mout[3][3]) { xpose(in(m1), out(mout)); }

AST_NAMESPACE_END
