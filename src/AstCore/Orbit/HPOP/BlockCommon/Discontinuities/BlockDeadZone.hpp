///
/// @file      BlockDeadZone.hpp
/// @brief     ~
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/FuncBlock.hpp"

AST_NAMESPACE_BEGIN

/// @brief 死区块
class AST_CORE_API BlockDeadZone : public FuncBlock
{
public:
    BlockDeadZone();

    err_t evaluate(const SimTime &simTime) override;
    
    /// @brief 设置死区下限
    /// @param lowerLimit 死区下限
    void setLowerLimit(double lowerLimit);
    
    /// @brief 设置死区上限
    /// @param upperLimit 死区上限
    void setUpperLimit(double upperLimit);
    
    /// @brief 获取死区下限
    /// @return 死区下限
    double getLowerLimit() const;
    
    /// @brief 获取死区上限
    /// @return 死区上限
    double getUpperLimit() const;
    
protected:
    double* input_{nullptr};  // 输入值
    double* output_{&outputBuffer_};
    double outputBuffer_{0.0};
    double lowerLimit_{-0.1};  // 死区下限，默认为-0.1
    double upperLimit_{0.1};   // 死区上限，默认为0.1
};

AST_NAMESPACE_END