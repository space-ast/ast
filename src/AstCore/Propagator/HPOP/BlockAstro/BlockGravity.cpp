///
/// @file      BlockGravity.cpp
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

#include "BlockGravity.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstUtil/IdentifierAPI.hpp"


AST_NAMESPACE_BEGIN

BlockGravity::BlockGravity()
    : BlockDerivative{}
    , gravityAxes_{aAxesECF()}
    , propagationAxes_{aAxesECI()}
    , posPtr_{&vectorBuffer_}
    , accGravityPtr_{&vectorBuffer_}
    , velocityDerivativePtr_{&vectorBuffer_}
    , vectorBuffer_{}
    , gravityCalculator_()
{
    init();
}

BlockGravity::BlockGravity(const GravityField &gravityField, int degree, int order, Axes *gravityAxes, Axes *propagationAxes)
    : BlockGravity((GravityField(gravityField)), degree, order, gravityAxes, propagationAxes)
{
    
}

BlockGravity::BlockGravity(GravityField &&gravityField, int degree, int order, Axes *gravityAxes, Axes *propagationAxes)
    : BlockDerivative{}
    , gravityAxes_{gravityAxes}
    , propagationAxes_{propagationAxes}
    , posPtr_{&vectorBuffer_}
    , accGravityPtr_{&vectorBuffer_}
    , velocityDerivativePtr_{&vectorBuffer_}
    , vectorBuffer_{}
    , gravityCalculator_(std::move(gravityField), degree, order)
{
    init();
    if(!gravityAxes)
    {
        gravityAxes_ = aAxesECF();
    }
    if(!propagationAxes)
    {
        propagationAxes_ = aAxesECI();
    }
}



void BlockGravity::init()
{
    static auto identifierPos = aIdentifier(kIdentifierPos);
    static auto identifierAccGravity = aIdentifier(kIdentifierAccGravity);
    static auto identifierVel = aIdentifier(kIdentifierVel);
    
    inputPorts_ = {
        // 位置
        {
            identifierPos,
            (signal_t*)&posPtr_,
            3,
            DataPort::eDouble
        }
    };

    outputPorts_ = {
        // 重力加速度
        {
            identifierAccGravity,
            (signal_t*)&accGravityPtr_,
            3,
            DataPort::eDouble
        }
    };

    derivativePorts_ = {
        // 速度导数
        {
            identifierVel,
            (signal_t*)&velocityDerivativePtr_,
            3,
            DataPort::eDouble
        }
    };
}

errc_t BlockGravity::prepare(const SimTime &simTime, Rotation &rotation)
{
    auto& tp = simTime.timePoint();
    // 根据长期变化率更新重力场系数
    if(considerVariations_)
        gravityCalculator_.updateVariations(tp);
    // 求解坐标转换矩阵
    return aAxesTransform(propagationAxes_, gravityAxes_, tp, rotation);
}

errc_t BlockGravity::run(const SimTime &simTime)
{
    Rotation rotation;
    errc_t rc = prepare(simTime, rotation);
    if (A_UNLIKELY(rc != eNoError))
    {
        aError("failed to transform from propagation axes to gravity axes");
        return rc;
    }
    Vector3d posInGravityAxes;  // 位置(重力坐标系下)
    Vector3d accInGravityAxes;  // 重力加速度(重力坐标系下)
    // 将位置变换预报坐标系到重力场坐标系
    posInGravityAxes = rotation.transformVector(*posPtr_);
    // 计算总加速度（中心天体 + 摄动，重力坐标系下）
    gravityCalculator_.calcTotalAcceleration(posInGravityAxes, accInGravityAxes);
    // 将加速度变换回报坐标系：a_prop = Rᵀ · a_grav
    Vector3d accInPropagationAxes = rotation.transformVectorInv(accInGravityAxes);
    *accGravityPtr_ = accInPropagationAxes;
    *velocityDerivativePtr_ += accInPropagationAxes;
    return eNoError;
}


AST_NAMESPACE_END

