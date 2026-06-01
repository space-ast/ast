///
/// @file      LocalHorizonalFrame.cpp
/// @brief     局部水平坐标转换实现
/// @details   提供大地坐标到NED坐标、ENU坐标转换函数的实现
/// @author    axel
/// @date      2026-06-01
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

#include "LocalHorizonalFrame.hpp"
#include "AstCore/AER.hpp"
#include "AstMath/MathOperator.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstUtil/Math.hpp"
#include "AstUtil/Logger.hpp"
#include <cmath>


AST_NAMESPACE_BEGIN
using namespace math;

void aGeodeticToNEDTransform(const GeodeticPoint& origin, Rotation& rot)
{
    const array2d angle = { origin.longitude(), -kHalfPI - origin.latitude() };
	aEuler32ToMatrix(angle, rot.getMatrix());
}

void aGeodeticToENUTransform(const GeodeticPoint& origin, Rotation& rot)
{
    const array2d angle = { kHalfPI + origin.longitude(), kHalfPI - origin.latitude() };
	aEuler31ToMatrix(angle, rot.getMatrix());
}


void aBodyFixedToGeodetic(const Vector3d& cart, GeodeticPoint& lla, double radius, double flatFact)
{
	const double EPS = 1.0e-14;

	double B, H, xy, newN, newB, ee, sinB;

	lla.longitude() = atan2(cart[1], cart[0]); // longitude

	// initial value
	// ee = 1.0 - sqrtSafe(1.0 - flatFact);
	ee = flatFact * (2 - flatFact);
	xy = hypot(cart[0], cart[1]);
	H = cart.norm() -  radius * (1 - flatFact);
	newB = atan(cart[2] / (xy * (1 - ee * radius / (radius + H))));
	int niter = 0;
	do
	{
		B = newB;
		sinB = sin(B);
		newN = radius / sqrt(1 - ee * sqrtSafe(sinB));
		newB = atan((cart[2] + ee * newN * sinB) / xy);
		if (++niter > 1000) {
			aWarning("maximum number of iterations reached");
			break;
		}
	} while (fabs(newB - B) > EPS);

	lla.latitude() = newB;
	lla.altitude() = xy / cos(newB) - newN;
}



void aGeodeticToBodyFixed(const GeodeticPoint& lla, Vector3d& cart, double radius, double flatFact)
{
	double slat = sin(lla.latitude());
	//double ee = 1.0 - sqrtSafe(1.0 - flatFact);
	double ee = flatFact * (2 - flatFact);
	double N = radius / sqrt(1.0 - ee * sqrtSafe(slat));
	double temp = (N + lla.altitude()) * cos(lla.latitude());
	cart[0] = temp * cos(lla.longitude());
	cart[1] = temp * sin(lla.longitude());
	cart[2] = (N * (1.0 - ee) + lla.altitude()) * slat;
}


void aGeodeticToBodyFixed(const GeodeticPoint& point, Vector3d& bodyFixed, double radius)
{
	double r = radius + point.altitude();

	double sinLat, cosLat;
	double sinLon, cosLon;
	sincos(point.latitude(), &sinLat, &cosLat);
	sincos(point.longitude(), &sinLon, &cosLon);
	double xy = r * cosLat;
	bodyFixed[0] = xy * cosLon;
	bodyFixed[1] = xy * sinLon;
	bodyFixed[2] = r * sinLat;
}


void aBodyFixedToGeodetic(const Vector3d& bodyFixed, GeodeticPoint& point, double radius)
{
	double xy = hypot(bodyFixed[0], bodyFixed[1]);
	point.longitude() = atan2(bodyFixed[1], bodyFixed[0]);
	point.latitude() = atan2(bodyFixed[2], xy);
	point.altitude() = hypot(xy, bodyFixed[2]) - radius;
}


void aGeodeticToBodyFixed(const GeodeticPoint& point, Vector3d& bodyFixed, double xRadius, double yRadius, double zRadius)
{
	// todo
}

void aBodyFixedToGeodetic(const Vector3d& bodyFixed, GeodeticPoint& point, double xRadius, double yRadius, double zRadius)
{
	// todo
}

AST_NAMESPACE_END
