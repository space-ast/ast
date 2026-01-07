///
/// @file      SOFA.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-06
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
#include "AstUtil/Constants.h"

AST_NAMESPACE_BEGIN


/// @brief 计算给定时间点的岁差角，按照IAU1976规范
/// @param tp 时间点
/// @param zeta 输出的zeta角（弧度）
/// @param z 输出的z角（弧度）
/// @param theta 输出的theta角（弧度）
AST_CORE_CAPI void aPrecession_IAU1976(const TimePoint& tp, double& zeta, double& z, double& theta);





AST_NAMESPACE_END
