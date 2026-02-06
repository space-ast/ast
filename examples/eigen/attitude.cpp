///
/// @file      attitude.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-02-03
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

#if defined(AST_WITH_EIGEN) && defined(A_CXX14)

#include "Eigen/Dense"
#include <cstdio>
#include <cassert>
#include <cmath>

#define Error(fmt, ...)      fprintf(stderr, fmt, ##__VA_ARGS__)
#define TO_EULER_EPS         1e-15
constexpr double kHalfPI     =  1.5707963267948966192313216916395	                 ;	    ///< 0.5 * pi
constexpr double kPI         =  3.141592653589793238462643383279502884197169399375105;      ///< pi


#if  defined(__GNUC__) && !defined(__clang__) 
using ::sincos;
#else

inline void sincos (double x, double *psin, double *pcos)
{
    *psin = sin(x);
    *pcos = cos(x);
}

#endif


template<typename _Scalar>
int sign(_Scalar val)
{
    if(val > 0)
        return 1;
    else if(val < 0)
        return -1;
    return 0;
}


using Eigen::Matrix3d;
using Quaternion = Eigen::Quaterniond;
using AngleAxis = Eigen::AngleAxisd;


/// @brief 欧拉角
/// @details 欧拉角是一种表示旋转的参数化方法，
///          它将旋转分解为三个连续的旋转轴上的角度。
///          欧拉角通常用于描述物体的旋转状态，
///          例如在机器人学、航空航天和计算机图形学中。
class Euler
{
public:
    // 欧拉角转序
    enum ERotationOrder
    {
        eX = 1,
        eY = 2,
        eZ = 3,
        eXYZ = 123,
        eXZY = 132,
        eYXZ = 213,
        eYZX = 231,
        eZXY = 312,
        eZYX = 321,
        eXYX = 121,
        eXZX = 131,
        eYXY = 212,
        eYZY = 232,
        eZXZ = 313,
        eZYZ = 323
    };
    
    double angle1() const{return angle1_;}
    double angle2() const{return angle2_;}
    double angle3() const{return angle3_;}
    double& angle1(){return angle1_;}
    double& angle2(){return angle2_;}
    double& angle3(){return angle3_;}
    

public:
	double	angle1_;	///< 第一个角
	double	angle2_;	///< 第二个角
	double	angle3_;	///< 第三个角
};




/// @brief 四元数转坐标转换矩阵
/// @param quat 四元数
/// @param m 坐标转换矩阵
void QuatToMatrix(const Quaternion& quat, Matrix3d& m)
{
	double xy = quat.x() * quat.y();
	double yz = quat.y() * quat.z();
	double zx = quat.z() * quat.x();
	double sx = quat.w() * quat.x();
	double sy = quat.w() * quat.y();
	double sz = quat.w() * quat.z();
	double xx = quat.x() * quat.x();
	double yy = quat.y() * quat.y();
	double zz = quat.z() * quat.z();

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

/// @brief 坐标转换矩阵转四元数
/// @param mtx 坐标转换矩阵
/// @param quat 四元数
void MatrixToQuat(const Matrix3d& mtx, Quaternion& quat)
{
	double w = 1.0 + mtx(0, 0) + mtx(1,1) + mtx(2,2);
	quat.w() = sqrt(w) * 0.5;
	double tempdbl = 4.0 * quat.w();
	quat.x() = (mtx(1,2) - mtx(2,1)) / tempdbl;
	quat.y() = (mtx(2,0) - mtx(0,2)) / tempdbl;
	quat.z() = (mtx(0,1) - mtx(1,0)) / tempdbl;
}



/// @brief 欧拉角123转坐标转换矩阵
/// @param euler 欧拉角
/// @param mtx 坐标转换矩阵
void Euler123ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	double cosa1, sina1, cosa2, sina2, cosa3, sina3;
	sincos(a1, &sina1, &cosa1);
	sincos(a2, &sina2, &cosa2);
	sincos(a3, &sina3, &cosa3);
	
	mtx <<
		cosa2 * cosa3,	cosa1 * sina3 + cosa3 * sina1 * sina2, sina1 * sina3 - cosa1 * cosa3 * sina2,
	   -cosa2 * sina3, cosa1 * cosa3 - sina1 * sina2 * sina3, cosa3 * sina1 + cosa1 * sina2 * sina3,
		sina2,            -cosa2 * sina1,                              cosa1 * cosa2
	;
}

/// @brief 坐标转换矩阵转欧拉角123
/// @param mtx 坐标转换矩阵
/// @param euler 欧拉角
void MatrixToEuler123(const Matrix3d& mtx, Euler& euler)
{
	if (std::abs(mtx(2, 0)) < 1. - TO_EULER_EPS)
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


/// @brief 欧拉角121转坐标转换矩阵
/// @param euler 欧拉角
/// @param mtx 坐标转换矩阵
void Euler121ToMatrix(const Euler& euler, Matrix3d& mtx)
{
	double a1 = euler.angle1();
	double a2 = euler.angle2();
	double a3 = euler.angle3();
	double cosa1, sina1, cosa2, sina2, cosa3, sina3;
	sincos(a1, &sina1, &cosa1);
	sincos(a2, &sina2, &cosa2);
	sincos(a3, &sina3, &cosa3);
	mtx <<
		cosa2,            sina1 * sina2,                               -cosa1 * sina2,
		sina2 * sina3,  cosa1 * cosa3 - cosa2 * sina1 * sina3,  cosa3 * sina1 + cosa1 * cosa2 * sina3,
		cosa3 * sina2, -cosa1 * sina3 - cosa2 * cosa3 * sina1,  cosa1 * cosa2 * cosa3 - sina1 * sina3
	;
}

/// @brief 坐标转换矩阵转欧拉角121
/// @param mtx 坐标转换矩阵
/// @param euler 欧拉角
void MatrixToEuler121(const Matrix3d& mtx, Euler& euler)
{
	if (std::abs(mtx(0, 0)) < 1. - TO_EULER_EPS)
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

/// @brief 四元数转角度轴
/// @param quat 四元数
/// @param aa 角度轴
void QuatToAngleAxis(const Quaternion& quat, AngleAxis& aa)
{
	double n = quat.vec().norm();
	if(n)
	{
    	if (quat.w() < 0){
			aa.angle() = 2 * atan2(n, -(quat.w()));
			aa.axis() = quat.vec() / -n;
		}else{
			aa.angle() = 2 * atan2(n, (quat.w()));
			aa.axis() = quat.vec() / n;
		};
	}
	else
	{
		aa.angle() = 0;
		aa.axis() = {1, 0, 0};
	}
}

/// @brief 角度轴转四元数
/// @param aa 角度轴
/// @param quat 四元数
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

#endif

int main(){

}