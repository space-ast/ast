///
/// @file      SpheroidShape.hpp
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

#pragma once

#include "AstGlobal.h"
#include "BodyShape.hpp"

AST_NAMESPACE_BEGIN

/// @brief 旋转椭球体/扁球体
/// @details 有两个轴长度相同，且与旋转轴垂直。
class AST_CORE_API SpheroidShape final : public BodyShape
{
public:
    AST_OBJECT(SpheroidShape)
    AST_PROPERT(majorAxis)
    AST_PROPERT(flatfactor)
    static SpheroidShape* NewFromMajorAxisFlatFactor(double majorAxis, double flatFactor);
    static SpheroidShape* NewFromMajorMinorAxis(double majorAxis, double minorAxis);
    
    SpheroidShape() = default;
    SpheroidShape(double majorAxis, double flatFactor);
    using BodyShape::transform;
    
    double majorAxis() const override{return majorAxis_;}

    void transform(const Vector3d& cartesian, GeodeticPoint& detic) const override;

    void transform(const GeodeticPoint& detic, Vector3d& cartesian) const override;
public:
    double flatFactor() const { return flatFactor_; }
PROPERTIES:
    length_d    majorAxis_{};  ///< 长轴长度
    double      flatFactor_{}; ///< 扁率因子
};

AST_NAMESPACE_END
