///
/// @file      Quaternion.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
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
#include "AstCore/MathOperator.hpp"

AST_NAMESPACE_BEGIN


/// @brief 四元数
class Quaternion
{
public:
	/// @brief 创建单位四元数
	static Quaternion Identity(){return Quaternion{1,0,0,0};}
	
	double norm() const{return _AST norm(*this); }
	void normalize(){ _AST normalize(*this); }
	Quaternion normalized() const{return _AST normalized(*this);}
	void setIdentity(){ *this = {1,0,0,0};}
	double qs() const{return m_qs;}
	double qx() const{return m_qx;}
	double qy() const{return m_qy;}
	double qz() const{return m_qz;}
	double& qs(){return m_qs;}
	double& qx(){return m_qx;}
	double& qy(){return m_qy;}
	double& qz(){return m_qz;}

	A_DEF_POD_ITERABLE(double)
public:
	// 设置为public仅为实现聚合初始化，不要直接访问成员变量
	double m_qs;
	double m_qx;
	double m_qy;
	double m_qz;
};


AST_NAMESPACE_END


AST_DECL_TYPE_ALIAS(Quaternion)