///
/// @file      SpatialConditionVisitor.hpp
/// @brief     空间条件访问者接口
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

class SpatialCondition;
class SpatialCondAccessToObjects;
class SpatialCondCombined;
class SpatialCondConditionAtLocation;
class SpatialCondGridBoundingVolume;
class SpatialCondLighting;
class SpatialCondOverTime;
class SpatialCondCalculationBounds;
class SpatialCondValidTimeAtLocation;

/// @brief 空间条件访问者接口
/// @ingroup Spatial
class SpatialConditionVisitor
{
public:
    virtual ~SpatialConditionVisitor() = default;

    virtual void visit(SpatialCondition& cond) = 0;
    virtual void visit(SpatialCondAccessToObjects& cond) = 0;
    virtual void visit(SpatialCondCombined& cond) = 0;
    virtual void visit(SpatialCondConditionAtLocation& cond) = 0;
    virtual void visit(SpatialCondGridBoundingVolume& cond) = 0;
    virtual void visit(SpatialCondLighting& cond) = 0;
    virtual void visit(SpatialCondOverTime& cond) = 0;
    virtual void visit(SpatialCondCalculationBounds& cond) = 0;
    virtual void visit(SpatialCondValidTimeAtLocation& cond) = 0;
};

AST_NAMESPACE_END
