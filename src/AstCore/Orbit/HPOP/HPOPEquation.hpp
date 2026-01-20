///
/// @file      HPOPEquation.hpp
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
class BlockDerivative;
class HPOPForceModel;

/// @brief     高精度轨道预报方程
/// @details   
/// 该类继承自 OrdinaryDifferentialEquation ，用于求解轨道动力学问题。
/// 其内部包含一个动力学系统 BlockDynamicSystem
/// 动力学系统 BlockDynamicSystem 由多个独立的函数块 FuncBlock 组成。
/// 每个函数块 FuncBlock 负责计算系统的一个或多个状态量或者导数。
/// 函数块之间可以通过输入输出端口进行信号连接，实现复杂的动力学模型。
class AST_CORE_API HPOPEquation : public OrdinaryDifferentialEquation
{
public:
    HPOPEquation();
    ~HPOPEquation();

    int getDimension() const final;

    err_t evaluate(const double t, const double* y, double* dy) final;
    
    /// @brief 设置仿真的参考历元
    void setEpoch(const TimePoint& epoch){ epoch_ = epoch; }
    



    /// @brief 设置HPOP力模型
    err_t setForceModel(const HPOPForceModel& forceModel);
protected:
    /// @brief 添加函数块
    void addBlock(FuncBlock* block);
    void addBlock(BlockDerivative* block);
    
    void clearBlocks();

    /// @brief 初始化仿真引擎
    err_t initialize();
protected:
    BlockDynamicSystem      dynamicSystem_; ///< 动力学系统
    TimePoint               epoch_;         ///< 仿真的参考历元
};

AST_NAMESPACE_END
