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
#include "AstAnalyzer/StudyVariable.hpp"
#include "AstAnalyzer/StudyConstraint.hpp"
#include <memory>

AST_NAMESPACE_BEGIN

class Interpreter;

/*!
    @addtogroup
    @{
*/

/// @brief 遍历搜索分析器，对变量进行嵌套循环遍历并执行仿真命令
class AST_ANALYZER_API SweepStudy : public Command
{
public:
    AST_OBJECT(SweepStudy)
public:
    SweepStudy();
    ~SweepStudy() override;

    errc_t execute() override;

    /// @brief 获取变量列表
    std::vector<SharedPtr<StudyVariable>>& variables() { return variables_; }
    const std::vector<SharedPtr<StudyVariable>>& variables() const { return variables_; }

    /// @brief 获取约束列表
    std::vector<SharedPtr<StudyConstraint>>& constraints() { return constraints_; }
    const std::vector<SharedPtr<StudyConstraint>>& constraints() const { return constraints_; }

    /// @brief 添加变量
    void addVariable(SharedPtr<StudyVariable> variable);

    /// @brief 移除变量
    void removeVariable(SharedPtr<StudyVariable> variable);

    /// @brief 添加约束
    void addConstraint(SharedPtr<StudyConstraint> constraint);

    /// @brief 移除约束
    void removeConstraint(SharedPtr<StudyConstraint> constraint);

    /// @brief 获取关联的执行命令
    Command* relatedCommand() const { return relatedCommand_.get(); }

    /// @brief 设置关联的执行命令
    void setRelatedCommand(Command* command) { relatedCommand_ = command; }

    /// @brief 获取脚本解释器
    Interpreter* interpreter() const { return interpreter_.get(); }

    /// @brief 计算总运行次数
    int totalRuns() const;

    /// @brief 执行单步分析
    errc_t executeStep(int stepIndex);

private:
    /// @brief 生成所有变量的值列表
    void generateValueLists();

    /// @brief 将步索引转换为各变量索引
    void stepIndexToVarIndices(int stepIndex, std::vector<int>& indices) const;

    std::vector<SharedPtr<StudyVariable>> variables_;        ///< 变量列表
    std::vector<SharedPtr<StudyConstraint>> constraints_;    ///< 约束列表
    mutable WeakPtr<Command> relatedCommand_;                   ///< 关联的执行命令
    mutable std::unique_ptr<Interpreter> interpreter_;          ///< 解释器
};

/*! @} */

AST_NAMESPACE_END

