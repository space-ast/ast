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
    : BlockTwoBody{kEarthGrav}
{
    
}

BlockTwoBody::BlockTwoBody(double twoBodyGM)
    : BlockDerivative{}
    , posCBI{&vectorBuffer}
    , accTwoBody{&vectorBuffer}
    , velocityDerivative_{&vectorBuffer}
    , vectorBuffer{}
    , twoBodyGM_(twoBodyGM)
{
    init();
}

void BlockTwoBody::init()
{
    static auto identifierPos = aIdentifier(kIdentifierPos);
    static auto identifierAccTwoBody = aIdentifier(kIdentifierAccTwoBody);
    static auto identifierVel = aIdentifier(kIdentifierVel);

    inputPorts_ = {
        // 位置
        {
            identifierPos,
            (signal_t*)&posCBI,
            3,
            DataPort::eDouble
        }
    };

    outputPorts_ = {
        // 二体加速度
        {
            identifierAccTwoBody,
            (signal_t*)&accTwoBody,
            3,
            DataPort::eDouble
        }
    };

    derivativePorts_ = {
        // 速度导数
        {
            identifierVel,
            (signal_t*)&velocityDerivative_,
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
    Vector3d accCBI = factor * (*posCBI);
    *accTwoBody = accCBI;
    *velocityDerivative_ += accCBI;
    return eNoError;
}



AST_NAMESPACE_END

