///
/// @file      HPOPUtil.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-04
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

#include "HPOPUtil.hpp"
#include "HPOP.hpp"
#include "HPOPEquation.hpp"
#include "AstCore/BlockDrag.hpp"
#include "AstCore/BlockSRP.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

errc_t aHPOPEquation_SetDragCoefficient(HPOPEquation* equation, double cd)
{
    if(!equation)
    {
        aWarning("equation is null");
        return -1;
    }
    auto dragBlock = equation->dragBlock();
    if(!dragBlock)
    {
        aWarning("dragBlock is null");
        return -1;
    }
    dragBlock->setCoefficient(cd);

    return eNoError;
}


errc_t aHPOPEquation_SetSRPCoefficient(HPOPEquation* equation, double cr)
{
    if(!equation)
    {
        aWarning("equation is null");
        return -1;
    }
    auto srpBlock = equation->srpBlock();
    if(!srpBlock)
    {
        aWarning("srpBlock is null");
        return -1;
    }
    srpBlock->setCoefficient(cr);

    return eNoError;
}



AST_NAMESPACE_END

