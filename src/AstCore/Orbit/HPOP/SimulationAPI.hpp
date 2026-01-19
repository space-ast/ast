///
/// @file      SimulationAPI.hpp
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

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

class FuncBlock;

/// @brief 获取函数块输入端口数量
/// @details 参考 simulink API的 ssGetNumInputPorts 函数
/// @param block 函数块指针
/// @return 输入端口数量
AST_CORE_CAPI size_t aGetNumInputPorts(FuncBlock* block);



/// @brief 获取函数块输出端口数量
/// @details 参考 simulink API的 ssGetNumOutputPorts 函数
/// @param block 函数块指针
/// @return 输出端口数量
AST_CORE_CAPI size_t aGetNumOutputPorts(FuncBlock* block);



/// @brief 获取函数块输入端口数据类型
/// @details 参考 simulink API的 ssGetInputPortDataType 函数
/// @param block 函数块指针
/// @param portIndex 输入端口索引
/// @return 数据类型
AST_CORE_CAPI int aGetInputPortDataType(FuncBlock* block, size_t portIndex);



/// @brief 获取函数块输出端口数据类型
/// @details 参考 simulink API的 ssGetOutputPortDataType 函数
/// @param block 函数块指针
/// @param portIndex 输出端口索引
/// @return 数据类型
AST_CORE_CAPI int aGetOutputPortDataType(FuncBlock* block, size_t portIndex);


/// @brief 获取函数块输入端口的信号(指向数据的指针)
/// @details 参考 simulink API的 ssGetInputPortSignal 函数
/// @param block 函数块指针
/// @param portIndex 输入端口索引
/// @return 信号(指向数据的指针)
AST_CORE_CAPI const void* aGetInputPortSignal(FuncBlock* block, size_t portIndex);


/// @brief 获取函数块输出端口的信号(指向数据的指针)
/// @details 参考 simulink API的 ssGetOutputPortSignal 函数
/// @param block 函数块指针
/// @param portIndex 输出端口索引
/// @return 信号(指向数据的指针)
AST_CORE_CAPI void* aGetOutputPortSignal(FuncBlock* block, size_t portIndex);



/// @brief 获取函数块输入端口的实数值信号
/// @details 参考 simulink API的 ssGetInputPortRealSignal 函数
/// @param block 函数块指针
/// @param portIndex 输入端口索引
/// @return 实数值指针
AST_CORE_CAPI double* aGetInputPortRealSignal(FuncBlock* block, size_t portIndex);



/// @brief 获取函数块输出端口的实数值信号
/// @details 参考 simulink API的 ssGetOutputPortRealSignal 函数
/// @param block 函数块指针
/// @param portIndex 输出端口索引
/// @return 实数值指针
AST_CORE_CAPI double* aGetOutputPortRealSignal(FuncBlock* block, size_t portIndex);



/// @brief 获取函数块输入端口的宽度
/// @details 参考 simulink API的 ssGetInputPortWidth 函数
/// @param block 函数块指针
/// @param portIndex 输入端口索引
/// @return 端口宽度
AST_CORE_CAPI int aGetInputPortWidth(FuncBlock* block, size_t portIndex);


/// @brief 获取函数块输出端口的宽度
/// @details 参考 simulink API的 ssGetOutputPortWidth 函数
/// @param block 函数块指针
/// @param portIndex 输出端口索引
/// @return 端口宽度
AST_CORE_CAPI int aGetOutputPortWidth(FuncBlock* block, size_t portIndex);


// -----------------
// 扩展接口
// -----------------


/// @brief 设置函数块输入端口的实数值信号
/// @param block 函数块指针
/// @param portIndex 输入端口索引
/// @param signal 信号(指向数据的指针)
/// @return 错误码
AST_CORE_CAPI err_t aSetInputPortRealSignal(FuncBlock* block, size_t portIndex, double* signal);


/// @brief 设置函数块输出端口的实数值信号
/// @param block 函数块指针
/// @param portIndex 输出端口索引
/// @param signal 信号(指向数据的指针)
/// @return 错误码
AST_CORE_CAPI err_t aSetOutputPortRealSignal(FuncBlock* block, size_t portIndex, double* signal);


AST_NAMESPACE_END
