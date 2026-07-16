///
/// @file      BlockThirdBodyGravity.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-06-24
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

#include "BlockThirdBodyGravity.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstUtil/Logger.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include <cstdio>

AST_NAMESPACE_BEGIN

// #define _AST_DEBUG_THIRD_BODY_GRAVITY

using namespace math;

BlockThirdBodyGravity::BlockThirdBodyGravity()
    : BlockDerivative{}
    , posCBI{&vectorBuffer}
    , accThirdBody{&vectorBuffer}
    , velocityDerivative_{&vectorBuffer}
    , vectorBuffer{}
    , thirdBody_{nullptr}
    , gravityAxes_{nullptr}
    , propagationAxes_{nullptr}
    , propagationFrame_{nullptr}
    , gravityCalculator_()
{
    init();
}

BlockThirdBodyGravity::BlockThirdBodyGravity(Point* thirdBody, GravityField&& gravityField,
                                             int degree, int order, Axes* gravityAxes,
                                             Frame* propagationFrame)
    : BlockDerivative{}
    , posCBI{&vectorBuffer}
    , accThirdBody{&vectorBuffer}
    , velocityDerivative_{&vectorBuffer}
    , vectorBuffer{}
    , thirdBody_{thirdBody}
    , gravityAxes_{gravityAxes}
    , propagationAxes_{nullptr}
    , propagationFrame_{propagationFrame}
    , gravityCalculator_(std::move(gravityField), degree, order)
{
    assert(thirdBody_ != nullptr);
    assert(propagationFrame_ != nullptr);
    assert(gravityAxes_ != nullptr);
    if (propagationFrame_)
    {
        propagationAxes_ = propagationFrame_->getAxes();
    }
    init();
}

void BlockThirdBodyGravity::init()
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
        // 三体重力加速度
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

errc_t BlockThirdBodyGravity::prepare(const SimTime& simTime, Vector3d& thirdBodyPos, Rotation& rotation)
{
    // @todo 将这里的空指针检查移动到初始阶段里
    if (!propagationFrame_ || !propagationAxes_ || !gravityAxes_ || !thirdBody_)
        return eErrorNullPtr;

    auto& tp = simTime.timePoint();

    // 1. 获取第三体在预报系中的位置
    errc_t err = thirdBody_->getPosIn(propagationFrame_, tp, thirdBodyPos);
    if (A_UNLIKELY(err != eNoError))
    {
        aError("failed to get third body position");
        return err;
    }

    // 2. 计算从预报系到第三体固连系的旋转矩阵
    errc_t rc = aAxesTransform(propagationAxes_, gravityAxes_, tp, rotation);
    if (A_UNLIKELY(rc != eNoError))
    {
        aError("failed to transform from propagation axes to gravity axes");
        return rc;
    }

    // 3. 若启用，更新重力场系数长期变化
    if (considerVariations_)
        gravityCalculator_.updateVariations(tp);

    return eNoError;
}

errc_t BlockThirdBodyGravity::run(const SimTime& simTime)
{
    Vector3d thirdBodyPos;
    Rotation rotation;
    errc_t err = prepare(simTime, thirdBodyPos, rotation);
    if (A_UNLIKELY(err != eNoError))
        return err;

    // 1. 计算航天器相对第三体的位置: r_rel = r - r₃
    Vector3d relPos = (*posCBI) - thirdBodyPos;

    // 2. 将相对位置变换到第三体固连系
    Vector3d relPosGravityAxes = rotation.transformVector(relPos);

    // 3. 在第三体固连系中计算球谐重力加速度（含中心项）
    Vector3d accGravityAxes;
    gravityCalculator_.calcTotalAcceleration(relPosGravityAxes, accGravityAxes);

    // 4. 将加速度逆变换回报系 → 直接项
    Vector3d accDirect = rotation.transformVectorInv(accGravityAxes);

    // 5. 计算间接项：中心天体在第三体引力场中的加速度
    //    中心天体相对第三体的位置为 -r₃，通过 calcTotalAcceleration
    //    可同时包含中心项与摄动项，确保 degree>0 时间接项也计入非球形影响
    Vector3d centralBodyRelPos = -thirdBodyPos;
    Vector3d centralBodyRelPosGravityAxes = rotation.transformVector(centralBodyRelPos);
    Vector3d accIndirectGravityAxes;
    gravityCalculator_.calcTotalAcceleration(centralBodyRelPosGravityAxes, accIndirectGravityAxes);
    Vector3d accIndirect = rotation.transformVectorInv(accIndirectGravityAxes);

    #ifdef _AST_DEBUG_THIRD_BODY_GRAVITY
    {
    // 6. 计算间接项: a_indirect = GM₃ · r₃ / |r₃|³
    auto thirdBodyGM = thirdBody_->getGM();
    double r3MagSqr = thirdBodyPos.squaredNorm();
    double r3Mag = std::sqrt(r3MagSqr);
    double indirectFactor = thirdBodyGM / (r3Mag * r3MagSqr);
    Vector3d accIndirect2 = indirectFactor * thirdBodyPos;
    std::printf("accIndirect:  %f, %f, %f\n", accIndirect.x(), accIndirect.y(), accIndirect.z());
    std::printf("accIndirect2: %f, %f, %f\n", accIndirect2.x(), accIndirect2.y(), accIndirect2.z());
    }
    #endif

    // 6. 总摄动 = 直接项 − 间接项
    Vector3d accTotal = accDirect - accIndirect;

    *accThirdBody = accTotal;
    *velocityDerivative_ += accTotal;
    return eNoError;
}

AST_NAMESPACE_END
