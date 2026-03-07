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
A_CLINKAGE_BEGIN

/*!
    @addtogroup 
    @{
*/

void axisar(
    const double  axis[3],
    double        angle,
    double        r[3][3]
);

void azlrec(
    double     range,
    double     az,
    double     el,
    bool       azccw,
    bool       elplsz,
    double     rectan[3]
);

double b1900();

double b1950();

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

double dpr();

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

void eul2m(
    double  angle3,
    double  angle2,
    double  angle1,
    int     axis3,
    int     axis2,
    int     axis1,
    double  r [3][3]
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

void ident(double matrix[3][3]);

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

double j1900();

double j1950();

double j2000();

double j2100();

double jyear();

// kclear

// kdata

// kinfo

// kplfrm

// ktotal

// kxtrct

// lastnb

// latcyl

void latrec(
    double    radius,
    double    lon,
    double    lat,
    double    rectan[3]
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

void m2eul(
    const double    r[3][3],
    int             axis3,
    int             axis2,
    int             axis1,
    double        * angle3,
    double        * angle2,
    double        * angle1  
);


void m2q(
    const double  r[3][3],
    double       q[4]     
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

void mxm(
    const double  m1[3][3],
    const double  m2[3][3],
    double        mout[3][3] 
);

void mxvg(
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

// pxform
err_t pxform(
    const char   * from,
    const char   * to,
    double         et,
    double         rotate[3][3] 
);

// pxfrm2

void q2m(
    const double  q[4],
    double        r[3][3] 
);

// qcktrc

// qderiv

// qdq2av

// qxq

void radrec( 
    double range,
    double ra,
    double dec,
    double rectan[3] 
);

void rav2xf(
    const double rot   [3][3],
    const double av    [3],
    double       xform [6][6]  
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

err_t tipbod(
    const char      * ref,
    int               body,
    double            et,
    double            tipm[3][3]
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

A_CLINKAGE_END
AST_NAMESPACE_END
