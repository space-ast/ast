///
/// @file      GravityCalculator.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-15
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

#include "GravityCalculator.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

void aPointMassField(const Vector3d& position, double GM,
                        Vector3d& acceleration, Matrix3d& hessian)
{
    // a = -GM/r³ · r
    // H = -GM/r³ · I + 3·GM/r⁵ · (r ⊗ r)
    const double r2    = position.squaredNorm();
    const double r     = std::sqrt(r2);
    const double factor = GM / (r2 * r);         // GM/r³
    const double cAcc  = -factor;                // 加速度系数：-GM/r³
    const double cOff  = 3.0 * factor / r2;      // 外积项系数：3·GM/r⁵

    for (int i = 0; i < 3; ++i)
    {
        acceleration[i] = cAcc * position[i];
        for (int j = 0; j < 3; ++j)
        {
            hessian(i, j) = cOff * position[i] * position[j];
        }
        hessian(i, i) += cAcc;
    }
}

GravityCalculator::GravityCalculator()
    : gravityField_{}
    , degree_{0}
    , order_{0}
{
    // @todo
}


GravityCalculator::GravityCalculator(const GravityField &gravityField, int degree, int order)
    : GravityCalculator((GravityField(gravityField)), degree, order)
{
}

GravityCalculator::GravityCalculator(GravityField &&gravityField, int degree, int order)
    : gravityField_(std::move(gravityField))
    // , degree_(degree)
    // , order_(order)
{
    initDegreeOrder(degree, order);
}

void GravityCalculator::setDegreeForGradient(int degree)
{
    this->degreeForGradient_ = std::min(degree_, degree);
    this->orderForGradient_  = std::min(order_,  degree);
}

void GravityCalculator::calcTotalAcceleration(const Vector3d &positionCBF, Vector3d &accelerationCBF)
{
    calcPertAcceleration(positionCBF, accelerationCBF);
    double factor = getGravityField().getGM() / (positionCBF.squaredNorm() * positionCBF.norm());
    accelerationCBF = accelerationCBF - positionCBF * factor;
}

void GravityCalculator::initDegreeOrder(int degree, int order)
{
    if(degree >= 0)
        degree_ = std::min(gravityField_.getMaxDegree(), degree);
    else
        degree_ = gravityField_.getMaxDegree();
    
    if(order >= 0)
        order_ = std::min(gravityField_.getMaxOrder(), order);
    else
        order_ = gravityField_.getMaxOrder();
}

AST_NAMESPACE_END
