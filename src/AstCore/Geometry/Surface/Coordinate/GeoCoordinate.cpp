///
/// @file      GeoCoordinate.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-12
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include "GeoCoordinate.hpp"
#include "GeocentricPoint.hpp"
#include "GeodeticPoint.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN



void aSurfaceCentricToDetic(const GeocentricPoint& centric, double flatFact, GeodeticPoint& detic)
{
	detic.latitude() = aSurfaceCentricToDeticLat(centric.latitude(), flatFact);
	detic.longitude() = centric.longitude();
	detic.altitude() = centric.altitude();
}

void aSurfaceDeticToCentric(const GeodeticPoint& detic, double flatFact, GeocentricPoint& centric)
{
    centric.latitude() = aSurfaceDeticToCentricLat(detic.latitude(), flatFact);
    centric.longitude() = detic.longitude();
    centric.altitude() = detic.altitude();
}

double aSurfaceCentricToDeticLat(double centric, double flatFact)
{
    double ee = flatFact * (2 - flatFact);

    return atan(tan(centric)/(1-ee));
}

double aSurfaceDeticToCentricLat(double detic, double flatFact)
{
    double ee = flatFact * (2 - flatFact);
    return atan((1-ee) * tan(detic));
}


AST_NAMESPACE_END
