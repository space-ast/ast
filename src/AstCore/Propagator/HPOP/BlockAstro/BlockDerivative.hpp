///
/// @file      BlockDerivative.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-20
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
#include "AstCore/FuncBlock.hpp"

AST_NAMESPACE_BEGIN

class SimTime;

/// @brief 导数函数块
/// @details
/// 导数函数块在普通函数块的基础上增加了导数端口（derivative ports）。
/// 导数端口用于与动力学系统自动建立连接关系(详细实现机制见BlockDynamicSystem)
/// 
/// 继承关系：
///   FuncBlock → BlockDerivative → 各具体力模型块（BlockTwoBody、BlockDrag 等）
///
/// 工作流程：
///   1. 每个导数块通过 run() 计算其贡献的加速度
///   2. 加速度通过输出端口（outputPorts_）发布
///   3. 同时累加到导数端口（derivativePorts_）对应的速度导数上
///   4. BlockDynamicSystem 自动收集所有导数端口的信号，构建常微分方程组
///
/// @see BlockDynamicSystem
class AST_CORE_API BlockDerivative: public FuncBlock
{
public:
    /// @brief 析构函数
    ~BlockDerivative() override = default;

    /// @brief 获取导数端口
    const std::vector<DataPort>& getDerivativePorts() const { return derivativePorts_; }
    std::vector<DataPort>& getDerivativePorts() { return derivativePorts_; }
    
protected:
    std::vector<DataPort> derivativePorts_{};  // 导数端口
};

AST_NAMESPACE_END
