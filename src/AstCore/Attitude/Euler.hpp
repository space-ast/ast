///
/// @file      Euler.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 根据 Apache 许可证 2.0 版本（"许可证"）授权；
/// 除非符合许可证，否则不得使用此文件。
/// 您可以在以下网址获取许可证副本：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 除非适用法律要求或书面同意，按"原样"分发的软件
/// 没有任何明示或暗示的担保或条件。
/// 请参阅许可证了解特定语言的权限和限制。
 
#pragma once
 
#include "AstGlobal.hpp"
 
AST_NAMESPACE_BEGIN
 
  
class Euler
{
public:
    // 欧拉角转序
    enum ERotationOrder
    {
        eXYZ = 123,
        eXZY = 132,
        eYXZ = 213,
        eYXZ = 231,
        eZXY = 312,
        eZYX = 321,
        eXYX = 121,
        eXZX = 131,
        eYXY = 212,
        eYZY = 232,
        eZXZ = 313,
        eZYZ = 323
    };
    
	void toMatrix(int seqFlag, Matrix3d& mtx) const;
	void toQuat(int seqFlag, Quaternion& quat) const;
	void toAxisAngle(int seqFlag, Vector3d& axis, double& angle);
    void fromQuat(const Quaternion& quat, int seqFlag);
	void fromAxisAngle(Vector3d& axis, double& angle, int seqFlag);
	void fromMatrix(const Matrix3d& mtx, int seqFlag);
    
    double* data() {return &m_angle1;}
    const double* data() const{return &m_angle1;}
    double angle1() const{return m_angle1;}
    double angle2() const{return m_angle2;}
    double angle3() const{return m_angle3;}
    double& angle1(){return m_angle1;}
    double& angle2(){return m_angle2;}
    double& angle3(){return m_angle3;}
public:
    // 设置为public只是为了聚合初始化，不要直接访问成员变量
	double	m_angle1;	///< 第一个角
	double	m_angle2;	///< 第二个角
	double	m_angle3;	///< 第三个角
};


AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Euler)