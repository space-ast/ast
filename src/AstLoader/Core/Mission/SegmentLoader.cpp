///
/// @file      SegmentLoader.cpp
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

#include "SegmentLoader.hpp"
#include "ResultLoader.hpp"
#include "SpacecraftStateLoader.hpp"
#include "AttributeResolve.hpp"
#include "AstCore/Segment.hpp"
#include "AstCore/Resolve.hpp"
#include "AstUtil/ObjectCalculation.hpp"
#include "AstUtil/ObjectLinker.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/Attribute.hpp"
#include "AstUtil/Logger.hpp"
#include "AstScript/Value.hpp"
#include "AstScript/ExprAttribute.hpp"



AST_NAMESPACE_BEGIN


errc_t aLoadShooterControl(const Value& value, SharedPtr<ExprAttribute>& control, Object* scope)
{
    std::string attrPath = value.toString();
    Attribute attr = aResolveAttribute(scope, attrPath);
    control = new ExprAttribute(attr);
    control->setParentScope(scope);
    if(!attr.isValid())
    {
        aWarning("failed to resolve attribute '%.*s'", attrPath.size(), attrPath.data());
    }
    return 0;   
}

errc_t aLoadShooterControls(const Value& dict, Object* scope)
{
    for(auto& item: dict.items())
    {
        std::string name = item.first;
        Value& value = *item.second;
        SharedPtr<ExprAttribute> control;
        aLoadShooterControl(value, control, scope);
        if(control)
            control->setName(name);
    }
    return 0;
}

errc_t aLoadSegment(const Value& dict, Segment& segment)
{
    // 一般来说，输出状态就是最终状态
    if(auto outputState = segment.getOutputState())
    {
        aLoadSpacecraftState(dict["FinalState"], *outputState);
    }
    // 加载 Results
    {
        auto& dictResults = dict["Results"];
        aLoadResults(dictResults, &segment);
    }
    // 加载 ShooterControls
    {
        auto& dictShooterControls = dict["ShooterControls"];
        aLoadShooterControls(dictShooterControls, &segment);
    }
    return eNoError;
}


AST_NAMESPACE_END
