///
/// @file      SpatialCalculationVisitor.hpp
/// @brief     空间计算访问者接口
/// @author    axel
/// @date      2026-07-29
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

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

class SpatialCalculation;
class SpatialCalcAltitude;
class SpatialCalcAngleToLocation;
class SpatialCalcAtmosphericDensity;
class SpatialCalcDistance;
class SpatialCalcFile;
class SpatialCalcPropagationDelay;
class SpatialCalcSEET0;
class SpatialCalcSEET1;
class SpatialCalcSEET2;
class SpatialCalcSEET3;
class SpatialCalcScalarAtLocation;
class SpatialCalcSatisfactionMetrics;
class SpatialCalcSolarIntensity;

/// @brief 空间计算访问者接口
/// @ingroup Spatial
class SpatialCalculationVisitor
{
public:
    virtual ~SpatialCalculationVisitor() = default;

    virtual void visit(SpatialCalculation& calc) = 0;
    virtual void visit(SpatialCalcAltitude& calc) = 0;
    virtual void visit(SpatialCalcAngleToLocation& calc) = 0;
    virtual void visit(SpatialCalcAtmosphericDensity& calc) = 0;
    virtual void visit(SpatialCalcDistance& calc) = 0;
    virtual void visit(SpatialCalcFile& calc) = 0;
    virtual void visit(SpatialCalcPropagationDelay& calc) = 0;
    virtual void visit(SpatialCalcSEET0& calc) = 0;
    virtual void visit(SpatialCalcSEET1& calc) = 0;
    virtual void visit(SpatialCalcSEET2& calc) = 0;
    virtual void visit(SpatialCalcSEET3& calc) = 0;
    virtual void visit(SpatialCalcScalarAtLocation& calc) = 0;
    virtual void visit(SpatialCalcSatisfactionMetrics& calc) = 0;
    virtual void visit(SpatialCalcSolarIntensity& calc) = 0;
};

AST_NAMESPACE_END
