///
/// @file      VolumeGridSpherical.hpp
/// @brief     球面体积网格
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
#include "GridCoord.hpp"
#include "AstCore/WeakPtr.hpp"

AST_NAMESPACE_BEGIN

class Frame;

/// @brief 球面体积网格（Azimuth, Elevation, Range）
/// @ingroup Spatial
class AST_CORE_API VolumeGridSpherical final : public VolumeGrid
{
public:
    VolumeGridSpherical();
    ~VolumeGridSpherical() override = default;

    void accept(VolumeGridVisitor& visitor) override { visitor.visit(*this); }

    size_t getGridPointCount() const override;
    void enumerateGridPoints(std::vector<Vector3d>& points) const override;
    void getBoundingBox(Vector3d& minCorner, Vector3d& maxCorner) const override;

    /// @name 坐标轴
    /// @{
    GridCoord& azimuth()  { return azimuth_; }
    const GridCoord& azimuth() const { return azimuth_; }
    GridCoord& elevation() { return elevation_; }
    const GridCoord& elevation() const { return elevation_; }
    GridCoord& range()    { return range_; }
    const GridCoord& range() const { return range_; }
    /// @}

    /// @name 参考坐标系
    /// @{
    void setReferenceFrame(Frame* frame);
    Frame* getReferenceFrame() const { return refFrame_.get(); }
    Frame* getFrame() const override { return refFrame_.get(); }
    /// @}

private:
    GridCoord       azimuth_{};   ///< 方位角坐标轴
    GridCoord       elevation_{}; ///< 仰角坐标轴
    GridCoord       range_{};     ///< 距离坐标轴
    WeakPtr<Frame>  refFrame_{};  ///< 参考坐标系
};

AST_NAMESPACE_END
