///
/// @file      UnitRep.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-08-21
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

#include "UnitRep.hpp"

#include <cstddef>

AST_NAMESPACE_BEGIN

// 编译期检查：单位表示类型的内存大小一致，两个double成员变量的偏移量一致，确保单位表示的比较能够正常工作

static_assert(offsetof(ScaleUnitRep, scale_) == offsetof(AffineUnitRep, scale_), "offsetof not match");
static_assert(offsetof(ScaleUnitRep, scale_) == offsetof(LogarithmicUnitRep, reference_), "offsetof not match");
static_assert(offsetof(ScaleUnitRep, notused_) == offsetof(AffineUnitRep, offset_), "offsetof not match");
static_assert(offsetof(ScaleUnitRep, notused_) == offsetof(LogarithmicUnitRep, factor_), "offsetof not match");
static_assert(sizeof(ScaleUnitRep) == sizeof(AffineUnitRep), "sizeof not match");
static_assert(sizeof(ScaleUnitRep) == sizeof(LogarithmicUnitRep), "sizeof not match");


AST_NAMESPACE_END
