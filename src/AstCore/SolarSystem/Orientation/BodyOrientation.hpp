///
/// @file      PlanetOrientation.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-06
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup SolarSystem
    @{
*/

class Rotation;
class KinematicRotation;

/// @brief 天体姿态/指向
class AST_CORE_API BodyOrientation
{
public:
    BodyOrientation() = default;
    

    virtual ~BodyOrientation() = default;


    /// @brief 获取ICRF到天体固连系的旋转变换
    /// @param tp 时间点
    /// @param rotation 旋转变换
    virtual void getICRFToFixedTransform(const TimePoint& tp, Rotation &rotation) const = 0;


    /// @brief 获取ICRF到天体固连系的旋转变换
    /// @param tp 时间点
    /// @param rotation 旋转变换
    virtual void getICRFToFixedTransform(const TimePoint& tp, KinematicRotation &rotation) const = 0;
    

    /// @brief 获取ICRF到天体惯性系的旋转变换
    /// @param tp 时间点
    /// @param rotation 旋转变换
    virtual void getICRFToInertialTransform(const TimePoint& tp, Rotation &rotation) const = 0;


    /// @brief 获取MOD系的父系
    /// @return Axes* 内置轴系
    virtual Axes* getMODParent() const = 0;

    
    /// @brief 获取MOD系的旋转变换
    /// @param tp 时间点
    /// @param rot 旋转变换
    virtual void getMODTransform(const TimePoint& tp, Rotation &rot) const = 0;


    /// @brief 获取TOD系的父系
    /// @return Axes* 内置轴系
    virtual Axes* getTODParent() const = 0;


    /// @brief 获取TOD系的旋转变换
    /// @param tp 时间点
    /// @param rot 旋转变换
    virtual void getTODTransform(const TimePoint& tp, Rotation &rot) const = 0;
        
};

/*! @} */

AST_NAMESPACE_END
