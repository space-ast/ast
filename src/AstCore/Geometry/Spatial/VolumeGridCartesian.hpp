///
/// @file      VolumeGridCartesian.hpp
/// @brief     笛卡尔体积网格
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

/// @brief 笛卡尔体积网格（X, Y, Z 坐标）
/// @ingroup Spatial
class AST_CORE_API VolumeGridCartesian final : public VolumeGrid
{
public:
    VolumeGridCartesian();
    ~VolumeGridCartesian() override = default;

    void accept(VolumeGridVisitor& visitor) override { visitor.visit(*this); }

    size_t getGridPointCount() const override;
    void enumerateGridPoints(std::vector<Vector3d>& points) const override;
    void getBoundingBox(Vector3d& minCorner, Vector3d& maxCorner) const override;

    /// @name 坐标轴
    /// @{
    GridCoord& xCoord() { return xCoord_; }
    const GridCoord& xCoord() const { return xCoord_; }
    GridCoord& yCoord() { return yCoord_; }
    const GridCoord& yCoord() const { return yCoord_; }
    GridCoord& zCoord() { return zCoord_; }
    const GridCoord& zCoord() const { return zCoord_; }
    /// @}

    /// @name 参考坐标系
    /// @{
    void setReferenceFrame(Frame* frame);
    Frame* getReferenceFrame() const { return refFrame_.get(); }
    Frame* getFrame() const override { return refFrame_.get(); }
    /// @}

private:
    GridCoord       xCoord_{};     ///< X 坐标轴
    GridCoord       yCoord_{};     ///< Y 坐标轴
    GridCoord       zCoord_{};     ///< Z 坐标轴
    WeakPtr<Frame>  refFrame_{};   ///< 参考坐标系
};

AST_NAMESPACE_END
