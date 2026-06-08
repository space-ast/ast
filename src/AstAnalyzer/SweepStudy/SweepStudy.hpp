///
/// @file      SweepStudy.hpp
/// @brief     
/// @details   
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/Command.hpp"
#include "AstScript/Expr.hpp"
#include <memory>

AST_NAMESPACE_BEGIN

class Interpreter;

/*!
    @addtogroup
    @{
*/


class AST_ANALYZER_API SweepVariable : public ObjectNamed
{
public:
    AST_OBJECT(SweepVariable)

    double startValue() const { return startValue_; }
    void setStartValue(double value) { startValue_ = value; }

    double endValue() const { return endValue_; }
    void setEndValue(double value) { endValue_ = value; }
    
    double stepSize() const { return stepSize_; }
    void setStepSize(double value) { stepSize_ = value; }

    Expr* expr() const { return expr_.get(); }
    void setExpr(Expr* expr) { expr_ = expr; }

    errc_t getValue(double& value) const;
    errc_t setValue(double value);

    std::vector<double>& values() { return values_; }
    const std::vector<double>& values() const { return values_; }
private:
    double startValue_{0.0};        ///< 变量的起始值
    double endValue_{0.0};          ///< 变量的结束值
    double stepSize_{0.0};          ///< 分析步长
    SharedPtr<Expr> expr_;          ///< 遍历表达式
    std::vector<double> values_;    ///< 值列表
};


class AST_ANALYZER_API SweepOutput : public ObjectNamed
{
public:
    AST_OBJECT(SweepOutput)

    errc_t getValue(double& value) const;
    
    Expr* expr() const { return expr_.get(); }
    void setExpr(Expr* expr) { expr_ = expr; }

    std::vector<double>& values() {return values_;}
    const std::vector<double>& values() const {return values_;}

private:
    SharedPtr<Expr> expr_;          ///< 结果表达式
    std::vector<double> values_;    ///< 约束值列表
};




/// @brief 遍历搜索分析器（参数化扫描），对变量进行嵌套循环遍历并执行仿真命令
class AST_ANALYZER_API SweepStudy : public Command
{
public:
    AST_OBJECT(SweepStudy)
public:
    SweepStudy();
    ~SweepStudy() override;

    using VariableType = SweepVariable;
    using OutputType = SweepOutput;

    using VariableList = std::vector<SharedPtr<VariableType>>;
    using OutputList = std::vector<SharedPtr<OutputType>>;

    errc_t execute() override;

    /// @brief 获取变量列表
    VariableList& variables() { return variables_; }
    const VariableList& variables() const { return variables_; }

    /// @brief 获取输出列表
    OutputList& outputs() { return outputs_; }
    const OutputList& outputs() const { return outputs_; }

    /// @brief 添加变量
    void addVariable(VariableType* variable);

    /// @brief 移除变量
    void removeVariable(VariableType* variable);

    /// @brief 添加输出
    void addOutput(OutputType* output);

    /// @brief 移除输出
    void removeOutput(OutputType* output);

    /// @brief 获取关联的执行命令
    Command* relatedCommand() const { return relatedCommand_.get(); }

    /// @brief 设置关联的执行命令
    void setRelatedCommand(Command* command) { relatedCommand_ = command; }

    /// @brief 计算总运行次数
    int totalRuns() const;

    /// @brief 执行单步分析
    errc_t executeStep(int stepIndex);

private:
    /// @brief 生成所有变量的值列表
    void generateValueLists();

    /// @brief 将步索引转换为各变量索引
    void stepIndexToVarIndices(int stepIndex, std::vector<int>& indices) const;

    VariableList variables_;                            ///< 变量列表
    OutputList outputs_;                                ///< 输出列表
    mutable WeakPtr<Command> relatedCommand_;           ///< 关联的执行命令
};

/*! @} */

AST_NAMESPACE_END

