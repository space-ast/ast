///
/// @file      FeasibleRegionStudy.cpp
/// @brief     可行域研究实现
/// @details   对变量在界内进行网格搜索，每次迭代设置变量值、执行命令、验证约束，
///             最终确定满足所有约束的可行区域
/// @author    axel
/// @date      2026-06-08
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

#include "FeasibleRegionStudy.hpp"
#include "AstScript/Expr.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

// ------------------
// FeasibleRegionVariable
// ------------------

errc_t FeasibleRegionVariable::getValue(double& value) const
{
    if (!expr_)
        return eErrorNullPtr;
    return expr_->getValueDouble(value);
}

errc_t FeasibleRegionVariable::setValue(double value)
{
    if (!expr_)
        return eErrorNullPtr;
    return expr_->setValueDouble(value);
}

void FeasibleRegionVariable::generateValueList()
{
    values_.clear();
    if (steps_ < 1)
        steps_ = 1;

    if (steps_ == 1)
    {
        // 仅一个点时取中点
        values_.push_back((lower_ + upper_) * 0.5);
        return;
    }

    double range = upper_ - lower_;
    double step = range / static_cast<double>(steps_ - 1);
    values_.reserve(steps_);
    for (int i = 0; i < steps_; ++i)
    {
        values_.push_back(lower_ + i * step);
    }
}


// ------------------
// FeasibleRegionConstraint
// ------------------

errc_t FeasibleRegionConstraint::getValue(double& value) const
{
    if (!expr_)
        return eErrorNullPtr;
    return expr_->getValueDouble(value);
}


// ------------------
// FeasibleRegionStudy
// ------------------

void FeasibleRegionStudy::addVariable(VariableType* variable)
{
    if (variable)
        variables_.push_back(variable);
}

void FeasibleRegionStudy::removeVariable(VariableType* variable)
{
    auto it = std::find(variables_.begin(), variables_.end(), variable);
    if (it != variables_.end())
        variables_.erase(it);
}

void FeasibleRegionStudy::addConstraint(ConstraintType* constraint)
{
    if (constraint)
        constraints_.push_back(constraint);
}

void FeasibleRegionStudy::removeConstraint(ConstraintType* constraint)
{
    auto it = std::find(constraints_.begin(), constraints_.end(), constraint);
    if (it != constraints_.end())
        constraints_.erase(it);
}

int FeasibleRegionStudy::totalRuns() const
{
    if (variables_.empty())
        return 0;

    int64_t total = 1;
    for (const auto& var : variables_)
    {
        if (!var)
            continue;
        int steps = var->steps();
        if (steps < 1)
            steps = 1;

        total *= steps;
        if (total > INT_MAX)
        {
            aError("FeasibleRegionStudy: total runs overflow INT_MAX");
            return INT_MAX;
        }
    }
    return static_cast<int>(total);
}

void FeasibleRegionStudy::stepIndexToVarIndices(int stepIndex, std::vector<int>& indices) const
{
    indices.clear();
    if (variables_.empty())
        return;

    // 从最后一维开始计算各变量的索引（最后一维变化最快）
    indices.resize(variables_.size(), 0);
    int remaining = stepIndex;

    for (size_t i = variables_.size(); i > 0; --i)
    {
        const auto& var = variables_[i - 1];
        if (!var)
            continue;
        int count = var->steps();
        if (count < 1)
            count = 1;
        indices[i - 1] = remaining % count;
        remaining /= count;
    }
}

errc_t FeasibleRegionStudy::executeStep(int stepIndex, bool& isFeasible)
{
    isFeasible = true;

    // 生成各变量索引
    std::vector<int> indices;
    stepIndexToVarIndices(stepIndex, indices);

    // 设置各变量的当前值
    for (size_t i = 0; i < variables_.size() && i < indices.size(); ++i)
    {
        auto& var = variables_[i];
        if (!var)
            continue;
        auto& vals = var->values();
        if (indices[i] >= 0 && static_cast<size_t>(indices[i]) < vals.size())
        {
            errc_t ret = var->setValue(vals[indices[i]]);
            if (ret != eNoError)
                return ret;
        }
    }

    // 执行关联的命令（计算输出值）
    if (relatedCommand_)
    {
        errc_t ret = relatedCommand_->execute();
        if (ret != eNoError)
            return ret;
    }

    // 检查所有约束是否满足
    for (const auto& constraint : constraints_)
    {
        if (!constraint)
            continue;

        double value = 0.0;
        errc_t ret = constraint->getValue(value);
        if (ret != eNoError)
        {
            // 无法获取约束值，视为不满足
            isFeasible = false;
            break;
        }

        bool lowerOk = true;
        bool upperOk = true;

        if (constraint->useLower())
        {
            lowerOk = constraint->exclude()
                ? (value > constraint->lower())   // 排除模式：需严格大于下界
                : (value >= constraint->lower());  // 包含模式：大于等于下界
        }

        if (constraint->useUpper())
        {
            upperOk = constraint->exclude()
                ? (value < constraint->upper())   // 排除模式：需严格小于上界
                : (value <= constraint->upper());  // 包含模式：小于等于上界
        }

        if (!(lowerOk && upperOk))
        {
            isFeasible = false;
            break;
        }
    }

    return eNoError;
}

errc_t FeasibleRegionStudy::execute()
{
    if (variables_.empty())
    {
        aError("FeasibleRegionStudy: 变量列表为空");
        return eErrorInvalidParam;
    }

    // 检查变量下界不大于上界
    for (const auto& var : variables_)
    {
        if (!var)
            continue;
        if (var->lower() >= var->upper())
        {
            aError("FeasibleRegionStudy: 变量下界必须严格小于上界");
            return eErrorInvalidParam;
        }
        if (var->steps() < 1)
        {
            aError("FeasibleRegionStudy: 变量采样点数必须大于 0");
            return eErrorInvalidParam;
        }
    }

    // 生成各变量的采样值列表
    for (auto& var : variables_)
    {
        if (var)
            var->generateValueList();
    }

    // 执行网格搜索
    int total = totalRuns();
    int feasibleCount = 0;
    for (int i = 0; i < total; ++i)
    {
        bool isFeasible = false;
        errc_t ret = executeStep(i, isFeasible);
        if (ret != eNoError)
            return ret;

        if (isFeasible)
            ++feasibleCount;
    }

    if (feasibleCount == 0)
    {
        aWarning("FeasibleRegionStudy: 未找到任何可行点");
    }

    return eNoError;
}

AST_NAMESPACE_END
