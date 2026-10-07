///
/// @file      FrameICRF.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-11
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
#include "AstCore/Frame.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Coordinate
    @{
*/

class Point;
class Axes;

class AST_CORE_API FrameICRF : public Frame
{
public:
    FrameICRF() = default;
    ~FrameICRF() override = default;
    static FrameICRF* Instance();
    
    Frame* getParent() const override;
    Axes* getAxes() const override;
    Point* getOrigin() const override;
    errc_t getTransform(const TimePoint& tp, Transform& transform) const override;
    errc_t getTransform(const TimePoint& tp, KinematicTransform& transform) const override;
    errc_t getTransform(const TimePoint& tp, AccelerationTransform& transform) const override;
};

A_ALWAYS_INLINE Frame* aFrameICRF()
{
    return FrameICRF::Instance();
}

/*! @} */

AST_NAMESPACE_END
