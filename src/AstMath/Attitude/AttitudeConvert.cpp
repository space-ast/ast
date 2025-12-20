/// @file      AttitudeConvert.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#include "AttitudeConvert.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Euler.hpp"
#include "AstMath/Quaternion.hpp"
#include "AstMath/AngleAxis.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/MathOperator.hpp"
#include "AstCore/Constants.h"
#include "AstUtil/Logger.hpp"

#include <math.h>
 

#define TO_EULER_EPS 1e-15


AST_NAMESPACE_BEGIN

namespace
{

	err_t aEulerToMatrix(double angle, int axis, Matrix3d& mtx)
	{
		assert(Euler::eX <= axis && axis <= Euler::eZ);

		double c = cos(angle);
		double s = sin(angle);
		switch (axis)
		{
			case Euler::eX:
				mtx(0, 0) = 1.0;	mtx(0, 1) = 0.0;	mtx(0, 2) = 0.0;
				mtx(1, 0) = 0.0;	mtx(1, 1) = c;		mtx(1, 2) = s;
				mtx(2, 0) = 0.0;	mtx(2, 1) = -s;		mtx(2, 2) = c;
				return 0;
			case Euler::eY:
				mtx(0, 0) = c;		mtx(0, 1) = 0.0;	mtx(0, 2) = -s;
				mtx(1, 0) = 0.0;	mtx(1, 1) = 1.0;	mtx(1, 2) = 0.0;
				mtx(2, 0) = s;		mtx(2, 1) = 0.0;	mtx(2, 2) = c;
				return 0;
			case Euler::eZ:
				mtx(0, 0) = c;		mtx(0, 1) = s;		mtx(0, 2) = 0.0;
				mtx(1, 0) = -s;		mtx(1, 1) = c;		mtx(1, 2) = 0.0;
				mtx(2, 0) = 0.0;	mtx(2, 1) = 0.0;	mtx(2, 2) = 1.0;
				return 0;
		}
		// error: invalid parameter
		aError("invalid axis: %d", axis);
		return eErrorInvalidParam;
	}
};
// namespace end
 

void aQuatToMatrix(const Quaternion& quat, Matrix3d& m)
{
	double xy = quat.qx() * quat.qy();
	double yz = quat.qy() * quat.qz();
	double zx = quat.qz() * quat.qx();
	double sx = quat.qs() * quat.qx();
	double sy = quat.qs() * quat.qy();
	double sz = quat.qs() * quat.qz();
	double xx = quat.qx() * quat.qx();
	double yy = quat.qy() * quat.qy();
	double zz = quat.qz() * quat.qz();

	m(0,0) = 1.0 - 2.0 * (yy + zz);	m(0,1) = 2.0 * (xy + sz);		m(0,2) = 2.0 * (zx - sy);
	m(1,0) = 2.0 * (xy - sz);		m(1,1) = 1.0 - 2.0 * (xx + zz);	m(1,2) = 2.0 * (yz + sx);
	m(2,0) = 2.0 * (zx + sy);		m(2,1) = 2.0 * (yz - sx);		m(2,2) = 1.0 - 2.0 * (xx + yy);

	for (int i = 0; i < 9; i++)
	{
		if (m(i) > 1.0)
			m(i) = 1;
		else if (m(i) < -1.0)
			m(i) = -1;
	}
}

void aMatrixToQuat(const Matrix3d& mtx, Quaternion& quat)
{
	double w = 1.0 + mtx(0, 0) + mtx(1,1) + mtx(2,2);
	quat.qs() = sqrt(w) * 0.5;
	double tempdbl = 4.0 * quat.qs();
	quat.qx() = (mtx(1,2) - mtx(2,1)) / tempdbl;
	quat.qy() = (mtx(2,0) - mtx(0,2)) / tempdbl;
	quat.qz() = (mtx(0,1) - mtx(1,0)) / tempdbl;
}

err_t aMatrixToEuler(const Matrix3d& mtx, int seq, Euler& euler)
{
	switch (seq)
	{
		// ABC类型
		case 123:
			aMatrixToEuler123(mtx, euler);
			return 0;
		case 132:
			aMatrixToEuler132(mtx, euler);
			return 0;
		case 213:
			aMatrixToEuler213(mtx, euler);
			return 0;
		case 231:
			aMatrixToEuler231(mtx, euler);
			return 0;
		case 312:
			aMatrixToEuler312(mtx, euler);
			return 0;
		case 321:
			aMatrixToEuler321(mtx, euler);
			return 0;
			// ABA类型
		case 121:
			aMatrixToEuler121(mtx, euler);
			return 0;
		case 131:
			aMatrixToEuler131(mtx, euler);
			return 0;
		case 212:
			aMatrixToEuler212(mtx, euler);
			return 0;
		case 232:
			aMatrixToEuler232(mtx, euler);
			return 0;
		case 313:
			aMatrixToEuler313(mtx, euler);
			return 0;
		case 323:
			aMatrixToEuler323(mtx, euler);
			return 0;
	}
	aError("invalid rotation sequence: %d", seq);
	return eErrorInvalidParam;
}

err_t aEulerToMatrix(const Euler& euler, int seq, Matrix3d& mtx)
{
	switch (seq)
	{
		// ABC类型
		case 123:
			aEuler123ToMatrix(euler, mtx);
			return 0;
		case 132:
			aEuler132ToMatrix(euler, mtx);
			return 0;
		case 213:
			aEuler213ToMatrix(euler, mtx);
			return 0;
		case 231:
			aEuler231ToMatrix(euler, mtx);
			return 0;
		case 312:
			aEuler312ToMatrix(euler, mtx);
			return 0;
		case 321:
			aEuler321ToMatrix(euler, mtx);
			return 0;
			// ABA类型
		case 121:
			aEuler121ToMatrix(euler, mtx);
			return 0;
		case 131:
			aEuler131ToMatrix(euler, mtx);
			return 0;
		case 212:
			aEuler212ToMatrix(euler, mtx);
			return 0;
		case 232:
			aEuler232ToMatrix(euler, mtx);
			return 0;
		case 313:
			aEuler313ToMatrix(euler, mtx);
			return 0;
		case 323:
			aEuler323ToMatrix(euler, mtx);
			return 0;
	}
	aError("invalid rotation sequence: %d", seq);
	return eErrorInvalidParam;
}

err_t _aEulerToMatrix(const Euler& euler, int seq, Matrix3d& mtx)
{
	err_t rc;
	int axis;
	Matrix3d mat;

	axis = seq / 100;
	rc = aEulerToMatrix(euler.angle1(), axis, mtx);

	seq = seq - axis * 100;
	axis = seq / 10;
	rc |= aEulerToMatrix(euler.angle2(), axis, mat);
	mtx = mat * mtx;

	seq = seq - axis * 10;
	rc |= aEulerToMatrix(euler.angle3(), seq, mat);
	mtx = mat * mtx;

	return rc;
}

// matrix and euler convertion


void aEuler123ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a2) * cos(a3),	cos(a1) * sin(a3) + cos(a3) * sin(a1) * sin(a2), sin(a1) * sin(a3) - cos(a1) * cos(a3) * sin(a2),
		-cos(a2) * sin(a3), cos(a1) * cos(a3) - sin(a1) * sin(a2) * sin(a3), cos(a3) * sin(a1) + cos(a1) * sin(a2) * sin(a3),
		sin(a2),            -cos(a2) * sin(a1),                              cos(a1) * cos(a2)
	};
}

void aMatrixToEuler123(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(2, 0)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(-mtx(2, 1), mtx(2, 2));
		euler.angle2() = asin(mtx(2, 0));
		euler.angle3() = atan2(-mtx(1, 0), mtx(0, 0));
	}
	else {              // angle2 旋转90°/270°时有奇异
		euler.angle1() = atan2(mtx(1, 2), mtx(1, 1));
		euler.angle2() = sign(mtx(2, 0)) * kHalfPI;
		euler.angle3() = 0.;
	}
}


void aEuler231ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a1) * cos(a2),								  sin(a2),           -cos(a2) * sin(a1),
		sin(a1) * sin(a3) - cos(a1) * cos(a3) * sin(a2),  cos(a2) * cos(a3),  cos(a1) * sin(a3) + cos(a3) * sin(a1) * sin(a2),
		cos(a3) * sin(a1) + cos(a1) * sin(a2) * sin(a3), -cos(a2) * sin(a3),  cos(a1) * cos(a3) - sin(a1) * sin(a2) * sin(a3)
	};
}

void aMatrixToEuler231(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(0, 1)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(-mtx(0, 2), mtx(0, 0));
		euler.angle2() = asin(mtx(0, 1));
		euler.angle3() = atan2(-mtx(2, 1), mtx(1, 1));
	}
	else {              // angle2 旋转90°/270°时有奇异
		euler.angle1() = atan2(mtx(2, 0), mtx(2, 2));
		euler.angle2() = sign(mtx(0, 1)) * kHalfPI;
		euler.angle3() = 0.;
	}
}


void aEuler312ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a1) * cos(a3) - sin(a1) * sin(a2) * sin(a3), cos(a3) * sin(a1) + cos(a1) * sin(a2) * sin(a3), -cos(a2) * sin(a3),
		-cos(a2) * sin(a1),                           cos(a1) * cos(a2),          sin(a2),
		cos(a1) * sin(a3) + cos(a3) * sin(a1) * sin(a2), sin(a1) * sin(a3) - cos(a1) * cos(a3) * sin(a2),  cos(a2) * cos(a3) 
	};
}


void aMatrixToEuler312(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(1, 2)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(-mtx(1, 0), mtx(1, 1));
		euler.angle2() = asin(mtx(1, 2));
		euler.angle3() = atan2(-mtx(0, 2), mtx(2, 2));
	}
	else {              // angle2 旋转90°/270°时有奇异
		euler.angle1() = atan2(mtx(0, 1), mtx(0, 0));
		euler.angle2() = sign(mtx(1, 2)) * kHalfPI;
		euler.angle3() = 0.;
	}
}



void aEuler321ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a1) * cos(a2),                           cos(a2) * sin(a1),        -sin(a2),
		cos(a1) * sin(a2) * sin(a3) - cos(a3) * sin(a1), cos(a1) * cos(a3) + sin(a1) * sin(a2) * sin(a3), cos(a2) * sin(a3),
		sin(a1) * sin(a3) + cos(a1) * cos(a3) * sin(a2), cos(a3) * sin(a1) * sin(a2) - cos(a1) * sin(a3), cos(a2) * cos(a3)
	};
}



void aMatrixToEuler321(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(0, 2)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(mtx(0, 1), mtx(0, 0));
		euler.angle2() = -asin(mtx(0, 2));
		euler.angle3() = atan2(mtx(1, 2), mtx(2, 2));
	}
	else {            // angle2 旋转90°/270°时有奇异
		euler.angle1() = atan2(mtx(2, 1), mtx(2, 0));
		euler.angle2() = -sign(mtx(0, 2)) * kHalfPI;
		euler.angle3() = 0;
	}
}

void aEuler213ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a1) * cos(a3) + sin(a1) * sin(a2) * sin(a3), cos(a2) * sin(a3), cos(a1) * sin(a2) * sin(a3) - cos(a3) * sin(a1),
		cos(a3) * sin(a1) * sin(a2) - cos(a1) * sin(a3), cos(a2) * cos(a3), sin(a1) * sin(a3) + cos(a1) * cos(a3) * sin(a2),
		cos(a2) * sin(a1),        -sin(a2),                           cos(a1) * cos(a2)
	};
}


void aMatrixToEuler213(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(2, 1)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(mtx(2, 0), mtx(2, 2));
		euler.angle2() = -asin(mtx(2, 1));
		euler.angle3() = atan2(mtx(0, 1), mtx(1, 1));
	}
	else {            // 旋转90°/270°时有奇异
		euler.angle1() = atan2(mtx(1, 0), mtx(1, 2));
		euler.angle2() = -sign(mtx(2, 1)) * kHalfPI;
		euler.angle3() = 0;
	}
}

void aEuler132ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a2) * cos(a3), sin(a1) * sin(a3) + cos(a1) * cos(a3) * sin(a2), cos(a3) * sin(a1) * sin(a2) - cos(a1) * sin(a3),
		-sin(a2),                           cos(a1) * cos(a2),                           cos(a2) * sin(a1),
		cos(a2) * sin(a3), cos(a1) * sin(a2) * sin(a3) - cos(a3) * sin(a1), cos(a1) * cos(a3) + sin(a1) * sin(a2) * sin(a3)
	};
}


void aMatrixToEuler132(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(1, 0)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(mtx(1, 2), mtx(1, 1));
		euler.angle2() = -asin(mtx(1, 0));
		euler.angle3() = atan2(mtx(2, 0), mtx(0, 0));
	}
	else {            // 旋转90°/270°时有奇异
		euler.angle1() = atan2(mtx(0, 2), mtx(0, 1));
		euler.angle2() = -sign(mtx(1, 0)) * kHalfPI;
		euler.angle3() = 0;
	}
}

// ABA

void aEuler121ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a2),            sin(a1) * sin(a2),                               -cos(a1) * sin(a2),
		sin(a2) * sin(a3),  cos(a1) * cos(a3) - cos(a2) * sin(a1) * sin(a3),  cos(a3) * sin(a1) + cos(a1) * cos(a2) * sin(a3),
		cos(a3) * sin(a2), -cos(a1) * sin(a3) - cos(a2) * cos(a3) * sin(a1),  cos(a1) * cos(a2) * cos(a3) - sin(a1) * sin(a3)
	};
}


void aMatrixToEuler121(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(0, 0)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(mtx(0, 1), -mtx(0, 2));
		euler.angle2() = acos(mtx(0, 0));
		euler.angle3() = atan2(mtx(1, 0), mtx(2, 0));
	}
	else {            // angle2 旋转 0°/180°时有奇异
		euler.angle1() = atan2(mtx(1, 2), mtx(1, 1));
		if (mtx(0, 0) < 0)
			euler.angle2() = kPI;
		else
			euler.angle2() = 0;
		euler.angle3() = 0;
	}
}

void aEuler232ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a1) * cos(a2) * cos(a3) - sin(a1) * sin(a3), cos(a3) * sin(a2), -cos(a1) * sin(a3) - cos(a2) * cos(a3) * sin(a1),
		-cos(a1) * sin(a2),         cos(a2),                             sin(a1) * sin(a2),
		cos(a3) * sin(a1) + cos(a1) * cos(a2) * sin(a3), sin(a2) * sin(a3),   cos(a1) * cos(a3) - cos(a2) * sin(a1) * sin(a3)
	};
}


void aMatrixToEuler232(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(1, 1)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(mtx(1, 2), -mtx(1, 0));
		euler.angle2() = acos(mtx(1, 1));
		euler.angle3() = atan2(mtx(2, 1), mtx(0, 1));
	}
	else {            // 旋转 0°/180°时有奇异
		euler.angle1() = atan2(mtx(2, 0), mtx(2, 2));
		if (mtx(1, 1) < 0)
			euler.angle2() = kPI;
		else
			euler.angle2() = 0;
		euler.angle3() = 0;
	}
}

void aEuler313ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a1) * cos(a3) - cos(a2) * sin(a1) * sin(a3), cos(a3) * sin(a1) + cos(a1) * cos(a2) * sin(a3), sin(a2) * sin(a3),
		-cos(a1) * sin(a3) - cos(a2) * cos(a3) * sin(a1), cos(a1) * cos(a2) * cos(a3) - sin(a1) * sin(a3), cos(a3) * sin(a2),
		sin(a1) * sin(a2),                          -cos(a1) * sin(a2),         cos(a2)
	};
}


void aMatrixToEuler313(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(2, 2)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(mtx(2, 0), -mtx(2, 1));
		euler.angle2() = acos(mtx(2, 2));
		euler.angle3() = atan2(mtx(0, 2), mtx(1, 2));
	}
	else {            // 旋转 0°/180°时有奇异
		euler.angle1() = atan2(mtx(0, 1), mtx(0, 0));
		if (mtx(2, 2) < 0)
			euler.angle2() = kPI;
		else
			euler.angle2() = 0;
		euler.angle3() = 0;
	}
}


void aEuler131ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a2),                             cos(a1) * sin(a2),                           sin(a1) * sin(a2),
		-cos(a3) * sin(a2),   cos(a1) * cos(a2) * cos(a3) - sin(a1) * sin(a3), cos(a1) * sin(a3) + cos(a2) * cos(a3) * sin(a1),
		sin(a2) * sin(a3), -cos(a3) * sin(a1) - cos(a1) * cos(a2) * sin(a3), cos(a1) * cos(a3) - cos(a2) * sin(a1) * sin(a3)
	};
}

void aMatrixToEuler131(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(0, 0)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(mtx(0, 2), mtx(0, 1));
		euler.angle2() = acos(mtx(0, 0));
		euler.angle3() = atan2(mtx(2, 0), -mtx(1, 0));
	}
	else {            // 旋转 0°/180°时有奇异
		euler.angle1() = atan2(mtx(1, 2), mtx(1, 1));
		if (mtx(0, 0) < 0)
			euler.angle2() = kPI;
		else
			euler.angle2() = 0;
		euler.angle3() = 0;
	}
}

void aEuler212ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();

	mtx = {
		cos(a1) * cos(a3) - cos(a2) * sin(a1) * sin(a3),  sin(a2) * sin(a3), -cos(a3) * sin(a1) - cos(a1) * cos(a2) * sin(a3),
		sin(a1) * sin(a2),          cos(a2),                             cos(a1) * sin(a2),
		cos(a1) * sin(a3) + cos(a2) * cos(a3) * sin(a1), -cos(a3) * sin(a2),   cos(a1) * cos(a2) * cos(a3) - sin(a1) * sin(a3)
	};
}

void aMatrixToEuler212(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(1, 1)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(mtx(1, 0), mtx(1, 2));
		euler.angle2() = acos(mtx(1, 1));
		euler.angle3() = atan2(mtx(0, 1), -mtx(2, 1));
	}
	else {            // 旋转 0°/180°时有奇异
		euler.angle1() = atan2(mtx(2, 0), mtx(2, 2));
		if (mtx(1, 1) < 0)
			euler.angle2() = kPI;
		else
			euler.angle2() = 0;
		euler.angle3() = 0;
	}
}




void aEuler323ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	mtx = {
		cos(a1) * cos(a2) * cos(a3) - sin(a1) * sin(a3), cos(a1) * sin(a3) + cos(a2) * cos(a3) * sin(a1), -cos(a3) * sin(a2),
		-cos(a3) * sin(a1) - cos(a1) * cos(a2) * sin(a3), cos(a1) * cos(a3) - cos(a2) * sin(a1) * sin(a3),  sin(a2) * sin(a3),
		cos(a1) * sin(a2),                           sin(a1) * sin(a2),          cos(a2)
	};
}

err_t aEulerToQuat(const Euler& euler, int seq, Quaternion& quat)
{
	Matrix3d mtx;
	err_t rc = aEulerToMatrix(euler, seq, mtx);
	aMatrixToQuat(mtx, quat);
	return rc;
}

err_t aQuatToEuler(const Quaternion& quat, int seq, Euler& euler)
{
	Matrix3d mtx;
	aQuatToMatrix(quat, mtx);
	return aMatrixToEuler(mtx, seq, euler);
}


void aMatrixToEuler323(const Matrix3d& mtx, Euler& euler)
{
	if (abs(mtx(2, 2)) < 1. - TO_EULER_EPS)
	{
		euler.angle1() = atan2(mtx(2, 1), mtx(2, 0));
		euler.angle2() = acos(mtx(2, 2));
		euler.angle3() = atan2(mtx(1, 2), -mtx(0, 2));
	}
	else {            // 旋转 0°/180°时有奇异
		euler.angle1() = atan2(mtx(0, 1), mtx(0, 0));
		if (mtx(2, 2) < 0)
			euler.angle2() = kPI;
		else
			euler.angle2() = 0;
		euler.angle3() = 0;
	}
}

void aQuatToAngleAxis(const Quaternion& quat, AngleAxis& aa)
{
	double n = quat.vec().norm();
	if(n)
	{
    	if (quat.qs() < 0){
			aa.angle() = 2 * atan2(n, -(quat.qs()));
			aa.axis() = quat.vec() / -n;
		}else{
			aa.angle() = 2 * atan2(n, (quat.qs()));
			aa.axis() = quat.vec() / n;
		};
	}
	else
	{
		aa.angle() = 0;
		aa.axis() = {1, 0, 0};
	}
}

void aAngleAxisToQuat(const AngleAxis &aa, Quaternion &quat)
{
	double half_angle = aa.angle() / 2.;
	double sina = sin(half_angle);
	double cosa = cos(half_angle);
	quat = {
		cosa,
		aa.axis()[0] * sina,
		aa.axis()[1] * sina,
		aa.axis()[2] * sina,
	};
}

void aAngleAxisToMatrix(const AngleAxis &aa, Matrix3d &mtx)
{
	Quaternion quat;
	aAngleAxisToQuat(aa, quat);
	aQuatToMatrix(quat, mtx);
}

void aMatrixToAngleAxis(const Matrix3d &mtx, AngleAxis &aa)
{
	Quaternion quat;
	aMatrixToQuat(mtx, quat);
	aQuatToAngleAxis(quat, aa);
}

AST_NAMESPACE_END
 

