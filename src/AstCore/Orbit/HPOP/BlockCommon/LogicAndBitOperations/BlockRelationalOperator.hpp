///
/// @file      BlockRelationalOperator.hpp
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

/// @brief 关系运算符类型枚举
enum class ERelationalOperatorType
{
    eEqual,             ///< 等于
    eNotEqual,         ///< 不等于
    eGreaterThan,      ///< 大于
    eLessThan,         ///< 小于
    eGreaterOrEqual,  ///< 大于等于
    eLessOrEqual      ///< 小于等于
};

/// @brief 关系运算符块
class AST_CORE_API BlockRelationalOperator: public FuncBlock
{
public:
    BlockRelationalOperator(ERelationalOperatorType type = ERelationalOperatorType::eEqual);

    err_t evaluate(const SimTime &simTime) override;
    
    /// @brief 设置关系运算符类型
    /// @param type 关系运算符类型
    void setOperatorType(ERelationalOperatorType type);
    
    /// @brief 获取关系运算符类型
    /// @return 关系运算符类型
    ERelationalOperatorType getOperatorType() const;

protected:
    ERelationalOperatorType operatorType_{ERelationalOperatorType::eEqual};
    double* input1_{nullptr};
    double* input2_{nullptr};
    int* output_{&outputBuffer_};
    int outputBuffer_{0};
};

AST_NAMESPACE_END