///
/// @file      BlockDrag.cpp
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

#include "BlockDrag.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstCore/Frame.hpp"
#include "AstUtil/Identifier.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/KinematicTransform.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;
// #define _AST_DEBUG_DRAG

BlockDrag::BlockDrag()
    : BlockDrag(nullptr, 0, 0, nullptr)
{
    
}

BlockDrag::~BlockDrag()
{
    if(atmosphere_)
    {
        delete atmosphere_;
        atmosphere_ = nullptr;
    }
}

BlockDrag::BlockDrag(Atmosphere* atmosphere, double dragCoefficient, double dragArea, Frame* propagationFrame)
    : atmosphere_(atmosphere)
    , dragCoefficient_(dragCoefficient)
    , dragArea_(dragArea)
    , propagationFrame_(propagationFrame)
{
    static auto identifierPos = aIdentifier(kIdentifierPos);
    static auto identifierAccDrag = aIdentifier(kIdentifierAccDrag);
    static auto identifierVel = aIdentifier(kIdentifierVel);
    static auto identifierMass = aIdentifier(kIdentifierMass);


    inputPorts_ = {
        // 位置
        {
            identifierPos,
            (signal_t*)&position_,
            3,
            DataPort::eDouble
        },
        // 速度
        {
            identifierVel,
            (signal_t*)&velocity_,
            3,
            DataPort::eDouble
        },
        // 质量
        {
            identifierMass,
            (signal_t*)&mass_,
            1,
            DataPort::eDouble
        }
    };

    outputPorts_ = {
        // 大气阻力加速度
        {
            identifierAccDrag,
            (signal_t*)&accDrag_,
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



errc_t BlockDrag::run(const SimTime& simTime)
{
    assert(atmosphere_);
    auto& tp = simTime.timePoint();

    Vector3d accDrag;
    {
        Vector3d _;
        Vector3d posInAtmosFrame;
        Vector3d atmosVelocity;     // 大气速度(预报坐标系下)
        KinematicTransform transform; // 预报坐标系到大气模型参考坐标系的变换
        Frame* atmosFrame = atmosphere_->getFrame();
        propagationFrame_->getTransformTo(atmosFrame, tp, transform);
        transform.transformPosition(*position_, posInAtmosFrame);

        // 计算大气密度
        double density = atmosphere_->getDensity(tp, posInAtmosFrame);
        transform.inverse().transformPositionVelocity(posInAtmosFrame, Vector3d::Zero(), _, atmosVelocity);

        // 计算航天器相对于大气的速度
        Vector3d relVelocity = *velocity_ - atmosVelocity;
        // 计算阻力加速度
        // -1/2·Cd·S/m·rpo·v^2
        accDrag = -dragCoefficient_ * dragArea_ * density * relVelocity.norm() / (*mass_ * 2)  * relVelocity;
    }

#if defined(_AST_DEBUG_DRAG)
    Vector3d accDrag2;
    {
        Vector3d posInAtmosFrame;
        Vector3d velInAtmosFrame;
        KinematicTransform transform; // 预报坐标系到大气模型参考坐标系的变换
        Frame* atmosFrame = atmosphere_->getFrame();
        propagationFrame_->getTransformTo(atmosFrame, tp, transform);

        transform.getRotation().transformVector(*position_, posInAtmosFrame);
        transform.getRotation().transformVector(*velocity_, velInAtmosFrame);
        Vector3d anglvel{0, 0, kEarthAngVel};
        velInAtmosFrame = velInAtmosFrame - anglvel.cross(posInAtmosFrame);
        double density = atmosphere_->getDensity(tp, posInAtmosFrame);
        // 计算航天器相对于大气的速度
        Vector3d relVelocity = velInAtmosFrame;
        // 计算阻力加速度
        // -1/2·Cd·S/m·rpo·v^2
        accDrag2 = -dragCoefficient_ * dragArea_ * density * relVelocity.norm() / (*mass_ * 2)  * relVelocity;
        transform.getRotation().transformVectorInv(accDrag2, accDrag2);
    }

    double mag = accDrag_->norm();
    A_UNUSED(mag);
#endif
    // 输出阻力加速度
    *accDrag_ = accDrag;
    // 添加到速度导数上
    *velocityDerivative_ += accDrag;
    return eNoError;
}


AST_NAMESPACE_END
