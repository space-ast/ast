///
/// @file      BodyShape.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-12
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include "BodyShape.hpp"
#include "AstCore/SphereShape.hpp"
#include "AstCore/SpheroidShape.hpp"
#include "AstCore/EllipsoidShape.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/Constants.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

BodyShape::BodyShape()
{
    // 默认为组件对象
    this->setIsComponent(true);
}

errc_t BodyShape::transform(const Vector3d&, const Vector3d&,
                            GeodeticPoint&, LatLonAlt&) const
{
    // 默认实现: 仅旋转椭球与圆球给出了度量因子, 其余形状不支持该转换
    aError(_("该天体形状不支持大地坐标与天体固连系速度的相互转换"));
    return eErrorNotImplemented;
}

errc_t BodyShape::transform(const GeodeticPoint&, const LatLonAlt&,
                            Vector3d&, Vector3d&) const
{
    aError(_("该天体形状不支持大地坐标与天体固连系速度的相互转换"));
    return eErrorNotImplemented;
}

BodyShape* aWGS84Spheroid()
{
    static SharedPtr<BodyShape> bodyShape = SpheroidShape::NewFromMajorAxisFlatFactor(kEarthRadius_WGS84, kEarthFlatFact_WGS84);
    return bodyShape.get();
}

BodyShape* aCGCS2000Spheroid()
{
    static SharedPtr<BodyShape> bodyShape = SpheroidShape::NewFromMajorAxisFlatFactor(kEarthRadius_CGCS2000, kEarthFlatFact_CGCS2000);
    return bodyShape.get();
}

AST_NAMESPACE_END
