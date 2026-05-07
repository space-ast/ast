///
/// @file      DifferentialCorrectorLoader.cpp
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

#include "AstCore/ShooterControl.hpp"
#include "AstCore/ShooterResult.hpp"
#include "AstCore/ScriptingToolProfile.hpp"
#include "AstCore/DifferentialCorrectorProfile.hpp"
#include "DifferentialCorrectorProfileLoader.hpp"
#include "ShooterControlLoader.hpp"
#include "ShooterResultLoader.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

errc_t aLoadDifferentialCorrectorProfile(const Value& value, DifferentialCorrectorProfile& profile)
{
    // 加载 UseLineSearch
    {
        bool useLineSearch = value["UseLineSearch"];
        profile.setUseLineSearch(useLineSearch);
    }

    // 加载 LineSearchLowerBound
    {
        double lowerBound = value["LineSearchLowerBound"];
        profile.setLineSearchLowerBound(lowerBound);
    }
    // 加载 LineSearchUpperBound
    {
        double upperBound = value["LineSearchUpperBound"];
        profile.setLineSearchUpperBound(upperBound);
    }
    // 加载 LineSearchMaxNumCalls
    {
        int maxNumCalls = value["LineSearchMaxNumCalls"];
        profile.setLineSearchMaxNumCalls(maxNumCalls);
    }
    // 加载 LineSearchTolerance
    {
        double tolerance = value["LineSearchTolerance"];
        profile.setLineSearchTolerance(tolerance);
    }



    // 加载 UseHomotopy
    {
        bool useHomotopy = value["UseHomotopy"];
        profile.setUseHomotopy(useHomotopy);
    }

    // 加载 NumHomotopySteps
    {
        int numHomotopySteps = value["NumHomotopySteps"];
        profile.setNumHomotopySteps(numHomotopySteps);
    }
    
    // 加载 RootFindingAlgorithm
    {
        std::string rootFindingAlgorithm = value["RootFindingAlgorithm"];
    }

    // 加载 MaxIterations
    {
        int maxIterations = value["MaxIterations"];
        profile.setMaxIterations(maxIterations);
    }

    // 加载 FiniteDifferenceMethod
    {
        std::string finiteDifferenceMethod = value["FiniteDifferenceMethod"];
        EFiniteDifferenceMethod method = EFiniteDifferenceMethod::eForwardDifference;
        if(finiteDifferenceMethod == "Forward Difference")
        {
            method = EFiniteDifferenceMethod::eForwardDifference;
        }
        else if(finiteDifferenceMethod == "Central Difference")
        {
            method = EFiniteDifferenceMethod::eCentralDifference;
        }
        else if(finiteDifferenceMethod == "Backward Difference")
        {
            method = EFiniteDifferenceMethod::eBackwardDifference;
        }
        else{
            method = EFiniteDifferenceMethod::eForwardDifference;
            aWarning(
                "unsupported FiniteDifferenceMethod: '%s', use Forward Difference by default", 
                finiteDifferenceMethod.c_str()
            );
        }
        profile.setFiniteDifferenceMethod(method);
    }

    // 加载 ConvergenceCriteria
    {
        std::string convergenceCriteria = value["ConvergenceCriteria"];
        EConvergenceCriteria criteria = EConvergenceCriteria::eEqualityConstraintsWithinTolerance;
        if(convergenceCriteria == "Equality constraints within tolerance")
        {
            criteria = EConvergenceCriteria::eEqualityConstraintsWithinTolerance;
        }
        else{
            criteria = EConvergenceCriteria::eEqualityConstraintsWithinTolerance;
            aWarning(
                "unsupported ConvergenceCriteria: '%s', use Equality Constraints Within Tolerance by default", 
                convergenceCriteria.c_str()
            );
        }
        profile.setConvergenceCriteria(criteria);
    }

    // 加载 ScaleControls
    {
        std::string scaleControls = value["ScaleControls"];
        EScaleControls controls = EScaleControls::eByInitialValue;
        if(scaleControls == "By initial value")
        {
            controls = EScaleControls::eByInitialValue;
        }
        else{
            controls = EScaleControls::eByInitialValue;
            aWarning(
                "unsupported ScaleControls: '%s', use By initial value by default", 
                scaleControls.c_str()
            );
        }
        profile.setScaleControls(controls);
    }

    // 加载 ScaleGoals
    {
        std::string scaleGoals = value["ScaleGoals"];
        EScaleGoals goals = EScaleGoals::eByDesiredValue;
        if(scaleGoals == "By desired value")
        {
            goals = EScaleGoals::eByDesiredValue;
        }
        else{
            goals = EScaleGoals::eByDesiredValue;
            aWarning(
                "unsupported ScaleGoals: '%s', use By Desired Value by default", 
                scaleGoals.c_str()
            );
        }
        profile.setScaleGoals(goals);
    }

    // 加载 Controls
    {
        auto& dictControls = value["Controls"];
        std::vector<SharedPtr<ShooterControl>> controls;
        for(const auto& item : dictControls.items())
        {
            auto control = aNewObject<ShooterControl>(&profile);
            const auto& dictControl = *item.second;
            aLoadShooterControl(dictControl, *control);
            control->setName(item.first);
            controls.push_back(control);
        }
        profile.setControls(controls);
    }

    // 加载 Results
    {
        auto& dictResults = value["Results"];
        std::vector<SharedPtr<ShooterResult>> results;
        for(const auto& item : dictResults.items())
        {
            auto result = aNewObject<ShooterResult>(&profile);
            const auto& dictResult = *item.second;
            aLoadShooterResult(dictResult, *result);
            result->setName(item.first);
            results.push_back(result);
        }
        profile.setResults(results);
    }
    return eNoError;
}

AST_NAMESPACE_END
