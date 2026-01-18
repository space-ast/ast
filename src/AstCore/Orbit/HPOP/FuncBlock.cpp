///
/// @file      FuncBlock.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
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

#include "FuncBlock.hpp"
#include "AstUtil/Identifier.hpp"

AST_NAMESPACE_BEGIN

DataPort* FuncBlock::getInputPort(StringView portName)
{
    for(auto& port : inputPorts_){
        if(port.name_->equals(portName)){
            return &port;
        }
    }
    return nullptr;
}

DataPort* FuncBlock::getOutputPort(StringView portName)
{
    for(auto& port : outputPorts_){
        if(port.name_->equals(portName)){
            return &port;
        }
    }
    return nullptr;
}


DataPort* FuncBlock::getInputPort(size_t index)
{
    if(index >= inputPorts_.size()){
        return nullptr;
    }
    return &inputPorts_[index];
}


DataPort* FuncBlock::getOutputPort(size_t index)
{
    if(index >= outputPorts_.size()){
        return nullptr;
    }
    return &outputPorts_[index];
}


err_t FuncBlock::connect(FuncBlock* src, StringView srcPortName, FuncBlock* dst, StringView dstPortName)
{
    if(src == nullptr || dst == nullptr){
        return eErrorNullInput;
    }
    auto srcPort = src->getOutputPort(srcPortName);
    auto dstPort = dst->getInputPort(dstPortName);
    return dstPort->connect(srcPort);
}

err_t FuncBlock::connect(FuncBlock* src, size_t srcPortIndex, FuncBlock* dst, size_t dstPortIndex)
{
    if(src == nullptr || dst == nullptr){
        return eErrorNullInput;
    }
    auto srcPort = src->getOutputPort(srcPortIndex);
    auto dstPort = dst->getInputPort(dstPortIndex);
    return DataPort::connect(srcPort, dstPort);
}


AST_NAMESPACE_END
