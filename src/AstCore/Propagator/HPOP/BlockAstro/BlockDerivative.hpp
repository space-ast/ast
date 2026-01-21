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


/// @brief 导数函数块
/// @details 
/// 导数函数块是一个特殊的函数块，在普通函数块的基础上，添加了导数端口
/// 导数端口用于与动力学系统自动建立连接关系(详细实现机制见BlockDynamicSystem)
class AST_CORE_API BlockDerivative: public FuncBlock
{
public:
    /// @brief 析构函数
    ~BlockDerivative() override = default;

    /// @brief 获取导数端口
    const std::vector<DataPort>& getDerivativePorts() const { return derivativePorts_; }
    std::vector<DataPort>& getDerivativePorts() { return derivativePorts_; }
    
protected:
    std::vector<DataPort> derivativePorts_;  // 导数端口
};

AST_NAMESPACE_END
