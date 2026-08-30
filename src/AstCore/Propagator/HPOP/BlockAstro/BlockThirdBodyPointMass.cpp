///
/// @file      BlockThirdBodyPointMass.cpp
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

#include "BlockThirdBodyPointMass.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/JplDe.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstUtil/Logger.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

BlockThirdBodyPointMass::BlockThirdBodyPointMass()
    : BlockThirdBodyPointMass{aGetMoon(), kMoonGrav, aFrameECI()}
{

}

BlockThirdBodyPointMass::BlockThirdBodyPointMass(Point* thirdBody, double thirdBodyGM, Frame* propagationFrame)
    : BlockDerivative{}
    , posCBI{&vectorBuffer}
    , accThirdBody{&vectorBuffer}
    , velocityDerivative_{&vectorBuffer}
    , vectorBuffer{}
    , thirdBody_{thirdBody}       // 三体
    , thirdBodyGM_{thirdBodyGM}   // 三体引力常量
    , propagationFrame_{propagationFrame}   // 预报系
{
    assert(propagationFrame_ != nullptr);
    assert(thirdBody_ != nullptr);
    static auto identifierPos = aIdentifier(kIdentifierPos);
    static auto identifierAccThirdBody = aIdentifier(kIdentifierAccThirdBody);
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
        // 三体加速度
        {
            identifierAccThirdBody,
            (signal_t*)&accThirdBody,
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

errc_t BlockThirdBodyPointMass::run(const SimTime &simTime)
{
    assert(propagationFrame_ != nullptr);
    assert(thirdBody_ != nullptr);

    auto& tp = simTime.timePoint();
    Vector3d thirdBodyPos;
    errc_t err = thirdBody_->getPosIn(propagationFrame_, tp, thirdBodyPos);
    if (A_UNLIKELY(err != eNoError))
    {
        aError("failed to get third body position");
        return err;
    }
    // 间接引力
    double magThirdBodyPosSqr = thirdBodyPos.squaredNorm();
    double indirect;
    // 如果三体在预报系中的位置为零，说明该三体与预报中心天体相同，将其间接引力设为0
    if(A_UNLIKELY(magThirdBodyPosSqr == 0))
    {
        aWarning("third body relative position is zero");
        indirect = 0.0;
    }
    else
    {
        double magThirdBodyPos = std::sqrt(magThirdBodyPosSqr);
        indirect = thirdBodyGM_ / (magThirdBodyPos * magThirdBodyPosSqr);
    }
    // 直接引力
    Vector3d thirdBodyRelPos = thirdBodyPos - (*posCBI);
    double magThirdBodyRelPosSqr = thirdBodyRelPos.squaredNorm();
    double magThirdBodyRelPos = std::sqrt(magThirdBodyRelPosSqr);
    double direct = thirdBodyGM_ / (magThirdBodyRelPos * magThirdBodyRelPosSqr);
    // 总引力
    Vector3d accTotal = direct * thirdBodyRelPos - indirect * thirdBodyPos;
    *accThirdBody = accTotal;
    *velocityDerivative_ += accTotal;
    return eNoError;
}

AST_NAMESPACE_END


