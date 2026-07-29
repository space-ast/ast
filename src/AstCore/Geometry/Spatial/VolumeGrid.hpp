///
/// @file      VolumeGrid.hpp
/// @brief     体积网格基类
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
#include "AstUtil/ObjectNamed.hpp"
#include "AstMath/Vector.hpp"
#include "VolumeGridVisitor.hpp"

#include <vector>

AST_NAMESPACE_BEGIN

/// @brief 体积网格基类
/// @ingroup Spatial
class AST_CORE_API VolumeGrid : public ObjectNamed
{
public:
    VolumeGrid();
    ~VolumeGrid() override = default;

    /// @brief 接受访问者
    virtual void accept(VolumeGridVisitor& visitor) = 0;

    /// @brief 获取网格点总数
    virtual size_t getGridPointCount() const = 0;

    /// @brief 枚举所有网格点坐标
    /// @param points 输出的网格点坐标列表
    virtual void enumerateGridPoints(std::vector<Vector3d>& points) const = 0;

    /// @brief 获取包围盒
    /// @param[out] minCorner 最小角点坐标
    /// @param[out] maxCorner 最大角点坐标
    virtual void getBoundingBox(Vector3d& minCorner, Vector3d& maxCorner) const = 0;
};

AST_NAMESPACE_END
