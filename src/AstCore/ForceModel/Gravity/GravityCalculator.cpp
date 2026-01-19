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

AST_NAMESPACE_BEGIN

GravityCalculator::GravityCalculator()
    : gravityField_{}
    , degree_{0}
    , order_{0}
{
    // @todo
}

GravityCalculator::GravityCalculator (const GravityField &gravityField)
    : GravityCalculator(gravityField, gravityField.getMaxDegree(), gravityField.getMaxOrder())
{}


GravityCalculator::GravityCalculator(const GravityField &gravityField, int degree, int order)
    : gravityField_(gravityField)
    , degree_(degree)
    , order_(order)
{
    degree_ = std::min(gravityField_.getMaxDegree(), degree);
    order_ = std::min(gravityField_.getMaxOrder(), order);
}

GravityCalculator::GravityCalculator(StringView gravityFilePath, int degree, int order)
    : gravityField_()
    , degree_(degree)
    , order_(order)
{
    err_t err = gravityField_.load(gravityFilePath);
    if(err != eNoError)
    {
        aError("Failed to load gravity file: %s", gravityFilePath.data());
    }
    degree_ = std::min(gravityField_.getMaxDegree(), degree);
    order_ = std::min(gravityField_.getMaxOrder(), order);
}



void GravityCalculator::calcTotalAcceleration(const Vector3d &positionCBF, Vector3d &accelerationCBF)
{
    calcPertAcceleration(positionCBF, accelerationCBF);
    double factor = getGravityField().getGM() / (positionCBF.squaredNorm() * positionCBF.norm());
    accelerationCBF = accelerationCBF - positionCBF * factor;
}

AST_NAMESPACE_END
