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
#include "AstCore/BlockDerivative.hpp"
#include "AstUtil/Identifier.hpp"
#include "AstUtil/Logger.hpp"

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


void BlockDynamicSystem::addDerivativeBlock(BlockDerivative* block)
{
    this->derivativeBlocks_.push_back(block);
    this->BlockSystem::addBlock(block);
}

void BlockDynamicSystem::clearBlocks()
{
    this->BlockSystem::clearBlocks();
    this->derivativeBlocks_.clear();
}

void BlockDynamicSystem::reset()
{
    this->clearBlocks();
    this->state_.clear();
    this->derivative_.clear();
    this->stateMap_.clear();
    this->derivativeMap_.clear();
}

void BlockDynamicSystem::addBlock(BlockDerivative* block)
{
    return addDerivativeBlock(block);
}

err_t BlockDynamicSystem::initialize()
{
    // 1. 模型排序
    err_t err = sortBlocks();
    if(err!=0)
        return err;

    // 2. 创建状态量映射表
    err = createStateMap();
    if(err!=0)
        return err;

    // 3. 连接信号
    return connectSignalsByNames();
}


err_t BlockDynamicSystem::sortBlocks()
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
    return 0;
}

/// @brief 创建状态量映射表
err_t BlockDynamicSystem::createStateMap()
{
    size_t size = derivativeBlocks_.size();
    int totalStateDimension = 0;
    std::vector<int> stateDimensions;           // 状态量维度
    std::vector<Identifier*> stateIdentifiers;  // 状态量标识符

    for(auto block: blocks_)
    {
        // 遍历该函数块的所有输出端口
        auto& outputPorts = block->getOutputPorts();
        for(auto& port : outputPorts)
        {
            // 汇总所有状态量信号
            auto name = port.name_;
            stateMap_[name] = port.getSignal<double>();
        }
    }

    // 遍历所有的状态量导数，统计微分状态量的维度
    for(auto block:derivativeBlocks_)
    {
        // 遍历该函数块的所有导数端口
        auto& derivativePorts = block->getDerivativePorts();
        for(auto& port : derivativePorts)
        {
            auto name = port.name_;
            auto iter = std::find(stateIdentifiers.begin(), stateIdentifiers.end(), name);
            if(iter == stateIdentifiers.end())
            {
                // 未添加过该状态量
                int width = port.getWidth();
                stateIdentifiers.push_back(name);
                stateDimensions.push_back(width);
                totalStateDimension += width;
            }else{
                // 已经添加过该状态量
                // 检查状态量维度是否一致
                if(port.getWidth() != stateDimensions[iter - stateIdentifiers.begin()])
                {
                    aError("state dimension of %s is not consistent", name->c_str());
                    return -1;
                }
                // @todo: 检测状态量是否支持累加，避免意外的覆盖
            }
        }
    }
    
    // 分配状态量和导数向量
    state_.resize(totalStateDimension);
    derivative_.resize(totalStateDimension);

    // 初始化状态量映射表
    int offset = 0;
    for(size_t index=0;index<stateIdentifiers.size();index++)
    {
        auto name = stateIdentifiers[index];
        auto width = stateDimensions[index];
        stateMap_[name] = state_.data() + offset;
        derivativeMap_[name] = derivative_.data() + offset;
        offset += width;
    }
    return 0;
}



err_t BlockDynamicSystem::connectSignalsByNames()
{
    for(auto block: blocks_)
    {
        // 遍历该函数块的所有输入端口
        auto& inputPorts = block->getInputPorts();
        for(auto& port : inputPorts)
        {
            // 连接信号
            auto name = port.name_;
            auto iter = stateMap_.find(name);
            if(iter != stateMap_.end())
            {
                // 连接状态量信号
                port.setSignal(iter->second);
            }
        }
    }
    for(auto block: derivativeBlocks_)
    {
        // 遍历该函数块的所有导数端口
        auto& derivativePorts = block->getDerivativePorts();
        for(auto& port : derivativePorts)
        {
            // 连接信号
            auto name = port.name_;
            auto iter = derivativeMap_.find(name);
            if(iter != derivativeMap_.end())
            {
                // 连接导数信号
                port.setSignal(iter->second);
            }
        }
    }
    return 0;
}


AST_NAMESPACE_END
