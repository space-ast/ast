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

AST_NAMESPACE_BEGIN


ScStateCalculation* aResolveBuiltinScStateCalculation(StringView name)
{
    if(name == "Argument_of_Latitude")
    {
        auto calculation = new ScStateCalcArgLat();
        calculation->setName(name);
        aAddObject(calculation);
        return calculation;
    }
    else
    {
        aError("unsupported builtin calculation '%.*s'", name.size(), name.data());
    }
    return nullptr;
}

ScStateCalculation* aResolveScStateCalculation(StringView name)
{
    if(auto calc = aFindObject<ScStateCalculation*>(name))
        return calc;
    return aResolveBuiltinScStateCalculation(name);
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
        aError("frame '%s' not found", frameName.c_str());
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
        aError("body '%s' not found", bodyName.c_str());
        return eErrorNullPtr;
    };
    calculation.addDelayedLink(resolveFunc);
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
        aError("point '%s' not found for calculation '%s<%s>'", pointName.c_str(), calcPtr->getName().c_str(), calcPtr->typeName().c_str());
        return eErrorNullPtr;
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
            aError("unsupported difference order to use: '%s', use default 'Current Minus Initial'", differenceOrderToUse.c_str());
        }
        calculation.setDifferenceOrderToUse(orderToUse);
    }
    {
        auto& calcObject = value["CalcObject"];
        if(calcObject.isString())
        {
            auto calc = aResolveScStateCalculation(calcObject.toString());
            if(calc)
                calculation.setCalculation(calc);
        }
        else
        {
            SharedPtr<ObjectCalculation> result;
            Object* scope = &calculation;
            errc_t rc = aLoadResult(calcObject, result, scope);
            if(rc ==0 && result)
                calculation.setCalculation(aobject_cast<ScStateCalculation*>(result.get()));
        }
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
    else
    {
        result.reset();
        aError("unsupported result type: '%s'", type.c_str());
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
