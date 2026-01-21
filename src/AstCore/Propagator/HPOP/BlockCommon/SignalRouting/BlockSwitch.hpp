///
/// @file      BlockSwitch.hpp
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

/// @brief 开关块
class AST_CORE_API BlockSwitch: public FuncBlock
{
public:
    BlockSwitch();

    err_t run(const SimTime &simTime) override;
    
    /// @brief 设置阈值
    /// @param threshold 阈值
    void setThreshold(double threshold);
    
    /// @brief 获取阈值
    /// @return 阈值
    double getThreshold() const;

protected:
    double threshold_{0.0};
    double* input1_{nullptr};  // 输入1
    double* input2_{nullptr};  // 输入2
    double* control_{nullptr}; // 控制输入
    double* output_{&outputBuffer_};
    double outputBuffer_{0.0};
};

AST_NAMESPACE_END