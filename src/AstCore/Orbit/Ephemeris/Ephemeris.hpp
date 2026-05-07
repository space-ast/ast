///
/// @file      Ephemeris.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "AstCore/Point.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "AstCore/TimeInterval.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class Ephemeris;
using PEphemeris = Ephemeris*;
using HEphemeris = SharedPtr<Ephemeris>;


/// @brief 星历接口
/// @details 在点接口的基础上增加了对可用时间间隔的获取方法
class AST_CORE_API Ephemeris: public Point
{
public:
    Ephemeris() = default;
    virtual ~Ephemeris() override = default;
    virtual Frame* getFrame() const override = 0;
    virtual errc_t getPos(const TimePoint& tp, Vector3d& pos) const override = 0;
    virtual errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override = 0;
public:
    virtual errc_t getInterval(TimeInterval& interval) const = 0;
};

/*! @} */

AST_NAMESPACE_END
