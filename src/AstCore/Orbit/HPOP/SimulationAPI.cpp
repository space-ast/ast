///
/// @file      SimulationAPI.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-18
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

#include "SimulationAPI.hpp"
#include "Simulation.hpp"

AST_NAMESPACE_BEGIN


size_t aGetNumInputPorts(FuncBlock* block)
{
    if(!block)
        return 0;
    return block->getNumInputPorts();
}

size_t aGetNumOutputPorts(FuncBlock* block)
{
    if(!block)
        return 0;
    return block->getNumOutputPorts();
}

int aGetInputPortDataType(FuncBlock* block, size_t portIndex)
{
    if(!block)
        return -1;
    auto port = block->getInputPort(portIndex);
    if(!port)
        return -1;
    return port->getDataType();
}

int aGetOutputPortDataType(FuncBlock* block, size_t portIndex)
{
    if(!block)
        return -1;
    auto port = block->getOutputPort(portIndex);
    if(!port)
        return -1;
    return port->getDataType();
}

const void* aGetInputPortSignal(FuncBlock* block, size_t portIndex)
{
    if(!block)
        return nullptr;
    auto port = block->getInputPort(portIndex);
    if(!port)
        return nullptr;
    return port->getSignal();
}


void* aGetOutputPortSignal(FuncBlock* block, size_t portIndex)
{
    if(!block)
        return nullptr;
    auto port = block->getOutputPort(portIndex);
    if(!port)
        return nullptr;
    return port->getSignal();
}


double* aGetInputPortRealSignal(FuncBlock* block, size_t portIndex)
{
    if(!block)
        return nullptr;
    auto port = block->getInputPort(portIndex);
    if(!port)
        return nullptr;
    return port->getSignalDouble();
}


double* aGetOutputPortRealSignal(FuncBlock* block, size_t portIndex)
{
    if(!block)
        return nullptr;
    auto port = block->getOutputPort(portIndex);
    if(!port)
        return nullptr;
    return port->getSignalDouble();
}

int aGetInputPortWidth(FuncBlock* block, size_t portIndex)
{
    if(!block)
        return -1;
    auto port = block->getInputPort(portIndex);
    if(!port)
        return -1;
    return port->getWidth();
}

int aGetOutputPortWidth(FuncBlock* block, size_t portIndex)
{
    if(!block)
        return -1;
    auto port = block->getOutputPort(portIndex);
    if(!port)
        return -1;
    return port->getWidth();
}




AST_NAMESPACE_END
