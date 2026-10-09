///
/// @file      PointProxy.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-09
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
#include "Point.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Geometry
    @{
*/

/// @brief 点代理类
/// @details 将点接口原样转发给被代理的点对象，自身不产生任何计算。
class AST_CORE_API PointProxy final: public Point
{
public:
    PointProxy() = default;
    PointProxy(Point* point):impl_(point){}
    ~PointProxy() override = default;

public:
    void setImpl(Point* impl){impl_ = impl;}
    Point* impl() const {return impl_.get();}
public:
    Frame* getFrame() const override;
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const override;
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override;
    errc_t getPosVelAcc(const TimePoint& tp, Vector3d& pos, Vector3d& vel, Vector3d& acc) const override;
    errc_t getInterval(TimeInterval& interval) const override;
private:
    WeakPtr<Point> impl_{};
};




/*! @} */

AST_NAMESPACE_END
