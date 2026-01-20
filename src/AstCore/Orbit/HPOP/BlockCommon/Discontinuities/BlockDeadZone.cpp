///
/// @file      BlockDeadZone.cpp
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

#include "BlockDeadZone.hpp"
#include "AstUtil/IdentifierAPI.hpp"

AST_NAMESPACE_BEGIN

BlockDeadZone::BlockDeadZone()
    : input_(nullptr)
    , output_(&outputBuffer_)
    , outputBuffer_(0.0)
    , lowerLimit_(-0.1)
    , upperLimit_(0.1)
{
    static auto identifier_input = aIdentifier("x");
    static auto identifier_output = aIdentifier("y");

    inputPorts_ = {
        DataPort{
            identifier_input,
            (ptr_t*)&input_,
            1,
            DataPort::eDouble,
        },
    };
    outputPorts_ = {
        DataPort{
            identifier_output,
            (ptr_t*)&output_,
            1,
            DataPort::eDouble,
        },
    };
}

err_t BlockDeadZone::evaluate(const SimTime &simTime)
{
    if (input_ == nullptr || output_ == nullptr)
    {
        return eErrorNullInput;
    }
    
    // 实现死区特性
    double inputValue = *input_;
    if (inputValue > lowerLimit_ && inputValue < upperLimit_)
    {
        // 输入在死区范围内，输出为0
        *output_ = 0.0;
    }
    else if (inputValue <= lowerLimit_)
    {
        // 输入小于等于死区下限，输出为输入减去死区下限
        *output_ = inputValue - lowerLimit_;
    }
    else
    {
        // 输入大于等于死区上限，输出为输入减去死区上限
        *output_ = inputValue - upperLimit_;
    }
    
    return eNoError;
}

void BlockDeadZone::setLowerLimit(double lowerLimit)
{
    lowerLimit_ = lowerLimit;
}

void BlockDeadZone::setUpperLimit(double upperLimit)
{
    upperLimit_ = upperLimit;
}

double BlockDeadZone::getLowerLimit() const
{
    return lowerLimit_;
}

double BlockDeadZone::getUpperLimit() const
{
    return upperLimit_;
}

AST_NAMESPACE_END