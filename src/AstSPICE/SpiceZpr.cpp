///
/// @file      SpiceZpr.cpp
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

#include "SpiceZpr.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstMath/AngleAxis.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN

void axisar(const Vector3d & axis, double angle, Matrix3d & r)
{
    aAngleAxisToMatrix(AngleAxis(-angle, axis), r);
}

void azlrec(double range, double az, double el, bool azccw, bool elplsz, Vector3d &rectan)
{
    if ( !azccw )
        az = -az;

    if ( !elplsz )
        el = -el;

    latrec(range, az, el, rectan);
}

void latrec(double radius, double lon, double lat, Vector3d &rectan)
{
    double cos_lon, sin_lon, cos_lat, sin_lat;
    sincos(lon, &sin_lon, &cos_lon);
    sincos(lat, &sin_lat, &cos_lat);

    rectan[0] = radius * cos_lon * cos_lat;
    rectan[1] = radius * sin_lon * cos_lat;
    rectan[2] = radius * sin_lat;
}

AST_NAMESPACE_END
