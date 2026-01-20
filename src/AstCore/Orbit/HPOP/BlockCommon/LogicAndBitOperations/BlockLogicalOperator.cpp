///
/// @file      BlockLogicalOperator.cpp
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

#include "BlockLogicalOperator.hpp"
#include "AstUtil/IdentifierAPI.hpp"

AST_NAMESPACE_BEGIN

BlockLogicalOperator::BlockLogicalOperator(ELogicalOperatorType type)
    : operatorType_(type)
    , input1_(nullptr)
    , input2_(nullptr)
    , output_(&outputBuffer_)
    , outputBuffer_(0)
{
    static auto identifier_input1 = aIdentifier("x1");
    static auto identifier_input2 = aIdentifier("x2");
    static auto identifier_output = aIdentifier("y");

    if (operatorType_ == ELogicalOperatorType::eNot)
    {
        // 逻辑非操作只需要一个输入
        inputPorts_ = {
            DataPort{
                identifier_input1,
                (ptr_t*)&input1_,
                1,
                DataPort::eInt,
            },
        };
    }
    else
    {
        // 其他逻辑操作需要两个输入
        inputPorts_ = {
            DataPort{
                identifier_input1,
                (ptr_t*)&input1_,
                1,
                DataPort::eInt,
            },
            DataPort{
                identifier_input2,
                (ptr_t*)&input2_,
                1,
                DataPort::eInt,
            },
        };
    }
    
    outputPorts_ = {
        DataPort{
            identifier_output,
            (ptr_t*)&output_,
            1,
            DataPort::eInt,
        },
    };
}

err_t BlockLogicalOperator::evaluate(const SimTime &simTime)
{
    if (output_ == nullptr)
    {
        return eErrorNullInput;
    }
    
    if (operatorType_ == ELogicalOperatorType::eNot)
    {
        // 逻辑非操作
        if (input1_ == nullptr)
        {
            return eErrorNullInput;
        }
        *output_ = (*input1_) == 0 ? 1 : 0;
    }
    else
    {
        // 其他逻辑操作需要两个输入
        if (input1_ == nullptr || input2_ == nullptr)
        {
            return eErrorNullInput;
        }
        
        bool val1 = (*input1_) != 0;
        bool val2 = (*input2_) != 0;
        
        switch (operatorType_)
        {
        case ELogicalOperatorType::eAnd:
            *output_ = (val1 && val2) ? 1 : 0;
            break;
        case ELogicalOperatorType::eOr:
            *output_ = (val1 || val2) ? 1 : 0;
            break;
        case ELogicalOperatorType::eXor:
            *output_ = (val1 != val2) ? 1 : 0;
            break;
        default:
            *output_ = 0;
            break;
        }
    }
    
    return eNoError;
}

void BlockLogicalOperator::setOperatorType(ELogicalOperatorType type)
{
    operatorType_ = type;
    
    // 更新输入端口配置
    static auto identifier_input1 = aIdentifier("x1");
    static auto identifier_input2 = aIdentifier("x2");

    if (operatorType_ == ELogicalOperatorType::eNot)
    {
        // 逻辑非操作只需要一个输入
        inputPorts_ = {
            DataPort{
                identifier_input1,
                (ptr_t*)&input1_,
                1,
                DataPort::eInt,
            },
        };
    }
    else
    {
        // 其他逻辑操作需要两个输入
        inputPorts_ = {
            DataPort{
                identifier_input1,
                (ptr_t*)&input1_,
                1,
                DataPort::eInt,
            },
            DataPort{
                identifier_input2,
                (ptr_t*)&input2_,
                1,
                DataPort::eInt,
            },
        };
    }
}

ELogicalOperatorType BlockLogicalOperator::getOperatorType() const
{
    return operatorType_;
}

AST_NAMESPACE_END