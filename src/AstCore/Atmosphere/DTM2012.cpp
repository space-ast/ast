///
/// @file      DTM2012.cpp
/// @brief     DTM-2012 大气模型实现
/// @details   封装 DTM_12 命名空间中的 dtm2012() 计算函数，
///            提供标准的 AtmosphereBase 接口。
/// @author    axel
/// @date      2026-06-27
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "DTM2012.hpp"

#include <cmath>

#include "AstUtil/Logger.hpp"
#include "AstWeather/DTM_12.h"
#include "AstCore/TimePoint.hpp"
#include "AstCore/JulianDate.hpp"
#include "AstCore/DateTime.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/// @brief 不透明实现，存储 DTM_12 计算所需的所有状态结构体
struct DTM2012::Impl
{
    pardtmtype  pardtm;   ///< 温度/密度系数（从文件加载）
    plgdtmtype  plgdtm;   ///< Legendre 多项式
    hlocaltype  hlocal;   ///< 地方时相关量
    eclipttype  eclipt;   ///< 黄道相关量
    datmotype   datmo;    ///< 模型参数
    constype    cons;     ///< 物理常数
};

DTM2012::DTM2012(Frame *frame, BodyShape *bodyShape,
                 double f107Daily, double f107Average, double kp,
                 const std::string &coeffFilePath)
    : AtmosphereBase(frame, bodyShape)
    , F107Daily_{f107Daily}
    , F107Average_{f107Average}
    , kp_{kp}
    , impl_{new Impl{}}
{
    // 初始化 DTM_12 的结构体
    impl_->datmo.npara = 95;
    impl_->datmo.itype = 0;
    impl_->datmo.ilin  = 0;

    // 如果提供了系数文件路径，加载系数
    if (!coeffFilePath.empty())
    {
        initialized_ = (aLoadDTM2012Coefficients(coeffFilePath, impl_->pardtm) == eNoError);
    }
}

DTM2012::~DTM2012() = default;

/// @brief 从文件加载 DTM2012 系数
errc_t aLoadDTM2012Coefficients(const std::string &path, pardtmtype &pardtm)
{
    static const int kDTM2012ExpectedNpdtm = 95;  // DTM2012 标准参数数量

    int  i, j_test, npdtm;
    int  Iok;

    FILE *infile;
    char longstr1[210];

#ifdef _MSC_VER
    errno_t errs = fopen_s(&infile, path.c_str(), "r");
    if (errs != 0 || infile == nullptr)
    {
        aWarning("DTM2012: failed to open coefficient file '%s'.", path.c_str());
        return eErrorInvalidFile;
    }
#else
    infile = fopen(path.c_str(), "r");
    if (infile == nullptr)
    {
        aWarning("DTM2012: failed to open coefficient file '%s'.", path.c_str());
        return eErrorInvalidFile;
    }
#endif

    // 读标题行
    if (!fgets(longstr1, 110, infile)) { Iok = 0; goto cleanup; }

    // 读参数数量行并校验
    if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
    sscanf(longstr1, "%i", &npdtm);

    if (npdtm != kDTM2012ExpectedNpdtm)
    {
        aWarning("DTM2012: coefficient file '%s' has %d parameters, expected %d. File may be from a different DTM version.",
                 path.c_str(), npdtm, kDTM2012ExpectedNpdtm);
        fclose(infile);
        return eErrorInvalidFile;
    }

    Iok = 1;

    // ...... termes ONE
    {
        int i_local = 1;
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                       &j_test,  &pardtm.tt.T_one,  &pardtm.dtt.T_one,
                   &pardtm.h.T_one,   &pardtm.dh.T_one,
                   &pardtm.he.T_one,  &pardtm.dhe.T_one,
                   &pardtm.ox.T_one,  &pardtm.dox.T_one,
                   &pardtm.az2.T_one, &pardtm.daz2.T_one,
                   &pardtm.o2.T_one,  &pardtm.do2.T_one,
                   &pardtm.az.T_one,  &pardtm.daz.T_one,
                   &pardtm.t0.T_one,  &pardtm.dt0.T_one,
                   &pardtm.tp.T_one,  &pardtm.dtp.T_one);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_one,  &pardtm.dtt.T_one,
            &pardtm.h.T_one,   &pardtm.dh.T_one,
            &pardtm.he.T_one,  &pardtm.dhe.T_one,
            &pardtm.ox.T_one,  &pardtm.dox.T_one,
            &pardtm.az2.T_one, &pardtm.daz2.T_one,
            &pardtm.o2.T_one,  &pardtm.do2.T_one,
            &pardtm.az.T_one,  &pardtm.daz.T_one,
            &pardtm.t0.T_one,  &pardtm.dt0.T_one,
            &pardtm.tp.T_one,  &pardtm.dtp.T_one);
#endif
        if (i_local != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in LAT
    for (i = 1; i < Nb_lat; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_lat[i],  &pardtm.dtt.T_lat[i],
                  &pardtm.h.T_lat[i],   &pardtm.dh.T_lat[i],
                  &pardtm.he.T_lat[i],  &pardtm.dhe.T_lat[i],
                  &pardtm.ox.T_lat[i],  &pardtm.dox.T_lat[i],
                  &pardtm.az2.T_lat[i], &pardtm.daz2.T_lat[i],
                  &pardtm.o2.T_lat[i],  &pardtm.do2.T_lat[i],
                  &pardtm.az.T_lat[i],  &pardtm.daz.T_lat[i],
                  &pardtm.t0.T_lat[i],  &pardtm.dt0.T_lat[i],
                  &pardtm.tp.T_lat[i],  &pardtm.dtp.T_lat[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_lat[i],  &pardtm.dtt.T_lat[i],
            &pardtm.h.T_lat[i],   &pardtm.dh.T_lat[i],
            &pardtm.he.T_lat[i],  &pardtm.dhe.T_lat[i],
            &pardtm.ox.T_lat[i],  &pardtm.dox.T_lat[i],
            &pardtm.az2.T_lat[i], &pardtm.daz2.T_lat[i],
            &pardtm.o2.T_lat[i],  &pardtm.do2.T_lat[i],
            &pardtm.az.T_lat[i],  &pardtm.daz.T_lat[i],
            &pardtm.t0.T_lat[i],  &pardtm.dt0.T_lat[i],
            &pardtm.tp.T_lat[i],  &pardtm.dtp.T_lat[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in Flux
    for (i = 1; i < Nb_flux; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_flux[i],  &pardtm.dtt.T_flux[i],
                  &pardtm.h.T_flux[i],   &pardtm.dh.T_flux[i],
                  &pardtm.he.T_flux[i],  &pardtm.dhe.T_flux[i],
                  &pardtm.ox.T_flux[i],  &pardtm.dox.T_flux[i],
                  &pardtm.az2.T_flux[i], &pardtm.daz2.T_flux[i],
                  &pardtm.o2.T_flux[i],  &pardtm.do2.T_flux[i],
                  &pardtm.az.T_flux[i],  &pardtm.daz.T_flux[i],
                  &pardtm.t0.T_flux[i],  &pardtm.dt0.T_flux[i],
                  &pardtm.tp.T_flux[i],  &pardtm.dtp.T_flux[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_flux[i],  &pardtm.dtt.T_flux[i],
            &pardtm.h.T_flux[i],   &pardtm.dh.T_flux[i],
            &pardtm.he.T_flux[i],  &pardtm.dhe.T_flux[i],
            &pardtm.ox.T_flux[i],  &pardtm.dox.T_flux[i],
            &pardtm.az2.T_flux[i], &pardtm.daz2.T_flux[i],
            &pardtm.o2.T_flux[i],  &pardtm.do2.T_flux[i],
            &pardtm.az.T_flux[i],  &pardtm.daz.T_flux[i],
            &pardtm.t0.T_flux[i],  &pardtm.dt0.T_flux[i],
            &pardtm.tp.T_flux[i],  &pardtm.dtp.T_flux[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in kp
    for (i = 1; i < Nb_kp; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_kp[i],  &pardtm.dtt.T_kp[i],
                  &pardtm.h.T_kp[i],   &pardtm.dh.T_kp[i],
                  &pardtm.he.T_kp[i],  &pardtm.dhe.T_kp[i],
                  &pardtm.ox.T_kp[i],  &pardtm.dox.T_kp[i],
                  &pardtm.az2.T_kp[i], &pardtm.daz2.T_kp[i],
                  &pardtm.o2.T_kp[i],  &pardtm.do2.T_kp[i],
                  &pardtm.az.T_kp[i],  &pardtm.daz.T_kp[i],
                  &pardtm.t0.T_kp[i],  &pardtm.dt0.T_kp[i],
                  &pardtm.tp.T_kp[i],  &pardtm.dtp.T_kp[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_kp[i],  &pardtm.dtt.T_kp[i],
            &pardtm.h.T_kp[i],   &pardtm.dh.T_kp[i],
            &pardtm.he.T_kp[i],  &pardtm.dhe.T_kp[i],
            &pardtm.ox.T_kp[i],  &pardtm.dox.T_kp[i],
            &pardtm.az2.T_kp[i], &pardtm.daz2.T_kp[i],
            &pardtm.o2.T_kp[i],  &pardtm.do2.T_kp[i],
            &pardtm.az.T_kp[i],  &pardtm.daz.T_kp[i],
            &pardtm.t0.T_kp[i],  &pardtm.dt0.T_kp[i],
            &pardtm.tp.T_kp[i],  &pardtm.dtp.T_kp[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in SLat
    for (i = 1; i < Nb_SLat; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_SLat[i],  &pardtm.dtt.T_SLat[i],
                  &pardtm.h.T_SLat[i],   &pardtm.dh.T_SLat[i],
                  &pardtm.he.T_SLat[i],  &pardtm.dhe.T_SLat[i],
                  &pardtm.ox.T_SLat[i],  &pardtm.dox.T_SLat[i],
                  &pardtm.az2.T_SLat[i], &pardtm.daz2.T_SLat[i],
                  &pardtm.o2.T_SLat[i],  &pardtm.do2.T_SLat[i],
                  &pardtm.az.T_SLat[i],  &pardtm.daz.T_SLat[i],
                  &pardtm.t0.T_SLat[i],  &pardtm.dt0.T_SLat[i],
                  &pardtm.tp.T_SLat[i],  &pardtm.dtp.T_SLat[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_SLat[i],  &pardtm.dtt.T_SLat[i],
            &pardtm.h.T_SLat[i],   &pardtm.dh.T_SLat[i],
            &pardtm.he.T_SLat[i],  &pardtm.dhe.T_SLat[i],
            &pardtm.ox.T_SLat[i],  &pardtm.dox.T_SLat[i],
            &pardtm.az2.T_SLat[i], &pardtm.daz2.T_SLat[i],
            &pardtm.o2.T_SLat[i],  &pardtm.do2.T_SLat[i],
            &pardtm.az.T_SLat[i],  &pardtm.daz.T_SLat[i],
            &pardtm.t0.T_SLat[i],  &pardtm.dt0.T_SLat[i],
            &pardtm.tp.T_SLat[i],  &pardtm.dtp.T_SLat[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in SASLat
    for (i = 1; i < Nb_SASLat; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_SASLat[i],  &pardtm.dtt.T_SASLat[i],
                  &pardtm.h.T_SASLat[i],   &pardtm.dh.T_SASLat[i],
                  &pardtm.he.T_SASLat[i],  &pardtm.dhe.T_SASLat[i],
                  &pardtm.ox.T_SASLat[i],  &pardtm.dox.T_SASLat[i],
                  &pardtm.az2.T_SASLat[i], &pardtm.daz2.T_SASLat[i],
                  &pardtm.o2.T_SASLat[i],  &pardtm.do2.T_SASLat[i],
                  &pardtm.az.T_SASLat[i],  &pardtm.daz.T_SASLat[i],
                  &pardtm.t0.T_SASLat[i],  &pardtm.dt0.T_SASLat[i],
                  &pardtm.tp.T_SASLat[i],  &pardtm.dtp.T_SASLat[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_SASLat[i],  &pardtm.dtt.T_SASLat[i],
            &pardtm.h.T_SASLat[i],   &pardtm.dh.T_SASLat[i],
            &pardtm.he.T_SASLat[i],  &pardtm.dhe.T_SASLat[i],
            &pardtm.ox.T_SASLat[i],  &pardtm.dox.T_SASLat[i],
            &pardtm.az2.T_SASLat[i], &pardtm.daz2.T_SASLat[i],
            &pardtm.o2.T_SASLat[i],  &pardtm.do2.T_SASLat[i],
            &pardtm.az.T_SASLat[i],  &pardtm.daz.T_SASLat[i],
            &pardtm.t0.T_SASLat[i],  &pardtm.dt0.T_SASLat[i],
            &pardtm.tp.T_SASLat[i],  &pardtm.dtp.T_SASLat[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in NSLat
    for (i = 1; i < Nb_NSLat; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_NSLat[i],  &pardtm.dtt.T_NSLat[i],
                  &pardtm.h.T_NSLat[i],   &pardtm.dh.T_NSLat[i],
                  &pardtm.he.T_NSLat[i],  &pardtm.dhe.T_NSLat[i],
                  &pardtm.ox.T_NSLat[i],  &pardtm.dox.T_NSLat[i],
                  &pardtm.az2.T_NSLat[i], &pardtm.daz2.T_NSLat[i],
                  &pardtm.o2.T_NSLat[i],  &pardtm.do2.T_NSLat[i],
                  &pardtm.az.T_NSLat[i],  &pardtm.daz.T_NSLat[i],
                  &pardtm.t0.T_NSLat[i],  &pardtm.dt0.T_NSLat[i],
                  &pardtm.tp.T_NSLat[i],  &pardtm.dtp.T_NSLat[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_NSLat[i],  &pardtm.dtt.T_NSLat[i],
            &pardtm.h.T_NSLat[i],   &pardtm.dh.T_NSLat[i],
            &pardtm.he.T_NSLat[i],  &pardtm.dhe.T_NSLat[i],
            &pardtm.ox.T_NSLat[i],  &pardtm.dox.T_NSLat[i],
            &pardtm.az2.T_NSLat[i], &pardtm.daz2.T_NSLat[i],
            &pardtm.o2.T_NSLat[i],  &pardtm.do2.T_NSLat[i],
            &pardtm.az.T_NSLat[i],  &pardtm.daz.T_NSLat[i],
            &pardtm.t0.T_NSLat[i],  &pardtm.dt0.T_NSLat[i],
            &pardtm.tp.T_NSLat[i],  &pardtm.dtp.T_NSLat[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in SANSLat
    for (i = 1; i < Nb_SANSLat; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_SANSLat[i],  &pardtm.dtt.T_SANSLat[i],
                  &pardtm.h.T_SANSLat[i],   &pardtm.dh.T_SANSLat[i],
                  &pardtm.he.T_SANSLat[i],  &pardtm.dhe.T_SANSLat[i],
                  &pardtm.ox.T_SANSLat[i],  &pardtm.dox.T_SANSLat[i],
                  &pardtm.az2.T_SANSLat[i], &pardtm.daz2.T_SANSLat[i],
                  &pardtm.o2.T_SANSLat[i],  &pardtm.do2.T_SANSLat[i],
                  &pardtm.az.T_SANSLat[i],  &pardtm.daz.T_SANSLat[i],
                  &pardtm.t0.T_SANSLat[i],  &pardtm.dt0.T_SANSLat[i],
                  &pardtm.tp.T_SANSLat[i],  &pardtm.dtp.T_SANSLat[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_SANSLat[i],  &pardtm.dtt.T_SANSLat[i],
            &pardtm.h.T_SANSLat[i],   &pardtm.dh.T_SANSLat[i],
            &pardtm.he.T_SANSLat[i],  &pardtm.dhe.T_SANSLat[i],
            &pardtm.ox.T_SANSLat[i],  &pardtm.dox.T_SANSLat[i],
            &pardtm.az2.T_SANSLat[i], &pardtm.daz2.T_SANSLat[i],
            &pardtm.o2.T_SANSLat[i],  &pardtm.do2.T_SANSLat[i],
            &pardtm.az.T_SANSLat[i],  &pardtm.daz.T_SANSLat[i],
            &pardtm.t0.T_SANSLat[i],  &pardtm.dt0.T_SANSLat[i],
            &pardtm.tp.T_SANSLat[i],  &pardtm.dtp.T_SANSLat[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in DiAn
    for (i = 1; i < Nb_DiAn; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_DiAn[i],  &pardtm.dtt.T_DiAn[i],
                  &pardtm.h.T_DiAn[i],   &pardtm.dh.T_DiAn[i],
                  &pardtm.he.T_DiAn[i],  &pardtm.dhe.T_DiAn[i],
                  &pardtm.ox.T_DiAn[i],  &pardtm.dox.T_DiAn[i],
                  &pardtm.az2.T_DiAn[i], &pardtm.daz2.T_DiAn[i],
                  &pardtm.o2.T_DiAn[i],  &pardtm.do2.T_DiAn[i],
                  &pardtm.az.T_DiAn[i],  &pardtm.daz.T_DiAn[i],
                  &pardtm.t0.T_DiAn[i],  &pardtm.dt0.T_DiAn[i],
                  &pardtm.tp.T_DiAn[i],  &pardtm.dtp.T_DiAn[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_DiAn[i],  &pardtm.dtt.T_DiAn[i],
            &pardtm.h.T_DiAn[i],   &pardtm.dh.T_DiAn[i],
            &pardtm.he.T_DiAn[i],  &pardtm.dhe.T_DiAn[i],
            &pardtm.ox.T_DiAn[i],  &pardtm.dox.T_DiAn[i],
            &pardtm.az2.T_DiAn[i], &pardtm.daz2.T_DiAn[i],
            &pardtm.o2.T_DiAn[i],  &pardtm.do2.T_DiAn[i],
            &pardtm.az.T_DiAn[i],  &pardtm.daz.T_DiAn[i],
            &pardtm.t0.T_DiAn[i],  &pardtm.dt0.T_DiAn[i],
            &pardtm.tp.T_DiAn[i],  &pardtm.dtp.T_DiAn[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in SDiAn
    for (i = 1; i < Nb_SDiAn; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_SDiAn[i],  &pardtm.dtt.T_SDiAn[i],
                  &pardtm.h.T_SDiAn[i],   &pardtm.dh.T_SDiAn[i],
                  &pardtm.he.T_SDiAn[i],  &pardtm.dhe.T_SDiAn[i],
                  &pardtm.ox.T_SDiAn[i],  &pardtm.dox.T_SDiAn[i],
                  &pardtm.az2.T_SDiAn[i], &pardtm.daz2.T_SDiAn[i],
                  &pardtm.o2.T_SDiAn[i],  &pardtm.do2.T_SDiAn[i],
                  &pardtm.az.T_SDiAn[i],  &pardtm.daz.T_SDiAn[i],
                  &pardtm.t0.T_SDiAn[i],  &pardtm.dt0.T_SDiAn[i],
                  &pardtm.tp.T_SDiAn[i],  &pardtm.dtp.T_SDiAn[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_SDiAn[i],  &pardtm.dtt.T_SDiAn[i],
            &pardtm.h.T_SDiAn[i],   &pardtm.dh.T_SDiAn[i],
            &pardtm.he.T_SDiAn[i],  &pardtm.dhe.T_SDiAn[i],
            &pardtm.ox.T_SDiAn[i],  &pardtm.dox.T_SDiAn[i],
            &pardtm.az2.T_SDiAn[i], &pardtm.daz2.T_SDiAn[i],
            &pardtm.o2.T_SDiAn[i],  &pardtm.do2.T_SDiAn[i],
            &pardtm.az.T_SDiAn[i],  &pardtm.daz.T_SDiAn[i],
            &pardtm.t0.T_SDiAn[i],  &pardtm.dt0.T_SDiAn[i],
            &pardtm.tp.T_SDiAn[i],  &pardtm.dtp.T_SDiAn[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in TDi
    for (i = 1; i < Nb_TDi; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_TDi[i],  &pardtm.dtt.T_TDi[i],
                  &pardtm.h.T_TDi[i],   &pardtm.dh.T_TDi[i],
                  &pardtm.he.T_TDi[i],  &pardtm.dhe.T_TDi[i],
                  &pardtm.ox.T_TDi[i],  &pardtm.dox.T_TDi[i],
                  &pardtm.az2.T_TDi[i], &pardtm.daz2.T_TDi[i],
                  &pardtm.o2.T_TDi[i],  &pardtm.do2.T_TDi[i],
                  &pardtm.az.T_TDi[i],  &pardtm.daz.T_TDi[i],
                  &pardtm.t0.T_TDi[i],  &pardtm.dt0.T_TDi[i],
                  &pardtm.tp.T_TDi[i],  &pardtm.dtp.T_TDi[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_TDi[i],  &pardtm.dtt.T_TDi[i],
            &pardtm.h.T_TDi[i],   &pardtm.dh.T_TDi[i],
            &pardtm.he.T_TDi[i],  &pardtm.dhe.T_TDi[i],
            &pardtm.ox.T_TDi[i],  &pardtm.dox.T_TDi[i],
            &pardtm.az2.T_TDi[i], &pardtm.daz2.T_TDi[i],
            &pardtm.o2.T_TDi[i],  &pardtm.do2.T_TDi[i],
            &pardtm.az.T_TDi[i],  &pardtm.daz.T_TDi[i],
            &pardtm.t0.T_TDi[i],  &pardtm.dt0.T_TDi[i],
            &pardtm.tp.T_TDi[i],  &pardtm.dtp.T_TDi[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in AMg
    for (i = 1; i < Nb_AMg; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_AMg[i],  &pardtm.dtt.T_AMg[i],
                  &pardtm.h.T_AMg[i],   &pardtm.dh.T_AMg[i],
                  &pardtm.he.T_AMg[i],  &pardtm.dhe.T_AMg[i],
                  &pardtm.ox.T_AMg[i],  &pardtm.dox.T_AMg[i],
                  &pardtm.az2.T_AMg[i], &pardtm.daz2.T_AMg[i],
                  &pardtm.o2.T_AMg[i],  &pardtm.do2.T_AMg[i],
                  &pardtm.az.T_AMg[i],  &pardtm.daz.T_AMg[i],
                  &pardtm.t0.T_AMg[i],  &pardtm.dt0.T_AMg[i],
                  &pardtm.tp.T_AMg[i],  &pardtm.dtp.T_AMg[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_AMg[i],  &pardtm.dtt.T_AMg[i],
            &pardtm.h.T_AMg[i],   &pardtm.dh.T_AMg[i],
            &pardtm.he.T_AMg[i],  &pardtm.dhe.T_AMg[i],
            &pardtm.ox.T_AMg[i],  &pardtm.dox.T_AMg[i],
            &pardtm.az2.T_AMg[i], &pardtm.daz2.T_AMg[i],
            &pardtm.o2.T_AMg[i],  &pardtm.do2.T_AMg[i],
            &pardtm.az.T_AMg[i],  &pardtm.daz.T_AMg[i],
            &pardtm.t0.T_AMg[i],  &pardtm.dt0.T_AMg[i],
            &pardtm.tp.T_AMg[i],  &pardtm.dtp.T_AMg[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in Lon
    for (i = 1; i < Nb_Lon; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_Lon[i],  &pardtm.dtt.T_Lon[i],
                  &pardtm.h.T_Lon[i],   &pardtm.dh.T_Lon[i],
                  &pardtm.he.T_Lon[i],  &pardtm.dhe.T_Lon[i],
                  &pardtm.ox.T_Lon[i],  &pardtm.dox.T_Lon[i],
                  &pardtm.az2.T_Lon[i], &pardtm.daz2.T_Lon[i],
                  &pardtm.o2.T_Lon[i],  &pardtm.do2.T_Lon[i],
                  &pardtm.az.T_Lon[i],  &pardtm.daz.T_Lon[i],
                  &pardtm.t0.T_Lon[i],  &pardtm.dt0.T_Lon[i],
                  &pardtm.tp.T_Lon[i],  &pardtm.dtp.T_Lon[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_Lon[i],  &pardtm.dtt.T_Lon[i],
            &pardtm.h.T_Lon[i],   &pardtm.dh.T_Lon[i],
            &pardtm.he.T_Lon[i],  &pardtm.dhe.T_Lon[i],
            &pardtm.ox.T_Lon[i],  &pardtm.dox.T_Lon[i],
            &pardtm.az2.T_Lon[i], &pardtm.daz2.T_Lon[i],
            &pardtm.o2.T_Lon[i],  &pardtm.do2.T_Lon[i],
            &pardtm.az.T_Lon[i],  &pardtm.daz.T_Lon[i],
            &pardtm.t0.T_Lon[i],  &pardtm.dt0.T_Lon[i],
            &pardtm.tp.T_Lon[i],  &pardtm.dtp.T_Lon[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

    // ...... termes in dPhas
    for (i = 1; i < Nb_dPhas; i++)
    {
        if (!fgets(longstr1, 210, infile)) { Iok = 0; goto cleanup; }
#ifdef _MSC_VER
        sscanf_s(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
                  &j_test,  &pardtm.tt.T_dPhas[i],  &pardtm.dtt.T_dPhas[i],
                  &pardtm.h.T_dPhas[i],   &pardtm.dh.T_dPhas[i],
                  &pardtm.he.T_dPhas[i],  &pardtm.dhe.T_dPhas[i],
                  &pardtm.ox.T_dPhas[i],  &pardtm.dox.T_dPhas[i],
                  &pardtm.az2.T_dPhas[i], &pardtm.daz2.T_dPhas[i],
                  &pardtm.o2.T_dPhas[i],  &pardtm.do2.T_dPhas[i],
                  &pardtm.az.T_dPhas[i],  &pardtm.daz.T_dPhas[i],
                  &pardtm.t0.T_dPhas[i],  &pardtm.dt0.T_dPhas[i],
                  &pardtm.tp.T_dPhas[i],  &pardtm.dtp.T_dPhas[i]);
#else
        sscanf(longstr1, " %4d %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf %13lf %9lf",
            &j_test,
            &pardtm.tt.T_dPhas[i],  &pardtm.dtt.T_dPhas[i],
            &pardtm.h.T_dPhas[i],   &pardtm.dh.T_dPhas[i],
            &pardtm.he.T_dPhas[i],  &pardtm.dhe.T_dPhas[i],
            &pardtm.ox.T_dPhas[i],  &pardtm.dox.T_dPhas[i],
            &pardtm.az2.T_dPhas[i], &pardtm.daz2.T_dPhas[i],
            &pardtm.o2.T_dPhas[i],  &pardtm.do2.T_dPhas[i],
            &pardtm.az.T_dPhas[i],  &pardtm.daz.T_dPhas[i],
            &pardtm.t0.T_dPhas[i],  &pardtm.dt0.T_dPhas[i],
            &pardtm.tp.T_dPhas[i],  &pardtm.dtp.T_dPhas[i]);
#endif
        if (i != j_test) { Iok = 0; goto cleanup; }
    }

cleanup:
    fclose(infile);

    if (Iok == 0)
    {
        aWarning("DTM2012: coefficient file '%s' format mismatch.", path.c_str());
        return eErrorInvalidFile;
    }
    return eNoError;

} // function aLoadDTM2012Coefficients


double DTM2012::getDensity(const TimePoint &tp, const Vector3d &posInBodyFixed) const
{
    if (!initialized_)
    {
        aWarning("coefficient file not loaded, returning zero density.");
        return 0.0;
    }

    assert(frame_ != nullptr);
    assert(bodyShape_ != nullptr);

    // 从地固系位置获取地理坐标
    double alt = 0.0, lat = 0.0, lon = 0.0;
    this->getGeodetic(posInBodyFixed, lat, lon, alt);

    // DTM2012 要求高度 > 120 km，输入单位为 km
    double hellp = alt / 1000.0;  // m -> km

    if (hellp < 120.0)
    {
        aWarning("altitude %f km is below 120 km, returning zero density.", hellp);
        return 0.0;
    }

    // 获取当前时间的年月日时分秒
    DateTime dttm;
    aTimePointToUTC(tp, dttm);

    // 计算年积日（含小数部分），DTM2012 要求 double 类型，1-indexed
    // 参考 DTM_12.cpp dtm_wrapper() 第 2315-2316 行：
    //   dayofyear = day_of_year(day, month, year) + hour/24 + minute/1440 + sec/86400
    double dayOfYear = static_cast<double>(dttm.dayOfYear())
                     + (dttm.hour() / 24.0)
                     + (dttm.minute() / (24.0 * 60.0))
                     + (dttm.second() / (24.0 * 3600.0));

    // 计算地方太阳时 [rad]
    // 参考 DTM_12.cpp local_solar_time() 第 772-800 行：
    //   LST = (2π / 24) * ( (secOfDay / 3600) + (lon_deg / 15) )
    // 其中 secOfDay = hour*3600 + minute*60 + sec，lon_deg 为经度 [°]
    // 输出归一化到 [0, 2π]
    const double pi = kPI;
    double nsec = dttm.hour() * 3600.0 + dttm.minute() * 60.0 + dttm.second();
    double lonDeg = lon * 180.0 / pi;
    double hl = (2.0 * pi / 24.0) * ((nsec / 3600.0) + (lonDeg / 15.0));

    // 归一化到 [0, 2*pi]
    hl = std::fmod(hl, 2.0 * pi);
    if (hl < 0.0)
        hl += 2.0 * pi;

    // =========================================================================
    // 准备 DTM2012 核心计算所需的输入参数
    // =========================================================================
    // 所有数组为 Fortran 风格 1-indexed，索引 0 不使用。
    //
    // -- 太阳射电通量 --
    // f[1] = 前日瞬时 F10.7 射电通量 (instantaneous flux at day-1)
    // f[2] = 0（保留）
    // fbar[1] = 81 天平均 F10.7 射电通量 (mean flux at t)
    // fbar[2] = 0（保留）
    //
    // 参考：DTM_12.cpp dtm2012() 注释
    //   "f[1] = instantaneous flux at (day - 1) / f[2] = 0"
    //   "fbar[1] = mean flux at t / fbar[2] = 0"
    //
    // -- 地磁活动指数 --
    // akp[1] = Kp 延迟 3 小时值（瞬时 Kp，反映当前地磁活动强度）
    // akp[2] = 0（保留位，标准用法恒为 0）
    // akp[3] = 过去 24 小时平均 Kp（反映持续性地磁活动水平）
    // akp[4] = 0（保留位，标准用法恒为 0）
    //
    // gldtm_XX() 中的使用方式（DTM_12.cpp 第 480-493 行）：
    //   dkp  = akp[1] + (T_kp[5] + c2fi * T_kp[6]) * akp[2]
    //   dkpm = akp[3] + T_kp[10] * akp[4]
    // 当 akp[2]=akp[4]=0 时，dkp=akp[1]（瞬时效应），dkpm=akp[3]（平均效应）。
    //
    // 参考：
    //   DTM_12.cpp dtm2012() 注释：
    //     "akp[1] = kp delayed by 3 hours, akp[3] = mean of last 24 hours /
    //      akp[2] & akp[4] = 0"
    //   DTM_12.cpp dtm_wrapper() 第 2376-2438 行：
    //     "akp[2] = akp[4] = 0"
    //     "akp[1] = kp delayed by 3 hours"
    //     "akp[3] = mean of last 24 hours"
    // =========================================================================

    double f[3]    = {0.0, 0.0, 0.0};
    double fbar[3] = {0.0, 0.0, 0.0};
    double akp[5]  = {0.0, 0.0, 0.0, 0.0, 0.0};

    f[1]    = F107Daily_;
    fbar[1] = F107Average_;
    akp[1]  = kp_;
    akp[3]  = kp_;
    // akp[2] = akp[4] = 0（已由数组初始化置零）

    // =========================================================================
    // 调用 DTM2012 核心计算
    // =========================================================================
    //
    // 输入参数说明（参考 DTM_12.cpp dtm2012() 注释，第 56-65 行）：
    //
    //   day     — 年积日，含小数部分，[1-366]。小数部分表示当天已过时间。
    //             例：6 月 9 日 12:00 UTC = 160.5
    //   f[3]    — 瞬时太阳射电通量，f[1]=前日 F10.7，f[2]=0
    //   fbar[3] — 平均太阳射电通量，fbar[1]=81天均值，fbar[2]=0
    //   akp[5]  — 地磁活动指数，akp[1]=瞬时 Kp，akp[3]=24h平均 Kp
    //   hellp   — 高度 [km]，必须 > 120
    //   hl      — 地方太阳时 [rad]，范围 [0, 2π]
    //   latgd   — 大地纬度 [rad]
    //   lon     — 经度 [rad]
    //
    // 输出参数说明：
    //   tz      — 当前高度大气温度 [K]
    //   tinf    — 外逸层温度 [K]
    //   tp120   — 120km 处温度梯度 [K/km]
    //   ro      — 总大气密度 [g/cm³]
    //   d[7]    — 各成分分密度（1-indexed）：
    //             d[1]=H, d[2]=He, d[3]=O, d[4]=N₂, d[5]=O₂, d[6]=N（未使用）
    //   wmm     — 平均分子量 [g/mol]
    // =========================================================================

    double tz = 0.0, tinf = 0.0, tp120 = 0.0, ro = 0.0, wmm = 0.0;
    double d[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    DTM_12::dtm2012(
        impl_->plgdtm, impl_->hlocal, impl_->eclipt,
        impl_->datmo, impl_->cons, impl_->pardtm,
        dayOfYear, f, fbar, akp,
        hellp, hl, lat, lon,
        tz, tinf, tp120, ro,
        d, wmm
    );

    // ro 单位是 g/cm³，转换为 kg/m³
    double density = ro * 1000.0;

    return density;
}

AST_NAMESPACE_END
