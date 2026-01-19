///
/// @file      FuncBlock.hpp
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

#pragma once

#include "AstGlobal.h"
#include "DataPort.hpp"
#include "SimTime.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/// @brief 函数块/功能块
class AST_CORE_API FuncBlock
{
public:
    FuncBlock() = default;
    virtual ~FuncBlock(){};

    /// @brief 计算函数块
    /// @param simTime 仿真时间
    /// @return 错误码
    virtual err_t evaluate(const SimTime& simTime) = 0;


    /// @brief 获取输入端口
    /// @param ports 输入端口向量
    const std::vector<DataPort>& getInputPorts() const { return inputPorts_; }

    /// @brief 获取输出端口
    /// @param ports 输出端口向量
    const std::vector<DataPort>& getOutputPorts() const { return outputPorts_; }


    /// @brief 获取输入端口数量
    /// @return 输入端口数量
    size_t getNumInputPorts() const { return inputPorts_.size(); }

    /// @brief 获取输出端口数量
    /// @return 输出端口数量
    size_t getNumOutputPorts() const { return outputPorts_.size(); }

    /// @brief 获取输入端口
    /// @param portName 端口名称
    /// @return 输入端口指针
    DataPort* getInputPort(StringView portName);

    /// @brief 获取输出端口
    /// @param portName 端口名称
    /// @return 输出端口指针
    DataPort* getOutputPort(StringView portName);

    /// @brief 获取输入端口
    /// @param portIndex 端口索引
    /// @return 输入端口指针
    DataPort* getInputPort(size_t portIndex);

    /// @brief 获取输出端口
    /// @param portIndex 端口索引
    /// @return 输出端口指针
    DataPort* getOutputPort(size_t portIndex);

    /// @brief 获取输入端口数据类型
    /// @param portIndex 端口索引
    /// @return 输入端口数据类型
    DataPort::EValueType getInputPortDataType(size_t portIndex);


    /// @brief 获取输出端口数据类型
    /// @param portIndex 端口索引
    /// @return 输出端口数据类型
    DataPort::EValueType getOutputPortDataType(size_t portIndex);

    /// @brief 获取输入端口信号
    /// @param portIndex 端口索引
    /// @return 输入端口信号指针
    void* getInputPortSignal(size_t portIndex);


    /// @brief 获取输出端口信号
    /// @param portIndex 端口索引
    /// @return 输出端口信号指针
    void* getOutputPortSignal(size_t portIndex);


    /// @brief 获取输入端口实数值信号
    /// @param portIndex 端口索引
    /// @return 输入端口实数值信号指针
    double* getInputPortRealSignal(size_t portIndex);


    /// @brief 获取输出端口实数值信号
    /// @param portIndex 端口索引
    /// @return 输出端口实数值信号指针
    double* getOutputPortRealSignal(size_t portIndex);

    
    /// @brief 获取输入端口宽度
    /// @param portIndex 端口索引
    /// @return 输入端口宽度
    int getInputPortWidth(size_t portIndex);


    /// @brief 获取输出端口宽度
    /// @param portIndex 端口索引
    /// @return 输出端口宽度
    int getOutputPortWidth(size_t portIndex);

    /// @brief 连接函数块的端口
    /// @param src 源函数块
    /// @param srcPortName 源端口名称
    /// @param dst 目标函数块
    /// @param dstPortName 目标端口名称
    /// @return 错误码
    static err_t connect(FuncBlock* src, StringView srcPortName, FuncBlock* dst, StringView dstPortName);

    /// @brief 连接函数块的端口
    /// @param src 源函数块
    /// @param srcPortIndex 源端口索引
    /// @param dst 目标函数块
    /// @param dstPortIndex 目标端口索引
    /// @return 错误码
    static err_t connect(FuncBlock* src, size_t srcPortIndex, FuncBlock* dst, size_t dstPortIndex);

protected:
    std::vector<DataPort> inputPorts_;
    std::vector<DataPort> outputPorts_;
};

AST_NAMESPACE_END
