///
/// @file      BlockDynamicSystem.cpp
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

#include "BlockDynamicSystem.hpp"

AST_NAMESPACE_BEGIN

/// @brief 检查一个函数块是否依赖于另一个函数块
/// @param block 要检查的函数块
/// @param other 另一个函数块
/// @return 如果block依赖于other，则返回true；否则返回false
static bool aAstroBlockDependentOn(FuncBlock* block, FuncBlock* other)
{
    auto& inputPorts = block->getInputPorts();
    auto& otherOutputPorts = other->getOutputPorts();

    for (auto& port : inputPorts)
    {
        auto iter = std::find_if(otherOutputPorts.begin(), otherOutputPorts.end(),
            [&](auto& otherPort)
            {
                return otherPort.name_ == port.name_;
            });
        if (iter != otherOutputPorts.end())
        {
            return true;
        }
    }
    return false;
}


void BlockDynamicSystem::sortBlocks()
{
    size_t size = blocks_.size();
    for(size_t index=0;index<size;index++)
    {
        auto frontBlock = blocks_[index];
        for(size_t laterIndex=index+1;laterIndex<size;laterIndex++)
        {
            auto laterBlock = blocks_[laterIndex];
            if(aAstroBlockDependentOn(frontBlock, laterBlock))
            {
                std::swap(blocks_[laterIndex],blocks_[index]);
            }
        }
    }
}

void BlockDynamicSystem::connectSignalsByNames()
{
    
}


AST_NAMESPACE_END
