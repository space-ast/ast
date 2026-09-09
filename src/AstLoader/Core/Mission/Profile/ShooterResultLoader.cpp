///
/// @file      ShooterResultLoader.cpp
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

#include "ShooterResultLoader.hpp"
#include "AstCore/ShooterResult.hpp"
#include "AstCore/Sequence.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/ObjectCalculation.hpp"
#include "AstScript/ExprCalculation.hpp"
#include "AstScript/Value.hpp"

AST_NAMESPACE_BEGIN

errc_t aLoadShooterResult(const Value& value, ShooterResult& result)
{
    std::string type = value["Type"];
    if(type != "ShooterResult")
    {
        aError(_("不支持的类型：'%s'，应为 'ShooterResult'"), type.c_str());
        return -1;
    }

    result.setActive(value["Active"]);
    result.setDesired(value["Desired"]);
    result.setScale(value["Scale"]);
    result.setTolerance(value["Tolerance"]);
    result.setWeight(value["Weight"]);
    result.setValid(value["Valid"]);
    {
        std::string resultName = value["ResultName"];
        std::string parentSegmentName = value["ParentSegmentName"];
        std::string parentType = value["ParentType"];
        std::string calcType = value["CalcType"];
        A_UNUSED(parentType);
        A_UNUSED(calcType);
        Sequence* sequence = aGetAncestorScope<Sequence*>(&result);
        if(sequence == nullptr)
        {
            aError(_("未找到父序列 '%s'"), parentSegmentName.c_str());
            return -1;
        }
        auto segment = sequence->getSegmentByPath(parentSegmentName);
        if(!segment)
        {
            aError(_("获取结果 '%s' 的段 '%s' 失败"), parentSegmentName.c_str(), resultName.c_str());
            return -1;
        }
        auto calculation = aFindChild<ObjectCalculation*>(segment, resultName);
        if(calculation == nullptr)
        {
            aError(_("未找到结果对象 '%s'"), resultName.c_str());
            return -1;
        }
        Expr* expr = new ExprCalculation(segment->getOutputState(), calculation);
        expr->setParentScope(&result);
        result.setExpr(expr);
    }
    return 0;
}

AST_NAMESPACE_END
