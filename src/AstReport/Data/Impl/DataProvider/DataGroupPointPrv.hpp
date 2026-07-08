///
/// @file      DataGroupPointPrv.hpp
/// @brief     点位置数据组 (派生量)
/// @details   点的位置分量及派生量：模、方向余弦、球面角、方向角、速度
/// @author    axel
/// @date      2026-07-08
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

#include <cmath>

#include "AstGlobal.h"
#include "AstReport/DataGroupTimeVar.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Span.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


/// @brief 点位置数据组 (派生量) — 分量、模、方向余弦、球面角、方向角、速度
class DataGroupPointPrv : public DataGroupTimeVar
{
public:
    struct Data
    {
        TimePoint time_;
        Vector3d  pos_;
        Vector3d  vel_;

        const TimePoint& getTime() const { return time_; }

        // 位置分量 & 模
        double getX() const { return pos_.x(); }
        double getY() const { return pos_.y(); }
        double getZ() const { return pos_.z(); }
        double getMagnitude() const { return pos_.norm(); }

        // 方向余弦
        double getXOverMag() const { double m = pos_.norm(); return (m < 1e-12) ? 0.0 : pos_.x() / m; }
        double getYOverMag() const { double m = pos_.norm(); return (m < 1e-12) ? 0.0 : pos_.y() / m; }
        double getZOverMag() const { double m = pos_.norm(); return (m < 1e-12) ? 0.0 : pos_.z() / m; }

        // 球面角
        double getRightAscension()    const { return std::atan2(pos_.y(), pos_.x()); }
        double getDeclination()       const { double m = pos_.norm(); return (m < 1e-12) ? 0.0 : std::asin(pos_.z() / m); }
        double getCoDeclination()     const { double m = pos_.norm(); return (m < 1e-12) ? 0.0 : std::acos(pos_.z() / m); }
        double getNegativeDeclination() const { return -getDeclination(); }

        // 方向角
        double getDirectionAngleX() const { double m = pos_.norm(); return (m < 1e-12) ? 0.0 : std::acos(pos_.x() / m); }
        double getDirectionAngleY() const { double m = pos_.norm(); return (m < 1e-12) ? 0.0 : std::acos(pos_.y() / m); }
        double getDirectionAngleZ() const { double m = pos_.norm(); return (m < 1e-12) ? 0.0 : std::acos(pos_.z() / m); }

        // 速度
        double getVelocityX()         const { return vel_.x(); }
        double getVelocityY()         const { return vel_.y(); }
        double getVelocityZ()         const { return vel_.z(); }
        double getVelocityMagnitude() const { return vel_.norm(); }

        // Detic 坐标 — 需要 BodyShape，暂不支持
        // double getDeticLatitude()  const;
        // double getDeticLongitude() const;
        // double getDeticAltitude()  const;
    };
    static DataElements Elements();

    DataGroupPointPrv() = default;
    ~DataGroupPointPrv() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getPoint() const { return point_.get(); }
    Frame* getFrame() const { return frame_.get(); }
private:
    WeakPtr<Point> point_;
    WeakPtr<Frame> frame_;
};


/*! @} */

AST_NAMESPACE_END
