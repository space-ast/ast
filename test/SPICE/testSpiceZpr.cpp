///
/// @file      testSpiceZpr.cpp
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

#define _USE_MATH_DEFINES
#include "AstSPICE/SpiceZpr.h"
#include "AstTest/Test.h"
#include "AstUtil/Literals.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/RunTime.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/MathOperator.hpp"
#include <cmath>
#include <cstdio>
#include <array>
#include <vector>

#ifndef AST_NO_CSPICE
#include "SpiceUsr.h"

AST_USING_NAMESPACE
using namespace _AST literals;


TEST(SpiceZpr, appndc)
{
}

TEST(SpiceZpr, appndd)
{
}

TEST(SpiceZpr, appndi)
{
}

TEST(SpiceZpr, axisar)
{
    static const struct {
        double axis[3];
        double angle;
    } testData[] = {
        {{1.0, 0.0, 0.0}, M_PI / 2.0},
        {{1.0, 1.0, 0.0}, M_PI / 4.0},
        {{-5.0, 0.0, 1.0}, M_PI / 8.0}
    };
    
    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
    {
        const auto& data = testData[i];
        double axis[3] = {data.axis[0], data.axis[1], data.axis[2]};
        double angle = data.angle;

        double r1[3][3], r2[3][3];
        axisar(axis, angle, r1);
        axisar_c(axis, angle, r2);
        // printf("r1 = \n");
        // printf("%lf, %lf, %lf\n", r1[0][0], r1[0][1], r1[0][2]);
        // printf("%lf, %lf, %lf\n", r1[1][0], r1[1][1], r1[1][2]);
        // printf("%lf, %lf, %lf\n", r1[2][0], r1[2][1], r1[2][2]);
        // printf("r2 = \n");
        // printf("%lf, %lf, %lf\n", r2[0][0], r2[0][1], r2[0][2]);
        // printf("%lf, %lf, %lf\n", r2[1][0], r2[1][1], r2[1][2]);
        // printf("%lf, %lf, %lf\n", r2[2][0], r2[2][1], r2[2][2]);
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                EXPECT_NEAR(r1[j][k], r2[j][k], 1e-12);
            }
        }
    }
}

TEST(SpiceZpr, azlcpo)
{
}

TEST(SpiceZpr, azlrec)
{
    static const struct {
        double range;
        double az;
        double el;
    } testData[] = {
        {1.0, 0.0, 0.0},
        {10.0, M_PI / 4.0, M_PI / 4.0},
        {5.0, M_PI / 2.0, M_PI / 2.0},
        {0.000,    0.000_deg,    0.000_deg},
        {1.000,    0.000_deg,    0.000_deg},
        {1.000,  270.000_deg,    0.000_deg},
        {1.000,    0.000_deg,  -90.000_deg},
        {1.000,  180.000_deg,    0.000_deg},
        {1.000,   90.000_deg,    0.000_deg},
        {1.000,    0.000_deg,   90.000_deg},
        {1.414,  315.000_deg,    0.000_deg},
        {1.414,    0.000_deg,  -45.000_deg},
        {1.414,  270.000_deg,  -45.000_deg},
        {1.732,  315.000_deg,  -35.264_deg},
    };
    
    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
    {
        const auto& data = testData[i];
        double range = data.range;
        double az = data.az;
        double el = data.el;
        
        static const struct {
            bool azccw;
            bool elplsz;
        } testData2[] = {
            {true, true},
            {false, true},
            {true, false},
            {false, false},
        };
        for (size_t j = 0; j < sizeof(testData2) / sizeof(testData2[0]); j++)
        {
            const auto& data = testData2[j];
            bool azccw = data.azccw;
            bool elplsz = data.elplsz;
            double rectan1[3], rectan2[3];
            azlrec(range, az, el, azccw, elplsz, rectan1);
            azlrec_c(range, az, el, azccw, elplsz, rectan2);
            
            for (int j = 0; j < 3; j++)
            {
                EXPECT_NEAR(rectan1[j], rectan2[j], 1e-12);
            }
        }
    }
}

TEST(SpiceZpr, b1900)
{
    EXPECT_EQ(b1900(), b1900_c());
}

TEST(SpiceZpr, b1950)
{
    EXPECT_EQ(b1950(), b1950_c());
}

TEST(SpiceZpr, badkpv)
{
}

TEST(SpiceZpr, bltfrm)
{
}

TEST(SpiceZpr, bodc2n)
{
}

TEST(SpiceZpr, bodc2s)
{
}

TEST(SpiceZpr, boddef)
{
}

TEST(SpiceZpr, bodfnd)
{
}

TEST(SpiceZpr, bodn2c)
{
}

TEST(SpiceZpr, bods2c)
{
}

TEST(SpiceZpr, bodvar)
{
}

TEST(SpiceZpr, bodvcd)
{
}

TEST(SpiceZpr, bodvrd)
{
}

TEST(SpiceZpr, brcktd)
{
}

TEST(SpiceZpr, brckti)
{
}

TEST(SpiceZpr, bschoc)
{
}

TEST(SpiceZpr, bschoi)
{
}

TEST(SpiceZpr, bsrchc)
{
}

TEST(SpiceZpr, bsrchd)
{
}

TEST(SpiceZpr, bsrchi)
{
}

TEST(SpiceZpr, card)
{
}

TEST(SpiceZpr, ccifrm)
{
}

TEST(SpiceZpr, cgv2el)
{
}

TEST(SpiceZpr, chbder)
{
}

TEST(SpiceZpr, chbigr)
{
}

TEST(SpiceZpr, chbint)
{
}

TEST(SpiceZpr, chbval)
{
}

TEST(SpiceZpr, chkin)
{
}

TEST(SpiceZpr, chkout)
{
}

TEST(SpiceZpr, cidfrm)
{
}

TEST(SpiceZpr, ckcls)
{
}

TEST(SpiceZpr, ckcov)
{
}

TEST(SpiceZpr, ckfrot)
{
}

TEST(SpiceZpr, ckfxfm)
{
}

TEST(SpiceZpr, ckgpav)
{
}

TEST(SpiceZpr, ckgp)
{
}

TEST(SpiceZpr, ckgr02)
{
}

TEST(SpiceZpr, ckgr03)
{
}

TEST(SpiceZpr, cklpf)
{
}

TEST(SpiceZpr, ckmeta)
{
}

TEST(SpiceZpr, cknr02)
{
}

TEST(SpiceZpr, cknr03)
{
}

TEST(SpiceZpr, ckobj)
{
}

TEST(SpiceZpr, ckopn)
{
}

TEST(SpiceZpr, ckupf)
{
}

TEST(SpiceZpr, ckw01)
{
}

TEST(SpiceZpr, ckw02)
{
}

TEST(SpiceZpr, ckw03)
{
}

TEST(SpiceZpr, ckw05)
{
}

TEST(SpiceZpr, clearc)
{
}

TEST(SpiceZpr, cleard)
{
}

TEST(SpiceZpr, cleari)
{
}

TEST(SpiceZpr, clight)
{
}

TEST(SpiceZpr, clpool)
{
}

TEST(SpiceZpr, cmprss)
{
}

TEST(SpiceZpr, cnmfrm)
{
}

TEST(SpiceZpr, conics)
{
}

TEST(SpiceZpr, convrt)
{
}

TEST(SpiceZpr, copy)
{
}

TEST(SpiceZpr, cposr)
{
}

TEST(SpiceZpr, cpos)
{
}

TEST(SpiceZpr, cvpool)
{
}

TEST(SpiceZpr, cyllat)
{
}

TEST(SpiceZpr, cylrec)
{
}

TEST(SpiceZpr, cylsph)
{
}

TEST(SpiceZpr, dafac)
{
}

TEST(SpiceZpr, dafbbs)
{
}

TEST(SpiceZpr, dafbfs)
{
}

TEST(SpiceZpr, dafcls)
{
}

TEST(SpiceZpr, dafcs)
{
}

TEST(SpiceZpr, dafdc)
{
}

TEST(SpiceZpr, dafec)
{
}

TEST(SpiceZpr, daffna)
{
}

TEST(SpiceZpr, daffpa)
{
}

TEST(SpiceZpr, dafgda)
{
}

TEST(SpiceZpr, dafgh)
{
}

TEST(SpiceZpr, dafgn)
{
}

TEST(SpiceZpr, dafgsr)
{
}

TEST(SpiceZpr, dafgs)
{
}

TEST(SpiceZpr, dafhsf)
{
}

TEST(SpiceZpr, dafopr)
{
}

TEST(SpiceZpr, dafopw)
{
}

TEST(SpiceZpr, dafps)
{
}

TEST(SpiceZpr, dafrda)
{
}

TEST(SpiceZpr, dafrfr)
{
}

TEST(SpiceZpr, dafrs)
{
}

TEST(SpiceZpr, dafus)
{
}

TEST(SpiceZpr, dasac)
{
}

TEST(SpiceZpr, dasadc)
{
}

TEST(SpiceZpr, dasadd)
{
}

TEST(SpiceZpr, dasadi)
{
}

TEST(SpiceZpr, dascls)
{
}

TEST(SpiceZpr, dasdc)
{
}

TEST(SpiceZpr, dasec)
{
}

TEST(SpiceZpr, dashfn)
{
}

TEST(SpiceZpr, dashfs)
{
}

TEST(SpiceZpr, daslla)
{
}

TEST(SpiceZpr, dasllc)
{
}

TEST(SpiceZpr, dasonw)
{
}

TEST(SpiceZpr, dasopr)
{
}

TEST(SpiceZpr, dasops)
{
}

TEST(SpiceZpr, dasopw)
{
}

TEST(SpiceZpr, dasrdc)
{
}

TEST(SpiceZpr, dasrdd)
{
}

TEST(SpiceZpr, dasrdi)
{
}

TEST(SpiceZpr, dasrfr)
{
}

TEST(SpiceZpr, dasudc)
{
}

TEST(SpiceZpr, dasudd)
{
}

TEST(SpiceZpr, dasudi)
{
}

TEST(SpiceZpr, daswbr)
{
}

TEST(SpiceZpr, dazldr)
{
}

TEST(SpiceZpr, dcyldr)
{
}

TEST(SpiceZpr, deltet)
{
}

TEST(SpiceZpr, det)
{
}

TEST(SpiceZpr, dgeodr)
{
}

TEST(SpiceZpr, diags2)
{
}

TEST(SpiceZpr, diff)
{
}

TEST(SpiceZpr, dlabbs)
{
}

TEST(SpiceZpr, dlabfs)
{
}

TEST(SpiceZpr, dlabns)
{
}

TEST(SpiceZpr, dlaens)
{
}

TEST(SpiceZpr, dlafns)
{
}

TEST(SpiceZpr, dlafps)
{
}

TEST(SpiceZpr, dlaopn)
{
}

TEST(SpiceZpr, dlatdr)
{
}

TEST(SpiceZpr, dnearp)
{
}

TEST(SpiceZpr, dp2hx)
{
}

TEST(SpiceZpr, dpgrdr)
{
}

TEST(SpiceZpr, dpmax)
{
}

TEST(SpiceZpr, dpmin)
{
}


TEST(SpiceZpr, dpr)
{
    EXPECT_EQ(dpr(), dpr_c());
}

TEST(SpiceZpr, drdazl)
{
}

TEST(SpiceZpr, drdcyl)
{
}

TEST(SpiceZpr, drdgeo)
{
}

TEST(SpiceZpr, drdlat)
{
}

TEST(SpiceZpr, drdpgr)
{
}

TEST(SpiceZpr, drdsph)
{
}

TEST(SpiceZpr, dskb02)
{
}

TEST(SpiceZpr, dskcls)
{
}

TEST(SpiceZpr, dskd02)
{
}

TEST(SpiceZpr, dskgd)
{
}

TEST(SpiceZpr, dskgtl)
{
}

TEST(SpiceZpr, dski02)
{
}

TEST(SpiceZpr, dskmi2)
{
}

TEST(SpiceZpr, dskn02)
{
}

TEST(SpiceZpr, dskobj)
{
}

TEST(SpiceZpr, dskopn)
{
}

TEST(SpiceZpr, dskp02)
{
}

TEST(SpiceZpr, dskrb2)
{
}

TEST(SpiceZpr, dsksrf)
{
}

TEST(SpiceZpr, dskstl)
{
}

TEST(SpiceZpr, dskv02)
{
}

TEST(SpiceZpr, dskw02)
{
}

TEST(SpiceZpr, dskx02)
{
}

TEST(SpiceZpr, dskxsi)
{
}

TEST(SpiceZpr, dskxv)
{
}

TEST(SpiceZpr, dskz02)
{
}

TEST(SpiceZpr, dsphdr)
{
}

TEST(SpiceZpr, dtpool)
{
}

TEST(SpiceZpr, ducrss)
{
}

TEST(SpiceZpr, dvcrss)
{
}

TEST(SpiceZpr, dvdot)
{
}

TEST(SpiceZpr, dvhat)
{
}

TEST(SpiceZpr, dvnorm)
{
}

TEST(SpiceZpr, dvpool)
{
}

TEST(SpiceZpr, dvsep)
{
}

TEST(SpiceZpr, edlimb)
{
}

TEST(SpiceZpr, ednmpt)
{
}

TEST(SpiceZpr, edpnt)
{
}

TEST(SpiceZpr, edterm)
{
}

TEST(SpiceZpr, ekacec)
{
}

TEST(SpiceZpr, ekaced)
{
}

TEST(SpiceZpr, ekacei)
{
}

TEST(SpiceZpr, ekaclc)
{
}

TEST(SpiceZpr, ekacld)
{
}

TEST(SpiceZpr, ekacli)
{
}

TEST(SpiceZpr, ekappr)
{
}

TEST(SpiceZpr, ekbseg)
{
}

TEST(SpiceZpr, ekccnt)
{
}

TEST(SpiceZpr, ekcii)
{
}

TEST(SpiceZpr, ekcls)
{
}

TEST(SpiceZpr, ekdelr)
{
}

TEST(SpiceZpr, ekffld)
{
}

TEST(SpiceZpr, ekfind)
{
}

TEST(SpiceZpr, ekgc)
{
}

TEST(SpiceZpr, ekgd)
{
}

TEST(SpiceZpr, ekgi)
{
}

TEST(SpiceZpr, ekifld)
{
}

TEST(SpiceZpr, ekinsr)
{
}

TEST(SpiceZpr, eklef)
{
}

TEST(SpiceZpr, eknelt)
{
}

TEST(SpiceZpr, eknseg)
{
}

TEST(SpiceZpr, ekntab)
{
}

TEST(SpiceZpr, ekopn)
{
}

TEST(SpiceZpr, ekopr)
{
}

TEST(SpiceZpr, ekops)
{
}

TEST(SpiceZpr, ekopw)
{
}

TEST(SpiceZpr, ekpsel)
{
}

TEST(SpiceZpr, ekrcec)
{
}

TEST(SpiceZpr, ekrced)
{
}

TEST(SpiceZpr, ekrcei)
{
}

TEST(SpiceZpr, ekssum)
{
}

TEST(SpiceZpr, ektnam)
{
}

TEST(SpiceZpr, ekucec)
{
}

TEST(SpiceZpr, ekuced)
{
}

TEST(SpiceZpr, ekucei)
{
}

TEST(SpiceZpr, ekuef)
{
}

TEST(SpiceZpr, el2cgv)
{
}

TEST(SpiceZpr, elemc)
{
}

TEST(SpiceZpr, elemd)
{
}

TEST(SpiceZpr, elemi)
{
}

TEST(SpiceZpr, eqncpv)
{
}

TEST(SpiceZpr, eqstr)
{
}

TEST(SpiceZpr, erract)
{
}

TEST(SpiceZpr, errch)
{
}

TEST(SpiceZpr, errdev)
{
}

TEST(SpiceZpr, errdp)
{
}

TEST(SpiceZpr, errint)
{
}

TEST(SpiceZpr, errprt)
{
}

TEST(SpiceZpr, esrchc)
{
}

TEST(SpiceZpr, et2lst)
{
}

TEST(SpiceZpr, et2utc)
{
}

TEST(SpiceZpr, etcal)
{
}

TEST(SpiceZpr, eul2m)
{
    static const struct{
        double angle3;
        double angle2;
        double angle1;
    } testData[] = {
        {0.0, 0.0, 0.0},
        {dpr(), 0.0, 0.0},
        {0.0, dpr(), 0.0},
        {0.0, 0.0, dpr()},
        {1,2,3},
        {4,-5,6},
        {-4,1,3.3}
    };
    for(auto& data : testData)
    {
        static const struct{
            int axis3;
            int axis2;
            int axis1;
        } testData2[] = {
            {1, 2, 3},
            {1, 3, 2},
            {2, 1, 3},
            {2, 3, 1},
            {3, 1, 2},
            {3, 2, 1},
            {1, 2, 1},
            {1, 3, 1},
            {2, 1, 2},
            {2, 3, 2},
            {3, 1, 3},
            {3, 2, 3}
        };
        double mtx[3][3];
        double mtx_c[3][3];
        for(auto& data2: testData2){
            eul2m(data.angle3, data.angle2, data.angle1, data2.axis3, data2.axis2, data2.axis1, mtx);
            eul2m_c(data.angle3, data.angle2, data.angle1, data2.axis3, data2.axis2, data2.axis1, mtx_c);
            for(int i = 0; i < 3; i++){
                for(int j = 0; j < 3; j++){
                    EXPECT_NEAR(mtx[i][j], mtx_c[i][j], 1e-12);
                }
            }
        }
    }
}

TEST(SpiceZpr, eul2xf)
{
}

TEST(SpiceZpr, evsgp4)
{
}

TEST(SpiceZpr, exists)
{
}

TEST(SpiceZpr, expool)
{
}

TEST(SpiceZpr, failed)
{
}

TEST(SpiceZpr, filld)
{
}

TEST(SpiceZpr, filli)
{
}

TEST(SpiceZpr, fovray)
{
}

TEST(SpiceZpr, fovtrg)
{
}

TEST(SpiceZpr, frame)
{
}

TEST(SpiceZpr, frinfo)
{
}

TEST(SpiceZpr, frmnam)
{
}

TEST(SpiceZpr, ftncls)
{
}

TEST(SpiceZpr, furnsh)
{
}

TEST(SpiceZpr, gcpool)
{
}

TEST(SpiceZpr, gdpool)
{
}

TEST(SpiceZpr, georec)
{
}

TEST(SpiceZpr, getcml)
{
}

TEST(SpiceZpr, getelm)
{
}

TEST(SpiceZpr, getfat)
{
}

TEST(SpiceZpr, getfov)
{
}

TEST(SpiceZpr, getfvn)
{
}

TEST(SpiceZpr, getmsg)
{
}

TEST(SpiceZpr, gfbail)
{
}

TEST(SpiceZpr, gfclrh)
{
}

TEST(SpiceZpr, gfdist)
{
}

TEST(SpiceZpr, gfevnt)
{
}

TEST(SpiceZpr, gffove)
{
}

TEST(SpiceZpr, gfilum)
{
}

TEST(SpiceZpr, gfinth)
{
}

TEST(SpiceZpr, gfocce)
{
}

TEST(SpiceZpr, gfoclt)
{
}

TEST(SpiceZpr, gfpa)
{
}

TEST(SpiceZpr, gfposc)
{
}

TEST(SpiceZpr, gfrefn)
{
}

TEST(SpiceZpr, gfrepf)
{
}

TEST(SpiceZpr, gfrepi)
{
}

TEST(SpiceZpr, gfrepu)
{
}

TEST(SpiceZpr, gfrfov)
{
}

TEST(SpiceZpr, gfrr)
{
}

TEST(SpiceZpr, gfsep)
{
}

TEST(SpiceZpr, gfsntc)
{
}

TEST(SpiceZpr, gfsstp)
{
}

TEST(SpiceZpr, gfstep)
{
}

TEST(SpiceZpr, gfstol)
{
}

TEST(SpiceZpr, gfsubc)
{
}

TEST(SpiceZpr, gftfov)
{
}

TEST(SpiceZpr, gfudb)
{
}

TEST(SpiceZpr, gfuds)
{
}

TEST(SpiceZpr, gipool)
{
}

TEST(SpiceZpr, gnpool)
{
}

TEST(SpiceZpr, halfpi)
{
}

TEST(SpiceZpr, hrmesp)
{
}

TEST(SpiceZpr, hrmint)
{
}

TEST(SpiceZpr, hx2dp)
{
}

TEST(SpiceZpr, ident)
{
    double mtx1[3][3];
    double mtx2[3][3];
    ident(mtx1);
    ident_c(mtx2);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            EXPECT_NEAR(mtx1[i][j], mtx2[i][j], 1e-12);
        }
    }
}

TEST(SpiceZpr, illumf)
{
}

TEST(SpiceZpr, illumg)
{
}

TEST(SpiceZpr, illum)
{
}

TEST(SpiceZpr, illum_pl02)
{
}

TEST(SpiceZpr, illum_plid_pl02)
{
}

TEST(SpiceZpr, ilumin)
{
}

TEST(SpiceZpr, index)
{
}

TEST(SpiceZpr, inedpl)
{
}

TEST(SpiceZpr, inelpl)
{
}

TEST(SpiceZpr, inrypl)
{
}

TEST(SpiceZpr, insrtc)
{
}

TEST(SpiceZpr, insrtd)
{
}

TEST(SpiceZpr, insrti)
{
}

TEST(SpiceZpr, inter)
{
}

TEST(SpiceZpr, intmax)
{
}

TEST(SpiceZpr, intmin)
{
}

TEST(SpiceZpr, invert)
{
}

TEST(SpiceZpr, invort)
{
}

TEST(SpiceZpr, invstm)
{
}

TEST(SpiceZpr, isordv)
{
}

TEST(SpiceZpr, isrchc)
{
}

TEST(SpiceZpr, isrchd)
{
}

TEST(SpiceZpr, isrchi)
{
}

TEST(SpiceZpr, isrot)
{
}

TEST(SpiceZpr, iswhsp)
{
}


TEST(SpiceZpr, j1900)
{
    EXPECT_EQ(j1900(), j1900_c());
}

TEST(SpiceZpr, j1950)
{
    EXPECT_EQ(j1950(), j1950_c());
}


TEST(SpiceZpr, j2000)
{
    EXPECT_EQ(j2000(), j2000_c());
}


TEST(SpiceZpr, j2100)
{
    EXPECT_EQ(j2100(), j2100_c());
}

TEST(SpiceZpr, jyear)
{
    EXPECT_EQ(jyear(), jyear_c());
}

TEST(SpiceZpr, kclear)
{
}

TEST(SpiceZpr, kdata)
{
}

TEST(SpiceZpr, kinfo)
{
}

TEST(SpiceZpr, kplfrm)
{
}

TEST(SpiceZpr, ktotal)
{
}

TEST(SpiceZpr, kxtrct)
{
}

TEST(SpiceZpr, lastnb)
{
}

TEST(SpiceZpr, latcyl)
{
}



TEST(SpiceZpr, latrec)
{
    static const struct {
        double radius;
        double lon;
        double lat;
    } testData[] = {
        {1.0, 0.0, 0.0},
        {5.0, M_PI / 4.0, M_PI / 4.0},
        {10.0, M_PI / 2.0, M_PI / 2.0},
        {0.000,    0.000_deg,    0.000_deg},
        {1.000,    0.000_deg,    0.000_deg},
        {1.000,   90.000_deg,    0.000_deg},
        {1.000,    0.000_deg,   90.000_deg},
        {1.000,  180.000_deg,    0.000_deg},
        {1.000,  -90.000_deg,    0.000_deg},
        {1.000,    0.000_deg,  -90.000_deg},
        {1.414,   45.000_deg,    0.000_deg},
        {1.414,    0.000_deg,   45.000_deg},
        {1.414,   90.000_deg,   45.000_deg},
        {1.732,   45.000_deg,   35.264_deg},
    };
    
    for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
    {
        const auto& data = testData[i];
        double radius = data.radius;
        double lon = data.lon;
        double lat = data.lat;
        
        double rectan1[3], rectan2[3];
        latrec(radius, lon, lat, rectan1);
        latrec_c(radius, lon, lat, rectan2);
        
        // printf("latrec test %zu:\n", i+1);
        // printf("radius: %lf, lon: %lf, lat: %lf\n", radius, lon, lat);
        // printf("rectan1: %lf, %lf, %lf\n", rectan1[0], rectan1[1], rectan1[2]);
        // printf("rectan2: %lf, %lf, %lf\n", rectan2[0], rectan2[1], rectan2[2]);
        
        for (int j = 0; j < 3; j++)
        {
            EXPECT_NEAR(rectan1[j], rectan2[j], 1e-12);
        }
    }
}

TEST(SpiceZpr, latsph)
{
}

TEST(SpiceZpr, latsrf)
{
}

TEST(SpiceZpr, lcase)
{
}

TEST(SpiceZpr, ldpool)
{
}

TEST(SpiceZpr, lgresp)
{
}

TEST(SpiceZpr, lgrind)
{
}

TEST(SpiceZpr, lgrint)
{
}

TEST(SpiceZpr, limbpt)
{
}

TEST(SpiceZpr, limb_pl02)
{
}

TEST(SpiceZpr, llgrid_pl02)
{
}

TEST(SpiceZpr, lmpool)
{
}

TEST(SpiceZpr, lparse)
{
}

TEST(SpiceZpr, lparsm)
{
}

TEST(SpiceZpr, lparss)
{
}

TEST(SpiceZpr, lspcn)
{
}

TEST(SpiceZpr, lstlec)
{
}

TEST(SpiceZpr, lstled)
{
}

TEST(SpiceZpr, lstlei)
{
}

TEST(SpiceZpr, lstltc)
{
}

TEST(SpiceZpr, lstltd)
{
}

TEST(SpiceZpr, lstlti)
{
}

TEST(SpiceZpr, ltime)
{
}

TEST(SpiceZpr, lx4dec)
{
}

TEST(SpiceZpr, lx4num)
{
}

TEST(SpiceZpr, lx4sgn)
{
}

TEST(SpiceZpr, lx4uns)
{
}

TEST(SpiceZpr, lxqstr)
{
}

TEST(SpiceZpr, m2eul)
{
    aInitialize();
    double mtx[3][3];
    aICRFToECFMatrix(TimePoint::FromUTC(2022, 1, 2, 3, 0, 0), mtx);
    // printf("mtx:\n");
    // for(int i = 0; i < 3; i++)
    // {
    //     for(int j = 0; j < 3; j++)
    //     {
    //         printf("%lf ", mtx[i][j]);
    //     }
    //     printf("\n");
    // }

    static const struct{
        int axis3;
        int axis2;
        int axis1;
    } testData[] = {
        {1, 2, 3},
        {1, 3, 2},
        {2, 1, 3},
        {2, 3, 1},
        {3, 1, 2},
        {3, 2, 1},
        {1, 2, 1},
        {1, 3, 1},
        {2, 1, 2},
        {2, 3, 2},
        {3, 1, 3},
        {3, 2, 3}
    };
    for(auto& data: testData)
    {
        double angle3, angle2, angle1;
        double angle3_c, angle2_c, angle1_c;
        m2eul(mtx, data.axis3, data.axis2, data.axis1, &angle3, &angle2, &angle1);
        m2eul_c(mtx, data.axis3, data.axis2, data.axis1, &angle3_c, &angle2_c, &angle1_c);
        printf("angle3: %lf, angle3_c: %lf\n", angle3, angle3_c);
        printf("angle2: %lf, angle2_c: %lf\n", angle2, angle2_c);
        printf("angle1: %lf, angle1_c: %lf\n", angle1, angle1_c);
        EXPECT_NEAR(angle3, angle3_c, 1e-12);
        EXPECT_NEAR(angle2, angle2_c, 1e-12);
        EXPECT_NEAR(angle1, angle1_c, 1e-12);
    }
}

TEST(SpiceZpr, m2q)
{
}

TEST(SpiceZpr, matchi)
{
}

TEST(SpiceZpr, matchw)
{
}

TEST(SpiceZpr, maxd)
{
}

TEST(SpiceZpr, maxi)
{
}

TEST(SpiceZpr, mequg)
{
}

TEST(SpiceZpr, mequ)
{
}

TEST(SpiceZpr, mind)
{
}

TEST(SpiceZpr, mini)
{
}

TEST(SpiceZpr, moved)
{
}

TEST(SpiceZpr, mtxmg)
{
}

TEST(SpiceZpr, mtxm)
{
}

TEST(SpiceZpr, mtxvg)
{
}

TEST(SpiceZpr, mtxv)
{
}

TEST(SpiceZpr, mxmg)
{
}

TEST(SpiceZpr, mxmtg)
{
}

TEST(SpiceZpr, mxmt)
{
}

TEST(SpiceZpr, mxm)
{
}

TEST(SpiceZpr, mxvg)
{
}

TEST(SpiceZpr, mxv)
{
}

TEST(SpiceZpr, namfrm)
{
}

TEST(SpiceZpr, ncposr)
{
}

TEST(SpiceZpr, ncpos)
{
}

TEST(SpiceZpr, nearpt)
{
}

TEST(SpiceZpr, nextwd)
{
}

TEST(SpiceZpr, npedln)
{
}

TEST(SpiceZpr, npelpt)
{
}

TEST(SpiceZpr, nplnpt)
{
}

TEST(SpiceZpr, nthwd)
{
}

TEST(SpiceZpr, nvc2pl)
{
}

TEST(SpiceZpr, nvp2pl)
{
}

TEST(SpiceZpr, occult)
{
}

TEST(SpiceZpr, ordc)
{
}

TEST(SpiceZpr, ordd)
{
}

TEST(SpiceZpr, orderc)
{
}

TEST(SpiceZpr, orderd)
{
}

TEST(SpiceZpr, orderi)
{
}

TEST(SpiceZpr, ordi)
{
}

TEST(SpiceZpr, oscelt)
{
}

TEST(SpiceZpr, oscltx)
{
}

TEST(SpiceZpr, pckcls)
{
}

TEST(SpiceZpr, pckcov)
{
}

TEST(SpiceZpr, pckfrm)
{
}

TEST(SpiceZpr, pcklof)
{
}

TEST(SpiceZpr, pckopn)
{
}

TEST(SpiceZpr, pckuof)
{
}

TEST(SpiceZpr, pckw02)
{
}

TEST(SpiceZpr, pcpool)
{
}

TEST(SpiceZpr, pdpool)
{
}

TEST(SpiceZpr, pgrrec)
{
}

TEST(SpiceZpr, phaseq)
{
}

TEST(SpiceZpr, pipool)
{
}

TEST(SpiceZpr, pi)
{
}

TEST(SpiceZpr, pjelpl)
{
}

TEST(SpiceZpr, pl2nvc)
{
}

TEST(SpiceZpr, pl2nvp)
{
}

TEST(SpiceZpr, pl2psv)
{
}

TEST(SpiceZpr, pltar)
{
}

TEST(SpiceZpr, pltexp)
{
}

TEST(SpiceZpr, pltnp)
{
}

TEST(SpiceZpr, pltnrm)
{
}

TEST(SpiceZpr, pltvol)
{
}

TEST(SpiceZpr, polyds)
{
}

TEST(SpiceZpr, posr)
{
}

TEST(SpiceZpr, pos)
{
}

TEST(SpiceZpr, prompt)
{
}

TEST(SpiceZpr, prop2b)
{
}

TEST(SpiceZpr, prsdp)
{
}

TEST(SpiceZpr, prsint)
{
}

TEST(SpiceZpr, psv2pl)
{
}

TEST(SpiceZpr, putcml)
{
}

TEST(SpiceZpr, pxform)
{
}

TEST(SpiceZpr, pxfrm2)
{
}

TEST(SpiceZpr, q2m)
{
}

TEST(SpiceZpr, qcktrc)
{
}

TEST(SpiceZpr, qderiv)
{
}

TEST(SpiceZpr, qdq2av)
{
}

TEST(SpiceZpr, qxq)
{
}

TEST(SpiceZpr, radrec)
{
    static const struct {
        double range;
        double ra;
        double dec;
    } testData[] = {
        {1.0, 0.0, 0.0},
        {5.0, M_PI / 4.0, M_PI / 4.0},
        {10.0, M_PI / 2.0, M_PI / 2.0},
        {0.000,    0.000_deg,    0.000_deg},
        {1.000,    0.000_deg,    0.000_deg},
        {1.000,   90.000_deg,    0.000_deg},
        {1.000,    0.000_deg,   90.000_deg},
        {1.000,  180.000_deg,    0.000_deg},
        {1.000,  -90.000_deg,    0.000_deg},
        {1.000,    0.000_deg,  -90.000_deg},
        {1.414,   45.000_deg,    0.000_deg},
        {1.414,    0.000_deg,   45.000_deg},
        {1.414,   90.000_deg,   45.000_deg},
        {1.732,   45.000_deg,   35.264_deg},
    };
    
    for(auto& data: testData)
    {
        double rectan[3];
        double rectanRef[3];
        radrec(data.range, data.ra, data.dec, rectan);
        radrec_c(data.range, data.ra, data.dec, rectanRef);
        EXPECT_NEAR(rectan[0], rectanRef[0], 1e-12);
        EXPECT_NEAR(rectan[1], rectanRef[1], 1e-12);
        EXPECT_NEAR(rectan[2], rectanRef[2], 1e-12);
    }
}

TEST(SpiceZpr, rav2xf)
{
    static const struct{
        double axis[3];
        double angle;
    } testData[]{
        {{1.0, 0.0, 0.0}, 2.0},
        {{0.0, 1.0, 0.0}, 3.0},
        {{0.0, 1.0, 0.0}, 4.0},
        {{3.0, 0.0, 1.0}, 5.0},
        {{1.0, -2.0, 3.0}, 7.0},
    };
    for(auto& data: testData)
    {
        double mtx[3][3];
        double xform[6][6];
        double xformRef[6][6];

        axisar(data.axis, data.angle, mtx);
        rav2xf_c(mtx, data.axis, xform);
        rav2xf_c(mtx, data.axis, xformRef);
        for(int i = 0; i < 6; i++)
        {
            for(int j = 0; j < 6; j++)
            {
                EXPECT_NEAR(xform[i][j], xformRef[i][j], 1e-12);
            }
        }
    }
    {
        aInitialize();
        KinematicRotation kr;
        aICRFToECFTransform(TimePoint::FromUTC(2010, 5, 1, 0, 0, 0), kr);
        Vector3d pos{1,2,3}, vel{4,5,6};
        Vector3d posOut, velOut;
        kr.transformVectorVelocity(pos, vel, posOut, velOut);

        printf("posVelOut:\n");
        printf("%12.6f %12.6f %12.6f\n", posOut[0], posOut[1], posOut[2]);
        printf("%12.6f %12.6f %12.6f\n", velOut[0], velOut[1], velOut[2]);

        double xform[6][6];
        rav2xf_c(kr.getRotation().getMatrix().data(), kr.getRotationRate().data(), xform);
        double state[6]{pos[0], pos[1], pos[2], vel[0], vel[1], vel[2]};
        double stateOut[6];
        mxvg(xform, state, 6, 6, stateOut);
        
        printf("stateOut:\n");
        for(int i = 0; i < 6; i++)
        {
            printf("%12.6f ", stateOut[i]);
        }
        printf("\n");

        EXPECT_NEAR(stateOut[0], posOut[0], 1e-12);
        EXPECT_NEAR(stateOut[1], posOut[1], 1e-12);
        EXPECT_NEAR(stateOut[2], posOut[2], 1e-12);
        EXPECT_NEAR(stateOut[3], velOut[0], 1e-12);
        EXPECT_NEAR(stateOut[4], velOut[1], 1e-12);
        EXPECT_NEAR(stateOut[5], velOut[2], 1e-12);
    }
}

TEST(SpiceZpr, raxisa)
{
}

TEST(SpiceZpr, rdtext)
{
}

TEST(SpiceZpr, recazl)
{
}

TEST(SpiceZpr, reccyl)
{
}

TEST(SpiceZpr, recgeo)
{
}

TEST(SpiceZpr, reclat)
{
}

TEST(SpiceZpr, recpgr)
{
}

TEST(SpiceZpr, recrad)
{
}

TEST(SpiceZpr, recsph)
{
}

TEST(SpiceZpr, removc)
{
}

TEST(SpiceZpr, removd)
{
}

TEST(SpiceZpr, removi)
{
}

TEST(SpiceZpr, reordc)
{
}

TEST(SpiceZpr, reordd)
{
}

TEST(SpiceZpr, reordi)
{
}

TEST(SpiceZpr, reordl)
{
}

TEST(SpiceZpr, repmct)
{
}

TEST(SpiceZpr, repmc)
{
}

TEST(SpiceZpr, repmd)
{
}

TEST(SpiceZpr, repmf)
{
}

TEST(SpiceZpr, repmi)
{
}

TEST(SpiceZpr, repml)
{
}

TEST(SpiceZpr, repmot)
{
}

TEST(SpiceZpr, reset)
{
}

TEST(SpiceZpr, return)
{
}

TEST(SpiceZpr, rotate)
{
}

TEST(SpiceZpr, rotmat)
{
}

TEST(SpiceZpr, rotvec)
{
}

TEST(SpiceZpr, rpd)
{
}

TEST(SpiceZpr, rquad)
{
}

TEST(SpiceZpr, saelgv)
{
}

TEST(SpiceZpr, scard)
{
}

TEST(SpiceZpr, scdecd)
{
}

TEST(SpiceZpr, sce2c)
{
}

TEST(SpiceZpr, sce2s)
{
}

TEST(SpiceZpr, sce2t)
{
}

TEST(SpiceZpr, scencd)
{
}

TEST(SpiceZpr, scfmt)
{
}

TEST(SpiceZpr, scpart)
{
}

TEST(SpiceZpr, scs2e)
{
}

TEST(SpiceZpr, sct2e)
{
}

TEST(SpiceZpr, sctiks)
{
}

TEST(SpiceZpr, sdiff)
{
}

TEST(SpiceZpr, setmsg)
{
}

TEST(SpiceZpr, set)
{
}

TEST(SpiceZpr, shellc)
{
}

TEST(SpiceZpr, shelld)
{
}

TEST(SpiceZpr, shelli)
{
}

TEST(SpiceZpr, sigerr)
{
}

TEST(SpiceZpr, sincpt)
{
}

TEST(SpiceZpr, size)
{
}

TEST(SpiceZpr, spd)
{
}

TEST(SpiceZpr, sphcyl)
{
}

TEST(SpiceZpr, sphlat)
{
}

TEST(SpiceZpr, sphrec)
{
}

TEST(SpiceZpr, spk14a)
{
}

TEST(SpiceZpr, spk14b)
{
}

TEST(SpiceZpr, spk14e)
{
}

TEST(SpiceZpr, spkacs)
{
}

TEST(SpiceZpr, spkapo)
{
}

TEST(SpiceZpr, spkapp)
{
}

TEST(SpiceZpr, spkaps)
{
}

TEST(SpiceZpr, spkcls)
{
}

TEST(SpiceZpr, spkcov)
{
}

TEST(SpiceZpr, spkcpo)
{
}

TEST(SpiceZpr, spkcpt)
{
}

TEST(SpiceZpr, spkcvo)
{
}

TEST(SpiceZpr, spkcvt)
{
}

TEST(SpiceZpr, spkezp)
{
}

TEST(SpiceZpr, spkezr)
{
}

TEST(SpiceZpr, spkez)
{
}

TEST(SpiceZpr, spkgeo)
{
}

TEST(SpiceZpr, spkgps)
{
}

TEST(SpiceZpr, spklef)
{
}

TEST(SpiceZpr, spkltc)
{
}

TEST(SpiceZpr, spkobj)
{
}

TEST(SpiceZpr, spkopa)
{
}

TEST(SpiceZpr, spkopn)
{
}

TEST(SpiceZpr, spkpds)
{
}

TEST(SpiceZpr, spkpos)
{
}

TEST(SpiceZpr, spkpvn)
{
}

TEST(SpiceZpr, spksfs)
{
}

TEST(SpiceZpr, spkssb)
{
}

TEST(SpiceZpr, spksub)
{
}

TEST(SpiceZpr, spkuds)
{
}

TEST(SpiceZpr, spkuef)
{
}

TEST(SpiceZpr, spkw02)
{
}

TEST(SpiceZpr, spkw03)
{
}

TEST(SpiceZpr, spkw05)
{
}

TEST(SpiceZpr, spkw08)
{
}

TEST(SpiceZpr, spkw09)
{
}

TEST(SpiceZpr, spkw10)
{
}

TEST(SpiceZpr, spkw12)
{
}

TEST(SpiceZpr, spkw13)
{
}

TEST(SpiceZpr, spkw15)
{
}

TEST(SpiceZpr, spkw17)
{
}

TEST(SpiceZpr, spkw18)
{
}

TEST(SpiceZpr, spkw20)
{
}

TEST(SpiceZpr, srfc2s)
{
}

TEST(SpiceZpr, srfcss)
{
}

TEST(SpiceZpr, srfnrm)
{
}

TEST(SpiceZpr, srfrec)
{
}

TEST(SpiceZpr, srfs2c)
{
}

TEST(SpiceZpr, srfscc)
{
}

TEST(SpiceZpr, srfxpt)
{
}

TEST(SpiceZpr, ssize)
{
}

TEST(SpiceZpr, stelab)
{
}

TEST(SpiceZpr, stlabx)
{
}

TEST(SpiceZpr, stpool)
{
}

TEST(SpiceZpr, str2et)
{
}

TEST(SpiceZpr, subpnt)
{
}

TEST(SpiceZpr, subpt)
{
}

TEST(SpiceZpr, subpt_pl02)
{
}

TEST(SpiceZpr, subslr)
{
}

TEST(SpiceZpr, subsol)
{
}

TEST(SpiceZpr, subsol_pl02)
{
}

TEST(SpiceZpr, sumad)
{
}

TEST(SpiceZpr, sumai)
{
}

TEST(SpiceZpr, surfnm)
{
}

TEST(SpiceZpr, surfpt)
{
}

TEST(SpiceZpr, surfpv)
{
}

TEST(SpiceZpr, swpool)
{
}

TEST(SpiceZpr, sxform)
{
}

TEST(SpiceZpr, szpool)
{
}

TEST(SpiceZpr, tangpt)
{
}

TEST(SpiceZpr, termpt)
{
}

TEST(SpiceZpr, term_pl02)
{
}

TEST(SpiceZpr, timdef)
{
}

TEST(SpiceZpr, timout)
{
}

TEST(SpiceZpr, tipbod)
{
}

TEST(SpiceZpr, tisbod)
{
}

TEST(SpiceZpr, tkfram)
{
}

TEST(SpiceZpr, tkvrsn)
{
}

TEST(SpiceZpr, tparch)
{
}

TEST(SpiceZpr, tparse)
{
}

TEST(SpiceZpr, tpictr)
{
}

TEST(SpiceZpr, trace)
{
}

TEST(SpiceZpr, trcdep)
{
}

TEST(SpiceZpr, trcnam)
{
}

TEST(SpiceZpr, trcoff)
{
}

TEST(SpiceZpr, trgsep)
{
}

TEST(SpiceZpr, tsetyr)
{
}

TEST(SpiceZpr, twopi)
{
}

TEST(SpiceZpr, twovec)
{
}

TEST(SpiceZpr, twovxf)
{
}

TEST(SpiceZpr, tyear)
{
}

TEST(SpiceZpr, ucase)
{
}

TEST(SpiceZpr, ucrss)
{
}

TEST(SpiceZpr, uddc)
{
}

TEST(SpiceZpr, uddf)
{
}

TEST(SpiceZpr, udf)
{
}

TEST(SpiceZpr, union)
{
}

TEST(SpiceZpr, unitim)
{
}

TEST(SpiceZpr, unload)
{
}

TEST(SpiceZpr, unormg)
{
}

TEST(SpiceZpr, unorm)
{
}

TEST(SpiceZpr, utc2et)
{
}

TEST(SpiceZpr, vaddg)
{
}

TEST(SpiceZpr, vadd)
{
}

TEST(SpiceZpr, valid)
{
}

TEST(SpiceZpr, vcrss)
{
}

TEST(SpiceZpr, vdistg)
{
}

TEST(SpiceZpr, vdist)
{
}

TEST(SpiceZpr, vdotg)
{
}

TEST(SpiceZpr, vdot)
{
}

TEST(SpiceZpr, vequg)
{
}

TEST(SpiceZpr, vequ)
{
}

TEST(SpiceZpr, vhatg)
{
}

TEST(SpiceZpr, vhat)
{
}

TEST(SpiceZpr, vlcom3)
{
}

TEST(SpiceZpr, vlcomg)
{
}

TEST(SpiceZpr, vlcom)
{
}

TEST(SpiceZpr, vminug)
{
}

TEST(SpiceZpr, vminus)
{
}

TEST(SpiceZpr, vnormg)
{
}

TEST(SpiceZpr, vnorm)
{
}

TEST(SpiceZpr, vpack)
{
}

TEST(SpiceZpr, vperp)
{
}

TEST(SpiceZpr, vprjpi)
{
}

TEST(SpiceZpr, vprjp)
{
}

TEST(SpiceZpr, vprojg)
{
}

TEST(SpiceZpr, vproj)
{
}

TEST(SpiceZpr, vrelg)
{
}

TEST(SpiceZpr, vrel)
{
}

TEST(SpiceZpr, vrotv)
{
}

TEST(SpiceZpr, vsclg)
{
}

TEST(SpiceZpr, vscl)
{
}

TEST(SpiceZpr, vsepg)
{
}

TEST(SpiceZpr, vsep)
{
}

TEST(SpiceZpr, vsubg)
{
}

TEST(SpiceZpr, vsub)
{
}

TEST(SpiceZpr, vtmvg)
{
}

TEST(SpiceZpr, vtmv)
{
}

TEST(SpiceZpr, vupack)
{
}

TEST(SpiceZpr, vzerog)
{
}

TEST(SpiceZpr, vzero)
{
}

TEST(SpiceZpr, wncard)
{
}

TEST(SpiceZpr, wncomd)
{
}

TEST(SpiceZpr, wncond)
{
}

TEST(SpiceZpr, wndifd)
{
}

TEST(SpiceZpr, wnelmd)
{
}

TEST(SpiceZpr, wnexpd)
{
}

TEST(SpiceZpr, wnextd)
{
}

TEST(SpiceZpr, wnfetd)
{
}

TEST(SpiceZpr, wnfild)
{
}

TEST(SpiceZpr, wnfltd)
{
}

TEST(SpiceZpr, wnincd)
{
}

TEST(SpiceZpr, wninsd)
{
}

TEST(SpiceZpr, wnintd)
{
}

TEST(SpiceZpr, wnreld)
{
}

TEST(SpiceZpr, wnsumd)
{
}

TEST(SpiceZpr, wnunid)
{
}

TEST(SpiceZpr, wnvald)
{
}

TEST(SpiceZpr, xf2eul)
{
}

TEST(SpiceZpr, xf2rav)
{
}

TEST(SpiceZpr, xfmsta)
{
}

TEST(SpiceZpr, xpose6)
{
}

TEST(SpiceZpr, xposeg)
{
}

TEST(SpiceZpr, xpose)
{
}

#endif

GTEST_MAIN()