///
/// @file      BlockThirdBody.cpp
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

#include "BlockThirdBody.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/JplDe.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

BlockThirdBody::BlockThirdBody()
    : BlockThirdBody{kMoonGrav}
{

}

BlockThirdBody::BlockThirdBody(double thirdBodyGM)
    : BlockDerivative{}
    , posCBI{&vectorBuffer}
    , accThirdBody{&vectorBuffer}
    , velocityDerivative_{&vectorBuffer}
    , vectorBuffer{}
    , thirdBodyGM_{thirdBodyGM}  // 
{
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

err_t BlockThirdBody::run(const SimTime &simTime)
{
    // @fixme
    // 现在只支持计算月球三体引力
    // 后续再支持其他天体
    auto& tp = simTime.timePoint();
    Vector3d thirdBodyPos;
    err_t err = aJplDeGetPosICRF(tp, JplDe::eMoon, JplDe::eEarth, thirdBodyPos);
    if (A_UNLIKELY(err != eNoError))
    {
        aError("failed to get third body position");
        return err;
    }
    // 间接引力
    double magThirdBodyPosSqr = thirdBodyPos.squaredNorm();
    double magThirdBodyPos = std::sqrt(magThirdBodyPosSqr);
    double indirect = thirdBodyGM_ / (magThirdBodyPos * magThirdBodyPosSqr);
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


