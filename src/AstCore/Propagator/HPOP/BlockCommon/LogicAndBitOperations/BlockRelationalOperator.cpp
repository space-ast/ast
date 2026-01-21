///
/// @file      BlockRelationalOperator.cpp
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

#include "BlockRelationalOperator.hpp"
#include "AstUtil/IdentifierAPI.hpp"

AST_NAMESPACE_BEGIN

BlockRelationalOperator::BlockRelationalOperator(ERelationalOperatorType type)
    : operatorType_(type)
    , input1_(nullptr)
    , input2_(nullptr)
    , output_(&outputBuffer_)
    , outputBuffer_(0)
{
    static auto identifier_input1 = aIdentifier("x1");
    static auto identifier_input2 = aIdentifier("x2");
    static auto identifier_output = aIdentifier("y");

    inputPorts_ = {
        DataPort{
            identifier_input1,
            (signal_t*)&input1_,
            1,
            DataPort::eDouble,
        },
        DataPort{
            identifier_input2,
            (signal_t*)&input2_,
            1,
            DataPort::eDouble,
        },
    };
    
    outputPorts_ = {
        DataPort{
            identifier_output,
            (signal_t*)&output_,
            1,
            DataPort::eInt,
        },
    };
}

err_t BlockRelationalOperator::run(const SimTime &simTime)
{
    if (input1_ == nullptr || input2_ == nullptr || output_ == nullptr)
    {
        return eErrorNullInput;
    }
    
    double value1 = *input1_;
    double value2 = *input2_;
    
    switch (operatorType_)
    {
    case ERelationalOperatorType::eEqual:
        *output_ = (value1 == value2) ? 1 : 0;
        break;
    case ERelationalOperatorType::eNotEqual:
        *output_ = (value1 != value2) ? 1 : 0;
        break;
    case ERelationalOperatorType::eGreaterThan:
        *output_ = (value1 > value2) ? 1 : 0;
        break;
    case ERelationalOperatorType::eLessThan:
        *output_ = (value1 < value2) ? 1 : 0;
        break;
    case ERelationalOperatorType::eGreaterOrEqual:
        *output_ = (value1 >= value2) ? 1 : 0;
        break;
    case ERelationalOperatorType::eLessOrEqual:
        *output_ = (value1 <= value2) ? 1 : 0;
        break;
    default:
        *output_ = 0;
        break;
    }
    
    return eNoError;
}

void BlockRelationalOperator::setOperatorType(ERelationalOperatorType type)
{
    operatorType_ = type;
}

ERelationalOperatorType BlockRelationalOperator::getOperatorType() const
{
    return operatorType_;
}

AST_NAMESPACE_END