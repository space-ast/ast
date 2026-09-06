///
/// @file      VisTrajectory.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-06
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
#include "VisObject.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/TimeInterval.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 轨迹可视化对象
/// @details 用于可视化空间中的轨迹，可以配置点、坐标系、时间间隔和步长。
class AST_VISUALIZATION_API VisTrajectory final: public VisObject
{
public:
    VisTrajectory() = default;
    ~VisTrajectory() = default;
    A_DISABLE_COPY(VisTrajectory)
public:
    Frame* frame() const { return frame_.get(); }
    void setFrame(Frame* frame) { frame_ = frame; }
    Point* point() const { return point_.get(); }
    void setPoint(Point* point) { point_ = point; }
    const TimeInterval& interval() const { return interval_; }
    void setInterval(const TimeInterval& interval) { interval_ = interval; }
    double step() const { return step_; }
    void setStep(double step) { step_ = step; }
public:
    /// @brief 采样整条轨迹
    /// @param positions 输出采样位置（SI，米）
    void getPositions(std::vector<Vector3d>& positions) const;
private:
    WeakPtr<Frame> frame_{};
    WeakPtr<Point> point_{};
    TimeInterval interval_{};
    double step_{6};
};


/*! @} */

AST_NAMESPACE_END
