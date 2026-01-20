///
/// @file      HPOPEngine.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "AstMath/OrdinaryDifferentialEquation.hpp"
#include "AstCore/BlockDynamicSystem.hpp"
#include "AstCore/SimTime.hpp"
#include <memory>
#include <vector>


AST_NAMESPACE_BEGIN

class FuncBlock;

/// @brief     HPOP仿真引擎
/// @details   ~
class AST_CORE_API HPOPEngine : public OrdinaryDifferentialEquation
{
public:
    HPOPEngine();
    ~HPOPEngine();

    int getDimension() const override;

    err_t evaluate(const double t, const double* y, double* dy) override;
    
    /// @brief 设置仿真的参考历元
    void setEpoch(const TimePoint& epoch){ epoch_ = epoch; }
    
    /// @brief 添加函数块
    void addBlock(FuncBlock* block);

    /// @brief 初始化仿真引擎
    void initialize();

    /// @brief 对函数块进行排序
    void sortBlocks();

    /// @brief 连接函数块的信号
    void connectSignals();

protected:
    BlockDynamicSystem      dynamicSystem_; ///< 动力学系统
    TimePoint               epoch_;         ///< 仿真的参考历元
};

AST_NAMESPACE_END
