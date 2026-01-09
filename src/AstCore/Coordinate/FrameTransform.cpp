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
#include "AstCore/RunTime.hpp"
#include "AstUtil/Math.hpp"
#include "SOFA.hpp"

// #define AST_DEBUG_FRAME 1


AST_NAMESPACE_BEGIN


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

void aGTODToECFTransform(const TimePoint &tp, Rotation &rotation)
{
    return aGTODToECFMatrix(tp, rotation.getMatrix());
}

A_ALWAYS_INLINE double poleMotionS(const TimePoint &tp)
{
    const double S_PRIME_RATE = -47e-6 * kArcSecToRad;
    return S_PRIME_RATE * tp.julianCenturyFromJ2000TT();
}
struct SXY{
    double s, x, y;
};

#if 0
static void aPoleMotionMatrix_1(const SXY& sxy, Matrix3d &matrix)
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

static void aPoleMotionMatrix_2(const SXY& sxy, Matrix3d &matrix)
{
    aEuler321ToMatrix({0, -sxy.x, -sxy.y}, matrix);
}
#endif

static void aPoleMotionMatrix_3(const SXY& sxy, Matrix3d &matrix)
{
    aEuler321ToMatrix({sxy.s, -sxy.x, -sxy.y}, matrix);
}

void aGTODToECFMatrix(const TimePoint &tp, Matrix3d &matrix)
{
    SXY sxy;
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

AST_NAMESPACE_END

