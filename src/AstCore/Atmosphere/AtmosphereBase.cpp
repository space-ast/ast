///
/// @file      AtmosphereBase.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-25
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AtmosphereBase.hpp"
#include "AstCore/GeodeticPoint.hpp"
#include "AstCore/SpheroidShape.hpp"
#include <cassert>

AST_NAMESPACE_BEGIN

AtmosphereBase::AtmosphereBase(Frame * frame, BodyShape * bodyShape)
    : frame_{frame}
    , bodyShape_{bodyShape}
{
	assert(frame != nullptr);
	assert(bodyShape != nullptr);
}


void AtmosphereBase::getGeodetic(const Vector3d &posInBodyFixed, double &latitude, double &longitude, double &altitude) const
{
    if(A_UNLIKELY(useApproximateAltitude_))
	{
		// 参考 #hpop/approximateAltitudeComputation.htm

		if(auto spheroidShape = aobject_cast<SpheroidShape*>(bodyShape_))
		{
			double flatFactor = spheroidShape->flatFactor();
			double majorAxis = spheroidShape->majorAxis();
			double xy = hypot(posInBodyFixed.x(), posInBodyFixed.y());
			double latsph = std::atan2(posInBodyFixed.z(), xy);   // 球形下的纬度，atan2 安全处理 xy==0（极点）
			double r = hypot(xy, posInBodyFixed.z());
			altitude = r - majorAxis * (1 - flatFactor) / sqrt(1 - (2 * flatFactor - flatFactor * flatFactor) * square(cos(latsph)));

			// @todo 这里要考虑怎么高效计算地大地纬度，例如不经过迭代计算
			GeodeticPoint geodeticPoint;
			bodyShape_->transform(posInBodyFixed, geodeticPoint);
			latitude = geodeticPoint.latitude();
			longitude = geodeticPoint.longitude();
			return;
		}
	}

	{
		GeodeticPoint geodeticPoint;
		bodyShape_->transform(posInBodyFixed, geodeticPoint);
		latitude = geodeticPoint.latitude();
		longitude = geodeticPoint.longitude();
		altitude = geodeticPoint.altitude();
	}
}

double AtmosphereBase::getAltitude(const Vector3d &posInBodyFixed) const
{
    if(A_UNLIKELY(useApproximateAltitude_))
	{
		if(auto spheroidShape = aobject_cast<SpheroidShape*>(bodyShape_))
		{
			double flatFactor = spheroidShape->flatFactor();
			double majorAxis = spheroidShape->majorAxis();
			double xy = hypot(posInBodyFixed.x(), posInBodyFixed.y());
			double latsph = std::atan2(posInBodyFixed.z(), xy);   // 球形下的纬度，atan2 安全处理 xy==0（极点）
			double r = hypot(xy, posInBodyFixed.z());
			double altitude = r - majorAxis * (1 - flatFactor) / sqrt(1 - (2 * flatFactor - flatFactor * flatFactor) * square(cos(latsph)));
			return altitude;
		}
	}
	{
		GeodeticPoint geodeticPoint;
		bodyShape_->transform(posInBodyFixed, geodeticPoint);
		return geodeticPoint.altitude();
	}
}



AST_NAMESPACE_END


