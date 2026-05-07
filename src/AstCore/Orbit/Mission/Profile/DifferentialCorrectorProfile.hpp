///
/// @file      DifferentialCorrectorProfile.hpp
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

#pragma once

#include "AstGlobal.h"
#include "TargeterProfile.hpp"
#include "AstCore/ShooterControl.hpp"
#include "AstCore/ShooterResult.hpp"
#include "AstCore/ScriptingToolProfile.hpp"
#include "AstCore/TargeterGraph.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 有限差分方法
enum class EFiniteDifferenceMethod
{
    eForwardDifference,     ///< 前向差分
    eCentralDifference,     ///< 中心差分
    eBackwardDifference,    ///< 后向差分
};

/// @brief 根求解算法
enum class ERootFindingAlgorithm
{
    eSecantMethod,     
};

/// @brief 收敛准则
enum class EConvergenceCriteria
{
    eEqualityConstraintsWithinTolerance, ///< 等式约束在容差范围内
};

/// @brief 控制变量缩放方式
enum class EScaleControls
{
    eByInitialValue, ///< 由初始值缩放
};

/// @brief 控制变量缩放目标
enum class EScaleGoals
{
    eByDesiredValue, ///< 由期望值缩放
};

class Sequence;

/// @brief 微分修正器配置，用于求解非线性方程，可以用于修正轨道参数以满足约束条件要求
class AST_CORE_API DifferentialCorrectorProfile : public TargeterProfile
{
public:
    AST_OBJECT(DifferentialCorrectorProfile)
    AST_PROPERT(useHomotopy)
    AST_PROPERT(numHomotopySteps)
    AST_PROPERT(useLineSearch)
    AST_PROPERT(lineSearchTolerance)
    AST_PROPERT(lineSearchMaxNumCalls)
    AST_PROPERT(lineSearchLowerBound)
    AST_PROPERT(lineSearchUpperBound)
    AST_PROPERT(maxIterations)
    using ControlVector = std::vector<SharedPtr<ShooterControl>>;
    using ResultVector = std::vector<SharedPtr<ShooterResult>>;
    using TargeterGraphVector = std::vector<SharedPtr<TargeterGraph>>;

    static DifferentialCorrectorProfile* New();

    DifferentialCorrectorProfile() = default;
    ~DifferentialCorrectorProfile() override = default;

    errc_t execute() override;


protected:
    /// @brief 执行与微分修正器绑定的命令
    errc_t executeRelatedCommand() const;

    /// @brief 获取与微分修正器绑定的序列
    Sequence* getRelatedSequence() const;

    /// @brief 获取与微分修正器绑定的命令
    Command* getRelatedCommand() const;
PROPERTIES:
    bool useHomotopy() const{return useHomotopy_;}
    void setUseHomotopy(bool useHomotopy){useHomotopy_ = useHomotopy;}
    
    int numHomotopySteps() const{return numHomotopySteps_;}
    void setNumHomotopySteps(int numHomotopySteps){numHomotopySteps_ = numHomotopySteps;}


    bool useLineSearch() const{return useLineSearch_;}
    void setUseLineSearch(bool useLineSearch){useLineSearch_ = useLineSearch;}

    double lineSearchTolerance() const{return lineSearchTolerance_;}
    void setLineSearchTolerance(double tolerance){lineSearchTolerance_ = tolerance;}

    int lineSearchMaxNumCalls() const{return lineSearchMaxNumCalls_;}
    void setLineSearchMaxNumCalls(int maxNumCalls){lineSearchMaxNumCalls_ = maxNumCalls;}

    double lineSearchLowerBound() const{return lineSearchLowerBound_;}
    void setLineSearchLowerBound(double lowerBound){lineSearchLowerBound_ = lowerBound;}

    double lineSearchUpperBound() const{return lineSearchUpperBound_;}
    void setLineSearchUpperBound(double upperBound){lineSearchUpperBound_ = upperBound;}
    
    int maxIterations() const{return maxIterations_;}
    void setMaxIterations(int maxIterations){maxIterations_ = maxIterations;}
public:
    ERootFindingAlgorithm rootFindingAlgorithm() const{return rootFindingAlgorithm_;}
    void setRootFindingAlgorithm(ERootFindingAlgorithm algorithm){rootFindingAlgorithm_ = algorithm;}

    EFiniteDifferenceMethod finiteDifferenceMethod() const{return finiteDifferenceMethod_;}
    void setFiniteDifferenceMethod(EFiniteDifferenceMethod method){finiteDifferenceMethod_ = method;}

    EConvergenceCriteria convergenceCriteria() const{return convergenceCriteria_;}
    void setConvergenceCriteria(EConvergenceCriteria criteria){convergenceCriteria_ = criteria;}

    EScaleControls scaleControls() const{return scaleControls_;}
    void setScaleControls(EScaleControls controls){scaleControls_ = controls;}

    EScaleGoals scaleGoals() const{return scaleGoals_;}
    void setScaleGoals(EScaleGoals goals){scaleGoals_ = goals;}
    
    const ControlVector& controls() const{return controls_;}
    void setControls(const ControlVector& controls){controls_ = controls;}

    const ResultVector& results() const{return results_;}
    void setResults(const ResultVector& results){results_ = results;}

    ShooterResult* getResult(StringView name) const;
    ShooterControl* getControl(StringView name) const;
private:
    mutable WeakPtr<Command> relatedCommand_;

    EConvergenceCriteria convergenceCriteria_{EConvergenceCriteria::eEqualityConstraintsWithinTolerance};
    EFiniteDifferenceMethod finiteDifferenceMethod_{EFiniteDifferenceMethod::eForwardDifference};
    double lineSearchLowerBound_ = 1e-6;
    double lineSearchUpperBound_ = 10.0;
    double lineSearchTolerance_ = 1e-6;
    int lineSearchMaxNumCalls_ = 10;
    int maximumRunsToLog_ = 25;
    int maxIterations_ = 50;
    int numHomotopySteps_ = 1;
    ERootFindingAlgorithm rootFindingAlgorithm_{ERootFindingAlgorithm::eSecantMethod};
    EScaleControls scaleControls_{EScaleControls::eByInitialValue};
    EScaleGoals scaleGoals_{EScaleGoals::eByDesiredValue};
    bool useHomotopy_ = false;
    bool useLineSearch_ = false;

    ControlVector controls_;
    ResultVector results_;
    TargeterGraphVector graphs_;
    SharedPtr<ScriptingToolProfile> scriptingTool_;
};

/*! @} */

AST_NAMESPACE_END
