///
/// @file      BlockSaturation.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/FuncBlock.hpp"

AST_NAMESPACE_BEGIN

/// @brief 饱和块
class AST_CORE_API BlockSaturation: public FuncBlock
{
public:
    BlockSaturation(double lowerLimit = -1.0, double upperLimit = 1.0);

    err_t run(const SimTime &simTime) override;
    
    /// @brief 设置下限
    /// @param lowerLimit 下限值
    void setLowerLimit(double lowerLimit);
    
    /// @brief 获取下限
    /// @return 下限值
    double getLowerLimit() const;
    
    /// @brief 设置上限
    /// @param upperLimit 上限值
    void setUpperLimit(double upperLimit);
    
    /// @brief 获取上限
    /// @return 上限值
    double getUpperLimit() const;
    
    /// @brief 设置上下限
    /// @param lowerLimit 下限值
    /// @param upperLimit 上限值
    void setLimits(double lowerLimit, double upperLimit);

protected:
    double lowerLimit_{-1.0};
    double upperLimit_{1.0};
    double* input_{nullptr};
    double* output_{&outputBuffer_};
    double outputBuffer_{0.0};
};

AST_NAMESPACE_END