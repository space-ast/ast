///
/// @file      BlockMass.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-09
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
#include "AstCore/BlockDerivative.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 质量函数块
/// @details
/// 提供固定的航天器质量值，通过输出端口供其他力模型块（BlockDrag、BlockSRP）使用。
///
/// 当前为恒定质量模型，未来可扩展为变质量模型（考虑燃料消耗）。
///
/// 输出端口：
///   - "Mass"：航天器质量（标量）
///
/// @note run() 为空操作，质量值在构造时固定
class AST_CORE_API BlockMass: public BlockDerivative
{
public:
    BlockMass(double mass);
    ~BlockMass();
    errc_t run(const SimTime& simTime) final{return 0;}
protected:
    double* massPtr_{&mass_};   ///< 指向质量值的指针（用于输出端口信号）
    double mass_{};             ///< 航天器质量 [kg]
};

/*! @} */

AST_NAMESPACE_END
