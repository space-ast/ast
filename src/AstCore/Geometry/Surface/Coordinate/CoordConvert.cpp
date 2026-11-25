///
/// @file      CoordConvert.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-11-25
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "CoordConvert.hpp"
#include "AstUtil/Math.hpp"
#include "AstMath/Vector.hpp"
#include "LatLonRad.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

void aCartToLLR(const Vector3d &cart, LLR &llr)
{
    const double radius = cart.norm();
    llr.setRadius(radius);
    if ( radius == 0.0 )
    {
        // 零向量没有方向，经度与纬度均置零
        llr.setLongitude(0.0);
        llr.setLatitude(0.0);
        return;
    }
    llr.setLongitude(std::atan2(cart.y(), cart.x()));
    llr.setLatitude(std::asin(cart.z() / radius));
}

void aLLRToCart(const LLR &llr, Vector3d &cart)
{
    double cos_lon, sin_lon, cos_lat, sin_lat;
    sincos(llr.longitude(), &sin_lon, &cos_lon);
    sincos(llr.latitude(), &sin_lat, &cos_lat);

    cart[0] = llr.radius() * cos_lon * cos_lat;
    cart[1] = llr.radius() * sin_lon * cos_lat;
    cart[2] = llr.radius() * sin_lat;
}

AST_NAMESPACE_END

