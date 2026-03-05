///
/// @file      BuiltinAxes.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/Axes.hpp"
#include "AstCore/AxesRoot.hpp"
#include "AstCore/AxesICRF.hpp"
#include "AstCore/AxesMacro.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup Coordinate
    @{
*/


_AST_DECL_AXES(ECF)
_AST_DECL_AXES(J2000)
_AST_DECL_AXES(MOD)
_AST_DECL_AXES(TOD)
_AST_DECL_AXES(GTOD)

_AST_DECL_AXES(B1950Vallado)
_AST_DECL_AXES(B1950Spice)
_AST_DECL_AXES(FK4)
_AST_DECL_AXES(DE118)
_AST_DECL_AXES(DE96)
_AST_DECL_AXES(DE102)
_AST_DECL_AXES(DE108)
_AST_DECL_AXES(DE111)
_AST_DECL_AXES(DE114)
_AST_DECL_AXES(DE122)
_AST_DECL_AXES(DE125)
_AST_DECL_AXES(DE130)
_AST_DECL_AXES(Galactic)
_AST_DECL_AXES(DE200)
_AST_DECL_AXES(DE202)
_AST_DECL_AXES(MarsIAU)
_AST_DECL_AXES(EclipJ2000)
_AST_DECL_AXES(EclipB1950)
_AST_DECL_AXES(DE140)
_AST_DECL_AXES(DE142)
_AST_DECL_AXES(DE143)


/*! @} */

AST_NAMESPACE_END
