///
/// @file      Freestanding.cpp
/// @brief     高层可视化 API 实现（全局函数）
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

#include "Freestanding.hpp"
#include "AstVisualizationGlobal.h"

#include "VisView.hpp"
#include "VisGroup.hpp"
#include "VisTrajectory.hpp"
#include "VisCelestialBody.hpp"
#include "VisGroundTrack.hpp"
#include "VisRenderer.hpp"

#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TimeInterval.hpp"

AST_NAMESPACE_BEGIN

namespace visualization
{

namespace
{

/// @brief 当前可视化视图（单例）
VisView* _currentView()
{
    static SharedPtr<VisView> view = new VisView();
    return view;
}

/// @brief 默认时间区间：当点自身提供有限非空区间时用作缺省，否则保持原值
void setDefaultInterval(Point* pt, TimeInterval& interval)
{
    if (!pt) return;
    TimeInterval iv;
    if (pt->getInterval(iv) == eNoError && iv.isValid() && !iv.isInf()) {
        interval = iv;
    }
}

}

VisView* gcv()
{
    return _currentView();
}

VisTrajectory* trajectory(Point* point)
{
    VisTrajectory* traj = new VisTrajectory();
    if (point) {
        traj->setPoint(point);
        if (!traj->frame()) {
            traj->setFrame(point->getFrame());
        }
        TimeInterval interval = traj->interval();
        setDefaultInterval(point, interval);
        traj->setInterval(interval);
    }
    gcv()->objects()->add(traj);
    return traj;
}

VisGroundTrack* groundTrack(Point* point)
{
    VisGroundTrack* gt = new VisGroundTrack();
    if (point) {
        gt->setPoint(point);
        if (!gt->body()) {
            auto frame = point->getFrame();
            Body* body = nullptr;
            if(frame)
                body = frame->getBody();
            if(!body)
                body = aGetEarth();
            gt->setBody(body);
        }
        TimeInterval interval = gt->interval();
        setDefaultInterval(point, interval);
        gt->setInterval(interval);
    }
    gcv()->objects()->add(gt);
    return gt;
}

VisCelestialBody* planet(Body* body)
{
    VisCelestialBody* vis = new VisCelestialBody();
    vis->setBody(body);
    gcv()->objects()->add(vis);
    return vis;
}

void show(const TimePoint& epoch)
{
    auto renderer = VisRenderer::Default();
    if(renderer)
        renderer->render(*gcv(), epoch);
    else
        aWarning("No default renderer set.");
}

void show()
{
    show(TimePoint::CurrentTime());
}

}

AST_NAMESPACE_END
