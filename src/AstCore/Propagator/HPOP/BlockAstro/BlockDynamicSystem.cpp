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
#include "AstUtil/IdentifierAPI.hpp"

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
            [&](DataPort& otherPort)
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

errc_t BlockDynamicSystem::run(const SimTime &simTime)
{
    this->fillDerivativeData(0.0);      // 填充导数数据为0
    this->fillAccumulateData(0.0);      // 填充累加状态量数据为0
    return this->BlockSystem::run(simTime);
}


void BlockDynamicSystem::addBlock(BlockDerivative* block)
{
    return addDerivativeBlock(block);
}

size_t BlockDynamicSystem::getStateIndex(Identifier *id) const
{
    auto iter = derivativeMap_.find(id);
    if(iter != derivativeMap_.end())
    {
        const double* data = iter->second;
        size_t index = data - derivative_.data();
        if(A_UNLIKELY(index >= derivative_.size()))
            return -1;
        return index;
    }
    return -1;
}

size_t BlockDynamicSystem::getStateIndex(StringView name) const
{
    return getStateIndex(aIdentifier(name));
}

errc_t BlockDynamicSystem::initialize()
{
    // 1. 函数块排序
    errc_t err = sortBlocks();
    if(err!=0)
        return err;

    // 2. 创建状态量映射表
    err = createStateMap();
    if(err!=0)
        return err;

    // 3. 连接信号
    return connectSignalsByNames();
}


errc_t BlockDynamicSystem::sortBlocks()
{
    /// @todo 考虑使用拓扑排序算法对模型进行排序，例如Kahn 算法或者DFS 算法
    size_t size = blocks_.size();
    for(size_t index=0;index<size;index++)
    {
        for(size_t laterIndex=index+1;laterIndex<size;laterIndex++)
        {
            if(aAstroBlockDependentOn(blocks_[index], blocks_[laterIndex]))
            {
                std::swap(blocks_[laterIndex],blocks_[index]);
            }
        }
    }
    return 0;
}

/// @brief 创建状态量映射表
errc_t BlockDynamicSystem::createStateMap()
{
    {
        int totalWidth = 0;
        std::vector<int> widths;           // 状态量维度
        std::vector<Identifier*> identifiers;  // 状态量标识符

        // 遍历所有的状态量导数，统计微分状态量的维度
        for(auto block:derivativeBlocks_)
        {
            // 遍历该函数块的所有导数端口
            auto& derivativePorts = block->getDerivativePorts();
            for(auto& port : derivativePorts)
            {
                auto name = port.name_;
                auto iter = std::find(identifiers.begin(), identifiers.end(), name);
                if(iter == identifiers.end())
                {
                    // 未添加过该状态量
                    int width = port.getWidth();
                    identifiers.push_back(name);
                    widths.push_back(width);
                    totalWidth += width;
                }else{
                    // 已经添加过该状态量
                    // 检查状态量维度是否一致
                    if(port.getWidth() != widths[iter - identifiers.begin()])
                    {
                        aError("state dimension of %s is not consistent", name->c_str());
                        return -1;
                    }
                    // @todo: 检测状态量是否支持累加，避免意外的覆盖
                }
            }
        }
        
        // 分配状态量和导数向量
        state_.resize(totalWidth);
        derivative_.resize(totalWidth);

        // 初始化状态量映射表
        int offset = 0;
        for(size_t index=0;index<identifiers.size();index++)
        {
            auto name = identifiers[index];
            auto width = widths[index];
            // @note 
            // 这里将导数对应的状态量插入到了stateMap_中
            // 在后面的逻辑里会根据状态量映射表，将所有block的outputPorts里的信号指针替换为最新的
            // 所以这里的逻辑一定要在遍历输出端口汇总输出状态量之前执行
            stateMap_.emplace(name, state_.data() + offset);
            derivativeMap_.emplace(name, derivative_.data() + offset);
            offset += width;
        }
    }

    {
        int totalWidth = 0;
        std::vector<int> widths;                    // 状态量维度
        std::vector<Identifier*> identifiers;       // 状态量标识符
        std::vector<DataPort*> accumulatePorts;     // 累加状态量输出端口
        
        for(auto block: blocks_)
        {
            // 遍历该函数块的所有输出端口
            auto& outputPorts = block->getOutputPorts();
            for(auto& port : outputPorts)
            {
                // 汇总所有状态量信号
                auto name = port.name_;
                if(port.getMode() == DataPort::eAccumulate)
                {
                    accumulatePorts.push_back(&port);
                    auto iter = std::find(identifiers.begin(), identifiers.end(), name);
                    if(iter == identifiers.end())
                    {
                        // 未添加过该状态量
                        int width = port.getWidth();
                        identifiers.push_back(name);
                        widths.push_back(width);
                        totalWidth += width;
                    }else{
                        // 已经添加过该状态量
                        // 检查状态量维度是否一致
                        if(port.getWidth() != widths[iter - identifiers.begin()])
                        {
                            aError("state dimension of %s is not consistent", name->c_str());
                            return -1;
                        }
                    }
                }
                else
                {
                    // 如果状态量映射表中不存在该状态量，则添加
                    auto result = stateMap_.emplace(name, port.getSignal<double>());
                    // 如果状态量映射表中已存在该状态量，则更新信号指针
                    if(!result.second)
                    {
                        port.setSignal<double>(stateMap_[name]);
                    }
                }
            }
        }
        // 分配状态量和导数向量
        accumulate_.resize(totalWidth);

        // 初始化状态量映射表
        int offset = 0;
        for(size_t index=0;index<identifiers.size();index++)
        {
            auto name = identifiers[index];
            auto width = widths[index];
            stateMap_.emplace(name, accumulate_.data() + offset);
            offset += width;
        }
        // 设置累加状态量信号
        for(auto& port : accumulatePorts)
        {
            port->setSignal<double>(stateMap_[port->name_]);
        }
    }

    
    return 0;
}



errc_t BlockDynamicSystem::connectSignalsByNames()
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
            if(A_LIKELY(iter != stateMap_.end()))
            {
                // 连接状态量信号
                port.setSignal(iter->second);
            }else{
                // 未找到所需状态量信号
                aError("state '%s' is not found for input port", name->c_str());
                return -1;
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
            if(A_LIKELY(iter != derivativeMap_.end()))
            {
                // 连接导数信号
                port.setSignal(iter->second);
            }else{
                // 未找到所需导数信号
                aError("derivative '%s' is not found for block", name->c_str());
                return -1;
            }
        }
    }
    return 0;
}


AST_NAMESPACE_END
