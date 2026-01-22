///
/// @file      BlockOut.cpp
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

#include "BlockOut.hpp"
#include "AstUtil/IdentifierAPI.hpp"

AST_NAMESPACE_BEGIN

BlockOut::BlockOut()
    : input_(nullptr)
    , value_(0.0)
{
    static auto identifier_input = aIdentifier("x");

    inputPorts_ = {
        DataPort{
            identifier_input,
            (signal_t*)&input_,
            1,
            DataPort::eDouble,
        },
    };
}

err_t BlockOut::run(const SimTime &simTime)
{
    if (input_ == nullptr)
    {
        return eErrorNullInput;
    }
    value_ = *input_;
    return eNoError;
}

double BlockOut::getValue() const
{
    return value_;
}

AST_NAMESPACE_END