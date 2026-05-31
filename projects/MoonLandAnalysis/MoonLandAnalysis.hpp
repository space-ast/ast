///
/// @file      MoonLandAnalysis.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-14
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
#include "AstCore/Command.hpp"
#include "AstCore/TimePoint.hpp"

AST_USING_NAMESPACE

class MoonLandCommand : public Command
{
public:
    errc_t execute() override;

protected:
    TimePoint prlTime_{};        ///< 近月制动的时间点
    TimePoint landTime_{};       ///< 着陆时间点
    double lloHeight_{0.0};      ///< 着陆前环月轨道高度
    double landSiteLon_{0.0};    ///< 着陆点的经度
    double landSiteLat_{0.0};    ///< 着陆点的纬度
};




