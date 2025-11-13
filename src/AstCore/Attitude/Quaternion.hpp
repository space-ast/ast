#pragma once
#include "AstCore/AstCoreExport.h"
#include "AstTypesForward.h"
#include "AstCore/Vector.hpp"
#include <array>

AST_NAMESPACE_BEGIN

/// @brief 四元数
class Quaternion
{
public:
	double* data() const noexcept{return (double*)this; }
	double qs() const{return m_qs;}
	double qx() const{return m_qx;}
	double qy() const{return m_qy;}
	double qz() const{return m_qz;}

public:
	// 设置为public仅为实现聚合初始化，不要直接访问成员变量
	double m_qs;
	double m_qx;
	double m_qy;
	double m_qz;
};


AST_NAMESPACE_END


AST_DECL_TYPE_ALIAS(Quaternion)