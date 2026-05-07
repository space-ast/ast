///
/// @file      ShooterControlLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
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

#include "ShooterControlLoader.hpp"
#include "AstScript/Value.hpp"
#include "AstScript/ExprAttribute.hpp"
#include "AstCore/ShooterControl.hpp"
#include "AstCore/Sequence.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstLoader/AttributeResolve.hpp"

AST_NAMESPACE_BEGIN

errc_t aLoadShooterControl(const Value& value, ShooterControl& control)
{
    std::string type = value["Type"];
    if(type != "ShooterControl")
    {
        aError("unsupported type: '%s', expected 'ShooterControl'", type.c_str());
        return -1;
    }

    control.setActive(value["Active"]);
    control.setCorrection(value["Correction"]);
    control.setMaxStep(value["MaxStep"]);
    control.setPerturbation(value["Perturbation"]);
    control.setScale(value["Scale"]);
    control.setTolerance(value["Tolerance"]);
    control.setTotalCorrection(value["TotalCorrection"]);
    {
        std::string parentSegmentName = value["ParentSegmentName"];
        std::string controlName = value["ControlName"];
        Sequence* sequence = aGetAncestorScope<Sequence*>(&control);
        if(!sequence)
        {
            aError("failed to get sequence for control '%s'", controlName.c_str());
            return eErrorNullPtr;
        }
        auto command = sequence->getCommandByPath(parentSegmentName);
        if(!command)
        {
            aError("failed to get command '%s' for control '%s'", parentSegmentName.c_str(), controlName.c_str());
            return eErrorNullPtr;
        }
        Attribute attr = aResolveAttribute(command, controlName);
        if(!attr.isValid())
            aWarning("failed to resolve attribute '%s' for control '%s'", controlName.c_str(), parentSegmentName.c_str());
        Expr* expr = new ExprAttribute(attr);
        expr->setParentScope(&control);
        control.setExpr(expr);
    }
    return 0;
}

AST_NAMESPACE_END

