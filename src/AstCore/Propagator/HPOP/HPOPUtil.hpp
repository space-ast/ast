///
/// @file      HPOPUtil.hpp
/// @brief     高精度轨道预报的辅助工具函数
/// @details   
/// @author    axel
/// @date      2026-09-04
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

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class HPOP;
class HPOPEquation;

/// @brief 在无需重新初始化HPOP方程的情况下设置阻力系数
/// @details 其内部实现可能会随版本变化而更改，但是接口会保持不变
/// @param equation HPOP方程实例
/// @param cd 阻力系数参数值
/// @return 
AST_CORE_CAPI errc_t aHPOPEquation_SetDragCoefficient(HPOPEquation* equation, double cd);


/// @brief 在无需重新初始化HPOP方程的情况下设置SRP光压系数
/// @details 其内部实现可能会随版本变化而更改，但是接口会保持不变
/// @param equation HPOP方程实例
/// @param cr SRP光压系数参数值
/// @return 
AST_CORE_CAPI errc_t aHPOPEquation_SetSRPCoefficient(HPOPEquation* equation, double cr);


/*
note:
为什么不写为 aHPOP_SetSRPCoefficient？
因为这也声明会存在歧义，而该函数并不会修改 HPOP.forceModel() 中的 SRP光压系数，阻力系数同理
*/

/*! @} */

AST_NAMESPACE_END
