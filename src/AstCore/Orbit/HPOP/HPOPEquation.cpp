///
/// @file      HPOPEquation.cpp
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

#include "HPOPEquation.hpp"
#include "HPOP.hpp"                 // for HPOPForceModel
#include "AstCore/Simulation.hpp"   // for blocks

AST_NAMESPACE_BEGIN

HPOPEquation::HPOPEquation()
{

}

HPOPEquation::~HPOPEquation()
{

}

err_t HPOPEquation::evaluate(const double t, const double* y, double* dy)
{
    SimTime time;                       // 仿真时间
    time.setTimePoint(epoch_ + t);      // 设置仿真时间点
    time.setElapsedTime(t);             // 设置仿真的相对时间
    return dynamicSystem_.evaluate(time);
}

int HPOPEquation::getDimension() const
{
    // @fixme
    return dynamicSystem_.getDimension();
}

/// @brief 初始化仿真引擎
err_t HPOPEquation::initialize()
{
    return dynamicSystem_.initialize();
}


void HPOPEquation::addBlock(FuncBlock *block)
{
    dynamicSystem_.addBlock(block);
}

void HPOPEquation::addBlock(BlockDerivative *block)
{
    dynamicSystem_.addBlock(block);
}

void HPOPEquation::clearBlocks()
{
    dynamicSystem_.clearBlocks();
}

err_t HPOPEquation::setForceModel(const HPOPForceModel& forceModel)
{
    // 将力模型配置转换为动力学系统的一个个函数块
    BlockDerivative* derivativeBlock;
    auto& gravity = forceModel.gravity_;

    // 清除旧的函数块
    this->clearBlocks();

    // 添加运动学函数块
    derivativeBlock = new BlockMotion();
    this->addBlock(derivativeBlock);

    // 添加重力函数块
    derivativeBlock = new BlockGravity(gravity.model_, gravity.degree_, gravity.order_);
    this->addBlock(derivativeBlock);

    // 添加月球引力函数块
    if(forceModel.useMoonGravity_)
    {
        derivativeBlock = new BlockThirdBody();
        this->addBlock(derivativeBlock);
    }

    return this->initialize();
}


AST_NAMESPACE_END
