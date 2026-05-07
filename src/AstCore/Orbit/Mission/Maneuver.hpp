///
/// @file      Maneuver.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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
#include "MissionCommand.hpp"
#include "AstCore/Burn.hpp"
#include "Segment.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class Burn;

/// @brief 机动轨道段，用于建模任务序列(MissionCommand)中的机动任务
/// @details 机动负责执行任务序列中的机动任务
class AST_CORE_API Maneuver: public Segment
{
public:
    AST_OBJECT(Maneuver)
    AST_PROPERT(burn)
    Maneuver() = default;
    ~Maneuver() = default;
public:
    errc_t execute() override;
PROPERTIES:
    Burn* burn() const {return burn_.get();}
    void setBurn(Burn* burn);
private:
    WeakPtr<Burn> burn_;        ///< 发动机点火
};


/*! @} */

AST_NAMESPACE_END
