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

#include "AstUtil/UnitRep.hpp"

#include <cstddef>

AST_NAMESPACE_BEGIN

// 编译期检查：单位表示类型的内存大小一致，两个double成员变量的偏移量一致，确保单位表示的比较能够正常工作
// 说明：这些类型因基类含虚函数而并非标准布局(standard-layout)类型，GCC/Clang 对非标准布局类型使用 offsetof
//       会发出 -Winvalid-offsetof 告警（在 -Werror 下升级为错误）。此处的 offsetof 仅用于编译期布局一致性检查，
//       主流工具链均支持该扩展用法，故在此针对性屏蔽该告警，不影响 static_assert 本身的校验。
#if defined(A_GCC) || defined(A_CLANG)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

static_assert(offsetof(ScaleUnitRep, scale_) == offsetof(AffineUnitRep, scale_), "offsetof not match");
static_assert(offsetof(ScaleUnitRep, scale_) == offsetof(LogarithmicUnitRep, reference_), "offsetof not match");
static_assert(offsetof(ScaleUnitRep, notused_) == offsetof(AffineUnitRep, offset_), "offsetof not match");
static_assert(offsetof(ScaleUnitRep, notused_) == offsetof(LogarithmicUnitRep, factor_), "offsetof not match");
static_assert(sizeof(ScaleUnitRep) == sizeof(AffineUnitRep), "sizeof not match");
static_assert(sizeof(ScaleUnitRep) == sizeof(LogarithmicUnitRep), "sizeof not match");

#if defined(A_GCC) || defined(A_CLANG)
#   pragma GCC diagnostic pop
#endif


AST_NAMESPACE_END
