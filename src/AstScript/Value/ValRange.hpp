///
/// @file      ValRange.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-26
/// @copyright 版权所有 (C) 2025-present, ast项目.
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
#include "Value.hpp"

AST_NAMESPACE_BEGIN


/// @brief     范围值
/// @details   表示一个范围值，包含起始值、结束值和步长。
/// @todo      目前只支持浮点数类型的范围值
class ValRange : public Value
{
public:
    AST_EXPR(ValRange)
    /// @brief     构造函数
    /// @param     start 范围的起始值
    /// @param     stop 范围的结束值
    /// @param     step 范围的步长，默认值为1.0
    ValRange(double start, double stop, double step=1.0);
    ~ValRange() override = default;

    /// @brief     获取范围的起始值
    /// @return    范围的起始值
    double start() const { return start_; }

    /// @brief     获取范围的结束值
    /// @return    范围的结束值
    double stop() const { return stop_; }

    /// @brief     获取范围的步长
    /// @return    范围的步长
    double step() const { return step_; }

    /// @brief     获取范围的元素数量
    /// @return    范围的元素数量
    int size() const { return (int)((stop_ - start_) / step_) + 1; }

    /// @brief     获取范围的表达式
    /// @param     context 上下文对象
    /// @return    范围的表达式字符串
    std::string getExpression(Object* context) const override;
    
protected:
    double start_;
    double stop_;
    double step_;
};


AST_NAMESPACE_END
