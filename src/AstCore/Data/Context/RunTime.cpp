/// @file      RunTime.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      26.11.2025
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
 
 
#include "RunTime.hpp"
#include "DataContext.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/FundamentalArguments.hpp"
#include "AstCore/GlobalContext.hpp"
#include <assert.h>
#include "RunTimeData.hpp"
#include "RunTimeSolarSystem.hpp"

#define AST_DEFAULT_FILE_LEAPSECOND             "Time/Leap_Second.dat"
#define AST_DEFAULT_FILE_JPLDE                  "SolarSystem/plneph.430"
#define AST_DEFAULT_FILE_EOP                    "SolarSystem/Earth/EOP-All.txt"
#define AST_DEFAULT_FILE_SPACEWEATHER           "SolarSystem/Earth/SW-Last5Years.txt"
#define AST_DEFAULT_FILE_IAUX                   "IERS-conventions/2010/tab5.2a.txt"
#define AST_DEFAULT_FILE_IAUY                   "IERS-conventions/2010/tab5.2b.txt"
#define AST_DEFAULT_FILE_IAUS                   "IERS-conventions/2010/tab5.2d.txt"
#define AST_DEFAULT_FILE_IAUXYS_PRECOMPUTED     "Test/ICRF/IAU2006_XYS.dat"
#define AST_DEFAULT_DIR_SOLARSYSTEM             "SolarSystem/"

AST_NAMESPACE_BEGIN



// 线程本地存储的当前全局上下文指针
A_THREAD_LOCAL DataContext* t_currentDataContext = nullptr;
std::unique_ptr<DataContext> g_defaultDataContext = nullptr;
static GlobalContext s_globalContext;

GlobalContext *aGlobalContext_Get()
{
    return &s_globalContext;
}

IAUXYS* aGlobalContext_GetIAUXYS()
{
    return s_globalContext.iauXYS();
}


err_t LeapSecond::loadDefault()
{
    fs::path datafile = fs::path(aDataDirGet()) / AST_DEFAULT_FILE_LEAPSECOND;
    err_t err = this->load(datafile.string().c_str());
    if (err)
    {
        aWarning("failed to load leapsecond from default data file: %s", datafile.string().c_str());
    }
    return err;
}

err_t JplDe::openDefault()
{
    fs::path datafile = fs::path(aDataDirGet()) / AST_DEFAULT_FILE_JPLDE;

    err_t err = this->open(datafile.string().c_str());
    if (err)
    {
        aWarning("failed to load jpl de from default data file:\n%s", datafile.string().c_str());
    }
    return err;
}

err_t EOP::loadDefault()
{
    fs::path filepath = fs::path(aDataDirGet()) / AST_DEFAULT_FILE_EOP;
    err_t err = load(filepath.string());
    if (err)
    {
        aWarning("failed to load eop from default data file:\n%s", filepath.string().c_str());
    }
    return err;
}

err_t SpaceWeather::loadDefault()
{
    fs::path filepath = fs::path(aDataDirGet()) / AST_DEFAULT_FILE_SPACEWEATHER;
    err_t err = load(filepath.string());
    if (err)
    {
        aWarning("failed to load space weather from default data file:\n%s", filepath.string().c_str());
    }
    return err;
}

err_t IAUXYS::loadDefault()
{
    fs::path filepathX = fs::path(aDataDirGet()) / AST_DEFAULT_FILE_IAUX;
    fs::path filepathY = fs::path(aDataDirGet()) / AST_DEFAULT_FILE_IAUY;
    fs::path filepathS = fs::path(aDataDirGet()) / AST_DEFAULT_FILE_IAUS;
    err_t err = load(filepathX.string(), filepathY.string(), filepathS.string());
    if (err)
    {
        aWarning("failed to load iaux from default data file:\n%s", filepathX.string().c_str());
    }
    return err;
}


err_t IAUXYSPrecomputed::loadDefault()
{
    fs::path filepath = fs::path(aDataDirGet()) / AST_DEFAULT_FILE_IAUXYS_PRECOMPUTED;
    err_t err = load(filepath.string());
    if (err)
    {
        aWarning("failed to load iauxys precomputed from default data file:\n%s", filepath.string().c_str());
    }
    return err;
}


err_t SolarSystem::loadDefault()
{
    fs::path dirpath = fs::path(aDataDirGet()) / AST_DEFAULT_DIR_SOLARSYSTEM;
    err_t err = load(dirpath.string());
    if (err)
    {
        aWarning("failed to load solar system from default data dir:\n%s", dirpath.string().c_str());
    }
    return err;
}

err_t aInitialize(DataContext* context)
{
    err_t err = 0;
    
    // init global context
    auto globalCxt = aGlobalContext_Get();
    if(!globalCxt->iauXYS()->isLoaded())
    {
        err |= globalCxt->iauXYS()->loadDefault();
    }
    
    // init thread local data context
    err |= context->leapSecond()->loadDefault();
    err |= context->jplDe()->openDefault();
    err |= context->eop()->loadDefault();
    err |= context->spaceWeather()->loadDefault();
    err |= context->iauXYSPrecomputed()->loadDefault();
    err |= context->solarSystem()->loadDefault();

    if(err != eNoError) {
        aError("initialize failed: failed to load data.");
    }
    return err;
}


err_t aInitialize()
{
    auto context = aDataContext_EnsureCurrent();
    return aInitialize(context);
}

err_t aUninitialize()
{
    if (g_defaultDataContext) {
        if (t_currentDataContext == g_defaultDataContext.get()) {
            t_currentDataContext = nullptr;
        }
        g_defaultDataContext = nullptr;
    }
    // 线程局部变量 t_currentDataContext 通常不需要在此处显式删除，
    // 因为它指向的是 g_defaultGlobalContext，并且会在线程退出时自动销毁。
    return eNoError;
}

std::string aDataDirGet()
{
    std::string datadir;
    err_t rc = aDataDirGet(datadir);
    A_UNUSED(rc);
    return datadir;
}

err_t aDataDirGet(std::string &datadir)
{
    auto context = aDataContext_EnsureCurrent();
    if (A_UNLIKELY(context->dataDir().empty())) 
    {
        err_t rc = aDataDirGetDefault(datadir);
        aDataDirSet(datadir);
        return rc;
    }
    datadir = context->dataDir();
    return eNoError;
}

err_t aDataDirSet(StringView dirpath)
{
    if (!fs::is_directory(dirpath.to_string())) {
        aError("dirpath is not a directory.");
        return eErrorInvalidParam;
    }
    auto context = aDataContext_EnsureCurrent();
    context->setDataDir(dirpath);
    return eNoError;
}


DataContext* aDataContext_GetCurrent()
{
    assert(t_currentDataContext && "Current DataContext is null!");
    return t_currentDataContext;
}

DataContext* aDataContext_GetDefault()
{
    return g_defaultDataContext.get();
}


DataContext* aDataContext_EnsureDefault()
{
    if (!g_defaultDataContext) {
        g_defaultDataContext.reset(aDataContext_New());
    }
    return g_defaultDataContext.get();
}

DataContext* aDataContext_EnsureCurrent()
{
    if (A_UNLIKELY(!t_currentDataContext))
    {
        aDataContext_SetCurrent(aDataContext_EnsureDefault());
    }
    return t_currentDataContext;
}

void aDataContext_SetCurrent(DataContext* context)
{
    t_currentDataContext = context;
}

EOP * aDataContext_GetEOP()
{
    auto context = aDataContext_EnsureCurrent();
    return context->eop();
}

IAUXYSPrecomputed* aDataContext_GetIAUXYSPrecomputed()
{
    auto context = aDataContext_EnsureCurrent();
    return context->iauXYSPrecomputed();
}

DataContext* aDataContext_New()
{
    return new DataContext{};
}

// ----------
// Leap Second
// ----------


double aLeapSecondUTC(double jdUTC)
{
    auto context = aDataContext_EnsureCurrent();
    return context->leapSecond()->leapSecondUTC(jdUTC);
}

double aLeapSecondUTCMJD(double mjdUTC)
{
    auto context = aDataContext_EnsureCurrent();
    return context->leapSecond()->leapSecondUTCMJD(mjdUTC);
}

// ----------
// JPL DE
// ----------


err_t aJplDeGetPosVelICRF(
    const TimePoint& time,
    int target,
    int referenceBody,
    Vector3d& pos,
    Vector3d& vel
)
{
    auto context = aDataContext_EnsureCurrent();
    return context->jplDe()->getPosVelICRF(time, (JplDe::EDataCode)target, (JplDe::EDataCode)referenceBody, pos, vel);
}

err_t aJplDeGetPosICRF(
    const TimePoint& time,
    int target,
    int referenceBody,
    Vector3d& pos
)
{
    auto context = aDataContext_EnsureCurrent();
    return context->jplDe()->getPosICRF(time, (JplDe::EDataCode)target, (JplDe::EDataCode)referenceBody, pos);
}

err_t aJplDeGetNutation(const TimePoint &time, double &dpsi, double &deps)
{
    auto context = aDataContext_EnsureCurrent();
    return context->jplDe()->getNutation(time, dpsi, deps);
}

err_t aJplDeGetLibration(const TimePoint& time, Vector3d& ang)
{
    auto context = aDataContext_EnsureCurrent();
    return context->jplDe()->getLibration(time, ang);
}

err_t aJplDeGetLibration(const TimePoint &time, Euler &ang)
{
    auto context = aDataContext_EnsureCurrent();
    return context->jplDe()->getLibration(time, ang);
}

int aJplDeNum()
{
    auto context = aDataContext_EnsureCurrent();
    return context->jplDe()->getEphemVersion();
}

err_t aJplDeOpen(const char *filepath)
{
    auto context = aDataContext_EnsureCurrent();
    return context->jplDe()->open(filepath);
}

void aJplDeClose()
{
    auto context = aDataContext_EnsureCurrent();
    context->jplDe()->close();
}


// ----------
// EOP
// ----------


double aUT1MinusUTC_UTC(const JulianDate &jdUTC)
{
    auto context = aDataContext_EnsureCurrent();
    return context->eop()->getUT1MinusUTC_UTC(jdUTC);
}

void aPoleMotion(const TimePoint &tp, double &x, double &y)
{
    auto context = aDataContext_EnsureCurrent();
    context->eop()->getPoleMotion(tp, x, y);
}

void aPoleMotionUTC(const JulianDate &jdUTC, double &x, double &y)
{
    auto context = aDataContext_EnsureCurrent();
    context->eop()->getPoleMotionUTC(jdUTC, x, y);
}

double aLOD(const TimePoint &tp)
{
    auto context = aDataContext_EnsureCurrent();
    return context->eop()->getLOD(tp);
}

// ----------
// XYS
// ----------

void aTheoreticalXYS_IERS2010(double t, array3d& xys)
{
    auto globalCxt = aGlobalContext_Get();
    // 根据IERS 2010规范，计算行星基础参数
    FundamentalArguments fundargs;
    aFundamentalArguments_IERS2010(t, fundargs);
    // 根据IERS 2010规范，计算xys值
    globalCxt->iauXYS()->eval(t, fundargs, xys);
}

struct XYSCache
{
    double t;
    array3d xys;
};

A_THREAD_LOCAL XYSCache tXYSCache{std::numeric_limits<double>::quiet_NaN(), {0.0, 0.0, 0.0}};

void aTheoreticalXYS_IERS2010_Cache(double t, array3d& xys)
{
    if (t == tXYSCache.t) {
        xys = tXYSCache.xys;
    }else{
        aTheoreticalXYS_IERS2010(t, xys);
        tXYSCache.t = t;
        tXYSCache.xys = xys;
    }
}

void aTheoreticalXYS_IERS2010(const TimePoint& tp, array3d& xys)
{
    double t = tp.julianCenturyFromJ2000TT();
    aTheoreticalXYS_IERS2010_Cache(t, xys);
}

void aTheoreticalXYS_IERS2010_TT(const JulianDate& jdTT, array3d& xys)
{
    double t = jdTT.julianCenturyFromJ2000();
    aTheoreticalXYS_IERS2010_Cache(t, xys);
}

err_t aTheoreticalXYS_IERS2010Precomputed(const TimePoint& tp, array3d& xys)
{
    auto context = aDataContext_EnsureCurrent();
    return context->iauXYSPrecomputed()->getValue(tp, xys);
}


void aTheoreticalXYS(const TimePoint& tp, array3d& xys)
{
    if (aTheoreticalXYS_IERS2010Precomputed(tp, xys) != eNoError) {
        aTheoreticalXYS_IERS2010(tp, xys);
    }
}


void aXYCorrection(const TimePoint &tp, array2d &xyCorrection)
{
    auto context = aDataContext_EnsureCurrent();
    context->eop()->getXYCorrection(tp, xyCorrection);
}

void aXYS_IERS2010(const TimePoint& tp, array3d& xys)
{
    aTheoreticalXYS_IERS2010(tp, xys);
    // 应用EOP中的XY修正量
    // 参考IERS 2010规范 5.9节 P71 的相关说明
    // 应该先计算得到S，然后再应用XY修正量？
    // 这个先后顺序应该影响很小
    array2d xyCorrection;
    aXYCorrection(tp, xyCorrection);
    xys[0] += xyCorrection[0];
    xys[1] += xyCorrection[1];
}


void aXYS(const TimePoint &tp, array3d &xys)
{
    aTheoreticalXYS(tp, xys);
    // 应用EOP中的XY修正量
    // 参考IERS 2010规范 5.9节 P71 的相关说明
    // 应该先计算得到S，然后再应用XY修正量？
    // 这个先后顺序应该影响很小
    array2d xyCorrection;
    aXYCorrection(tp, xyCorrection);
    xys[0] += xyCorrection[0];
    xys[1] += xyCorrection[1];
}


// ----------
// SolarSystem
// ----------


SolarSystem *aGetSolarSystem()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem();
}

CelestialBody *aGetBody(StringView name)
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getBody(name);
}

CelestialBody* aGetMercury()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getMercury();
}

CelestialBody* aGetVenus()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getVenus();
}

CelestialBody* aGetEarth()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getEarth();
}

CelestialBody* aGetMars()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getMars();
}

CelestialBody* aGetJupiter()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getJupiter();
}

CelestialBody* aGetSaturn()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getSaturn();
}


CelestialBody* aGetUranus()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getUranus();
}   

CelestialBody* aGetNeptune()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getNeptune();
}

CelestialBody* aGetPluto()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getPluto();
}

CelestialBody* aGetMoon()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getMoon();
}

CelestialBody* aGetSun()
{
    auto context = aDataContext_EnsureCurrent();
    return context->solarSystem()->getSun();
}



AST_NAMESPACE_END