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
#include "AstMath/AttitudeConvert.hpp"
#include "AstMath/Euler.hpp"
#include "AstCore/RunTime.hpp"
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


AST_NAMESPACE_END

