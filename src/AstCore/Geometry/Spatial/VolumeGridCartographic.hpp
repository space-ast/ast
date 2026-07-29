///
/// @file      VolumeGridCartographic.hpp
/// @brief     经纬高体积网格
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

class CelestialBody;

/// @brief 经纬高体积网格（Latitude, Longitude, Altitude）
/// @ingroup Spatial
class AST_CORE_API VolumeGridCartographic final : public VolumeGrid
{
public:
    VolumeGridCartographic();
    ~VolumeGridCartographic() override = default;

    void accept(VolumeGridVisitor& visitor) override { visitor.visit(*this); }

    size_t getGridPointCount() const override;
    void enumerateGridPoints(std::vector<Vector3d>& points) const override;
    void getBoundingBox(Vector3d& minCorner, Vector3d& maxCorner) const override;
    Frame* getFrame() const override;

    /// @name 坐标轴
    /// @{
    GridCoord& latitude()  { return lat_; }
    const GridCoord& latitude() const { return lat_; }
    GridCoord& longitude() { return lon_; }
    const GridCoord& longitude() const { return lon_; }
    GridCoord& altitude()  { return alt_; }
    const GridCoord& altitude() const { return alt_; }
    /// @}

    /// @name 中心天体
    /// @{
    void setCentralBody(CelestialBody* body);
    CelestialBody* getCentralBody() const { return body_.get(); }
    /// @}

    /// @name 自适应拟合
    /// @{
    void setAutoFit(bool fit) { autoFit_ = fit; }
    bool getAutoFit() const { return autoFit_; }
    /// @}

private:
    GridCoord          lat_;                 ///< 纬度坐标轴
    GridCoord          lon_;                 ///< 经度坐标轴
    GridCoord          alt_;                 ///< 高度坐标轴
    WeakPtr<CelestialBody> body_;            ///< 中心天体
    bool               autoFit_{true};       ///< 是否自动拟合网格范围
};

AST_NAMESPACE_END
