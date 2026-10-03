///
/// @file      AttitudeTrajectoryRelated.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-30
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
#include "AttitudeProfile.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/Point.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 与轨迹相关的姿态
/// @details 即根据航天器的轨迹参数计算出的姿态
/// 例如，根据航天器当前的位置速度计算得到的 VVLH 姿态
class AST_CORE_API AttitudeTrajectoryRelated : public AttitudeProfile
{
public:
    AST_OBJECT(AttitudeTrajectoryRelated)

    AttitudeTrajectoryRelated() = default;
    AttitudeTrajectoryRelated(Point* point, Frame* frame);
    ~AttitudeTrajectoryRelated() override = default;
public:
    Axes* getParent() const override;
public:
    void setPoint(Point* point) { point_ = point; }
    void setFrame(Frame* frame) { frame_ = frame; }
    Point* point() const { return point_.get(); }
    Frame* frame() const { return frame_.get(); }
protected:
    errc_t getPosVelAccLocal(const TimePoint& tp, Vector3d& pos, Vector3d& vel, Vector3d& acc) const;
    errc_t getPosVelLocal(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const;
    errc_t getPosLocal(const TimePoint& tp, Vector3d& pos) const;
private:
    WeakPtr<Point> point_{};    ///< 轨迹点
    WeakPtr<Frame> frame_{};    ///< 轨迹参考系
};



/*! @} */

AST_NAMESPACE_END
