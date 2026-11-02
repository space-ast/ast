///
/// @file      ResultLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-03
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

#include "ResultLoader.hpp"
#include "AstCore/ScStateCalcAllHeaders.hpp"
#include "AstCore/Resolve.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/ObjectLinker.hpp"
#include "AstLoader/LoaderCommon.hpp"

AST_NAMESPACE_BEGIN


ScStateCalculation* aResolveBuiltinScStateCalculation(StringView name)
{
    ScStateCalculation* calculation = nullptr;
    if(name == "Argument_of_Latitude")
    {
        calculation = new ScStateCalcArgLat();
    }
    else if(name == "Semimajor_Axis")
    {
        calculation = new ScStateCalcSemiMajorAxis();
    }
    else if(name == "Epoch")
    {
        calculation = new ScStateCalcEpoch();
    }
    else
    {
        aError(_("不支持的内置计算量 '%.*s'"), name.size(), name.data());
        return nullptr;
    }
    calculation->setName(name);
    aAddObject(calculation);
    return calculation;
}

ScStateCalculation* aResolveScStateCalculation(StringView name)
{
    if(auto calc = aFindObject<ScStateCalculation*>(name))
        return calc;
    return aResolveBuiltinScStateCalculation(name);
}


ObjectCalculation* aValueToResult(const Value& value, Object* scope)
{
    if(value.isString())
    {
        return aResolveScStateCalculation(value.toString());
    }
    else
    {
        SharedPtr<ObjectCalculation> result;
        aLoadResult(value, result, scope);
        return result.take();
    }
}



errc_t aLoadCalculation(const Value& value, ScStateCalcFrameRelated& calculation)
{
    std::string frameName = value["CoordSystem"];
    auto calcPtr = &calculation;
    auto resolveFunc = [calcPtr, frameName]() -> errc_t {
        if(auto frame = aResolveFrame(frameName))
        {
            calcPtr->setFrame(frame);
            return eNoError;
        }
        aError(_("未找到坐标系 '%s'"), frameName.c_str());
        return eErrorNullPtr;
    };
    calculation.addDelayedLink(resolveFunc);
    return 0;
}

errc_t aLoadCalculation(const Value& value, ScStateCalcBodyRelated& calculation)
{
    std::string bodyName = value["CentralBody"];
    auto calcPtr = &calculation;
    auto resolveFunc = [calcPtr, bodyName]() -> errc_t {
        if(auto body = aResolveBody(bodyName))
        {
            calcPtr->setBody(body);
            return eNoError;
        }
        aError(_("未找到天体 '%s'"), bodyName.c_str());
        return eErrorNullPtr;
    };
    calculation.addDelayedLink(resolveFunc);
    return 0;
}

errc_t aLoadCalculation(const Value& value, ScStateCalcSemiMajorAxis& calculation)
{
    aLoadCalculation(value, static_cast<ScStateCalcBodyRelated&>(calculation));

    // 目前只支持密切轨道根数，平轨道根数（Brouwer-Lyddane、Kozai-Izsak）尚未实现
    std::string elementType = value["ElementType"];
    if(!elementType.empty() && elementType != "Osculating")
    {
        aWarning(_("暂不支持半长轴根数类型 '%s'，按密切轨道根数计算"), elementType.c_str());
    }
    return 0;
}

errc_t aLoadCalculation(const Value& value, ScStateCalcPointRelated& calculation)
{
    std::string pointName = value["ReferencePoint"];
    auto calcPtr = &calculation;
    auto resolveFunc = [calcPtr, pointName]() -> errc_t {
        if(auto point = aResolvePoint(pointName))
        {
            calcPtr->setPoint(point);
            return eNoError;
        }
        aError(_("未找到参考点 '%s'，用于计算量 '%s（%s）'"), pointName.c_str(), calcPtr->getName().c_str(), calcPtr->typeName().c_str());
        return eErrorNullPtr;
    };
    calculation.addDelayedLink(resolveFunc);
    return 0;
}

errc_t aLoadCalculation(const Value& value, ScStateCalcAngle& calculation)
{
    std::string vector1Name = value["Vector1"];
    std::string vector2Name = value["Vector2"];
    auto calcPtr = &calculation;
    auto resolveFunc = [calcPtr, vector1Name, vector2Name]() -> errc_t {
        errc_t rc = eNoError;
        if(auto vector = aFindObject<Vector*>(vector1Name))
            calcPtr->setVector1(vector);
        else
        {
            aError(_("未找到向量 '%s'，用于计算量 '%s（%s）'"), vector1Name.c_str(), calcPtr->getName().c_str(), calcPtr->typeName().c_str());
            rc = eErrorNullPtr;
        }
        if(auto vector = aFindObject<Vector*>(vector2Name))
            calcPtr->setVector2(vector);
        else
        {
            aError(_("未找到向量 '%s'，用于计算量 '%s（%s）'"), vector2Name.c_str(), calcPtr->getName().c_str(), calcPtr->typeName().c_str());
            rc = eErrorNullPtr;
        }
        return rc;
    };
    calculation.addDelayedLink(resolveFunc);
    return 0;
}

errc_t aLoadCalculation(const Value& value, ScStateCalcDifference& calculation)
{
    {
        std::string differenceOrderToUse = value["DifferenceOrderToUse"];
        EDifferenceOrderToUse orderToUse;
        if(differenceOrderToUse == "Current Minus Initial")
            orderToUse = EDifferenceOrderToUse::eCurrentMinusInitial;
        else if(differenceOrderToUse == "Initial Minus Current")
            orderToUse = EDifferenceOrderToUse::eInitialMinusCurrent;
        else
        {
            orderToUse = EDifferenceOrderToUse::eCurrentMinusInitial;
            aError(_("不支持的差值顺序：'%s'，默认使用 'Current Minus Initial'"), differenceOrderToUse.c_str());
        }
        calculation.setDifferenceOrderToUse(orderToUse);
    }
    {
        calculation.setCalculation(aobject_cast<ScStateCalculation*>(aValueToResult(value["CalcObject"], &calculation)));
    }
    return 0;
}


errc_t aLoadCalculation(const Value& value, ScStateCalcDifferenceOtherSegment& calculation)
{
    {
        std::string segmentStateToUse = value["SegmentStateToUse"];
        ESegmentStateToUse stateToUse;
        if(segmentStateToUse == "Final")
            stateToUse = ESegmentStateToUse::eFinal;
        else if(segmentStateToUse == "Initial")
            stateToUse = ESegmentStateToUse::eInitial;
        else
        {
            stateToUse = ESegmentStateToUse::eFinal;
            aError(_("不支持的轨道段状态：'%s'，默认使用 'Final'"), segmentStateToUse.c_str());
        }
        calculation.setSegmentStateToUse(stateToUse);
    }
    {
        std::string differenceOrderToUse = value["DifferenceOrderToUse"];
        ESegmentDifferenceOrder orderToUse;
        if(differenceOrderToUse == "Current Minus Other Segment")
            orderToUse = ESegmentDifferenceOrder::eCurrentMinusSegment;
        else if(differenceOrderToUse == "Other Segment Minus Current")
            orderToUse = ESegmentDifferenceOrder::eSegmentMinusCurrent;
        else
        {
            orderToUse = ESegmentDifferenceOrder::eCurrentMinusSegment;
            aError(_("不支持的差值顺序：'%s'，默认使用 'Current Minus Other Segment'"), differenceOrderToUse.c_str());
        }
        calculation.setDifferenceOrderToUse(orderToUse);
    }
    {
        std::string otherSegment = value["OtherSegment"];
        auto delayedLinkFunc = [&calculation, otherSegment](){
            auto sequence = aGetTopmostAncestorScope<Sequence*>(&calculation);
            if(!sequence)
            {
                aError(_("计算量 '%s' 所在的作用域不是 Sequence，无法解析 OtherSegment"), aObjectSimpleRepr(&calculation).c_str());
                return eErrorInvalidParam;
            }
            auto segment = sequence->getSegmentByPath(otherSegment);
            if(!segment)
            {
                aError(_("未找到轨道段 '%s'"), otherSegment.c_str());
                return eErrorNullPtr;
            }
            calculation.setOtherSegment(segment);
            return eNoError;
        };
        calculation.addDelayedLink(delayedLinkFunc);

    }
    {
        calculation.setCalculation(aobject_cast<ScStateCalculation*>(aValueToResult(value["CalcObject"], &calculation)));
    }
    return 0;
}


errc_t aLoadResult(const Value& value, SharedPtr<ObjectCalculation>& result, Object* scope)
{
    std::string type = value["Type"];
    if(type == "AsStateCalcEccentricity")
    {
        auto calculation = aNewObject<ScStateCalcEccentricity>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "AsStateCalcTrueAnomaly")
    {
        auto calculation = aNewObject<ScStateCalcTrueAnomaly>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "AsStateCalcSemiMajorAxis")
    {
        auto calculation = aNewObject<ScStateCalcSemiMajorAxis>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "AsStateCalcRadiusOfApoapsis")
    {
        auto calculation = aNewObject<ScStateCalcRadiusOfApoapsis>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "AsStateCalcVx")
    {
        auto calculation = aNewObject<ScStateCalcVx>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "AsStateCalcVy")
    {
        auto calculation = aNewObject<ScStateCalcVy>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "AsStateCalcVz")
    {
        auto calculation = aNewObject<ScStateCalcVz>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "AsStateCalcRMag")
    {
        auto calculation = aNewObject<ScStateCalcRMag>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "AsStateCalcVMag")
    {
        auto calculation = aNewObject<ScStateCalcVMag>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "CAgAsStateCalcAngle")
    {
        auto calculation = aNewObject<ScStateCalcAngle>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "CAgAsStateCalcDifference")
    {
        auto calculation = aNewObject<ScStateCalcDifference>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "AsStateCalcMu")
    {
        auto calculation = aNewObject<ScStateCalcMu>(scope);
        result = calculation;
        // aLoadCalculation(value, *calculation);
    }
    else if(type == "CAgAsStateCalcVBScript")
    {
        auto calculation = aNewObject<ScStateCalcVBScript>(scope);
        result = calculation;
        // aLoadCalculation(value, *calculation);
    }
    else if(type == "CAgAsStateCalcLongDriftRate")
    {
        auto calculation = aNewObject<ScStateCalcLongDriftRate>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else if(type == "CAgAsStateCalcDifferenceOtherSegment")
    {
        auto calculation = aNewObject<ScStateCalcDifferenceOtherSegment>(scope);
        result = calculation;
        aLoadCalculation(value, *calculation);
    }
    else
    {
        result.reset();
        aError(_("对象%s不支持的结果类型：'%s'"), aObjectSimpleRepr(scope).c_str(), type.c_str());
        return eErrorInvalidParam;
    }
    // if(result && !result->getParentScope())
    //     result->setParentScope(scope);
    return eNoError;
}

errc_t aLoadResults(const Value& dict, Object* scope)
{
    for(auto& item: dict.items())
    {
        std::string name = item.first;
        Value& value = *item.second;
        SharedPtr<ObjectCalculation> calculation;
        aLoadResult(value, calculation, scope);
        if(calculation)
            calculation->setName(name);
    }
    return 0;
}

AST_NAMESPACE_END
