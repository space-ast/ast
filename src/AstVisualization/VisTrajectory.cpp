///
/// @file      VisTrajectory.cpp
/// @brief     轨迹可视化对象实现
/// @details   实现按时间区间采样点位置的逻辑，供渲染后端获取轨迹点列。
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

#include "VisTrajectory.hpp"
#include "VisVisitor.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

void VisTrajectory::accept(VisVisitor& visitor)
{
    visitor.visit(*this);
}

void VisTrajectory::getPositions(std::vector<Vector3d>& positions) const
{
    positions.clear();

    Point* point = this->point();
    Frame* frame = this->frame();
    if (!point) {
        return ;
    }
    if (interval_.isEmpty()) {
        return;
    }

    if (step_ <= 0.0) {
        return;
    }

    const size_t maxPoints = 100000;   // 防御性上限
    TimePoint t = interval_.start();
    while (t <= interval_.stop() && positions.size() < maxPoints) 
    {
        Vector3d pos{};
        errc_t err = point->getPosIn(frame, t, pos);
        A_UNUSED(err);
        positions.push_back(pos);
        t = t + step_;
    }
}

AST_NAMESPACE_END
