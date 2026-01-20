///
/// @file      BlockTwoBody.cpp
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

#include "BlockTwoBody.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

BlockTwoBody::BlockTwoBody()
    : BlockAstro{}
    , posCBI{&vectorBuffer}
    , accTwoBody{&vectorBuffer}
    , vectorBuffer{}
    , twoBodyGM_(kEarthGrav)
{
    static auto identifierPos = aIdentifier(kIdentifierPos);
    static auto identifierAccTwoBody = aIdentifier(kIdentifierAccTwoBody);

    inputPorts_ = {
        {
            identifierPos,
            (ptr_t*)&posCBI,
            3,
            DataPort::eDouble
        }
    };

    outputPorts_ = {
        {
            identifierAccTwoBody,
            (ptr_t*)&accTwoBody,
            3,
            DataPort::eDouble
        }
    };

}

err_t BlockTwoBody::evaluate(const SimTime &simTime)
{
    double rSqr = posCBI->squaredNorm();
    double r = std::sqrt(rSqr);
    double factor = -twoBodyGM_ / (r * rSqr);
    *accTwoBody = factor * (*posCBI);
    return eNoError;
}



AST_NAMESPACE_END

