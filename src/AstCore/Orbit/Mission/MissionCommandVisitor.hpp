///
/// @file      MissianCommandVisitor.hpp
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class InitialState;
class Propagate;
class Maneuver;
class Sequence;


/// @brief 任务命令访问者接口
class MissionCommandVisitor
{
public:
    virtual ~MissionCommandVisitor() = default;
    virtual void visit(MissionCommand&) = 0;
    virtual void visit(InitialState&) = 0;
    virtual void visit(Propagate&) = 0;
    virtual void visit(Maneuver&) = 0;
    virtual void visit(Sequence&) = 0;
};



/*! @} */

AST_NAMESPACE_END
