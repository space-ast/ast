///
/// @file      SpiceZpr.h
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

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Spice
    @{
*/

// appndc

// appndd

// appndi


/// @brief 根据旋转轴和角度计算旋转矩阵(Axis and angle to rotation)
/// 输出的矩阵表示将向量绕轴旋转给定角度
/// @param[in] axis 旋转轴
/// @param[in] angle 旋转角度（弧度）
/// @param[out] r 输出旋转矩阵
AST_SPICE_CAPI void axisar(
    const Vector3d&  axis,
    double           angle,
    Matrix3d&        r
);


// azlcpo


/// @brief 将极坐标转换为直角坐标(AZ/EL to rectangular coordinates)
/// @param[in] range 范围（距离）
/// @param[in] az 方位角（弧度）
/// @param[in] el 俯仰角（弧度）
/// @param[in] azccw 如果为真，则方位角按逆时针方向计算，否则按顺时针方向计算
/// @param[in] elplsz 如果为真，则俯仰角按正值上增加，否则按负值上增加
/// @param[out] rectan 输出直角坐标（x, y, z）
AST_SPICE_CAPI void azlrec(
    double     range,
    double     az,
    double     el,
    bool       azccw,
    bool       elplsz,
    Vector3d&  rectan
);



/// @brief 贝塞耳纪元B1900.0历元(Besselian Date 1900.0 )
AST_SPICE_CAPI double b1900();


/// @brief 贝塞耳纪元B1950.0历元(Besselian Date 1950.0 )
AST_SPICE_CAPI double b1950();


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

// clight

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


/// @brief 弧度转换为角度的比例因子(Degrees per radian)
AST_SPICE_CAPI double dpr();

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

/// @brief 欧拉角转换为旋转矩阵(Euler angles to matrix)
/// @param angle3 旋转角度3（弧度）
/// @param angle2 旋转角度2（弧度）
/// @param angle1 旋转角度1（弧度）
/// @param axis3 旋转轴3（1, 2, 3 分别对应 X, Y, Z 轴）
/// @param axis2 旋转轴2（1, 2, 3 分别对应 X, Y, Z 轴）
/// @param axis1 旋转轴1（1, 2, 3 分别对应 X, Y, Z 轴）
/// @param r 输出旋转矩阵
AST_SPICE_CAPI void eul2m(
    double     angle3,
    double     angle2,
    double     angle1,
    int        axis3,
    int        axis2,
    int        axis1,
    Matrix3d&  r
);

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

/// @brief 获取单位矩阵(Identity Matrix)
/// @param matrix 输出单位矩阵
AST_SPICE_CAPI void ident(Matrix3d& matrix);

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

/// @brief J1900.0历元(Julian Date of 1900.0 JAN 0.5)
AST_SPICE_CAPI double j1900();

/// @brief J1950.0历元(Julian Date of 1950.0 JAN 1.0)
AST_SPICE_CAPI double j1950();


/// @brief J2000.0历元(Julian Date of 2000 JAN 1.5)
AST_SPICE_CAPI double j2000();


/// @brief J2100.0历元(Julian Date of 2100 JAN 1.5)
AST_SPICE_CAPI double j2100();


/// @brief  儒略年秒数(Seconds per julian year)
AST_SPICE_CAPI double jyear();

// kclear

// kdata

// kinfo

// kplfrm

// ktotal

// kxtrct

// lastnb

// latcyl

/// @brief 将纬度经度转换为直角坐标(Latitudinal to rectangular coordinates)
/// @param[in] radius 半径
/// @param[in] lon 经度（弧度）
/// @param[in] lat 纬度（弧度）
/// @param[out] rectan 输出直角坐标（x, y, z）
AST_SPICE_CAPI 
void latrec(
    double    radius,
    double    lon,
    double    lat,
    Vector3d&  rectan
);

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

/// @brief 将旋转矩阵转换为欧拉角 (Matrix to Euler angles) 
/// @param r 旋转矩阵
/// @param axis3 第3轴（1, 2, 3 分别对应 X, Y, Z 轴）
/// @param axis2 第2轴（1, 2, 3 分别对应 X, Y, Z 轴）
/// @param axis1 第1轴（1, 2, 3 分别对应 X, Y, Z 轴）
/// @param angle3 输出第3轴的角度（弧度）
/// @param angle2 输出第2轴的角度（弧度）
/// @param angle1 输出第1轴的角度（弧度）
AST_SPICE_CAPI void  m2eul(
    const Matrix3d& r,
    int             axis3,
    int             axis2,
    int             axis1,
    double        & angle3,
    double        & angle2,
    double        & angle1
);

/// @brief 将旋转矩阵转换为四元数(Matrix to quaternion)
/// @param r 旋转矩阵
/// @param q 输出四元数
AST_SPICE_CAPI void m2q(
    const Matrix3d&  r,
    Quaternion&      q
);

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

/// @brief 矩阵乘法(Matrix times matrix, 3x3)
/// @param m1 矩阵1
/// @param m2 矩阵2
/// @param mout 输出矩阵
AST_SPICE_CAPI void mxm(
    const Matrix3d&   m1,
    const Matrix3d&   m2,
    Matrix3d&         mout 
);

/// @brief 矩阵向量乘法(Matrix times vector, general dimension)
/// @param m1 矩阵1
/// @param v2 向量2
/// @param nr1 矩阵1的行数
/// @param nc1r2 矩阵1的列数（等于向量2的长度）
/// @param vout 输出向量
AST_SPICE_CAPI void mxvg(
    const void   * m1,
    const void   * v2,
    int            nr1,
    int            nc1r2,
    void         * vout
);


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

/// @brief 计算从一个参考系统到另一个参考系统的旋转矩阵(Position Transformation Matrix)
/// @param from 源参考系统
/// @param to 目标参考系统
/// @param et 时间点
/// @param rotate 输出旋转矩阵
/// @return 
AST_SPICE_API err_t pxform(
    StringView          from,
    StringView          to,
    const TimePoint&    et,
    Matrix3d&           rotate 
);


// pxfrm2

/// @brief 将四元数转换为旋转矩阵(Quaternion to matrix)
/// @param q 四元数
/// @param r 输出旋转矩阵
AST_SPICE_CAPI void q2m(
    const Quaternion&  q,
    Matrix3d&          r 
);

// qcktrc

// qderiv

// qdq2av

// qxq

/// @brief 从距离、赤经和赤纬计算该点的直角坐标( Range, RA and DEC to rectangular coordinates ) 
/// 赤经从 +X 轴向 +Y 轴方向测量，赤纬是点相对于 XY 平面的角度。
/// @param range 距离
/// @param ra 赤经（弧度）
/// @param dec 赤纬（弧度）
/// @param rectan 输出直角坐标（x, y, z）
AST_SPICE_CAPI void radrec( 
    double range,
    double ra,
    double dec,
    Vector3d& rectan
);



AST_SPICE_CAPI void rav2xf( 
    const Matrix3d&     rot,
    const Vector3d&     av,
    Matrix6d&           xform
);

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

// spkapo

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

// spkpos

// spkpvn

// spksfs

// spkssb

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

// stlabx

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

// tipbod
AST_SPICE_API err_t tipbod(
    Axes            * ref,
    CelestialBody   * body,
    const TimePoint & et,
    Matrix3d&         tipm 
);


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

// utc2et

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



/*! @} */

AST_NAMESPACE_END
