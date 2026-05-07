///
/// @file      AllTypes.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-22
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
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstScript/Value.hpp"
#include "AstScript/ValInt.hpp"
#include "AstScript/ValDouble.hpp"
#include "AstScript/ValString.hpp"
#include "AstScript/ValBool.hpp"
#include "AstScript/ValQuantity.hpp"
#include "AstScript/ValNull.hpp"
#include "AstScript/ValRange.hpp"
#include "AstScript/ValDict.hpp"
#include "AstScript/ValNamedVector.hpp"
#include "AstScript/Variable.hpp"
#include "AstScript/OpBin.hpp"
#include "AstScript/OpAssign.hpp"
#include "AstScript/OpUnary.hpp"
#include "AstScript/ExprCondition.hpp"
#include "AstScript/ExprRange.hpp"
#include "AstScript/ExprIf.hpp"
#include "AstScript/ExprLoop.hpp"
#include "AstScript/ExprVector.hpp"
#include "AstScript/ExprCatHorizontal.hpp"
#include "AstScript/ExprCatVertical.hpp"
#include "AstScript/ExprCall.hpp"
#include "AstScript/ExprMacroExpand.hpp"
#include "AstScript/ExprAttribute.hpp"
#include "AstScript/ExprCalculation.hpp"
#include "AstScript/Parser.hpp"
#include "AstScript/Symbol.hpp"
#include "AstScript/OpBinPredefined.hpp"
#include "AstScript/OpAssignPredefined.hpp"
#include "AstScript/OpUnaryPredefined.hpp"
#include "AstScript/IteratePredefined.hpp"
#include "AstScript/Types.hpp"
