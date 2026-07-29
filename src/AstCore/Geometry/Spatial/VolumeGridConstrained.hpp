///
/// @file      VolumeGridConstrained.hpp
/// @brief     约束体积网格
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

#include "VolumeGrid.hpp"
#include "AstCore/WeakPtr.hpp"

AST_NAMESPACE_BEGIN

class SpatialCondition;

/// @brief 约束体积网格
/// @details 参考另一个 VolumeGrid，仅保留满足 SpatialCondition 的网格点
/// @ingroup Spatial
class AST_CORE_API VolumeGridConstrained final : public VolumeGrid
{
public:
    VolumeGridConstrained();
    ~VolumeGridConstrained() override = default;

    void accept(VolumeGridVisitor& visitor) override { visitor.visit(*this); }

    size_t getGridPointCount() const override;
    void enumerateGridPoints(std::vector<Vector3d>& points) const override;
    void getBoundingBox(Vector3d& minCorner, Vector3d& maxCorner) const override;
    Frame* getFrame() const override;

    /// @name 参考网格
    /// @{
    void setReferenceGrid(VolumeGrid* grid);
    VolumeGrid* getReferenceGrid() const { return refGrid_.get(); }
    /// @}

    /// @name 约束条件
    /// @{
    void setCondition(SpatialCondition* condition);
    SpatialCondition* getCondition() const { return condition_.get(); }
    /// @}

private:
    WeakPtr<VolumeGrid>       refGrid_;    ///< 参考的体积网格
    WeakPtr<SpatialCondition> condition_;  ///< 约束的空间条件
};

AST_NAMESPACE_END
