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
#include "AstUtil/Constants.h"
#include "AstUtil/Math.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"
#include "AstSPICE/SpiceRunTime.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

using SpiceDouble = double;
using SpiceInt = int;




// appndc

// appndd

// appndi


void axisar(const Vector3d & axis, double angle, Matrix3d & r)
{
    // @todo
    aAngleAxisToMatrix(AngleAxis(-angle, axis), r);
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

// det

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

// halfpi

// hrmesp

// hrmint

// hx2dp


void ident(Matrix3d &matrix)
{
    matrix.setIdentity();
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

// invert

// invort

// invstm

// isordv

// isrchc

// isrchd

// isrchi

// isrot

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

void m2q(const Matrix3d &r, Quaternion &q)
{
    // @todo
    aMatrixToQuat(r.transpose(), q);
}


// matchi

// matchw

// maxd

// maxi

// mequg

// mequ

// mind

// mini

// moved

// mtxmg

// mtxm

// mtxvg

// mtxv

// mxmg

// mxmtg

// mxmt



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

// mxv

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

// pi

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
    Matrix3d&      rotate
)
{
    return pxform(from, to, aSpiceEtToTimePoint(et), rotate);
}

// pxfrm2

void q2m(const Quaternion &q, Matrix3d &r)
{
    // @todo
    aQuatToMatrix(q.conjugate(), r);
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

// rpd

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

// spd

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

// twopi

// twovec

// twovxf

// tyear

// ucase

// ucrss

// uddc

// uddf

// udf

// union

// unitim

// unload

// unormg

// unorm



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

// vadd

// valid

// vcrss

// vdistg

// vdist

// vdotg

// vdot

// vequg

// vequ

// vhatg

// vhat

// vlcom3

// vlcomg

// vlcom

// vminug

// vminus

// vnormg

// vnorm

// vpack

// vperp

// vprjpi

// vprjp

// vprojg

// vproj

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

// vscl

// vsepg

// vsep

// vsubg

// vsub

// vtmvg

// vtmv

// vupack

// vzerog

// vzero

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

// xpose6

// xposeg

// xpose









AST_NAMESPACE_END
