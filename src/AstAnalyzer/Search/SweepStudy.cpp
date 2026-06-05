///
/// @file      SweepStudy.cpp
/// @brief     遍历搜索分析器实现
/// @details   对变量进行嵌套循环遍历，每次迭代设置变量值、执行命令、评估约束
/// @author    axel
/// @date      2026-05-14
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

#include "SweepStudy.hpp"
#include "AstAnalyzer/StudyVariable.hpp"
#include "AstAnalyzer/StudyConstraint.hpp"
#include "AstCore/Command.hpp"
#include "AstScript/Interpreter.hpp"
#include "AstScript/SymbolScope.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

namespace
{
    /// @brief 为 SweepStudy 提供符号查找的 ISymbolScope 实现
    class SweepStudySymbolScope : public ISymbolScope
    {
    public:
        explicit SweepStudySymbolScope(SweepStudy* analyzer)
            : analyzer_(analyzer)
        {}

        errc_t addSymbol(StringView name, Expr* expr) override
        {
            aError("addSymbol not implemented");
            return eErrorNotImplemented;
        }

        Expr* findSymbol(StringView name, bool searchParent = true) const override
        {
            auto* analyzer = analyzer_.get();
            if (!analyzer)
            {
                aError("analyzer is null");
                return nullptr;
            }
            // 从变量列表中查找符号（变量自身是 ObjectNamed，返回其表达式）
            for (const auto& var : analyzer->variables())
            {
                if (var && name == var->name())
                {
                    return var->expr();
                }
            }
            return nullptr;
        }

        Expr* resolveSymbol(StringView name) override
        {
            return findSymbol(name);
        }

    private:
        WeakPtr<SweepStudy> analyzer_;
    };
}

SweepStudy::SweepStudy()
    : interpreter_(aNewInterpreter(new SweepStudySymbolScope(this)))
{
}

SweepStudy::~SweepStudy() = default;

void SweepStudy::addVariable(SharedPtr<StudyVariable> variable)
{
    if (variable)
        variables_.push_back(variable);
}

void SweepStudy::removeVariable(SharedPtr<StudyVariable> variable)
{
    auto it = std::find(variables_.begin(), variables_.end(), variable);
    if (it != variables_.end())
        variables_.erase(it);
}

void SweepStudy::addConstraint(SharedPtr<StudyConstraint> constraint)
{
    if (constraint)
        constraints_.push_back(constraint);
}

void SweepStudy::removeConstraint(SharedPtr<StudyConstraint> constraint)
{
    auto it = std::find(constraints_.begin(), constraints_.end(), constraint);
    if (it != constraints_.end())
        constraints_.erase(it);
}

int SweepStudy::totalRuns() const
{
    if (variables_.empty())
        return 0;

    int total = 1;
    for (const auto& var : variables_)
    {
        if (!var)
            continue;
        double start = var->startValue();
        double end = var->endValue();
        double step = var->stepSize();
        if (step == 0.0)
            continue;
        int count = static_cast<int>((end - start) / step) + 1;
        if (count < 1)
            count = 1;
        total *= count;
    }
    return total;
}

void SweepStudy::generateValueLists()
{
    for (auto& var : variables_)
    {
        if (!var)
            continue;
        auto& values = var->values();
        values.clear();

        double start = var->startValue();
        double end = var->endValue();
        double step = var->stepSize();
        if (step == 0.0)
        {
            values.push_back(start);
            continue;
        }

        int count = static_cast<int>((end - start) / step) + 1;
        if (count < 1)
            count = 1;

        values.reserve(count);
        for (int i = 0; i < count; ++i)
        {
            values.push_back(start + i * step);
        }
    }
}

void SweepStudy::stepIndexToVarIndices(int stepIndex, std::vector<int>& indices) const
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
        int count = static_cast<int>(var->values().size());
        if (count < 1)
            count = 1;
        indices[i - 1] = remaining % count;
        remaining /= count;
    }
}

errc_t SweepStudy::executeStep(int stepIndex)
{
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
            var->setValue(vals[indices[i]]);
        }
    }

    // 执行关联的命令
    if (relatedCommand_)
    {
        errc_t ret = relatedCommand_->execute();
        if (ret != eNoError)
            return ret;
    }

    // 评估所有约束并存储结果
    for (auto& constraint : constraints_)
    {
        if (!constraint)
            continue;
        double value = 0.0;
        errc_t ret = constraint->getValue(value);
        if (ret == eNoError)
        {
            constraint->values().push_back(value);
        }
    }

    return eNoError;
}

errc_t SweepStudy::execute()
{
    if (variables_.empty())
    {
        aError("遍历搜索分析器：变量列表为空");
        return eErrorInvalidParam;
    }

    // 生成各变量的值列表
    generateValueLists();

    // 清空约束的历史值
    for (auto& constraint : constraints_)
    {
        if (constraint)
            constraint->values().clear();
    }

    // 执行遍历搜索
    int total = totalRuns();
    for (int i = 0; i < total; ++i)
    {
        errc_t ret = executeStep(i);
        if (ret != eNoError)
            return ret;
    }

    return eNoError;
}

AST_NAMESPACE_END
