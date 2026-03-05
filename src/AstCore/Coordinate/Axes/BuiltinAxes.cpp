///
/// @file      BuiltinAxes.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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

#include "BuiltinAxes.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN



_AST_IMPL_AXES_DYNAMIC(ECF, ICRF)
_AST_IMPL_AXES_DYNAMIC(J2000, ECF)
_AST_IMPL_AXES_PSEUDO_INERTIAL(MOD, J2000)
_AST_IMPL_AXES_PSEUDO_INERTIAL(TOD, MOD)
_AST_IMPL_AXES_DYNAMIC(GTOD, TOD)

_AST_IMPL_AXES_INERTIAL(B1950, J2000)
_AST_IMPL_AXES_INERTIAL(B1950Spice, J2000)

AST_NAMESPACE_END
