///
/// @file      VolumeGridVisitor.hpp
/// @brief     体积网格访问者接口
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

class VolumeGrid;
class VolumeGridCartesian;
class VolumeGridCartographic;
class VolumeGridCylindrical;
class VolumeGridSpherical;
class VolumeGridSurfaceBearing;
class VolumeGridConstrained;

/// @brief 体积网格访问者接口
/// @ingroup Spatial
class VolumeGridVisitor
{
public:
    virtual ~VolumeGridVisitor() = default;

    virtual void visit(VolumeGrid& grid) = 0;
    virtual void visit(VolumeGridCartesian& grid) = 0;
    virtual void visit(VolumeGridCartographic& grid) = 0;
    virtual void visit(VolumeGridCylindrical& grid) = 0;
    virtual void visit(VolumeGridSpherical& grid) = 0;
    virtual void visit(VolumeGridSurfaceBearing& grid) = 0;
    virtual void visit(VolumeGridConstrained& grid) = 0;
};

AST_NAMESPACE_END
