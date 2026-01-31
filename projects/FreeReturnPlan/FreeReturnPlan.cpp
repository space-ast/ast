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
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/ODEIntegrator.hpp"
#include "AstMath/Util.hpp"
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

/// @brief     打靶函数
/// @details   计算给定变量下的约束值
/// @param     variable 输入变量数组
/// @param     constraint 输出约束数组
err_t freeRreturnTargetFunction(const double* variable, double* constraint)
{
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

    // 轨道初始状态
    {
        OrbElem initOrbitElement{
            6548151, 0, 21.0_deg,
            149.3340_deg, 0, 199.456_deg
        };
        TimePoint initOrbitEpoch = TimePoint::FromUTC(2028, 6, 24, 16, 25, 59.184);
        TimePoint orbitEpoch = initOrbitEpoch + varEpoch;

        OrbElem orbitElement = initOrbitElement;
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
        Vector3d impulseVNC {3160 + varImpulse, 0.0, 0.0};
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

    return 0;
}


int freeReturnTest1()
{
    aInitialize();
    double variable[3] = {0.0, 0.0, 0.0};
    double constraint[3] = {};
    freeRreturnTargetFunction(variable, constraint);
    aUninitialize();
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
    aUninitialize();
    return 0;
}

int targetfunc(void *p, int n, const double *x, double *fvec, int iflag )
{
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
    freeRreturnTargetFunction(x, y);
    printf("before hybrd1:\n");
    printf("x: %lf, %lf, %lf\n", x[0], x[1], x[2]);
    printf("y: %lf, %lf, %lf\n", y[0], y[1], y[2]);
    int err = hybrd1(&targetfunc, nullptr, 3, x, y, 1e-3, work, bufsize);
    freeRreturnTargetFunction(x, y);
    printf("after hybrd1:\n");
    printf("err: %d\n", err);
    printf("x: %lf, %lf, %lf\n", x[0], x[1], x[2]);
    printf("y: %lf, %lf, %lf\n", y[0], y[1], y[2]);
    aUninitialize();
    return 0;
}
