///
/// @file      Point.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-09
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Geometry
    @{
*/

class Frame;
class CelestialBody;

class AST_CORE_API Point: public ObjectNamed
{
public:
    AST_OBJECT(Point)
    
    Point() = default;
    ~Point() override = default;

    /// @brief 将点转换为天体
    /// @return 天体指针
    CelestialBody* toBody() const;

    /// @brief 获取点所在的参考坐标系
    /// @return 点所在的参考坐标系指针
    virtual Frame* getFrame() const = 0;

    /// @brief 获取点在指定时间点的位置，相对于点的参考坐标系
    /// @param tp 时间点
    /// @param pos 输出参数，点的位置向量
    /// @return 错误码
    virtual errc_t getPos(const TimePoint& tp, Vector3d& pos) const = 0;

    /// @brief 获取点在指定时间点的位置和速度，相对于点的参考坐标系
    /// @param tp 时间点
    /// @param pos 输出参数，点的位置向量
    /// @param vel 输出参数，点的速度向量
    /// @return 错误码
    virtual errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const = 0;
    
    /// @brief 获取点在指定时间点的位置，相对于指定参考坐标系
    /// @param frame 参考坐标系指针
    /// @param tp 时间点
    /// @param pos 输出参数，点的位置向量
    /// @return 错误码
    errc_t getPosIn(Frame* frame, const TimePoint& tp, Vector3d& pos) const;

    /// @brief 获取点在指定时间点的位置和速度，相对于指定参考坐标系
    /// @param frame 参考坐标系指针
    /// @param tp 时间点
    /// @param pos 输出参数，点的位置向量
    /// @param vel 输出参数，点的速度向量
    /// @return 错误码
    errc_t getPosVelIn(Frame* frame, const TimePoint& tp, Vector3d& pos, Vector3d& vel) const;

};

/*! @} */

AST_NAMESPACE_END
