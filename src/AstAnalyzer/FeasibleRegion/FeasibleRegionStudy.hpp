///
/// @file      IntervalAnalyzer.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-03
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
#include "AstCore/Command.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief  可行域变量
/// @details  可行域变量用于定义可行域的变量，例如下界和上界。
class AST_ANALYZER_API FeasibleRegionVariable: public ObjectNamed
{
public:
    AST_OBJECT(FeasibleRegionVariable)
public:
    FeasibleRegionVariable() = default;
    ~FeasibleRegionVariable() = default;

    double lower() const { return lower_; }
    void setLower(double lower) { lower_ = lower; }

    double upper() const { return upper_; }
    void setUpper(double upper) { upper_ = upper; }

    int steps() const { return steps_; }
    void setSteps(int steps) { steps_ = steps; }

    Expr* expr() const { return expr_.get(); }
    void setExpr(Expr* expr) { expr_ = expr; }

    /// @brief 生成值列表（在 [lower, upper] 范围内等距采样 steps 个点）
    void generateValueList();

    const std::vector<double>& values() const { return values_; }
    std::vector<double>& values() { return values_; }

    errc_t getValue(double& value) const;
    errc_t setValue(double value);

private:
    SharedPtr<Expr> expr_{};         ///< 变量表达式
    double lower_{};                ///< 下界
    double upper_{};                ///< 上界
    int steps_{10};                 ///< 采样点数（默认 10）
    std::vector<double> values_{};   ///< 采样值列表
};


/// @brief  可行域约束
/// @details  可行域约束用于定义可行域的边界条件，例如下界和上界。
class AST_ANALYZER_API FeasibleRegionConstraint: public ObjectNamed
{
public:
    AST_OBJECT(FeasibleRegionConstraint)

public:
    FeasibleRegionConstraint() = default;
    ~FeasibleRegionConstraint() = default;

    bool useLower() const { return useLower_; }
    void setUseLower(bool useLower) { useLower_ = useLower; }

    bool useUpper() const { return useUpper_; }
    void setUseUpper(bool useUpper) { useUpper_ = useUpper; }

    bool exclude() const { return exclude_; }
    void setExclude(bool exclude) { exclude_ = exclude; }
    
    double lower() const { return lower_; }
    void setLower(double lower) { lower_ = lower; }
    
    double upper() const { return upper_; }
    void setUpper(double upper) { upper_ = upper; }

    Expr* expr() const { return expr_.get(); }
    void setExpr(Expr* expr) { expr_ = expr; }

    errc_t getValue(double& value) const;

private:
    bool useLower_{};               ///< 是否启用下界（false 表示无下界）
    bool useUpper_{};               ///< 是否启用上界（false 表示无上界）
    bool exclude_{};                ///< true: 排除区间（可行域为区间补集）; false: 包含区间（可行域为区间本身）
    double lower_{};                ///< 下界值（仅当 useLower_=true 时有效）
    double upper_{};                ///< 上界值（仅当 useUpper_=true 时有效）
    SharedPtr<Expr> expr_{};          ///< 约束表达式
};


/// @brief  可行域研究，用于搜索满足约束的解空间
class AST_ANALYZER_API FeasibleRegionStudy: public Command
{
public:
    AST_OBJECT(FeasibleRegionStudy)
    using VariableType = FeasibleRegionVariable;
    using ConstraintType = FeasibleRegionConstraint;
    using VariableList = std::vector<SharedPtr<VariableType>>;
    using ConstraintList = std::vector<SharedPtr<ConstraintType>>;
public:
    FeasibleRegionStudy() = default;
    ~FeasibleRegionStudy() override = default;

    errc_t execute() override;

    VariableList& variables() { return variables_; }
    const VariableList& variables() const { return variables_; }

    ConstraintList& constraints() { return constraints_; }
    const ConstraintList& constraints() const { return constraints_; }

    /// @brief 获取关联的执行命令
    Command* relatedCommand() const { return relatedCommand_.get(); }

    /// @brief 设置关联的执行命令
    void setRelatedCommand(Command* command) { relatedCommand_ = command; }

    /// @brief 添加变量
    void addVariable(VariableType* variable);

    /// @brief 移除变量
    void removeVariable(VariableType* variable);

    /// @brief 添加约束
    void addConstraint(ConstraintType* constraint);

    /// @brief 移除约束
    void removeConstraint(ConstraintType* constraint);

    /// @brief 计算总运行次数
    int totalRuns() const;

    /// @brief 将步索引转换为各变量采样索引
    void stepIndexToVarIndices(int stepIndex, std::vector<int>& indices) const;

    /// @brief 执行单步分析（设置变量、执行命令、检查约束）
    errc_t executeStep(int stepIndex, bool& isFeasible);

private:
    VariableList variables_{};                           ///< 变量列表
    ConstraintList constraints_{};                       ///< 约束列表
    mutable WeakPtr<Command> relatedCommand_{};          ///< 关联的执行命令
};

/*! @} */

AST_NAMESPACE_END



