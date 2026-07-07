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
#include "AstCore/BodyShape.hpp"
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
	// ee = 1.0 - square(1.0 - flatFact);
	ee = flatFact * (2 - flatFact);
	xy = hypot(cart[0], cart[1]);

#if 0
	// 特判极点附近，避免除以 0 导致 NaN
	if (xy < 1.0e-10)
	{
		lla.latitude() = (cart[2] >= 0.0) ? kHalfPI : -kHalfPI;
		lla.altitude() = std::abs(cart[2]) - radius * (1.0 - flatFact);
		lla.longitude() = 0.0;
		return;
	}
#endif

	H = cart.norm() -  radius * (1 - flatFact);
	newB = std::atan2(cart[2], xy * (1 - ee * radius / (radius + H)));
	int niter = 0;
	do
	{
		B = newB;
		sinB = sin(B);
		newN = radius / sqrt(1 - ee * sinB * sinB);
		newB = std::atan2(cart[2] + ee * newN * sinB, xy);
		if (++niter > 1000) {
			aWarning("maximum number of iterations reached");
			break;
		}
	} while (fabs(newB - B) > EPS);

	// @todo 极点附近：cos(lat) → 0，xy/cos(lat) 数值不稳定
	lla.latitude() = newB;
	lla.altitude() = xy / cos(newB) - newN;
}



void aGeodeticToBodyFixed(const GeodeticPoint& lla, Vector3d& cart, double radius, double flatFact)
{
	double slat, clat, slon, clon;
	sincos(lla.latitude(), &slat, &clat);
	sincos(lla.longitude(), &slon, &clon);

	// ee = 1.0 - square(1.0 - flatFact);
    double ee = flatFact * (2.0 - flatFact);          // e² = 2f - f²
    double N = radius / sqrt(1.0 - ee * slat * slat); 

    double temp = (N + lla.altitude()) * clat;
    cart[0] = temp * clon;
    cart[1] = temp * slon;
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
	double slat, clat, slon, clon;
	sincos(point.latitude(), &slat, &clat);
	sincos(point.longitude(), &slon, &clon);

	// 由纬度 λ、经度 φ 算出表面法线的单位向量(nx, ny, nz) 
	double nx = clat * clon;
	double ny = clat * slon;
	double nz = slat;

	// 找椭球表面上的点 S
	// 根据法线方向(nx, ny, nz) = (Sx/a², Sy/b², Sz/c²) 
	// 和椭球方程 Sx²/a² + Sy²/b² + Sz²/c² = 1
	// 得到 
	//     Sx = a² * nx / d
	//     Sy = b² * ny / d
	//     Sz = c² * nz / d
	// 其中 d = √(a²·nx² + b²·ny² + c²·nz²)
	double xr2 = xRadius * xRadius;
	double yr2 = yRadius * yRadius;
	double zr2 = zRadius * zRadius;

	double d = sqrt(xr2 * nx * nx + yr2 * ny * ny + zr2 * nz * nz);

	// 最终结果 = 表面点 S + 高度 h 沿法线外推
	double h = point.altitude();
	bodyFixed[0] = (xr2 / d + h) * nx;
	bodyFixed[1] = (yr2 / d + h) * ny;
	bodyFixed[2] = (zr2 / d + h) * nz;
}

void aBodyFixedToGeodetic(const Vector3d& bodyFixed, GeodeticPoint& point, double xRadius, double yRadius, double zRadius)
{
	const double EPS = 1.0e-14;

	double xr2 = xRadius * xRadius;
	double yr2 = yRadius * yRadius;
	double zr2 = zRadius * zRadius;

	// 初始猜测: 球形近似
	double xy = hypot(bodyFixed[0], bodyFixed[1]);
	double lat = atan2(bodyFixed[2], xy);
	double lon = atan2(bodyFixed[1], bodyFixed[0]);

	double h, d, slat, clat, slon, clon, nx, ny, nz;
	double sx, sy, sz, newLat, newLon;

	int niter = 0;
	do
	{
		sincos(lat, &slat, &clat);
		sincos(lon, &slon, &clon);

		nx = clat * clon;
		ny = clat * slon;
		nz = slat;

		d = sqrt(xr2 * nx * nx + yr2 * ny * ny + zr2 * nz * nz);

		sx = xr2 * nx / d;
		sy = yr2 * ny / d;
		sz = zr2 * nz / d;

		h = (bodyFixed[0] - sx) * nx + (bodyFixed[1] - sy) * ny + (bodyFixed[2] - sz) * nz;

		double psx = bodyFixed[0] - h * nx;
		double psy = bodyFixed[1] - h * ny;
		double psz = bodyFixed[2] - h * nz;

		double nnx = psx / xr2;
		double nny = psy / yr2;
		double nnz = psz / zr2;
		double nn = sqrt(nnx * nnx + nny * nny + nnz * nnz);

		newLat = asin(nnz / nn);
		newLon = atan2(nny, nnx);

		if (++niter > 1000)
		{
			aWarning("maximum number of iterations reached");
			break;
		}

		double dlat = fabs(newLat - lat);
		double dlon = fabs(newLon - lon);

		lat = newLat;
		lon = newLon;

		if (dlat <= EPS && dlon <= EPS)
			break;
	} while (true);

	point.latitude() = newLat;
	point.longitude() = newLon;
	point.altitude() = h;
}


/// @brief 检查体形状是否为空，若为空则使用默认椭球体
inline BodyShape* checkBodyShape(BodyShape* bodyShape)
{
	if(!bodyShape)
	{
		aWarning("bodyShape is not given, use WGS84 spheroid as default");
		bodyShape = aWGS84Spheroid();
	}
	return bodyShape;
}

void aGeodeticToNED(const Vector3d& posInBodyFixed, const GeodeticPoint& origin, Vector3d& ned, BodyShape* bodyShape)
{
	bodyShape = checkBodyShape(bodyShape);
	Vector3d relativePos = posInBodyFixed - bodyShape->transform(origin);
	Rotation rot;
	aGeodeticToNEDTransform(origin, rot);
	ned = rot.transformVector(relativePos);
}

void aGeodeticToNED(const GeodeticPoint& point, const GeodeticPoint& origin, Vector3d& ned, BodyShape* bodyShape)
{
	bodyShape = checkBodyShape(bodyShape);
	Vector3d relativePos = bodyShape->transform(point) - bodyShape->transform(origin);
	Rotation rot;
	aGeodeticToNEDTransform(origin, rot);
	ned = rot.transformVector(relativePos);
}

void aNEDToGeodetic(const Vector3d& ned, const GeodeticPoint& origin, GeodeticPoint& point, BodyShape* bodyShape)
{
	bodyShape = checkBodyShape(bodyShape);
	Rotation rot;
	aGeodeticToNEDTransform(origin, rot);
	Vector3d relativePos = rot.transformVectorInv(ned);
	point = bodyShape->transform(relativePos + bodyShape->transform(origin));
}

void aGeodeticToENU(const Vector3d& posInBodyFixed, const GeodeticPoint& origin, Vector3d& enu, BodyShape* bodyShape)
{
	bodyShape = checkBodyShape(bodyShape);
	Vector3d relativePos = posInBodyFixed - bodyShape->transform(origin);
	Rotation rot;
	aGeodeticToENUTransform(origin, rot);
	enu = rot.transformVector(relativePos);
}

void aGeodeticToENU(const GeodeticPoint& point, const GeodeticPoint& origin, Vector3d& enu, BodyShape* bodyShape)
{
	bodyShape = checkBodyShape(bodyShape);
	Vector3d relativePos = bodyShape->transform(point) - bodyShape->transform(origin);
	Rotation rot;
	aGeodeticToENUTransform(origin, rot);
	enu = rot.transformVector(relativePos);
}

void aENUToGeodetic(const Vector3d& enu, const GeodeticPoint& origin, GeodeticPoint& point, BodyShape* bodyShape)
{
	bodyShape = checkBodyShape(bodyShape);
	Rotation rot;
	aGeodeticToENUTransform(origin, rot);
	Vector3d relativePos = rot.transformVectorInv(enu);
	point = bodyShape->transform(relativePos + bodyShape->transform(origin));
}


AST_NAMESPACE_END
