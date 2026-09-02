///
/// @file      Platform.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-01
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
#include "AstCore/Point.hpp"
#include "AstCore/Axes.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 通用平台类，用于表示空间中的平台对象，例如卫星、飞机、地面站等，具有位置、姿态两个属性
/// @details 内部分别用Point和Axes抽象类来表示位置、姿态两个属性
/// @note 为什么继承Point类？
/// 因为一个对象的位置是确定的，但是姿态可以存在多种定义，例如体轴、对地定向、对速度定向、对三维模型定向等
class AST_SIM_API Platform : public Point
{
public:
    Platform() = default;
    Platform(Point* location, Axes* orientation)
        : location_(location), orientation_(orientation) 
    {}
    ~Platform() = default;
public:
    Frame* getFrame() const override;
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const override;
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override;
    errc_t getInterval(TimeInterval& interval) const override;
public:
    Point* location() const {return location_.get();}
    Axes* orientation() const {return orientation_.get();}
    void setLocation(Point* location) {location_ = location;}
    void setOrientation(Axes* orientation) {orientation_ = orientation;}
private:
    SharedPtr<Point> location_{};         ///< 平台位置
    SharedPtr<Axes>  orientation_{};      ///< 平台姿态
};



/*! @} */

AST_NAMESPACE_END
