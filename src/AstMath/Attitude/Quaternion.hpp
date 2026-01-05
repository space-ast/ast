///
/// @file      Quaternion.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      15.11.2025
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include "AstMath/MathOperator.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/AttitudeConvert.hpp"

AST_NAMESPACE_BEGIN

class Quaternion;

/// @brief 四元数乘积
/// @param q1 四元数1
/// @param q2 四元数2
/// @return 四元数乘积
Quaternion aQuatProduct(const Quaternion& q1, const Quaternion& q2);


/// @brief 四元数
class Quaternion
{
public:
	/// @brief 创建单位四元数
	static Quaternion Identity(){return Quaternion{1,0,0,0};}

	/// @brief 从旋转矩阵创建四元数
	/// @param mtx 旋转矩阵
	/// @return 四元数
	static Quaternion FromRotationMatrix(const Matrix3d& mtx){return aMatrixToQuat(mtx);}

	/// @brief 计算四元数的范数
	/// @return 四元数的范数
	double norm() const{return _ASTMATH norm(*this); }
	/// @brief 归一化四元数
	void normalize(){ _ASTMATH normalize(*this); }
	/// @brief 归一化四元数
	/// @return 归一化四元数
	Quaternion normalized() const{return _ASTMATH normalized(*this);}
	/// @brief 设置四元数为单位四元数
	void setIdentity(){ *this = {1,0,0,0};}
	/// @brief 四元数的实部
	double qs() const{return m_qs;}
	/// @brief 四元数的虚部x
	double qx() const{return m_qx;}
	/// @brief 四元数的虚部y
	double qy() const{return m_qy;}
	/// @brief 四元数的虚部z
	double qz() const{return m_qz;}
	/// @brief 四元数的实部
	double& qs(){return m_qs;}
	/// @brief 四元数的虚部x
	double& qx(){return m_qx;}
	/// @brief 四元数的虚部y
	double& qy(){return m_qy;}
	/// @brief 四元数的虚部z
	double& qz(){return m_qz;}
	/// @brief 四元数的虚部向量
	const Vector3d& vec() const {return (const Vector3d&)m_qx;}

public:
	// 兼容Eigen接口

	/// @brief 四元数的虚部x
	double x() const{return qx();}
	/// @brief 四元数的虚部y
	double y() const{return qy();}
	/// @brief 四元数的虚部z
	double z() const{return qz();}
	/// @brief 四元数的实部
	double w() const{return qs();}

	A_DEF_POD_ITERABLE(double)
public:
	/// @brief 计算四元数的平方范数
	/// @return 四元数的平方范数
	double squaredNorm() const{return _ASTMATH squaredNorm(*this);}


	/// @brief 计算四元数的共轭
	/// @return 四元数的共轭
	Quaternion conjugate() const{return {qs(), -qx(), -qy(), -qz()};}

	/// @brief 计算四元数的逆
	/// @return 四元数的逆
	Quaternion inverse() const{return conjugate() / squaredNorm();}


	/// @brief 四元数乘积
	/// @param q2 四元数2
	/// @return 四元数乘积
	Quaternion operator*(const Quaternion& q2) const{return aQuatProduct(*this, q2);}

	/// @brief 四元数乘积赋值
	/// @param q2 四元数2
	/// @return 四元数乘积赋值
	Quaternion& operator*=(const Quaternion& q2){return *this = aQuatProduct(*this, q2);}
	

	/// @brief 四元数标量乘积
	/// @param val 标量
	/// @return 四元数标量乘积
	Quaternion operator*(double val) const{return Quaternion{*this} *= val;}

	/// @brief 四元数标量乘积赋值
	/// @param val 标量
	/// @return 四元数标量乘积赋值
	Quaternion& operator*=(double val){qs() *= val; qx() *= val; qy() *= val; qz() *= val; return *this;}


	/// @brief 四元数标量除法
	/// @param val 标量
	/// @return 四元数标量除法
	Quaternion operator/(double val) const{return Quaternion{*this} /= val;}

	/// @brief 四元数标量除法赋值
	/// @param val 标量
	/// @return 四元数标量除法赋值
	Quaternion& operator/=(double val){qs() /= val; qx() /= val; qy() /= val; qz() /= val; return *this;}
public:
	/// @brief 四元数转旋转矩阵
	/// @return 旋转矩阵
	Matrix3d toRotationMatrix() const{
		Matrix3d mtx;
		aQuatToMatrix(*this, mtx);
		return mtx;
	}

public:
	// 设置为public仅为实现聚合初始化，不要直接访问成员变量
	double m_qs;
	double m_qx;
	double m_qy;
	double m_qz;
};


/// @brief 四元数乘积
/// @param q1 四元数1
/// @param q2 四元数2
/// @return 四元数乘积
inline Quaternion aQuatProduct(const Quaternion& q1, const Quaternion& q2)
{
	return {
		q1.qs() * q2.qs() - q1.qx() * q2.qx() - q1.qy() * q2.qy() - q1.qz() * q2.qz(),
		q1.qs() * q2.qx() + q1.qx() * q2.qs() + q1.qy() * q2.qz() - q1.qz() * q2.qy(),
		q1.qs() * q2.qy() - q1.qx() * q2.qz() + q1.qy() * q2.qs() + q1.qz() * q2.qx(),
		q1.qs() * q2.qz() + q1.qx() * q2.qy() - q1.qy() * q2.qx() + q1.qz() * q2.qs(),
	};
}

/// @brief 矩阵转四元数
/// @param mtx 旋转矩阵
/// @return 四元数
A_ALWAYS_INLINE	Quaternion aMatrixToQuat(const Matrix3d& mtx)
{
	Quaternion q;
	aMatrixToQuat(mtx, q);
	return q;
}


AST_NAMESPACE_END


AST_DECL_TYPE_ALIAS(Quaternion)