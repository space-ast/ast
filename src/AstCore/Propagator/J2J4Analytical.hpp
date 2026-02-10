///
/// @file      Analytical.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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
#include "AstCore/OrbitElement.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Propagator
    @{
*/

class ModOrbElem;       ///< 改进轨道根数
class TimePoint;        ///< 时间点



/// @brief     J2J4 解析预报器模型
/// @details
/// 参考文献：
/// Vallado D A .Fundamentals of Astrodynamics and Applications, 4th ed.
/// 9.6节 Linearized Perturbations and Effects里的带谐项摄动对轨道根数的长期项表达式
class AST_CORE_API J2J4Analytical
{
public:
    J2J4Analytical(const ModOrbElem& modOrbElem, const TimePoint& epoch, double gm)
        : modOrbElem_(modOrbElem)
        , epoch_(epoch)
        , gm_(gm)
        , raanDot_(0)
        , argPeriDot_(0)
        , meanAnomalyDot_(0)
    {}

    /// @brief     轨道预报函数
    /// @details   ~
    /// @param     duration 时间间隔
    /// @param     gm       引力常数
    /// @param     j2       J2项
    /// @param     r        位置向量
    /// @param     v        速度向量
    /// @return    错误码
    err_t prop(double duration, Vector3d& r, Vector3d& v);

    /// @brief     轨道预报函数
    /// @details   ~
    /// @param     duration 时间间隔
    /// @param     gm       引力常数
    /// @param     j2       J2项
    /// @param     modOrbElem 改进轨道根数
    /// @return    错误码
    err_t prop(double duration, ModOrbElem& modOrbElem);


    /// @brief     轨道预报函数
    /// @details   ~
    /// @param     epoch    时间点
    /// @param     gm       引力常数
    /// @param     j2       J2项
    /// @param     r        位置向量
    /// @param     v        速度向量
    /// @return    错误码
    err_t prop(const TimePoint& epoch, Vector3d& r, Vector3d& v);

    /// @brief     轨道预报函数
    /// @details   ~
    /// @param     epoch    时间点
    /// @param     gm       引力常数
    /// @param     j2       J2项
    /// @param     modOrbElem 改进轨道根数
    /// @return    错误码
    err_t prop(const TimePoint& epoch, ModOrbElem& modOrbElem);
protected:
    /// @brief     轨道预报函数（不规范化角度）
    /// @details   ~
    /// @param     duration 时间间隔
    /// @param     modOrbElem 改进轨道根数
    /// @return    错误码
    err_t propNoNormalize(double duration, ModOrbElem& modOrbElem);
protected:
    /// @brief     设置升交点角速度
    /// @details   ~
    /// @param     raanDot 升交点角速度
    void setRaanDot(double raanDot) { raanDot_ = raanDot; }
    /// @brief     设置近心点角速度
    /// @details   ~
    /// @param     argPeriDot 近心点角速度
    void setArgPeriDot(double argPeriDot) { argPeriDot_ = argPeriDot; }
    /// @brief     设置平近点角速度
    /// @details   ~
    /// @param     meanAnomalyDot 平近点角速度
    void setMeanAnomalyDot(double meanAnomalyDot) { meanAnomalyDot_ = meanAnomalyDot; }
protected:
    ModOrbElem modOrbElem_;  ///< 改进轨道根数
    TimePoint epoch_;        ///< 参考时间点
    double gm_;              ///< 引力常数
    double raanDot_;         ///< 升交点角速度
    double argPeriDot_;      ///< 近心点角速度
    double meanAnomalyDot_;  ///< 平近点角速度
};


/*! @} */

AST_NAMESPACE_END
