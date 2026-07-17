///
/// @file      DataGroupCartVel.hpp
/// @brief     笛卡尔速度数据组
/// @details   对应 CartVel 服务，输出 ICRF 坐标系下的笛卡尔速度分量、速率、径向速度和迹向速度
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


/// @brief 笛卡尔速度数据组 — ICRF 坐标系下的 vx, vy, vz, speed, vr, vi 速度分量
class DataGroupCartVel : public DataGroupTimeVar
{
public:
    struct Data
    {
        TimePoint time_;
        Vector3d  pos_;    ///< 位置（用于计算径向/迹向速度分量）
        Vector3d  vel_;

        const TimePoint& getTime() const { return time_; }
        double getVX() const { return vel_.x(); }
        double getVY() const { return vel_.y(); }
        double getVZ() const { return vel_.z(); }

        /// @brief 速率 — 速度矢量的模 |vel|
        double getSpeed() const { return vel_.norm(); }

        /// @brief 径向速度 — 速度在位置方向上的投影 vr = vel·pos / |pos|
        double getRadialVel() const
        {
            double r = pos_.norm();
            return (r < 1e-12) ? 0.0 : vel_.dot(pos_) / r;
        }

        /// @brief 迹向速度 — 速度在垂直于径向方向上的分量 vi = |pos × vel| / |pos|
        double getInTrackVel() const
        {
            double r = pos_.norm();
            return (r < 1e-12) ? 0.0 : pos_.cross(vel_).norm() / r;
        }
    };
    static DataElements Elements();
    DataGroupCartVel() = default;
    ~DataGroupCartVel() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getPoint() const { return point_.get(); }
    Frame* getFrame() const { return frame_.get(); }
    void setPoint(Point* p) { point_ = p; }
    void setFrame(Frame* f) { frame_ = f; }
private:
    WeakPtr<Point> point_{};
    WeakPtr<Frame> frame_{};
};


/*! @} */

AST_NAMESPACE_END
