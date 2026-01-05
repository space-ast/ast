///
/// @file      Euler.hpp
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
#include "AstCore/AttitudeConvert.hpp"

AST_NAMESPACE_BEGIN
 
  
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
    
	err_t toMatrix(int seq, Matrix3d& mtx) const{return aEulerToMatrix(*this, seq,mtx); }
    err_t toQuat(int seq, Quaternion& quat) const{return aEulerToQuat(*this, seq, quat); }
    err_t fromMatrix(const Matrix3d& mtx, int seq){return aMatrixToEuler(mtx, seq, *this); }
    err_t fromQuat(const Quaternion& quat, int seq){return aQuatToEuler(quat, seq, *this); }

    double angle1() const{return angle1_;}
    double angle2() const{return angle2_;}
    double angle3() const{return angle3_;}
    double& angle1(){return angle1_;}
    double& angle2(){return angle2_;}
    double& angle3(){return angle3_;}
    
    A_DEF_POD_ITERABLE(double)

public:
    // 设置为public只是为了聚合初始化，不要直接访问成员变量
	double	angle1_;	///< 第一个角
	double	angle2_;	///< 第二个角
	double	angle3_;	///< 第三个角
};


AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Euler)