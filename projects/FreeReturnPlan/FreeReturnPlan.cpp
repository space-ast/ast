///
/// @file      FreeReturnPlan.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-20
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

#include "AstCore/FrameTransform.hpp"
#include "AstCore/HPOP.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/JplDe.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Environment.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/ODEIntegrator.hpp"
#include "AstMath/Util.hpp"
#include "AstPlot/AstPlot.hpp"
#include "AstOpt/NLPProblem.hpp"
#include "FreeReturnPlan.hpp"
#include "FreeReturnProblem.hpp"
#include "cminpack.h"

AST_USING_NAMESPACE

using namespace _AST literals;

// #define aLog(...) printf(__VA_ARGS__)

#ifndef aLog
#   define aLog(...) 
#endif



FreeReturnTargetFunction FreeReturnTargetFunction::Case1()
{
    FreeReturnTargetFunction problem;
    problem.initOrbitEpoch_ = TimePoint::FromUTC(2028, 6, 24, 16, 25, 59.184);
    problem.initOrbitElement_= {
        6548151, 0, 21.0_deg,
        149.3340_deg, 0, 199.456_deg
    };
    problem.initImpulse_ = 3160.0;
    return problem;
}

FreeReturnTargetFunction FreeReturnTargetFunction::Case2()
{
    FreeReturnTargetFunction problem;
    problem.initOrbitEpoch_ = TimePoint::FromUTC(2028, 6, 24, 16, 25, 59.184);
    problem.initOrbitElement_= {
        6548151, 0, 21.0_deg,
        149.3340_deg, 0, 199.456_deg
    };
    problem.initImpulse_ = 0.0;
    return problem;
}

int FreeReturnTargetFunction::operator()(const double* variable, double* constraint) const{
    // 输入变量
    const double varEpoch     = variable[0];            ///<  epoch 轨道历元（单位：秒）
    const double varRAAN      = variable[1];            ///<  RAAN 升交点赤经（单位：rad）
    const double varImpulse   = variable[2];            ///< 脉冲机动速度增量（单位： m/s）
    // 输出约束
    double& cnstrAltOfMoonPeri     = constraint[0];     ///< 近月点高度约束   (单位：m)
    double& cnstrEarthInclination  = constraint[1];     ///< 地球轨道倾角约束 (单位：rad)
    double& cnstrAltOfEarthPeri    = constraint[2];     ///< 近地点高度约束  (单位：m)
    // 全程力模型
    HPOPForceModel forceModel;
    forceModel.useMoonGravity_ = true;
    forceModel.moonGravity_ = 4.90280030555540e12;
    forceModel.gravity_.degree_ = 0;
    forceModel.gravity_.order_ = 0;
    forceModel.gravity_.model_ = "JGM3";
    
    // 全程轨道状态量
    TimePoint currentTime;
    Vector3d currentPos, currentVel;
    
    // 全程轨道状态记录
    // std::vector<Vector3d> posECIList_;
    posM2EList_.clear();
    posE2MList_.clear();

    // 轨道初始状态
    {
        TimePoint orbitEpoch = this->initOrbitEpoch_ + varEpoch;
        OrbElem orbitElement = this->initOrbitElement_;
        orbitElement.raan() += varRAAN;
        aOrbElemToCart(orbitElement, kEarthGrav, currentPos, currentVel);
        currentTime = orbitEpoch;
        aLog("--------------------------\n");
        aLog("init orbit state\n");
        aLog("orbitEpoch: %s\n", orbitEpoch.toString().c_str());
        aLog("currentPos: %s\ncurrentVel: %s\n", currentPos.toString().c_str(), currentVel.toString().c_str());
        aLog("--------------------------\n");
    }
    // 施加机动
    {
        Matrix3d matrixVNCToCBI;
        aVNCToFrameMatrix(currentPos, currentVel, matrixVNCToCBI);
        Vector3d impulseVNC {this->initImpulse_ + varImpulse, 0.0, 0.0};
        Vector3d impulseCBI = matrixVNCToCBI * impulseVNC;
        currentVel += impulseCBI;
        aLog("impulseCBI: %s\n", impulseCBI.toString().c_str());
    }
    // 轨道预报到近月点
    {
        aLog("--------------------------\n");
        aLog("Before Propagation:\n");
        aLog("currentPos: %s\ncurrentVel: %s\n", currentPos.toString().c_str(), currentVel.toString().c_str());
        aLog("--------------------------\n");
        HPOP propagator;
        propagator.setForceModel(forceModel);
        auto integrator = propagator.getIntegrator();
        TimePoint startTime = currentTime;
        // 近月点停止条件
        auto stopMoonPeriapsis = integrator->addEventDetector([=](const double* state, double t){
            const Vector3d& satPos = *(const Vector3d*)state;
            const Vector3d& satVel = *(const Vector3d*)(state + 3);
            TimePoint tp = startTime + t;
            Vector3d satPosInMoon;
            Vector3d satVelInMoon;
            if(0)
            {
                Vector3d moonPos, moonVel;
                aJplDeGetPosVelICRF(tp, JplDe::eMoon, JplDe::eEarth, moonPos, moonVel);
                satPosInMoon = satPos - moonPos;
                satVelInMoon = satVel - moonVel;
            }
            aEarthICRFToMoonICRF(tp, satPos, satVel, satPosInMoon, satVelInMoon);
            double dotProduct = satPosInMoon.dot(satVelInMoon);
            return dotProduct;
        });
        stopMoonPeriapsis->setDirection(ODEEventDetector::eIncrease);
        // 记录星历
        integrator->addStateObserver([&](const double* state, double t){
            posE2MList_.push_back(*(const Vector3d*)state);
        });
        TimePoint endTime = startTime + 1000000000.0_s;
        err_t rc = propagator.propagate(startTime, endTime, currentPos, currentVel);
        currentTime = endTime;
        if(rc){
            aError("Propagation failed: %d", rc);
        }
        aLog("--------------------------\n");
        aLog("After Propagation:\n");
        aLog("currentPos:  %s\ncurrentVel:  %s\n", currentPos.toString().c_str(), currentVel.toString().c_str());
        aLog("startTime:   %s\ncurrentTime: %s\n", startTime.toString().c_str(),  currentTime.toString().c_str());
        aLog("--------------------------\n");
        // 计算近月点高度
        Vector3d posInMoonICRF;
        aEarthICRFToMoonICRF(currentTime, currentPos, posInMoonICRF);
        cnstrAltOfMoonPeri = posInMoonICRF.norm() - kMoonRadius;
    }
    // 预报到近地点
    {
        HPOP propagator;
        propagator.setForceModel(forceModel);
        auto integrator = propagator.getIntegrator();
        TimePoint startTime = currentTime;
        // 近地点停止条件
        auto stopEarthPeriapsis = integrator->addEventDetector([](const double* state, double t){
            const Vector3d& satPos = *(const Vector3d*)state;
            const Vector3d& satVel = *(const Vector3d*)(state + 3);
            double dotProduct = satPos.dot(satVel);
            return dotProduct;
        });
        stopEarthPeriapsis->setDirection(ODEEventDetector::eIncrease);
        // 记录星历
        integrator->addStateObserver([&](const double* state, double t){
            posM2EList_.push_back(*(const Vector3d*)state);
        });
        TimePoint endTime = startTime + 1000000000.0_s;
        err_t rc = propagator.propagate(startTime, endTime, currentPos, currentVel);
        currentTime = endTime;
        if(rc){
            aError("Propagation failed: %d", rc);
        }
        aLog("--------------------------\n");
        aLog("After Propagation:\n");
        aLog("currentPos:  %s\ncurrentVel:  %s\n", currentPos.toString().c_str(), currentVel.toString().c_str());
        aLog("startTime:   %s\ncurrentTime: %s\n", startTime.toString(15).c_str(),  currentTime.toString(15).c_str());
        aLog("--------------------------\n");
        // 计算近地点高度
        cnstrAltOfEarthPeri = currentPos.norm() - kEarthRadius;
        // 计算轨道倾角
        OrbElem orbElem;
        aCartToOrbElem(currentPos, currentVel, kEarthGrav, orbElem);
        cnstrEarthInclination = orbElem.i();
    }
    aLog("cnstrAltOfMoonPeri: %lf m\n", cnstrAltOfMoonPeri);
    aLog("cnstrAltOfEarthPeri: %lf m\n", cnstrAltOfEarthPeri);
    aLog("cnstrEarthInclination: %lf deg\n", cnstrEarthInclination / deg);
    cnstrAltOfMoonPeri -= 100000_m;
    cnstrEarthInclination -= 43_deg;
    cnstrAltOfEarthPeri -= 50000_m;
    // cnstrAltOfMoonPeri /= 100000_m;
    // cnstrAltOfEarthPeri /= 100000_m;
    if (showLog_)
    {
        printf("variable: %.15g, %.15g, %.15g\n", variable[0], variable[1], variable[2]);
        printf("constraint: %.15g, %.15g, %.15g\n", constraint[0], constraint[1], constraint[2]);
        printf("\n");
    }
    if(plotOrbit_)
    {
        plt::hold(true);
        this->plotOrbit();
    }
    return 0;
}



void FreeReturnTargetFunction::plotOrbit() const
{
    std::vector<double> xList, yList, zList;
    xList.reserve(posE2MList_.size());
    yList.reserve(posE2MList_.size());
    zList.reserve(posE2MList_.size());
    for(const auto& posE2M : posE2MList_){
        xList.push_back(posE2M.x());
        yList.push_back(posE2M.y());
        zList.push_back(posE2M.z());
    }
    plt::plot3(xList, yList, zList, "r");
    xList.clear();
    yList.clear();
    zList.clear();
    xList.reserve(posM2EList_.size());
    yList.reserve(posM2EList_.size());
    zList.reserve(posM2EList_.size());
    for(const auto& posM2E : posM2EList_){
        xList.push_back(posM2E.x());
        yList.push_back(posM2E.y());
        zList.push_back(posM2E.z());
    }
    plt::hold(true);
    plt::plot3(xList, yList, zList, "b");
    // plt::title("Free Return Orbit");
    // plt::xlabel("x (m)");
    // plt::ylabel("y (m)");
    // plt::zlabel("z (m)");
    // plt::grid(true);
    
}


int freeReturnTest1()
{
    aInitialize();
    double variable[3] = {0.0, 0.0, 0.0};
    double constraint[3] = {};
    auto freeRreturnTargetFunction = FreeReturnTargetFunction::Case1();
    freeRreturnTargetFunction(variable, constraint);
    return 0;
}

int freeReturnTest2()
{
    aInitialize();
    NLPProblem problem(new FreeReturnProblem());
    double ustep = 1e-10;
    double x[3] = {0.0, 0.0, 0.0};
    double colmaj_jacobi[9] = {};
    problem.evalNLEJacobiBD(ustep, 3, x, 3, colmaj_jacobi);
    aColMajorMatrixPrint(colmaj_jacobi, 3, 3);
    return 0;
}

int targetfunc(void *p, int n, const double *x, double *fvec, int iflag )
{
    FreeReturnTargetFunction& freeRreturnTargetFunction = *(FreeReturnTargetFunction*)p;
    freeRreturnTargetFunction(x, fvec);
    return iflag;
}

int freeReturnTest3()
{
    aInitialize();
    double x[3]{0,0,0};
    double y[3]{};
    const int n = 3;
    const int bufsize = (n*(3*n+13))/2;
    double work[bufsize] = {};
    auto freeRreturnTargetFunction = FreeReturnTargetFunction::Case1();
    freeRreturnTargetFunction(x, y);
    printf("before hybrd1:\n");
    printf("x: %lf, %lf, %lf\n", x[0], x[1], x[2]);
    printf("y: %lf, %lf, %lf\n", y[0], y[1], y[2]);
    int err = hybrd1(&targetfunc, &freeRreturnTargetFunction, 3, x, y, 1e-3, work, bufsize);
    freeRreturnTargetFunction(x, y);
    
    // freeRreturnTargetFunction.plotOrbit();
    // plt::show();
    printf("after hybrd1:\n");
    printf("err: %d\n", err);
    printf("x: %lf, %lf, %lf\n", x[0], x[1], x[2]);
    printf("y: %lf, %lf, %lf\n", y[0], y[1], y[2]);
    if(abs(y[0]) > 0.1 || abs(y[1]) > 0.1 || abs(y[2]) > 3){
        aError("hybrd1 failed: %d", err);
        exit(-1);
    }
    return 0;
}


int freeReturnTest4()
{
    aInitialize();
    double x[3]{0,0,0};
    double y[3]{};
    const int n = 3;
    const int bufsize = (n*(3*n+13))/2;
    double work[bufsize] = {};
    plt::figure(true);
    auto freeRreturnTargetFunction = FreeReturnTargetFunction::Case2();
    freeRreturnTargetFunction.initImpulse_ = 3159;
    freeRreturnTargetFunction(x, y);
    printf("before hybrd1:\n");
    printf("x: %lf, %lf, %lf\n", x[0], x[1], x[2]);
    printf("y: %lf, %lf, %lf\n", y[0], y[1], y[2]);
    freeRreturnTargetFunction.showLog_ = true;
    freeRreturnTargetFunction.plotOrbit_ = true;
    int err = hybrd1(&targetfunc, &freeRreturnTargetFunction, 3, x, y, 1e-3, work, bufsize);
    freeRreturnTargetFunction(x, y);
    printf("after hybrd1:\n");
    printf("err: %d\n", err);
    printf("x: %lf, %lf, %lf\n", x[0], x[1], x[2]);
    printf("y: %lf, %lf, %lf\n", y[0], y[1], y[2]);
    if(aStdOutIsTerminal())
        plt::show();
    if(abs(y[0]) > 0.1 || abs(y[1]) > 0.1 || abs(y[2]) > 2){
        aError("hybrd1 failed: %d", err);
        exit(-1);
    }
    return 0;
}



void plotFreeReturnProblemForImpulseRange(double minImpulse, double maxImpulse, size_t nPoints, double draan=0)
{
    std::vector<double> impulseArr = plt::linspace(minImpulse, maxImpulse, nPoints);
    std::vector<double> moonAlt;
    std::vector<double> earthAlt;
    std::vector<double> earthInc;
    std::vector<double> earthIncDeg;
    size_t n = impulseArr.size();
    moonAlt.reserve(n);
    earthInc.reserve(n);
    earthIncDeg.reserve(n);
    earthAlt.reserve(n);


    FreeReturnTargetFunction freeRreturnTargetFunction = FreeReturnTargetFunction::Case2();
    double variable[3] = {0.0, draan, 0.0};
    double constraint[3] = {};
    for(auto impulse : impulseArr){
        variable[2] = impulse;
        freeRreturnTargetFunction(variable, constraint);
        moonAlt.push_back(constraint[0]);
        earthInc.push_back(constraint[1]);
        earthIncDeg.push_back(constraint[1] / 1_deg);
        earthAlt.push_back(constraint[2]);
    }
    //  figure1: 月球高度
    // plt::figure();
    plt::plot(impulseArr, moonAlt);
    plt::title("Moon Altitude");
    plt::xlabel("Impulse (N)");
    plt::ylabel("Moon Altitude (m)");
    plt::grid(true);
    
    //  figure3: 地球轨道倾角
    // plt::figure();
    // plt::plot(impulseArr, earthIncDeg);
    // plt::title("Earth Inclination");
    // plt::xlabel("Impulse (N)");
    // plt::ylabel("Earth Inclination (deg)");
    // plt::grid(true);
    // 
    // //  figure4: 地球轨道高度
    // plt::figure();
    // plt::plot(impulseArr, earthAlt);
    // plt::title("Earth Altitude");
    // plt::xlabel("Impulse (N)");
    // plt::ylabel("Earth Altitude (m)");
    // plt::grid(true);
}

void plotFreeReturnProblemForRAANRange(double minRAAN, double maxRAAN, size_t nPoints)
{
    std::vector<double> RAANArr = plt::linspace(minRAAN, maxRAAN, nPoints);
    std::vector<double> moonAlt;
    std::vector<double> earthAlt;
    std::vector<double> earthInc;
    std::vector<double> earthIncDeg;
    size_t n = RAANArr.size();
    moonAlt.reserve(n);
    earthInc.reserve(n);
    earthIncDeg.reserve(n);
    earthAlt.reserve(n);

    FreeReturnTargetFunction freeRreturnTargetFunction = FreeReturnTargetFunction::Case2();
    double variable[3] = {0.0, 0.0, 0.0};
    double constraint[3] = {};
    for(auto RAAN : RAANArr){
        variable[1] = RAAN;
        freeRreturnTargetFunction(variable, constraint);
        moonAlt.push_back(constraint[0]);
        earthInc.push_back(constraint[1]);
        earthIncDeg.push_back(constraint[1] / 1_deg);
        earthAlt.push_back(constraint[2]);
    }
    //  figure1: 月球高度
    plt::figure();
    plt::plot(RAANArr, moonAlt);
    plt::title("Moon Altitude");
    plt::xlabel("dRAAN (deg)");
    plt::ylabel("Moon Altitude (m)");
    plt::grid(true);    

    //  figure2: 地球轨道倾角
    plt::figure();
    plt::plot(RAANArr, earthIncDeg);
    plt::title("Earth Inclination");
    plt::xlabel("dRAAN (deg)");
    plt::ylabel("Earth Inclination (deg)");
    plt::grid(true);
    
    //  figure3: 地球轨道高度
    plt::figure();
    plt::plot(RAANArr, earthAlt);
    plt::title("Earth Altitude");
    plt::xlabel("dRAAN (deg)");
    plt::ylabel("Earth Altitude (m)");
    plt::grid(true);
}

void plotFreeReturnProblemForEpochRange(double minEpoch, double maxEpoch, size_t nPoints)
{
    std::vector<double> epochArr = plt::linspace(minEpoch, maxEpoch, nPoints);
    std::vector<double> moonAlt;
    std::vector<double> earthAlt;
    std::vector<double> earthInc;
    std::vector<double> earthIncDeg;
    size_t n = epochArr.size();
    moonAlt.reserve(n);
    earthInc.reserve(n);
    earthIncDeg.reserve(n);
    earthAlt.reserve(n);
    FreeReturnTargetFunction freeRreturnTargetFunction = FreeReturnTargetFunction::Case1();
    double variable[3] = {0.0, 0.0, 0.0};
    double constraint[3] = {};
    for(auto epoch : epochArr){
        variable[0] = epoch;
        freeRreturnTargetFunction(variable, constraint);
        moonAlt.push_back(constraint[0]);
        earthInc.push_back(constraint[1]);
        earthIncDeg.push_back(constraint[1] / 1_deg);
        earthAlt.push_back(constraint[2]);
    }
    //  figure1: 月球高度
    plt::figure();
    plt::plot(epochArr, moonAlt);
    plt::title("Moon Altitude");
    plt::xlabel("Epoch (s)");
    plt::ylabel("Moon Altitude (m)");
    plt::grid(true);    
    
    //  figure2: 地球轨道倾角
    plt::figure();
    plt::plot(epochArr, earthIncDeg);
    plt::title("Earth Inclination");
    plt::xlabel("Epoch (s)");
    plt::ylabel("Earth Inclination (deg)");
    plt::grid(true);
    
    //  figure3: 地球轨道高度
    plt::figure();
    plt::plot(epochArr, earthAlt);
    plt::title("Earth Altitude");
    plt::xlabel("Epoch (s)");
    plt::ylabel("Earth Altitude (m)");
    plt::grid(true);

}

void surfFreeReturnProblem()
{
    FreeReturnTargetFunction freeRreturnTargetFunction = FreeReturnTargetFunction::Case2();
    double variable[3] = {0.0, 0.0, 3160_m/s};
    double constraint[3] = {};
    freeRreturnTargetFunction(variable, constraint);
    printf("moonAlt: %lf, earthInc: %lf, earthAlt: %lf\n", constraint[0], constraint[1], constraint[2]);
    freeRreturnTargetFunction.plotOrbit();

    variable[2] = 2900_m/s;
    freeRreturnTargetFunction(variable, constraint);
    printf("moonAlt: %lf, earthInc: %lf, earthAlt: %lf\n", constraint[0], constraint[1], constraint[2]);
    plt::hold(true);
    plt::axis(plt::equal);
    freeRreturnTargetFunction.plotOrbit();
    if(aStdOutIsTerminal())
        plt::show();

    std::vector<double> raanArr = plt::linspace(-180_deg, 180.0_deg, 100);
    std::vector<double> ImpulseArr = plt::linspace(2.0_km/s, 3.2_km/s, 100);
    auto xypair = plt::meshgrid(raanArr, ImpulseArr);
    auto& x = xypair.first;
    auto& y = xypair.second;

    auto z = plt::transform(x, y, [&](double raan, double impulse){
        variable[1] = raan;
        variable[2] = impulse;
        freeRreturnTargetFunction(variable, constraint);
        return constraint[0];
    });

    plt::figure(true);
    plt::surf(x, y, z);
    plt::title("Moon Altitude");
    plt::xlabel("dRAAN (rad)");
    plt::ylabel("Impulse (N)");
    
    plt::figure();
    plt::contourf(x, y, z);
    plt::title("Moon Altitude");
    plt::xlabel("dRAAN (rad)");
    plt::ylabel("Impulse (N)");
}

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif


void plotFreeReturnProblem()
{
    plotFreeReturnProblemForEpochRange(-30_day, 30.0_day, 500);
    // plotFreeReturnProblemForRAANRange(0.0, 360.0_deg, 100);

    plt::figure(true);
    plt::hold(true);
    double start = 2000.0_m/s;
    double end = 3150.0_m/s;
    plotFreeReturnProblemForImpulseRange(start, end, 500, 0_deg);
    plotFreeReturnProblemForImpulseRange(start, end, 500, 40_deg);
    plotFreeReturnProblemForImpulseRange(start, end, 500, 80_deg);
    plotFreeReturnProblemForImpulseRange(start, end, 500, 120_deg);
    plotFreeReturnProblemForImpulseRange(start, end, 500, 160_deg);
    plotFreeReturnProblemForImpulseRange(start, end, 500, 200_deg);
    plotFreeReturnProblemForImpulseRange(start, end, 500, 240_deg);
    plotFreeReturnProblemForImpulseRange(start, end, 500, 280_deg);
    plotFreeReturnProblemForImpulseRange(start, end, 500, 320_deg);
    plt::legend({"0deg", "40deg", "80deg", "120deg", "160deg", "200deg", "240deg", "280deg", "320deg"})
        ->location(plt::legend::general_alignment::bottomleft);
    if(aStdOutIsTerminal())
        plt::show();

    surfFreeReturnProblem();
    bool isterm = aStdOutIsTerminal();
    printf("isterm: %d\n", isterm);
    if(isterm)
        plt::show();
    
}