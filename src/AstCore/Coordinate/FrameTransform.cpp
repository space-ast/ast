///
/// @file      FrameTransform.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-06
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

#include "FrameTransform.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstMath/Euler.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/Math.hpp"
#include "AstCore/SOFA.hpp"

// #define AST_DEBUG_FRAME 1


AST_NAMESPACE_BEGIN

void aECIToECFTransform(const TimePoint &tp, Rotation &rotation)
{
    return aICRFToECFTransform(tp, rotation);
}

void aECIToECFTransform(const TimePoint &tp, KinematicRotation &rotation)
{
    return aICRFToECFTransform(tp, rotation);
}

void aECIToECFMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    return aICRFToECFMatrix(tp, matrix);
}

void aECIToECF(const TimePoint &tp, const Vector3d &vecECI, Vector3d &vecECF)
{
    return aICRFToECF(tp, vecECI, vecECF);
}

void aECIToECF(const TimePoint &tp, const Vector3d &vecECI, const Vector3d &velECI, Vector3d &vecECF, Vector3d &velECF)
{
    return aICRFToECF(tp, vecECI, velECI, vecECF, velECF);
}

void aJ2000ToECFTransform(const TimePoint &tp, Rotation &rotation)
{
    aJ2000ToECFMatrix(tp, rotation.getMatrix());
}

void aJ2000ToECFTransform(const TimePoint & tp, KinematicRotation & rotationOut)
{
    KinematicRotation rotation;
    KinematicRotation temp;
    
    aJ2000ToMODTransform(tp, rotation.getRotation());
    
    aMODToTODTransform(tp, temp.getRotation());
    rotation.getRotation() *= temp.getRotation();

    aTODToGTODTransform(tp, temp);
    rotation.setRotationRate(temp.getRotationRate() * rotation.getRotation().getMatrix());
    rotation.getRotation() *= temp.getRotation();

    aGTODToECFTransform(tp, temp);
    rotation.getRotation() *= temp.getRotation();

    rotationOut = rotation;
}

void aJ2000ToECFMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    Rotation rotation;
    Rotation temp;
    aJ2000ToMODTransform(tp, rotation);
    aMODToTODTransform(tp, temp);
    rotation *= temp;
    aTODToGTODTransform(tp, temp);
    rotation *= temp;
    aGTODToECFTransform(tp, temp);
    rotation *= temp;
    matrix = rotation.getMatrix();
}

void aJ2000ToECF(const TimePoint &tp, const Vector3d &vecJ2000, Vector3d &vecECF)
{
    Rotation rotation;
    aJ2000ToECFTransform(tp, rotation);
    rotation.transformVector(vecJ2000, vecECF);
}

void aJ2000ToECF(const TimePoint &tp, const Vector3d &vecJ2000, const Vector3d &velJ2000, Vector3d &vecECF, Vector3d &velECF)
{
    KinematicRotation rotation;
    aJ2000ToECFTransform(tp, rotation);
    rotation.transformVectorVelocity(vecJ2000, velJ2000, vecECF, velECF);
}

// J2000 -> MOD 转换

void aJ2000ToMODTransform(const TimePoint &tp, Rotation &rotation)
{
    aJ2000ToMODMatrix(tp, rotation.getMatrix());
}


void aJ2000ToMODMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    double zeta, z, theta;
    aPrecession_IAU1976(tp, zeta, z, theta);
    aEuler323ToMatrix({-zeta, theta, -z}, matrix);
}


void aJ2000ToMOD(const TimePoint &tp, const Vector3d &vecJ2000, Vector3d &vecMOD)
{
    Rotation rotation;
    aJ2000ToMODTransform(tp, rotation);
    vecMOD = rotation.transformVector(vecJ2000);
}

// MOD -> TOD 转换

void aMODToTODTransform(const TimePoint &tp, Rotation &rotation)
{
    return aMODToTODMatrix(tp, rotation.getMatrix());
}


void aMODToTODMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    double t = tp.julianCenturyFromJ2000TT();
    double moe = aMeanObliquity_IAU1980(t);
    double dpsi, deps;
    
    aNutation(tp, dpsi, deps);

    #ifdef AST_DEBUG_FRAME
    
    dpsi = 0; deps = 0;
    aNutation_IAU1980(t, dpsi, deps);
    printf("IAU1980: dpsi = %.20f, deps = %.20f\n", dpsi, deps);

    dpsi = 0; deps = 0;
    err_t err = aJplDeGetNutation(tp, dpsi, deps);
    printf("JPL DE: dpsi = %.20f, deps = %.20f, err = %d\n", dpsi, deps, err);
    
    #endif

    double toe = moe + deps;
    aEuler131ToMatrix({moe, -dpsi, -toe}, matrix);
}

void aMODToTOD(const TimePoint &tp, const Vector3d &vecMOD, Vector3d &vecTOD)
{
    Rotation rotation;
    aMODToTODTransform(tp, rotation);
    vecTOD = rotation.transformVector(vecMOD);
}

// TOD -> GTOD 转换

void aTODToGTODTransform(const TimePoint &tp, Rotation &rotation)
{
    return aTODToGTODMatrix(tp, rotation.getMatrix());
}

void aTODToGTODTransform(const TimePoint &tp, KinematicRotation &rotation)
{
    aTODToGTODTransform(tp, rotation.getRotation());
    double lod = aLOD(tp);
    double omp = kEarthAngVel * (1 - lod / kSecondsPerDay);
    rotation.setRotationRate({0, 0, omp});
}

void aTODToGTODMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    double gast = aGAST_IAU1994(tp);
    aRotationZMatrix(gast, matrix);
}

void aTODToGTOD(const TimePoint &tp, const Vector3d &vecTOD, Vector3d &vecGTOD)
{
    Rotation rotation;
    aTODToGTODTransform(tp, rotation);
    vecGTOD = rotation.transformVector(vecTOD);
}

void aTODToGTOD(const TimePoint &tp, const Vector3d &vecTOD, const Vector3d &velTOD, Vector3d &vecGTOC, Vector3d &velGTOC)
{
    KinematicRotation rotation;
    aTODToGTODTransform(tp, rotation);
    rotation.transformVectorVelocity(vecTOD, velTOD, vecGTOC, velGTOC);
}

// GTOD -> ECF 转换

void aGTODToECFTransform(const TimePoint &tp, Rotation &rotation)
{
    return aGTODToECFMatrix(tp, rotation.getMatrix());
}

A_ALWAYS_INLINE double poleMotionS(const TimePoint &tp)
{
    const double S_PRIME_RATE = -47e-6 * kArcSecToRad;
    return S_PRIME_RATE * tp.julianCenturyFromJ2000TT();
}

struct PoleMotionSXY{
    double s, x, y;
};

#if 0
static void aPoleMotionMatrix_1(const PoleMotionSXY& sxy, Matrix3d &matrix)
{
    double cosx, sinx, cosy, siny;
    sincos(sxy.x, &sinx, &cosx);
    sincos(sxy.y, &siny, &cosy);
    matrix= {
        cosx, sinx * siny, sinx * cosy,
        0, cosy, -siny,
        -sinx, cosx*siny,cosx*cosy
    };
}

static void aPoleMotionMatrix_2(const PoleMotionSXY& sxy, Matrix3d &matrix)
{
    aEuler321ToMatrix({0, -sxy.x, -sxy.y}, matrix);
}
#endif

static void aPoleMotionMatrix_3(const PoleMotionSXY& sxy, Matrix3d &matrix)
{
    aEuler321ToMatrix({sxy.s, -sxy.x, -sxy.y}, matrix);
}

void aGTODToECFMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    PoleMotionSXY sxy;
    sxy.s = poleMotionS(tp);
    aPoleMotion(tp, sxy.x, sxy.y);
    // printf("sp = %.20g, x = %.20g, y = %.20g\n", sxy.s, sxy.x, sxy.y);
    // aPoleMotionMatrix_1(sxy, matrix);
    // aPoleMotionMatrix_2(sxy, matrix);
    aPoleMotionMatrix_3(sxy, matrix);
}

void aGTODToECF(const TimePoint &tp, const Vector3d &vecGTOD, Vector3d &vecECF)
{
    Rotation rotation;
    aGTODToECFTransform(tp, rotation);
    vecECF = rotation.transformVector(vecGTOD);
}

// ICRF -> ECF 转换

void aICRFToECFTransform(const TimePoint & tp, Rotation & rotation)
{
    return aICRFToECFMatrix(tp, rotation.getMatrix());
}

void aICRFToECFTransform(const TimePoint &tp, KinematicRotation &rotationOut)
{
    KinematicRotation rotation;
    KinematicRotation temp;
    
    aICRFToCIRFTransform(tp, rotation.getRotation());
    
    aCIRFToTIRFTransform(tp, temp);
    rotation.setRotationRate(temp.getRotationRate() * rotation.getRotation().getMatrix());
    rotation.getRotation() *= temp.getRotation();

    aTIRFToECFTransform(tp, temp);
    rotation.getRotation() *= temp.getRotation();

    rotationOut = rotation;
}

void aICRFToECFMatrix(const TimePoint & tp, Matrix3d & matrix)
{
    Rotation rotation;
    Rotation temp;
    aICRFToCIRFTransform(tp, rotation);
    aCIRFToTIRFTransform(tp, temp);
    rotation *= temp;
    aTIRFToECFTransform(tp, temp);
    rotation *= temp;
    matrix = rotation.getMatrix();
}

void aICRFToECF(const TimePoint & tp, const Vector3d & vecICRF, Vector3d & vecECF)
{
    Rotation rotation;
    aICRFToECFTransform(tp, rotation);
    vecECF = rotation.transformVector(vecICRF);
}

void aECFToICRF(const TimePoint &tp, const Vector3d &vecECF, Vector3d &vecICRF)
{
    Matrix3d matrix;
    aICRFToECFMatrix(tp, matrix);
    vecICRF = vecECF * matrix;
}

void aICRFToECF(const TimePoint &tp, const Vector3d &vecICRF, const Vector3d &velICRF, Vector3d &vecECF, Vector3d &velECF)
{
    KinematicRotation rotation;
    aICRFToECFTransform(tp, rotation);
    rotation.transformVectorVelocity(vecICRF, velICRF, vecECF, velECF);
}

// ICRF -> CIRF 转换

void aICRFToCIRFTransform(const TimePoint &tp, Rotation &rotation)
{
    return aICRFToCIRFMatrix(tp, rotation.getMatrix());
}


/// @brief 根据xys值计算ICRF到CIRF的旋转矩阵
/// @detail 参考SOFA库函数 iauC2ixys
/// @param xys 
/// @param matrix 
static void aXYSMatrix_ICRFToCIRF(const array3d& xys, Matrix3d &matrix)
{
    /*
    参考SOFA库函数 iauC2ixys
    */

    double r2, e, d;

    double x = xys[0];
    double y = xys[1];
    double s = xys[2];

    /* Obtain the spherical angles E and d. */
    r2 = x*x + y*y;
    e = (r2 > 0.0) ? atan2(y, x) : 0.0;
    d = atan(sqrt(r2 / (1.0 - r2)));

    /* Form the matrix. */
    aEuler323ToMatrix({e, d, -(e+s)}, matrix);
}

void aICRFToCIRFMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    array3d xys;
    aXYS(tp, xys);
    aXYSMatrix_ICRFToCIRF(xys, matrix);
}

void aICRFToCIRF(const TimePoint &tp, const Vector3d &vecICRF, Vector3d &vecCIRF)
{
    Rotation rotation;
    aICRFToCIRFTransform(tp, rotation);
    vecCIRF = rotation.transformVector(vecICRF);
}



// CIRF -> TIRF 转换

void aCIRFToTIRFTransform(const TimePoint & tp, Rotation & rotation)
{
    return aCIRFToTIRFMatrix(tp, rotation.getMatrix());
}

void aCIRFToTIRFTransform(const TimePoint &tp, KinematicRotation &rotation)
{
    aCIRFToTIRFTransform(tp, rotation.getRotation());
    double lod = aLOD(tp);
    double omp = kEarthAngVel * (1 - lod / kSecondsPerDay);
    rotation.setRotationRate({0, 0, omp});
}

void aCIRFToTIRFMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    double angle = aEarthRotationAngle_IAU2000(tp);
    aRotationZMatrix(angle, matrix);
}

void aCIRFToTIRF(const TimePoint & tp, const Vector3d & vecCIRF, Vector3d & vecTIRF)
{
    Rotation rotation;
    aCIRFToTIRFTransform(tp, rotation);
    vecTIRF = rotation.transformVector(vecCIRF);
}

void aCIRFToTIRF(const TimePoint &tp, const Vector3d &vecCIRF, const Vector3d &velCIRF, Vector3d &vecTIRF, Vector3d &velTIRF)
{
    KinematicRotation rotation;
    aCIRFToTIRFTransform(tp, rotation);
    rotation.transformVectorVelocity(vecCIRF, velCIRF, vecTIRF, velTIRF);
}

// TIRF -> ECF 转换

void aTIRFToECFTransform(const TimePoint & tp, Rotation & rotation)
{
    return aTIRFToECFMatrix(tp, rotation.getMatrix());
}

void aTIRFToECFMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    return aGTODToECFMatrix(tp, matrix);
}

void aTIRFToECF(const TimePoint & tp, const Vector3d & vecTIRF, Vector3d & vecECF)
{
    Rotation rotation;
    aTIRFToECFTransform(tp, rotation);
    vecECF = rotation.transformVector(vecTIRF);
}

AST_NAMESPACE_END

